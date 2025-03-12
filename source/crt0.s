.align 4
.section ".init"

.extern main
.global __start
.global _start
.global start
.global vbar_el1_synchronous

start:
_start:

mystery_vec_0:
    b mystery_vec_1
    nop
mystery_vec_1:
    b __start
mystery_vec_2:
    b .

__start:
    mov x7, x0
    mov x0, #0
    mov sp, x0
    dsb sy
    isb 
    dsb sy
    isb 
    mrs x0, SCTLR_EL1
    and x0, x0, #0xFFFFFFFFFFFFFFFE
    and x0, x0, #0xFFFFFFFFFFFFFFFB
    orr x0, x0, #8
    msr SCTLR_EL1, x0
    dsb sy
    isb 
    ldr x0, =vbar_el1_synchronous
    msr VBAR_EL1, x0
    mrs x0, CPACR_EL1
    orr x0, x0, #0x300000
    msr CPACR_EL1, x0
    isb 
    mrs x0, ISR_EL1
    and x0, x0, #0x100
    cbnz x0, mystery_vec_2
    msr DAIFClr, #4
    isb 
    
    ldr x0, =0x90000000
    mov sp, x0
    
    mrs x0, SCTLR_EL1
    orr x0, x0, #0x1000
    orr x0, x0, #0x800000
    msr SCTLR_EL1, x0

    ldr x0, =__bss_start__
    ldr x1, =__bss_end__
    mov x2, #0x0
    mov x3, #0x0
_bss_clear_loop:
    stp x2, x3, [x0, #0x0]
    add x0, x0, #0x10
    cmp x0, x1
    ble _bss_clear_loop

    mov w0, w7
    bl main
    b .

.pool

vbar_el1_synchronous:
    b .

.global get_currentel
get_currentel:
    mrs x0, currentel
    lsr x0, x0, #0x2
    and x0, x0, #0x3
    ret