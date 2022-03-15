MEMORY
{
  /* for more details see AN5289, Kap 4.2 Memory Mapping */
  /* STM32WBxxxG has 1MB Flash, but the upper 512KB are reserved for the RF Stacks*/
  FLASH (rx) :    ORIGIN = 0x08000000, LENGTH = 512K
  
  /* First 8 byte of RAM1 are reserved for BLE Stack */
  RAM (xrw) :     ORIGIN = 0x20000008, LENGTH = 0x2FFF8
  
  /* Shared ram for communication between M4 and M0, the rest of SRAM2 is reserved for RF Stack */
  RAM_SHARED (xrw) :  ORIGIN = 0x20030000, LENGTH = 10K
}

REGION_ALIAS("TEXT", FLASH);
REGION_ALIAS("ITCRAM", RAM);
REGION_ALIAS("DTCRAM", RAM);
REGION_ALIAS("RAM2", RAM);

INCLUDE armm_default_flash.ld
