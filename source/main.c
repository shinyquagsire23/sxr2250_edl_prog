#include "types.h"
#include "utils.h"
#include "tinyprintf.h"

#include <string.h>
#include "elf.h"
#include "uart.h"

void hard_reset(void)
{
    __asm__("ldr x0, =0xc264000\n"
		    "ldr w1, [x0]\n"
		    "and w1, w1, #0xFFFFFFFE\n"
		    "str w1, [x0]\n"
		    "b .\n");
}

const char* my_sleepover_ = "\n"
",[ ].         its                   \n"
"\\ o /         *MY*                 \n"
" :::        sleepover and           \n"
"  ::           *I*                  \n" 
" ;  i    get to choose the firehose \n"
"\n";

int main(void)
{
    uart_init(115200);

    uart_puts("\n\nStart sxr2250_edl_prog...\n");
    uart_puts(my_sleepover_);

    //uart_puts(my_sleepover_);

    /*if (*(vu32*)0x14828FA8 == 0x14000002) {
        hard_reset();
    }*/

    hard_reset();

    return 0;
}
