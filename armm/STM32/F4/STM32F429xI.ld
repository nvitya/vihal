MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 2048K
  DTCRAM (xrw) :  ORIGIN = 0x10000000, LENGTH = 64K
  RAM (xrw) :  ORIGIN = 0x20000000, LENGTH = 192K
}

REGION_ALIAS("TEXT", FLASH);
REGION_ALIAS("ITCRAM", RAM);
REGION_ALIAS("RAM2", RAM);

INCLUDE armm_default_flash.ld

