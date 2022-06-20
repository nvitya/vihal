/*
 *  file:     mcu_defs.h (ESP)
 *  brief:    ESP Risc-V MCU Family definitions
 *  version:  1.00
 *  date:     2022-02-03
 *  authors:  nvitya
*/

#pragma once

#include "rv32i_cpu.h"

#define MAX_CLOCK_SPEED         160000000

#define MCU_INTERNAL_RC_SPEED     1750000 // ring oscillator

#define HW_DMA_MAX_COUNT            32764

#define CLOCKCNT  (cpu_csr_read(0x7E2))  // use the mpccr instead of the mtime

inline void __attribute__((always_inline)) mcu_preinit_code()
{
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
