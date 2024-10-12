MEMORY
{
  ITCRAM (xrw)   :  ORIGIN = 0x20000000, LENGTH = 256K  /* striped SRAM0-SRAM3 */
  RAM (xrw)      :  ORIGIN = 0x20040000, LENGTH = 256K  /* striped SRAM4-SRAM7 */
  STACK1 (xrw)   :  ORIGIN = 0x20080000, LENGTH = 4K    /* SRAM8 */

  BLSTAGE2 (xrw) :  ORIGIN = 0x20081000, LENGTH = 256   /* SRAM9 */
}

REGION_ALIAS("TEXT",    ITCRAM);
REGION_ALIAS("DTCRAM",  RAM);
REGION_ALIAS("RAM2",    RAM);

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

__stack = ORIGIN(STACK1) + LENGTH(STACK1);
__Main_Stack_Size = 4096;

PROVIDE ( _Heap_Begin = _end_noinit ) ;
PROVIDE ( _Heap_Limit = ORIGIN(RAM)+LENGTH(RAM) ) ;

ENTRY(soft_entry)

SECTIONS
{
  .app_header : ALIGN(4)  /* application header (32 byte) for the bootloader, might not exist */
  {
    KEEP(*(.application_header))
  } > TEXT

  .isr_vector : ALIGN(4)  /* internally 1024 aligned, so better be the first */
  {
    FILL(0xFF)
    __vectors_start = ABSOLUTE(.) ;
    KEEP(*(.isr_vector))
    __vectors_end = ABSOLUTE(.) ;

  } > TEXT

  .resetentry : ALIGN(4)
  {
    KEEP(*(.resetentry))
    KEEP(*(.startup))
  } > TEXT


  .bl_second_stage : ALIGN(16)
  {
    FILL(0xFF)
    KEEP(*(.bl_second_stage_start))
    KEEP(*(.bl_second_stage))
    FILL(0xFF)
	. = ALIGN(256);
  } > BLSTAGE2 AT > TEXT

  PROVIDE( __bl_stage2_src = LOADADDR(.bl_second_stage) );
  PROVIDE( __bl_stage2_len = SIZEOF(.bl_second_stage) );

  .inits : ALIGN(4)
  {
    /* DATA COPY REGIONS */

    __data_regions_array_start = .;

    LONG(LOADADDR(.data));         LONG(ADDR(.data));         LONG(ADDR(.data)+SIZEOF(.data));
    LONG(LOADADDR(.data_DTCRAM));  LONG(ADDR(.data_DTCRAM));  LONG(ADDR(.data_DTCRAM)+SIZEOF(.data_DTCRAM));
    LONG(LOADADDR(.data_ITCRAM));  LONG(ADDR(.data_ITCRAM));  LONG(ADDR(.data_ITCRAM)+SIZEOF(.data_ITCRAM));
    LONG(LOADADDR(.data_RAM2));    LONG(ADDR(.data_RAM2));    LONG(ADDR(.data_RAM2)+SIZEOF(.data_RAM2));

    LONG(LOADADDR(.text_ITCRAM));  LONG(ADDR(.text_ITCRAM));  LONG(ADDR(.text_ITCRAM)+SIZEOF(.text_ITCRAM));
    LONG(LOADADDR(.text_DTCRAM));  LONG(ADDR(.text_DTCRAM));  LONG(ADDR(.text_DTCRAM)+SIZEOF(.text_DTCRAM));
    LONG(LOADADDR(.text_RAM2));    LONG(ADDR(.text_RAM2));    LONG(ADDR(.text_RAM2)+SIZEOF(.text_RAM2));

    __data_regions_array_end = .;

    /* DATA CLEAR REGIONS */

    __bss_regions_array_start = .;

    LONG(ADDR(.bss));         LONG(ADDR(.bss)+SIZEOF(.bss));
    LONG(ADDR(.bss_DTCRAM));  LONG(ADDR(.bss_DTCRAM)+SIZEOF(.bss_DTCRAM));
    LONG(ADDR(.bss_ITCRAM));  LONG(ADDR(.bss_ITCRAM)+SIZEOF(.bss_ITCRAM));
    LONG(ADDR(.bss_RAM2));    LONG(ADDR(.bss_RAM2)+SIZEOF(.bss_RAM2));

    __bss_regions_array_end = .;

  } > TEXT

  .text_ITCRAM  : ALIGN(4)  { *(.text_ITCRAM) } > ITCRAM  AT > TEXT
  .text_DTCRAM  : ALIGN(4)  { *(.text_DTCRAM) } > DTCRAM  AT > TEXT
  .text_RAM2    : ALIGN(4)  { *(.text_RAM2) }   > RAM2    AT > TEXT
  .text : ALIGN(4)
  {
    INCLUDE vihal_text_section.ld

  } > TEXT

  /* .ARM.exidx is sorted, so has to go in its own output section.  */
  PROVIDE_HIDDEN (__exidx_start = .);
  .ARM.exidx :
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } > TEXT
  PROVIDE_HIDDEN (__exidx_end = .);

  . = ALIGN(4);
  _etext = .;
  __etext = .;

  .data_DTCRAM  : ALIGN(4)  { *(.data_DTCRAM) } > DTCRAM  AT > TEXT
  .data_ITCRAM  : ALIGN(4)  { *(.data_ITCRAM) } > ITCRAM  AT > TEXT
  .data_RAM2    : ALIGN(4)  { *(.data_RAM2) }   > RAM2    AT > TEXT
  .data : ALIGN(4)
  {
    INCLUDE vihal_data_section.ld

    . = ALIGN(4);

  } > RAM AT > TEXT

  __app_image_end = LOADADDR(.data) + SIZEOF(.data);

  .bss_DTCRAM (NOLOAD) : ALIGN(4) { *(.bss_DTCRAM) } > DTCRAM
  .bss_ITCRAM (NOLOAD) : ALIGN(4) { *(.bss_ITCRAM) } > ITCRAM
  .bss_RAM2   (NOLOAD) : ALIGN(4) { *(.bss_RAM2) }   > RAM2
  .bss (NOLOAD) : ALIGN(4)
  {
    INCLUDE vihal_bss_section.ld

  } > RAM

  .noinit_DTCRAM (NOLOAD) : ALIGN(4)  { *(.noinit_DTCRAM) } > DTCRAM
  .noinit_ITCRAM (NOLOAD) : ALIGN(4)  { *(.noinit_ITCRAM) } > ITCRAM
  .noinit_RAM2   (NOLOAD) : ALIGN(4)  { *(.noinit_RAM2) }   > RAM2
  .noinit (NOLOAD) : ALIGN(4)
  {
    _noinit = .;
    *(.noinit .noinit.*)
    . = ALIGN(4) ;
    _end_noinit = .;
  } > RAM

  PROVIDE ( end = _end_noinit ); /* was _ebss */
  PROVIDE ( _end = _end_noinit );
  PROVIDE ( __end = _end_noinit );
  PROVIDE ( __end__ = _end_noinit );

  INCLUDE vihal_debug_sections.ld
}
