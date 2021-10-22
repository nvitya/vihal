/* memory definition required, like

  MEMORY
  {
    SDRAM (rwx) :    ORIGIN = 0x00000000, LENGTH = 1024K

    RAM (xrw)  :    ORIGIN = 0x10000000, LENGTH = 128K
    ITCRAM (rwx) :  ORIGIN = 0x20000000, LENGTH = 128K
    DTCRAM (xrw) :  ORIGIN = 0x30000000, LENGTH = 128K
    RAM2 (xrw) :    ORIGIN = 0x40000000, LENGTH = 128K
  }

if some regions missing they must be presented as alias like:

  REGION_ALIAS("ITCRAM", RAM);

  REGION_ALIAS("TEXT", SDRAM);

*/

OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")
OUTPUT_ARCH(riscv)

INCLUDE vihal_stack_predef.ld

ENTRY(_self_flashing_entry)

SECTIONS
{
  .app_header : ALIGN(4)  /* application header (32 byte) for the bootloader, might not exist */
  {
    KEEP(*(.application_header))
  } > TEXT

  .resetentry : ALIGN(4)
  {
    KEEP(*(.resetentry))
    KEEP(*(.startup))
  } > TEXT

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

  __global_pointer$ = ADDR(.data) + 0x800;

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

  INCLUDE vihal_stack_heap.ld
  INCLUDE vihal_debug_sections.ld
}
