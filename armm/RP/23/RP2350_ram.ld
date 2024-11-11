MEMORY
{
  ITCRAM (xrw) :  ORIGIN = 0x20000000, LENGTH = 256K   /* striped region 1 */
  RAM (xrw)    :  ORIGIN = 0x20040000, LENGTH = 256K   /* striped region 2 */
}

REGION_ALIAS("TEXT", ITCRAM);
REGION_ALIAS("DTCRAM",  RAM);
REGION_ALIAS("RAM2",    RAM);

INCLUDE armm_default_noflash.ld

