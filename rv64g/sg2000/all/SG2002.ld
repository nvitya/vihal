MEMORY
{
  CODERAM (xrw) :  ORIGIN = 0x8fe00000, LENGTH = 0x080000
  RAM (xrw)     :  ORIGIN = 0x8fe80000, LENGTH = 0x100000
  RAM2(xrw)     :  ORIGIN = 0x8ff80000, LENGTH = 0x080000  /* shared memory */
}

REGION_ALIAS("TEXT",   CODERAM);
REGION_ALIAS("ITCRAM", CODERAM);
REGION_ALIAS("DTCRAM", RAM);

INCLUDE rv64g_default_noflash.ld

