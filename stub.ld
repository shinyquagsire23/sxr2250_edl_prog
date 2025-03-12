OUTPUT_ARCH(aarch64)
ENTRY(start)

PHDRS
{
   /* I think this ordering is a security mitigation to prevent the stack from smashing .data */
   phdr_text PT_LOAD FLAGS(5);
   phdr_rodata PT_LOAD FLAGS(6);
   phdr_bss PT_LOAD FLAGS(6);
   phdr_data PT_LOAD FLAGS(6);
   phdr_dram PT_LOAD FLAGS(6);
}

MEMORY
  {
  ram : ORIGIN = 0x14980000, LENGTH = 3M /* TODO verify the length */
  dram : ORIGIN = 0x80000000, LENGTH = 0x0026f000 /* TODO verify the length */
  }

SECTIONS
{
    .init    : ALIGN(0x1000) 
    {
        __text_start = . ;
        KEEP (*(.init))
        . = ALIGN(4);  /* REQUIRED. LD is flaky without it. */
        } >ram :phdr_text =0xff
    .plt : { *(.plt) } >ram :phdr_text =0xff

    .text :   /* ALIGN (4): */
    {
        *(.text .stub .text.* .gnu.linkonce.t.*)
        KEEP (*(.text.*personality*))
        /* .gnu.warning sections are handled specially by elf32.em.  */
        *(.gnu.warning)
        *(.glue_7t) *(.glue_7) *(.vfp11_veneer)
        . = ALIGN(4);  /* REQUIRED. LD is flaky without it. */
    } >ram :phdr_text =0xff

    .fini           :
    {
        KEEP (*(.fini))
        . = ALIGN(0x1000);
        . += 0x100;
    } >ram :phdr_text =0xff

    __text_end = . ;

    .rodata : ALIGN(0x1000)
    {
        *(.rodata)
        *all.rodata*(*)
        *(.roda)
        *(.rodata.*)
        *(.gnu.linkonce.r*)
        SORT(CONSTRUCTORS)
        . = ALIGN(4);   /* REQUIRED. LD is flaky without it. */
        . += 0x1000;
    } >ram :phdr_rodata =0xFF

    .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >ram :phdr_rodata
    __exidx_start = .;
    .ARM.exidx   : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) } >ram :phdr_rodata
    __exidx_end = .;

/* Ensure the __preinit_array_start label is properly aligned.  We
   could instead move the label definition inside the section, but
   the linker would then create the section even if it turns out to
   be empty, which isn't pretty.  */
    . = ALIGN(32 / 8);
    PROVIDE (__preinit_array_start = .);
    .preinit_array     : { KEEP (*(.preinit_array)) } >ram :phdr_rodata =0xFF
    PROVIDE (__preinit_array_end = .);
    PROVIDE (__init_array_start = .);
    .init_array     : { KEEP (*(.init_array)) } >ram :phdr_rodata =0xFF
    PROVIDE (__init_array_end = .);
    PROVIDE (__fini_array_start = .);
    .fini_array     : { KEEP (*(.fini_array)) } >ram :phdr_rodata =0xFF
    PROVIDE (__fini_array_end = .);

    .ctors :
    {
    /* gcc uses crtbegin.o to find the start of the constructors, so
        we make sure it is first.  Because this is a wildcard, it
        doesn't matter if the user does not actually link against
        crtbegin.o; the linker won't look for a file to match a
        wildcard.  The wildcard also means that it doesn't matter which
        directory crtbegin.o is in.  */
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
        . = ALIGN(4);   /* REQUIRED. LD is flaky without it. */
    } >ram :phdr_rodata =0xFF

    .dtors :
    {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
        . = ALIGN(4);   /* REQUIRED. LD is flaky without it. */
    } >ram :phdr_rodata =0xFF

    .eh_frame :
    {
        KEEP (*(.eh_frame))
        . = ALIGN(4);   /* REQUIRED. LD is flaky without it. */
    } >ram :phdr_rodata =0xFF

    .gcc_except_table :
    {
        *(.gcc_except_table)
        . = ALIGN(4);   /* REQUIRED. LD is flaky without it. */
    } >ram :phdr_rodata =0xFF
    .jcr            : { KEEP (*(.jcr)) } >ram :phdr_rodata =0x00
    .got            : 
    {
        *(.got.plt) *(.got) 
        . = ALIGN(0x1000);
    } >ram :phdr_rodata =0x00

    /* I think this ordering is a security mitigation to prevent the stack from smashing .data */
    .bss : ALIGN(0x1000)   {
        __bss_start = ABSOLUTE(.);
        __bss_start__ = ABSOLUTE(.);
        *(.dynbss)
        *(.gnu.linkonce.b*)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);    /* REQUIRED. LD is flaky without it. */
        __bss_end__ = ABSOLUTE(.);
        
    } >ram :phdr_bss =0xFF
    
    .stack ALIGN(4) : {
        . = ALIGN(0x1000);
        __stack_start__ = ABSOLUTE(.);
        _stack = .; 
        *(.stack)
        . = . + 0x1000;
        __stack_end__ = ABSOLUTE(.);
        __end__ = ABSOLUTE(.);
        . = ALIGN(0x1000);
    } >ram :phdr_bss =0xFF

    .data : ALIGN(0x1000)   {
        __data_start = ABSOLUTE(.);
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d*)
        CONSTRUCTORS
        . = ALIGN(4);    /* REQUIRED. LD is flaky without it. */
        __data_end = ABSOLUTE(.);
        . = ALIGN(0x1000);
        . += 0x1000;
    } >ram :phdr_data =0xFF

    .dram : ALIGN(0x1000){
        . += 0x0026f000;
    } >dram : phdr_dram =0xFF

    /* Stabs debugging sections.  */
    .stab 0 : { *(.stab) }
    .stabstr 0 : { *(.stabstr) }
    .stab.excl 0 : { *(.stab.excl) }
    .stab.exclstr 0 : { *(.stab.exclstr) }
    .stab.index 0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment 0 : { *(.comment) }
    /*  DWARF debug sections.
        Symbols in the DWARF debugging sections are relative to the beginning
        of the section so we begin them at 0.  */
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
    .igot.plt  0 : { *(.igot.plt) }
    /* These must appear regardless of  .  */
}
