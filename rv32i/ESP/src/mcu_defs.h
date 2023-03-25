/*
 *  file:     mcu_defs.h (ESP)
 *  brief:    ESP Risc-V MCU Family definitions
 *  version:  1.00
 *  date:     2022-02-03
 *  authors:  nvitya
*/

#ifndef _MCU_DEFS_H
#define _MCU_DEFS_H

#include "rv32i_cpu.h"

#define MAX_CLOCK_SPEED         160000000

#define MCU_INTERNAL_RC_SPEED     1750000 // ring oscillator

#define HW_DMA_MAX_COUNT             4092

#define CLOCKCNT  (cpu_csr_read(0x7E2))  // use the mpccr instead of the mtime

inline void __attribute__((always_inline)) mcu_preinit_code()
{
  // disable watchdogs

  /* OpenOCD code:

   # Timer Group 0 & 1 WDTs
   mww 0x6001f064 0x50D83AA1
   mww 0x6001F048 0
   mww 0x60020064 0x50D83AA1
   mww 0x60020048 0

   # RTC WDT
   mww 0x600080a8 0x50D83AA1
   mww 0x60008090 0

   # SWD
   mww 0x600080b0 0x8F1D312A
   mww 0x600080ac 0x84B00000
 */

  TIMG0->WDTWPROTECT = 0x50D83AA1;
  TIMG0->WDTCONFIG0  = 0;

  TIMG1->WDTWPROTECT = 0x50D83AA1;
  TIMG1->WDTCONFIG0  = 0;

  RTC_CNTL->WDTWPROTECT = 0x50D83AA1;
  RTC_CNTL->WDTCONFIG0  = 0;

  RTC_CNTL->SWD_WPROTECT = 0x8F1D312A;
  RTC_CNTL->SWD_CONF     = 0x84B00000;
}

inline void __attribute__((always_inline)) mcu_postinit_code()
{
}

//#warning "implement IRQ handling here!"

inline void mcu_irq_priority_set(unsigned intnum, unsigned priority)
{

}

inline void mcu_irq_pending_clear(unsigned intnum)
{

}

inline void mcu_irq_pending_set(unsigned intnum)
{

}

inline void mcu_irq_enable(unsigned intnum)
{

}

inline void mcu_irq_disable(unsigned intnum)
{

}

inline bool mcu_irq_enabled(unsigned intnum)
{
  return false;
}

#endif
