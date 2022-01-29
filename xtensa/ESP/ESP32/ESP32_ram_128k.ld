MEMORY
{
 ITCRAM (rwx) :  ORIGIN = 0x40080400, LENGTH = 127k  /* the first 1k is reserved for the interrupt vectors ? */
 DTCRAM (xrw) :  ORIGIN = 0x3FFF0000, LENGTH = 64K
 /* DTCRAM (xrw) :  ORIGIN = 0x3FFB0000, LENGTH = 328K */
}

REGION_ALIAS("RAM",  DTCRAM);
REGION_ALIAS("RAM2", DTCRAM);

REGION_ALIAS("TEXT", ITCRAM);

INCLUDE xtensa_default_noflash.ld

