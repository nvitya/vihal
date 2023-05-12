/* The WIO_Terminal has a 16 kByte bootloader which can not be overwritten (locked)
   The normal application must be loaded right after the bootloader, at 0x4000
   
   The source code of the bootloader is here: https://github.com/Seeed-Studio/uf2-samdx1.git
*/

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00004000, LENGTH = 512K - 16k  
  RAM (xrw) :  ORIGIN = 0x20000000, LENGTH = 192K
}

REGION_ALIAS("TEXT", FLASH);
REGION_ALIAS("ITCRAM", RAM);
REGION_ALIAS("DTCRAM", RAM);
REGION_ALIAS("RAM2", RAM);

INCLUDE armm_default_flash.ld
