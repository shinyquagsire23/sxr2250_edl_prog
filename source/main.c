#include "types.h"
#include "utils.h"
#include "tinyprintf.h"

#include <string.h>
#include "elf.h"
#include "uart.h"
#include "tinyprintf.h"
#include "sxr2250_fh_part3_bin.h"
#include "uzlib/tinf.h"

#define IMEM_BASE (0x146aa000)
#define IMEM_REBOOT_REASON (*(vu32*)(IMEM_BASE + 0x65C))

#define TCSR_BOOT_MISC_DETECT (*(vu32*)(0x1FD3000))

void hard_reset(void)
{
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

    printf("%s", "\n\nStart sxr2250_edl_prog...\n");
    printf("%s", my_sleepover_);
    printf("In EL%u.\n", get_currentel());

    // These only work with a warm reset, which needs PMIC, boo.
    IMEM_REBOOT_REASON = 2; // bootloader
    TCSR_BOOT_MISC_DETECT = (TCSR_BOOT_MISC_DETECT & ~0x3F) | 0x2;

    // Interesting note: 0x14828000~0x14932000 is zeroed and writes do not go through.
    // TZ carveout I guess?

#if 0
    printf("Decompressing...\n");
    uzlib_init();
    printf("init done\n");
    
    d.source = sxr2250_fh_part3_bin;
    d.dest = (uint8_t*)0x80100000;
    d.destSize = sxr2250_fh_part3_bin_size;
    
    printf("init? adsf\n\n");
    // use last 32K of accessible DRAM as a dictionary buffer
    uzlib_uncompress_init(&d, (void*)0x80000000, 32*1024);
    printf("init asdfff\n");
    
    int res = uzlib_zlib_parse_header(&d); // returns dict_opt
    if (res < 0) {
        printf("Failed to parse header.\n");
        hard_reset();
    }
    printf("header\n");
    
    do {
        //res = uzlib_uncompress_chksum(&d);
        res = uzlib_uncompress(&d);
        printf("%x\n", res);
    } while (res == TINF_OK);
    
    if(res != TINF_DONE) {
        printf("Failed to decompress %x\n", res);
        hard_reset();
    }
    printf("\n");
    hexdump("", (u8*)0x80100000, 0x100);
    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)0x80100000;
    u32 num_phdrs = ehdr->e_phnum;
    printf("%x phdrs\n", num_phdrs);
    printf("%lx phdr offs\n", ehdr->e_phoff);

    Elf64_Phdr* phdrs = (Elf64_Phdr*)(0x80100000 + ehdr->e_phoff);
    for (int i = 0; i < num_phdrs; i++) {
        printf("%x %lx %lx\n", i, phdrs[i].p_offset, phdrs[i].p_paddr);
    }
#endif
    hard_reset();

    return 0;
}
