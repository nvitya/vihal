MEMORY
{
  FLASH (rx) :    ORIGIN = 0x00400000, LENGTH = 512K

  ITCRAM (rwx) :  ORIGIN = 0x00000000, LENGTH = 128K
  DTCRAM (xrw) :  ORIGIN = 0x20000000, LENGTH = 128K
  RAM2 (xrw) :    ORIGIN = 0x20400000, LENGTH = 0K
}

REGION_ALIAS("TEXT", FLASH);
REGION_ALIAS("RAM", DTCRAM);

INCLUDE armm_default_flash.ld
