MEMORY
{
 /* the internal 384k SRAM1 is accessible at 0x40380000 for I-BUS and at 0x3FC80000 for D-BUS
   Allocation of the 6 x 64k blocks:
   ...8 I-RAM
	...9 I-RAM
	...A I-RAM
	...B I-RAM
	...C D-RAM
	...D D-RAM
 */
 ITCRAM (rwx) :  ORIGIN = 0x40390000, LENGTH =  64k
 DTCRAM (xrw) :  ORIGIN = 0x3FCC0000, LENGTH =  64k
}

REGION_ALIAS("RAM",  DTCRAM);
REGION_ALIAS("RAM2", DTCRAM);

REGION_ALIAS("TEXT", ITCRAM);

INCLUDE rv32i_default_noflash.ld

