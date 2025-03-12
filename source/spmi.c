// Mostly lifted from https://git.codelinaro.org/linaro/qcomlt/u-boot/-/blob/caleb/rbx-integration/drivers/spmi/spmi-msm.c
#include "spmi.h"

#include <string.h>
#include "tinyprintf.h"

#define SPMI_CNFG_ADDR (0xc42d000)
#define SPMI_CORE_ADDR (0xc400000)
#define SPMI_CHNLS_ADDR (0xc500000)
#define SPMI_OBSRVR_ADDR (0xc440000)
#define SPMI_INTR_ADDR (0xc4c0000)

/*
spmi_bus: spmi0_bus: qcom,spmi@c42d000 {
        compatible = "qcom,spmi-pmic-arb";
        reg = <0xc42d000 0x4000>,
              <0xc400000 0x3000>,
              <0xc500000 0x400000>,
              <0xc440000 0x80000>,
              <0xc4c0000 0x10000>;
        reg-names = "cnfg", "core", "chnls", "obsrvr", "intr";
        interrupts-extended = <&pdc 1 IRQ_TYPE_LEVEL_HIGH>;
        interrupt-names = "periph_irq";
        interrupt-controller;
        #interrupt-cells = <4>;
        #address-cells = <2>;
        #size-cells = <0>;
        cell-index = <0>;
        qcom,channel = <0>;
        qcom,ee = <0>;
        qcom,bus-id = <0>;
    };
*/

/* PMIC Arbiter configuration registers */
#define PMIC_ARB_VERSION 0x0000
#define PMIC_ARB_VERSION_V2_MIN 0x20010000
#define PMIC_ARB_VERSION_V3_MIN 0x30000000
#define PMIC_ARB_VERSION_V5_MIN 0x50000000
#define PMIC_ARB_VERSION_V7_MIN 0x70000000

#define APID_MAP_OFFSET_V1_V2_V3 (0x800)
#define APID_MAP_OFFSET_V5 (0x900)
#define APID_MAP_OFFSET_V7 (0x2000)
#define ARB_CHANNEL_OFFSET(n) (0x4 * (n))
#define SPMI_CH_OFFSET(chnl) ((chnl) * 0x8000)
#define SPMI_V5_OBS_CH_OFFSET(chnl) ((chnl) * 0x80)
#define SPMI_V7_OBS_CH_OFFSET(chnl) ((chnl) * 0x20)
#define SPMI_V5_RW_CH_OFFSET(chnl) ((chnl) * 0x10000)
#define SPMI_V7_RW_CH_OFFSET(chnl) ((chnl) * 0x1000)

#define SPMI_OWNERSHIP_PERIPH2OWNER(x)  ((x) & 0x7)

#define SPMI_REG_CMD0 0x0
#define SPMI_REG_CONFIG 0x4
#define SPMI_REG_STATUS 0x8
#define SPMI_REG_WDATA 0x10
#define SPMI_REG_RDATA 0x18

#define SPMI_CMD_OPCODE_SHIFT 27
#define SPMI_CMD_SLAVE_ID_SHIFT 20
#define SPMI_CMD_ADDR_SHIFT 12
#define SPMI_CMD_ADDR_OFFSET_SHIFT 4
#define SPMI_CMD_BYTE_CNT_SHIFT 0

#define SPMI_CMD_EXT_REG_WRITE_LONG 0x00
#define SPMI_CMD_EXT_REG_READ_LONG 0x01

#define SPMI_STATUS_DONE 0x1

#define SPMI_MAX_CHANNELS 128
#define SPMI_MAX_CHANNELS_V5    512
#define SPMI_MAX_CHANNELS_V7    1024
#define SPMI_MAX_SLAVES 16
#define SPMI_MAX_PERIPH 256

#define SPMI_CHANNEL_READ_ONLY  BIT(31)
#define SPMI_CHANNEL_MASK   0xffff

#define EIO (1) // TODO
#define EPERM (2) // TODO

#define PID_SHIFT 8
#define PID_MASK (0xFF << PID_SHIFT)
#define REG_MASK 0xFF

int pmic_qcom_write(u32 reg, u8 val)
{
    return spmi_write(SPMI_USID, (reg & PID_MASK) >> PID_SHIFT, reg & REG_MASK, val);
}

int pmic_qcom_read(u32 reg)
{
    return spmi_read(SPMI_USID, (reg & PID_MASK) >> PID_SHIFT, reg & REG_MASK);
}


enum arb_ver {
    V1 = 1,
    V2,
    V3,
    V5 = 5,
    V7 = 7
};

/*
 * PMIC arbiter version 5 uses different register offsets for read/write vs
 * observer channels.
 */
enum pmic_arb_channel {
    PMIC_ARB_CHANNEL_RW,
    PMIC_ARB_CHANNEL_OBS,
};

u32 spmi_owner;
u32 max_channels;
uint32_t channel_map[SPMI_MAX_SLAVES][SPMI_MAX_PERIPH];
u32 arb_ver;
phys_addr_t arb_chnl;

struct msm_spmi_priv {
    phys_addr_t arb_chnl;  /* ARB channel mapping base */
    phys_addr_t spmi_chnls; /* SPMI channels */
    phys_addr_t spmi_obs;  /* SPMI observer */
    phys_addr_t spmi_cnfg;  /* SPMI config */
    u32 owner;  /* Current owner */
    unsigned int max_channels; /* Max channels */
    /* SPMI channel map */
    uint32_t channel_map[SPMI_MAX_SLAVES][SPMI_MAX_PERIPH];
    /* SPMI bus arbiter version */
    u32 arb_ver;
};

static u32 pmic_arb_fmt_cmd_v1(u8 opc, u8 sid, u8 pid, u8 off)
{
    return (opc << 27) | (sid << 20) | (pid << 12) | (off << 4) | 1;
}

static u32 pmic_arb_fmt_cmd_v2(u8 opc, u8 off)
{
    return (opc << 27) | (off << 4) | 1;
}

int spmi_write(int usid, int pid, int off, u8 val)
{
    unsigned channel;
    unsigned int ch_offset;
    uint32_t reg = 0;

    if (usid >= SPMI_MAX_SLAVES)
        return -EIO;
    if (pid >= SPMI_MAX_PERIPH)
        return -EIO;
    if (channel_map[usid][pid] & SPMI_CHANNEL_READ_ONLY)
        return -EPERM;

    channel = channel_map[usid][pid] & SPMI_CHANNEL_MASK;

    //printf("%s: [%d:%d] %s: channel %d\n", "spmi", usid, pid, __func__, channel);

    switch (arb_ver) {
    case V1:
        ch_offset = SPMI_CH_OFFSET(channel);

        reg = pmic_arb_fmt_cmd_v1(SPMI_CMD_EXT_REG_WRITE_LONG,
                      usid, pid, off);
        break;

    case V2:
        ch_offset = SPMI_CH_OFFSET(channel);

        reg = pmic_arb_fmt_cmd_v2(SPMI_CMD_EXT_REG_WRITE_LONG, off);
        break;

    case V5:
        ch_offset = SPMI_V5_RW_CH_OFFSET(channel);

        reg = pmic_arb_fmt_cmd_v2(SPMI_CMD_EXT_REG_WRITE_LONG, off);
        break;

    case V7:
    default:
        ch_offset = SPMI_V7_RW_CH_OFFSET(channel);

        reg = pmic_arb_fmt_cmd_v2(SPMI_CMD_EXT_REG_WRITE_LONG, off);
        break;
    }

    /* Disable IRQ mode for the current channel*/
    writel(0x0, SPMI_CHNLS_ADDR + ch_offset + SPMI_REG_CONFIG);

    /* Write single byte */
    writel(val, SPMI_CHNLS_ADDR + ch_offset + SPMI_REG_WDATA);

    /* Send write command */
    writel(reg, SPMI_CHNLS_ADDR + ch_offset + SPMI_REG_CMD0);

    /* Wait till CMD DONE status */
    reg = 0;
    while (!reg) {
        reg = readl(SPMI_CHNLS_ADDR + ch_offset +
                SPMI_REG_STATUS);
    }

    if (reg ^ SPMI_STATUS_DONE) {
        printf("SPMI write failure.\n");
        return -EIO;
    }

    return 0;
}

int spmi_read(int usid, int pid, int off)
{
    unsigned channel;
    unsigned int ch_offset;
    uint32_t reg = 0;

    if (usid >= SPMI_MAX_SLAVES)
        return -EIO;
    if (pid >= SPMI_MAX_PERIPH)
        return -EIO;

    channel = channel_map[usid][pid] & SPMI_CHANNEL_MASK;

    //printf("%s: [%d:%d] %s: channel %d\n", "spmi", usid, pid, __func__, channel);

    switch (arb_ver) {
    case V1:
        ch_offset = SPMI_CH_OFFSET(channel);

        /* Prepare read command */
        reg = pmic_arb_fmt_cmd_v1(SPMI_CMD_EXT_REG_READ_LONG,
                      usid, pid, off);
        break;

    case V2:
        ch_offset = SPMI_CH_OFFSET(channel);

        /* Prepare read command */
        reg = pmic_arb_fmt_cmd_v2(SPMI_CMD_EXT_REG_READ_LONG, off);
        break;

    case V5:
        ch_offset = SPMI_V5_OBS_CH_OFFSET(channel);

        /* Prepare read command */
        reg = pmic_arb_fmt_cmd_v2(SPMI_CMD_EXT_REG_READ_LONG, off);
        break;

    case V7:
    default:
        ch_offset = SPMI_V7_OBS_CH_OFFSET(channel);

        /* Prepare read command */
        reg = pmic_arb_fmt_cmd_v2(SPMI_CMD_EXT_REG_READ_LONG, off);
        break;
    }

    /* Disable IRQ mode for the current channel*/
    writel(0x0, SPMI_OBSRVR_ADDR + ch_offset + SPMI_REG_CONFIG);

    /* Request read */
    writel(reg, SPMI_OBSRVR_ADDR + ch_offset + SPMI_REG_CMD0);

    /* Wait till CMD DONE status */
    reg = 0;
    while (!reg) {
        reg = readl(SPMI_OBSRVR_ADDR + ch_offset + SPMI_REG_STATUS);
    }

    if (reg ^ SPMI_STATUS_DONE) {
        printf("SPMI read failure.\n");
        return -EIO;
    }

    /* Read the data */
    return readl(SPMI_OBSRVR_ADDR + ch_offset +
                SPMI_REG_RDATA) & 0xFF;
}

int spmi_init() {
    u32 hw_ver = readl(SPMI_CORE_ADDR + PMIC_ARB_VERSION);

    if (hw_ver < PMIC_ARB_VERSION_V3_MIN) {
        arb_ver = V2;
        arb_chnl = SPMI_CORE_ADDR + APID_MAP_OFFSET_V1_V2_V3;
        max_channels = SPMI_MAX_CHANNELS;
    } else if (hw_ver < PMIC_ARB_VERSION_V5_MIN) {
        arb_ver = V3;
        arb_chnl = SPMI_CORE_ADDR + APID_MAP_OFFSET_V1_V2_V3;
        max_channels = SPMI_MAX_CHANNELS;
    } else if (hw_ver < PMIC_ARB_VERSION_V7_MIN) {
        arb_ver = V5;
        arb_chnl = SPMI_CORE_ADDR + APID_MAP_OFFSET_V5;
        max_channels = SPMI_MAX_CHANNELS_V5;
    } else {
        /* TOFIX: handle second bus */
        arb_ver = V7;
        arb_chnl = SPMI_CORE_ADDR + APID_MAP_OFFSET_V7;
        max_channels = SPMI_MAX_CHANNELS_V7;
    }

    for (int i = 0; i < max_channels; i++) {
        uint32_t periph = readl(arb_chnl + ARB_CHANNEL_OFFSET(i));
        uint8_t slave_id = (periph & 0xf0000) >> 16;
        uint8_t pid = (periph & 0xff00) >> 8;

        channel_map[slave_id][pid] = i;

#if 0
        if (slave_id || pid) {
            printf("[%d:%d] = %d\n", slave_id, pid, i);
        }
#endif

        /* Mark channels read-only when from different owner */
        if (arb_ver == V5 || arb_ver == V7) {
            uint32_t cnfg = readl(SPMI_CNFG_ADDR + ARB_CHANNEL_OFFSET(i));
            uint8_t owner_rd = SPMI_OWNERSHIP_PERIPH2OWNER(cnfg);

            if (owner_rd != spmi_owner)
                channel_map[slave_id][pid] |= SPMI_CHANNEL_READ_ONLY;
        }
    }

    return 0;
}

/*
[:1] = 0
[1:1] = 1
[2:1] = 2
[3:1] = 3
[4:1] = 4
[5:1] = 5
[6:1] = 6
[7:1] = 7
[8:1] = 8
[9:1] = 9
[10:1] = 10
[11:1] = 11
[12:1] = 12
[13:1] = 13
[0:113] = 14
[0:112] = 15
[0:18] = 16
[0:114] = 17
[0:115] = 18
[0:116] = 19
[0:117] = 20
[0:118] = 21
[0:119] = 22
[0:120] = 23
[0:121] = 24
[0:122] = 25
[0:123] = 26
[0:124] = 27
[0:125] = 28
[0:126] = 29
[0:127] = 30
[0:128] = 31
[0:129] = 32
[0:130] = 33
[0:131] = 34
[0:132] = 35
[0:133] = 36
[0:134] = 37
[0:135] = 38
[0:136] = 39
[0:137] = 40
[0:138] = 41
[0:139] = 42
[0:140] = 43
[0:141] = 44
[0:142] = 45
[0:143] = 46
[0:144] = 47
[0:145] = 48
[0:146] = 49
[0:147] = 50
[0:148] = 51
[0:149] = 52
[0:150] = 53
[0:151] = 54
[0:152] = 55
[0:153] = 56
[0:154] = 57
[0:155] = 58
[0:156] = 59
[0:157] = 60
[0:158] = 61
[0:159] = 62
[0:160] = 63
[0:161] = 64
[0:162] = 65
[0:163] = 66
[0:164] = 67
[0:165] = 68
[0:166] = 69
[0:167] = 70
[0:168] = 71
[0:169] = 72
[0:170] = 73
[0:171] = 74
[0:172] = 75
[0:173] = 76
[0:174] = 77
[0:175] = 78
[0:176] = 79
[0:177] = 80
[0:178] = 81
[0:179] = 82
[0:180] = 83
[0:181] = 84
[0:182] = 85
[0:183] = 86
[7:48] = 87
[0:8] = 88
[1:8] = 89
[2:8] = 90
[3:8] = 91
[4:8] = 92
[5:8] = 93
[6:8] = 94
[7:8] = 95
[8:8] = 96
[9:8] = 97
[10:8] = 98
[11:8] = 99
[12:8] = 100
[13:8] = 101
[0:26] = 102
[7:22] = 103
[1:143] = 104
[1:141] = 105
[7:137] = 106
[7:141] = 107
[0:13] = 108
[0:188] = 109
[0:7] = 110
[0:81] = 111
[0:80] = 112
[0:100] = 113
[0:97] = 114
[0:98] = 115
[0:89] = 116
[0:90] = 117
[0:91] = 118
[0:84] = 119
[0:85] = 120
[0:86] = 121
[0:87] = 122
[0:88] = 123
[0:92] = 124
[0:94] = 125
[0:22] = 126
[0:30] = 127
[0:28] = 128
[0:24] = 129
[0:29] = 130
[0:25] = 131
[0:32] = 132
[0:33] = 133
[0:105] = 134
[0:108] = 135
[0:44] = 136
[0:45] = 137
[0:46] = 138
[0:4] = 139
[1:10] = 140
[2:10] = 141
[3:10] = 142
[5:10] = 143
[7:10] = 144
[3:167] = 145
[1:155] = 146
[1:158] = 147
[1:161] = 148
[1:164] = 149
[2:164] = 150
[2:170] = 151
[2:161] = 152
[3:170] = 153
[3:164] = 154
[5:167] = 155
[5:158] = 156
[5:161] = 157
[1:167] = 158
[1:170] = 159
[2:155] = 160
[2:158] = 161
[2:167] = 162
[3:155] = 163
[3:158] = 164
[3:161] = 165
[5:155] = 166
[5:170] = 167
[5:173] = 168
[5:176] = 169
[10:155] = 170
[10:156] = 171
[10:157] = 172
[5:164] = 173
[5:195] = 174
[1:193] = 175
[1:194] = 176
[1:195] = 177
[1:196] = 178
[1:197] = 179
[1:198] = 180
[1:199] = 181
[1:200] = 182
[1:201] = 183
[1:202] = 184
[1:203] = 185
[2:193] = 186
[2:194] = 187
[2:195] = 188
[3:193] = 189
[3:194] = 190
[3:195] = 191
[5:193] = 192
[5:194] = 193
[1:204] = 194
[1:205] = 195
[1:206] = 196
[1:207] = 197
[1:208] = 198
[1:209] = 199
[1:210] = 200
[1:211] = 201
[1:212] = 202
[1:213] = 203
[1:214] = 204
[1:215] = 205
[1:225] = 206
[1:226] = 207
[2:225] = 208
[3:225] = 209
[5:225] = 210
[1:15] = 211
[7:25] = 212
[7:9] = 213
[7:44] = 214
[7:81] = 215
[7:24] = 216
[7:29] = 217
[7:30] = 218
[7:28] = 219
[7:60] = 220
[7:14] = 221
[7:20] = 222
[7:37] = 223
[7:79] = 224
[7:12] = 225
[7:71] = 226
[1:71] = 227
[1:73] = 228
[7:72] = 229
[1:72] = 230
[1:228] = 231
[1:9] = 232
[1:238] = 233
[7:242] = 234
[1:6] = 235
[0:17] = 236
[0:6] = 237
[2:6] = 238
[3:6] = 239
[5:6] = 240
[10:6] = 241
[7:6] = 242
[1:5] = 243
[0:5] = 244
[2:5] = 245
[3:5] = 246
[5:5] = 247
[10:5] = 248
[7:5] = 249
[0:38] = 250
[0:39] = 251
[1:28] = 252
[2:28] = 253
[3:28] = 254
[5:28] = 255
[10:28] = 256
[0:54] = 257
[0:43] = 258
[0:107] = 259
[1:54] = 260
[1:57] = 261
[2:54] = 262
[2:57] = 263
[3:54] = 264
[3:57] = 265
[5:54] = 266
[5:57] = 267
[7:54] = 268
[7:58] = 269
[0:109] = 270
[0:41] = 271
[1:62] = 272
[1:216] = 273
[1:64] = 274
[1:65] = 275
[1:21] = 276
[7:38] = 277
[7:43] = 278
[7:39] = 279
[7:40] = 280
[7:41] = 281
[2:136] = 282
[7:78] = 283
[0:186] = 284
[0:9] = 285
[0:11] = 286
[0:15] = 287
[0:19] = 288
[0:21] = 289
[0:23] = 290
[0:27] = 291
[0:31] = 292
[0:34] = 293
[0:35] = 294
[0:36] = 295
[0:37] = 296
[0:40] = 297
[0:57] = 298
[0:61] = 299
[0:96] = 300
[0:99] = 301
[0:103] = 302
[0:104] = 303
[0:106] = 304
[0:110] = 305
[0:111] = 306
[0:184] = 307
[0:185] = 308
[0:187] = 309
[0:189] = 310
[0:232] = 311
[0:233] = 312
[0:254] = 313
[1:4] = 314
[1:11] = 315
[1:13] = 316
[1:14] = 317
[1:19] = 318
[1:20] = 319
[1:29] = 320
[1:60] = 321
[1:74] = 322
[1:81] = 323
[1:94] = 324
[1:136] = 325
[1:137] = 326
[1:138] = 327
[1:139] = 328
[1:140] = 329
[1:142] = 330
[1:144] = 331
[1:145] = 332
[1:146] = 333
[1:147] = 334
[1:152] = 335
[1:153] = 336
[1:154] = 337
[1:156] = 338
[1:157] = 339
[1:159] = 340
[1:160] = 341
[1:162] = 342
[1:163] = 343
[1:165] = 344
[1:166] = 345
[1:168] = 346
[1:169] = 347
[1:171] = 348
[1:172] = 349
[1:229] = 350
[1:232] = 351
[1:233] = 352
[1:234] = 353
[1:235] = 354
[1:239] = 355
[1:254] = 356
[1:255] = 357
[2:4] = 358
[2:9] = 359
[2:11] = 360
[2:14] = 361
[2:15] = 362
[2:29] = 363
[2:60] = 364
[2:81] = 365
[2:94] = 366
[2:137] = 367
[2:138] = 368
[2:139] = 369
[2:140] = 370
[2:141] = 371
[2:153] = 372
[2:154] = 373
[2:156] = 374
[2:157] = 375
[2:159] = 376
[2:160] = 377
[2:162] = 378
[2:163] = 379
[2:165] = 380
[2:166] = 381
[2:168] = 382
[2:169] = 383
[2:171] = 384
[2:172] = 385
[2:254] = 386
[2:255] = 387
[3:4] = 388
[3:9] = 389
[3:11] = 390
[3:14] = 391
[3:15] = 392
[3:29] = 393
[3:60] = 394
[3:81] = 395
[3:94] = 396
[3:136] = 397
[3:137] = 398
[3:138] = 399
[3:139] = 400
[3:140] = 401
[3:141] = 402
[3:153] = 403
[3:154] = 404
[3:156] = 405
[3:157] = 406
[3:159] = 407
[3:160] = 408
[3:162] = 409
[3:163] = 410
[3:165] = 411
[3:166] = 412
[3:168] = 413
[3:169] = 414
[3:171] = 415
[3:172] = 416
[3:254] = 417
[3:255] = 418
[5:4] = 419
[5:9] = 420
[5:11] = 421
[5:14] = 422
[5:15] = 423
[5:29] = 424
[5:60] = 425
[5:81] = 426
[5:94] = 427
[5:136] = 428
[5:137] = 429
[5:138] = 430
[5:139] = 431
[5:140] = 432
[5:141] = 433
[5:142] = 434
[5:143] = 435
[5:153] = 436
[5:154] = 437
[5:156] = 438
[5:157] = 439
[5:159] = 440
[5:160] = 441
[5:162] = 442
[5:163] = 443
[5:165] = 444
[5:166] = 445
[5:168] = 446
[5:169] = 447
[5:171] = 448
[5:172] = 449
[5:174] = 450
[5:175] = 451
[5:177] = 452
[5:178] = 453
[5:254] = 454
[5:255] = 455
[7:4] = 456
[7:11] = 457
[7:15] = 458
[7:23] = 459
[7:42] = 460
[7:45] = 461
[7:57] = 462
[7:61] = 463
[7:62] = 464
[7:94] = 465
[7:136] = 466
[7:138] = 467
[7:139] = 468
[7:140] = 469
[7:142] = 470
[7:143] = 471
[7:144] = 472
[7:145] = 473
[7:146] = 474
[7:147] = 475
[7:240] = 476
[7:241] = 477
[7:253] = 478
[7:254] = 479
[10:4] = 480
[10:9] = 481
[10:10] = 482
[10:11] = 483
[10:29] = 484
[10:81] = 485
[10:94] = 486
[10:136] = 487
[10:137] = 488
[10:138] = 489
[10:139] = 490
[10:153] = 491
[10:154] = 492
[10:254] = 493
[10:255] = 494
[11:1] = 768
[12:1] = 769
[0:1] = 770
[1:1] = 771
[2:1] = 772
[3:1] = 773
[4:1] = 774
[5:1] = 775
[6:1] = 776
[7:1] = 777
[8:1] = 778
[9:1] = 779
[10:1] = 780
[13:1] = 781
[9:38] = 782
[9:5] = 783
[9:6] = 784
[13:38] = 785
[13:5] = 786
[13:6] = 787
*/

#if 0
static struct dm_spmi_ops msm_spmi_ops = {
    .read = msm_spmi_read,
    .write = msm_spmi_write,
};

static int msm_spmi_probe(struct udevice *dev)
{
    struct msm_spmi_priv *priv = dev_get_priv(dev);
    phys_addr_t core_addr;
    u32 hw_ver;
    int i;

    core_addr = dev_read_addr_name(dev, "core");
    SPMI_CHNLS_ADDR = dev_read_addr_name(dev, "chnls");
    SPMI_OBSRVR_ADDR = dev_read_addr_name(dev, "obsrvr");
    dev_read_u32(dev, "qcom,ee", &priv->owner);

    hw_ver = readl(core_addr + PMIC_ARB_VERSION);

    if (hw_ver < PMIC_ARB_VERSION_V3_MIN) {
        priv->arb_ver = V2;
        priv->arb_chnl = core_addr + APID_MAP_OFFSET_V1_V2_V3;
        priv->max_channels = SPMI_MAX_CHANNELS;
    } else if (hw_ver < PMIC_ARB_VERSION_V5_MIN) {
        priv->arb_ver = V3;
        priv->arb_chnl = core_addr + APID_MAP_OFFSET_V1_V2_V3;
        priv->max_channels = SPMI_MAX_CHANNELS;
    } else if (hw_ver < PMIC_ARB_VERSION_V7_MIN) {
        priv->arb_ver = V5;
        priv->arb_chnl = core_addr + APID_MAP_OFFSET_V5;
        priv->max_channels = SPMI_MAX_CHANNELS_V5;
        SPMI_CNFG_ADDR = dev_read_addr_name(dev, "cnfg");
    } else {
        /* TOFIX: handle second bus */
        priv->arb_ver = V7;
        priv->arb_chnl = core_addr + APID_MAP_OFFSET_V7;
        priv->max_channels = SPMI_MAX_CHANNELS_V7;
        SPMI_CNFG_ADDR = dev_read_addr_name(dev, "cnfg");
    }

    dev_dbg(dev, "PMIC Arb Version-%d (%#x)\n", hw_ver >> 28, hw_ver);

    if (priv->arb_chnl == FDT_ADDR_T_NONE ||
        SPMI_CHNLS_ADDR == FDT_ADDR_T_NONE ||
        SPMI_OBSRVR_ADDR == FDT_ADDR_T_NONE)
        return -EINVAL;

    dev_dbg(dev, "priv->arb_chnl address (%#08llx)\n", priv->arb_chnl);
    dev_dbg(dev, "SPMI_CHNLS_ADDR address (%#08llx)\n", SPMI_CHNLS_ADDR);
    dev_dbg(dev, "SPMI_OBSRVR_ADDR address (%#08llx)\n", SPMI_OBSRVR_ADDR);
    /* Scan peripherals connected to each SPMI channel */
    for (i = 0; i < priv->max_channels; i++) {
        uint32_t periph = readl(priv->arb_chnl + ARB_CHANNEL_OFFSET(i));
        uint8_t slave_id = (periph & 0xf0000) >> 16;
        uint8_t pid = (periph & 0xff00) >> 8;

        priv->channel_map[slave_id][pid] = i;

        /* Mark channels read-only when from different owner */
        if (priv->arb_ver == V5 || priv->arb_ver == V7) {
            uint32_t cnfg = readl(SPMI_CNFG_ADDR + ARB_CHANNEL_OFFSET(i));
            uint8_t owner = SPMI_OWNERSHIP_PERIPH2OWNER(cnfg);

            if (owner != spmi_owner)
                priv->channel_map[slave_id][pid] |= SPMI_CHANNEL_READ_ONLY;
        }
    }
    return 0;
}
#endif