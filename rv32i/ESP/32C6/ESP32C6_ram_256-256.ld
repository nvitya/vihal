MEMORY
{
 ITCRAM (rwx) :  ORIGIN = 0x40800000, LENGTH = 256k
 DTCRAM (xrw) :  ORIGIN = 0x40840000, LENGTH = 256k
}

REGION_ALIAS("RAM",  DTCRAM);
REGION_ALIAS("RAM2", DTCRAM);

REGION_ALIAS("TEXT", ITCRAM);

INCLUDE rv32i_default_noflash.ld

