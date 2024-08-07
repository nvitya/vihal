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

OUTPUT_FORMAT("elf64-littleaarch64", "elf64-bigaarch64", "elf64-littleaarch64")
OUTPUT_ARCH(aarch64)

INCLUDE vihal_stack_predef.ld

ENTRY(_soft_entry)

SECTIONS
{
  .app_header : ALIGN(8)  /* application header (32 byte) for the bootloader, might not exist */
  {
    KEEP(*(.application_header))
  } > TEXT

  .resetentry : ALIGN(8)
  {
    KEEP(*(.resetentry))
    KEEP(*(.startup))
  } > TEXT

  .inits : ALIGN(8)
  {
    /* DATA COPY REGIONS */

    __data_regions_array_start = .;

    QUAD(LOADADDR(.data));         QUAD(ADDR(.data));         QUAD(ADDR(.data)+SIZEOF(.data));
    QUAD(LOADADDR(.data_DTCRAM));  QUAD(ADDR(.data_DTCRAM));  QUAD(ADDR(.data_DTCRAM)+SIZEOF(.data_DTCRAM));
    QUAD(LOADADDR(.data_ITCRAM));  QUAD(ADDR(.data_ITCRAM));  QUAD(ADDR(.data_ITCRAM)+SIZEOF(.data_ITCRAM));
    QUAD(LOADADDR(.data_RAM2));    QUAD(ADDR(.data_RAM2));    QUAD(ADDR(.data_RAM2)+SIZEOF(.data_RAM2));

    QUAD(LOADADDR(.text_ITCRAM));  QUAD(ADDR(.text_ITCRAM));  QUAD(ADDR(.text_ITCRAM)+SIZEOF(.text_ITCRAM));
    QUAD(LOADADDR(.text_DTCRAM));  QUAD(ADDR(.text_DTCRAM));  QUAD(ADDR(.text_DTCRAM)+SIZEOF(.text_DTCRAM));
    QUAD(LOADADDR(.text_RAM2));    QUAD(ADDR(.text_RAM2));    QUAD(ADDR(.text_RAM2)+SIZEOF(.text_RAM2));

    __data_regions_array_end = .;

    /* DATA CLEAR REGIONS */

    __bss_regions_array_start = .;

    QUAD(ADDR(.bss));         QUAD(ADDR(.bss)+SIZEOF(.bss));
    QUAD(ADDR(.bss_DTCRAM));  QUAD(ADDR(.bss_DTCRAM)+SIZEOF(.bss_DTCRAM));
    QUAD(ADDR(.bss_ITCRAM));  QUAD(ADDR(.bss_ITCRAM)+SIZEOF(.bss_ITCRAM));
    QUAD(ADDR(.bss_RAM2));    QUAD(ADDR(.bss_RAM2)+SIZEOF(.bss_RAM2));

    __bss_regions_array_end = .;

  } > TEXT

  .text_ITCRAM  : ALIGN(8)  { *(.text_ITCRAM) } > ITCRAM  AT > TEXT
  .text_DTCRAM  : ALIGN(8)  { *(.text_DTCRAM) } > DTCRAM  AT > TEXT
  .text_RAM2    : ALIGN(8)  { *(.text_RAM2) }   > RAM2    AT > TEXT
  .text : ALIGN(8)
  {
    INCLUDE vihal_text_section.ld

  } > TEXT

  /* Resource table for remote processors */
  .resource_table :
  {
    KEEP(*(.resource_table));
  } > TEXT

  . = ALIGN(8);
  _etext = .;
  __etext = .;

  .data_DTCRAM  : ALIGN(8)  { *(.data_DTCRAM) } > DTCRAM  AT > TEXT
  .data_ITCRAM  : ALIGN(8)  { *(.data_ITCRAM) } > ITCRAM  AT > TEXT
  .data_RAM2    : ALIGN(8)  { *(.data_RAM2) }   > RAM2    AT > TEXT
  .data : ALIGN(8)
  {
    INCLUDE vihal_data_section.ld
    . = ALIGN(8);
  } > RAM AT > TEXT

  __app_image_end = LOADADDR(.data) + SIZEOF(.data);

  __global_pointer$ = ADDR(.data) + 0x800;

  .bss_DTCRAM (NOLOAD) : ALIGN(8) { *(.bss_DTCRAM) } > DTCRAM
  .bss_ITCRAM (NOLOAD) : ALIGN(8) { *(.bss_ITCRAM) } > ITCRAM
  .bss_RAM2   (NOLOAD) : ALIGN(8) { *(.bss_RAM2) }   > RAM2
  .bss (NOLOAD) : ALIGN(8)
  {
    INCLUDE vihal_bss_section.ld

  } > RAM

  .noinit_DTCRAM (NOLOAD) : ALIGN(8)  { *(.noinit_DTCRAM) } > DTCRAM
  .noinit_ITCRAM (NOLOAD) : ALIGN(8)  { *(.noinit_ITCRAM) } > ITCRAM
  .noinit_RAM2   (NOLOAD) : ALIGN(8)  { *(.noinit_RAM2) }   > RAM2
  .noinit (NOLOAD) : ALIGN(8)
  {
    _noinit = .;
    *(.noinit .noinit.*)
    . = ALIGN(8) ;
    _end_noinit = .;
  } > RAM

  INCLUDE vihal_stack_heap.ld
  INCLUDE vihal_debug_sections.ld
}
