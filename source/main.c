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

int main(void)
{
    uart_init(115200);

    /*if (*(vu32*)0x14828FA8 == 0x14000002) {
        hard_reset();
    }*/

    hard_reset();

    return 0;
}
