
MEMORY
{
  FLASH (rx) :    ORIGIN = 0x08000000, LENGTH = 1024K

  ITCRAM (rwx) :  ORIGIN = 0x00000000, LENGTH = 64K
  DTCRAM (xrw) :  ORIGIN = 0x20000000, LENGTH = 128K
  RAM2 (xrw) :    ORIGIN = 0x24000000, LENGTH = 320K
  RAM3 (xrw) :    ORIGIN = 0x30000000, LENGTH = 32K
}

REGION_ALIAS("TEXT", FLASH);
/* REGION_ALIAS("RAM", DTCRAM); */ /* much faster than the AXI ram, but no DMA access */
REGION_ALIAS("RAM", RAM2); /* for DMA access */

INCLUDE armm_default_flash.ld

