/* memory definition required, like

  MEMORY
  {
    RAM (xrw)  :    ORIGIN = 0x10000000, LENGTH = 128K
    ITCRAM (rwx) :  ORIGIN = 0x20000000, LENGTH = 128K
    DTCRAM (xrw) :  ORIGIN = 0x30000000, LENGTH = 128K
    RAM2 (xrw) :    ORIGIN = 0x40000000, LENGTH = 128K
  }

if some regions missing they must be presented as alias like:

  REGION_ALIAS("ITCRAM", RAM);

  REGION_ALIAS("TEXT", SDRAM);

*/


PROVIDE( __stack = ORIGIN(RAM) + LENGTH(RAM) ) ;
__Main_Stack_Size = 4096;
PROVIDE ( _Heap_Begin = _end_noinit ) ;
PROVIDE ( _Heap_Limit = ORIGIN(RAM)+LENGTH(RAM)-__Main_Stack_Size ) ;

ENTRY(_soft_entry)

SECTIONS
{
  .isr_vector :
  {
    _iram_start = ABSOLUTE(.);
    _vector_table = ABSOLUTE(.);
    __vectors_start = ABSOLUTE(.);

    /* Vectors according to builds/RF-2015.2-win32/esp108_v1_2_s5_512int_2/config.html */
    . = 0x0;
    KEEP(*(.WindowVectors.text));
    . = 0x180;
    KEEP(*(.Level2InterruptVector.text));
    . = 0x1c0;
    KEEP(*(.Level3InterruptVector.text));
    . = 0x200;
    KEEP(*(.Level4InterruptVector.text));
    . = 0x240;
    KEEP(*(.Level5InterruptVector.text));
    . = 0x280;
    KEEP(*(.DebugExceptionVector.text));
    . = 0x2c0;
    KEEP(*(.NMIExceptionVector.text));
    . = 0x300;
    KEEP(*(.KernelExceptionVector.text));
    . = 0x340;
    KEEP(*(.UserExceptionVector.text));
    . = 0x3C0;
    KEEP(*(.DoubleExceptionVector.text));
    . = 0x400;
    _invalid_pc_placeholder = ABSOLUTE(.);
    *(.*Vector.literal)

    *(.UserEnter.literal);
    *(.UserEnter.text);
    . = ALIGN (16);
    *(.entry.text)
    *(.init.literal)
    *(.init)

    _init_end = ABSOLUTE(.);
  } > TEXT

  .inits : ALIGN(4)
  {
    /* DATA COPY REGIONS */

    __data_regions_array_start = .;

    LONG(LOADADDR(.data));         LONG(ADDR(.data));         LONG(ADDR(.data)+SIZEOF(.data));

    __data_regions_array_end = .;

    /* DATA CLEAR REGIONS */

    __bss_regions_array_start = .;

    LONG(ADDR(.bss));         LONG(ADDR(.bss)+SIZEOF(.bss));

    __bss_regions_array_end = .;

  } > TEXT


  .iram0.text :
  {
    /* Code marked as runnning out of IRAM */
    _iram_text_start = ABSOLUTE(.);


    KEEP(*(.init))
	 KEEP(*(.fini))

    . = ALIGN(4);

    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP(*(.preinit_array_sysinit .preinit_array_sysinit.*))
    KEEP(*(.preinit_array_platform .preinit_array_platform.*))
    KEEP(*(.preinit_array .preinit_array.*))
    PROVIDE_HIDDEN (__preinit_array_end = .);

    . = ALIGN(4);

    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);

    . = ALIGN(4);

    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);

    *(vtable)

    *(.iram1 .iram1.*)

  } > TEXT


  .flash.appdesc : ALIGN(0x10)
  {
    _rodata_start = ABSOLUTE(.);

    *(.rodata_desc .rodata_desc.*)               /* Should be the first.  App version info.        DO NOT PUT ANYTHING BEFORE IT! */
    *(.rodata_custom_desc .rodata_custom_desc.*) /* Should be the second. Custom app version info. DO NOT PUT ANYTHING BEFORE IT! */

    /* Create an empty gap within this section. Thanks to this, the end of this
     * section will match .flah.rodata's begin address. Thus, both sections
     * will be merged when creating the final bin image. */
    . = ALIGN(ALIGNOF(.flash.rodata));
  } > TEXT

  .flash.rodata : ALIGN(0x10)
  {
    _flash_rodata_start = ABSOLUTE(.);

    *(.rodata_wlog_error .rodata_wlog_error.*)
    *(.rodata_wlog_info .rodata_wlog_info.*)
    *(.rodata_wlog_warning .rodata_wlog_warning.*)

    *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata .srodata.*)


    *(.irom1.text) /* catch stray ICACHE_RODATA_ATTR */
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    __XT_EXCEPTION_TABLE_ = ABSOLUTE(.);
    *(.xt_except_table)
    *(.gcc_except_table .gcc_except_table.*)
    *(.gnu.linkonce.e.*)
    *(.gnu.version_r)
    . = (. + 3) & ~ 3;
    __eh_frame = ABSOLUTE(.);
    KEEP(*(.eh_frame))
    . = (. + 7) & ~ 3;

    /*  C++ constructor and destructor tables, properly ordered:  */
    __init_array_start = ABSOLUTE(.);
    KEEP (*crtbegin.*(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.*) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))

	 SORT(CONSTRUCTORS)

    __init_array_end = ABSOLUTE(.);
    KEEP (*crtbegin.*(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.*) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))

    /*  C++ exception handlers table:  */
    __XT_EXCEPTION_DESCS_ = ABSOLUTE(.);
    *(.xt_except_desc)
    *(.gnu.linkonce.h.*)
    __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
    *(.xt_except_desc_end)
    *(.dynamic)
    *(.gnu.version_d)
    /* Addresses of memory regions reserved via
       SOC_RESERVE_MEMORY_REGION() */
    soc_reserved_memory_region_start = ABSOLUTE(.);
    KEEP (*(.reserved_memory_address))
    soc_reserved_memory_region_end = ABSOLUTE(.);
    _rodata_end = ABSOLUTE(.);
    /* Literals are also RO data. */
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(*.literal)
    *(.literal.*)
    *(.text.startup.literal)
    
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);

  } > TEXT

  _flash_rodata_align = ALIGNOF(.flash.rodata);

  .flash.rodata_noload (NOLOAD) :
  {
    . = ALIGN (4);
    *(.rodata_wlog_debug .rodata_wlog_debug.*)
    *(.rodata_wlog_verbose .rodata_wlog_verbose.*)
  } > TEXT

  .flash.text :
  {
    _stext = .;
    _text_start = ABSOLUTE(.);

    *(.stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *(.irom0.text) /* catch stray ICACHE_RODATA_ATTR */

    *(.text .text.* )
    *(.text.unlikely .text.*_unlikely .text.unlikely.*)
    *(.text.exit .text.exit.*)
    *(.text.startup .text.startup.*)
    *(.text.hot .text.hot.*)
    *(SORT(.text.sorted.*))

    *(.fini.literal)
    *(.fini)
    *(.gnu.version)

    /** CPU will try to prefetch up to 16 bytes of
      * of instructions. This means that any configuration (e.g. MMU, PMS) must allow
      * safe access to up to 16 bytes after the last real instruction, add
      * dummy bytes to ensure this
      */
    . += 16;

    . = ALIGN (16);
    _text_end = ABSOLUTE(.);
    _etext = .;
    __etext = .;

    /* Similar to _iram_start, this symbol goes here so it is
       resolved by addr2line in preference to the first symbol in
       the flash.text segment.
    */
    _flash_cache_start = ABSOLUTE(0);

  } > TEXT


  .data : ALIGN(4)
  {
    _data_start = ABSOLUTE(.);

    *(.data_begin .data_begin.*)

    *(.gnu.linkonce.d.*)
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.sdata2)
    *(.sdata2.*)
    *(.gnu.linkonce.s2.*)
    *(.jcr)

    /* coredump mapping */
    _coredump_dram_start = ABSOLUTE(.);
    *(.dram1.coredump .dram1.coredump.*)
    _coredump_dram_end = ABSOLUTE(.);

    /* should be placed after coredump mapping */
    _esp_system_init_fn_array_start = ABSOLUTE(.);
    KEEP (*(SORT(.esp_system_init_fn) SORT(.esp_system_init_fn.*)))
    _esp_system_init_fn_array_end = ABSOLUTE(.);

    *(.data .data.*)
    *(.dram1 .dram1.*)

    . = ALIGN(4);
    _thread_local_start = ABSOLUTE(.);
    *(.tdata .tdata.* .gnu.linkonce.td.*)
    *(.tbss)
    *(.tbss.*)
    _thread_local_end = ABSOLUTE(.);
    . = ALIGN(4);

    *(.data_end .data_end.*)

    _data_end = ABSOLUTE(.);
    . = ALIGN(4);
  } > RAM

  .bss (NOLOAD) : ALIGN(4)
  {
    . = ALIGN (8);
    _bss_start = ABSOLUTE(.);

    *(.bss_begin .bss_begin.*)
    *(.bss .bss.*)
    *(.dynbss .dynsbss .gnu.linkonce.b .gnu.linkonce.b.* .gnu.linkonce.sb .gnu.linkonce.sb.* .gnu.linkonce.sb2 .gnu.linkonce.sb2.* .sbss .sbss.* .sbss2 .sbss2.* .scommon .share.mem)
    *(.ext_ram.bss .ext_ram.bss.*)
    *(COMMON)
    *(.bss_end .bss_end.*)

    . = ALIGN (8);
    _bss_end = ABSOLUTE(.);
  } > RAM

  .noinit (NOLOAD) : ALIGN(4)
  {
    . = ALIGN(4);
    _noinit = .;
    _noinit_start = ABSOLUTE(.);
    *(.noinit .noinit.*)
    . = ALIGN(4) ;
    _noinit_end = ABSOLUTE(.);
    _end_noinit = .;
  } > RAM

  PROVIDE ( end = _end_noinit ); /* was _ebss */
  PROVIDE ( _end = _end_noinit );
  PROVIDE ( __end = _end_noinit );
  PROVIDE ( __end__ = _end_noinit );

}
