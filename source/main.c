#include "types.h"
#include "utils.h"
#include "tinyprintf.h"

#include <string.h>
#include "elf.h"
#include "uart.h"
#include "spmi.h"
#include "tinyprintf.h"
#include "u-boot-dtb-gz_bin.h"
#include "uzlib/tinf.h"

#define IMEM_BASE (0x146aa000)
#define IMEM_REBOOT_REASON (*(vu32*)(IMEM_BASE + 0x65C))

#define TCSR_BOOT_MISC_DETECT (*(vu32*)(0x1FD3000))

#define EMERGENCY_DLOAD_MAGIC1    0x322A4F99
#define EMERGENCY_DLOAD_MAGIC2    0xC67E4350
#define EMERGENCY_DLOAD_MAGIC3    0x77777777
#define EMMC_DLOAD_TYPE     0x2

void hard_reset(void)
{
    // pshold-base
    // https://github.com/facebookincubator/oculus-linux-kernel/blob/oculus-quest3-kernel-master/arch/arm64/boot/dts/vendor/qcom/sdxlemur.dtsi#L201
    __asm__ volatile("ldr x0, =0xc264000\n"
		    "ldr w1, [x0]\n"
		    "and w1, w1, #0xFFFFFFFE\n"
		    "str w1, [x0]\n"
		    "b .\n");
}

extern int get_currentel(void);

const char* my_sleepover_ = "\n"
",[ ].         its                   \n"
"\\ o /         *MY*                 \n"
" :::        sleepover and           \n"
"  ::           *I*                  \n" 
" ;  i    get to choose the firehose \n"
"\n";

TINF_DATA d = {0};

//https://github.com/facebookincubator/oculus-linux-kernel/blob/oculus-go-kernel-master/drivers/power/reset/msm-poweroff.c
//https://github.com/facebookincubator/oculus-linux-kernel/blob/oculus-go-kernel-master/drivers/platform/msm/qpnp-power-on.c#L357
//https://github.com/facebookincubator/oculus-linux-kernel/blob/oculus-quest3-kernel-master/arch/arm64/boot/dts/vendor/qcom/anorak-qupv3.dtsi
int main(void)
{
    uart_init(115200);
    spmi_init();

    printf("%s", "\n\nStart sxr2250_edl_prog...\n");
    printf("%s", my_sleepover_);
    
    printf("In EL%u.\n", get_currentel());

    // These only work with a warm reset, which needs PMIC, boo.
    IMEM_REBOOT_REASON = 2; // bootloader
    TCSR_BOOT_MISC_DETECT = (TCSR_BOOT_MISC_DETECT & ~0x3F) | 0x2;

    u32 reason = pmic_qcom_read(0x7148);
    printf("Reboot reason? %x\n", reason);
    pmic_qcom_write(0x7148, ((reason & (0x7<<1)) | (0x2<<1)));
    printf("Reboot reason set? %x\n", pmic_qcom_read(0x7148));

    // Interesting note: 0x14828000~0x14932000 is zeroed and writes do not go through.
    // TZ carveout I guess?

    //hexdump("", (u8*)(0x80000000+0x029a2000), 0x1000);


#if 1
    printf("Decompress init...\n");
    uzlib_init();
    printf("Decompress init done.\n");
    
    d.source = u_boot_dtb_gz_bin;
    d.dest = (uint8_t*)0xA0000000;
    d.destSize = u_boot_dtb_gz_bin_size;
    
    // use last 32K of accessible DRAM as a dictionary buffer
    uzlib_uncompress_init(&d, (void*)0x80000000, 32*1024);
    
    int res = uzlib_zlib_parse_header(&d); // returns dict_opt
    if (res < 0) {
        printf("Failed to parse header.\n");
        hard_reset();
    }
    printf("Decompressing...\n");
    
    do {
        //res = uzlib_uncompress_chksum(&d);
        res = uzlib_uncompress(&d);
        printf("...\n");
    } while (res == TINF_OK);
    
    if(res != TINF_DONE) {
        printf("Failed to decompress %x\n", res);
        hard_reset();
    }
    printf("\n");
    //hexdump("", (u8*)0xA0000000, 0x100);
    printf("Jumping to u-boot...");
    printf("\n\n");
    
    void __attribute__((noreturn)) (*entry)(void* fdt) = (void*)0xA0000000;
    entry(NULL);
#endif
    hard_reset();

    return 0;
}
