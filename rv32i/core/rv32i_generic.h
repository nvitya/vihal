/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2021 Viktor Nagy, nvitya
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 * --------------------------------------------------------------------------- */
// file:     rv32i_generic.h
// brief:    RV32I Generic definitions
// created:  2021-10-02
// authors:  nvitya

#ifndef __RV32I_GENERIC_H
#define __RV32I_GENERIC_H

#include "stdint.h"
#include "rv32i_cpu.h"

#if !defined(MCU_NO_UNALIGNED)
  #define MCU_NO_UNALIGNED  1
#endif


extern "C" void mcu_interrupt_controller_init();

inline void __attribute__((always_inline)) mcu_interrupts_disable()
{
  cpu_csr_clrbits(CSR_MSTATUS, MSTATUS_MIE);  // clear machine interrupt enable bit
}

inline unsigned __attribute__((always_inline)) mcu_interrupts_save_and_disable()
{
  unsigned prevstate = cpu_csr_read(CSR_MSTATUS);
  cpu_csr_clrbits(CSR_MSTATUS, MSTATUS_MIE);  // clear machine interrupt enable bit
  return prevstate;
}

inline void __attribute__((always_inline)) mcu_interrupts_enable()
{
  cpu_csr_setbits(CSR_MSTATUS, MSTATUS_MIE);  // set machine interrupt enable bit
}

inline void __attribute__((always_inline)) mcu_interrupts_restore(unsigned prevstate)
{
  cpu_csr_write(CSR_MSTATUS, prevstate);
}

// for cortex-m compatibility
inline unsigned __get_PRIMASK()
{
  return cpu_csr_read(CSR_MSTATUS);
}

inline void __set_PRIMASK(unsigned aprimask)
{
  cpu_csr_write(CSR_MSTATUS, aprimask);
}

#if 0 // these unified IRQ handling functions are in platform specific mcu_defs.h

#warning "implement IRQ handling here!"

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


inline void __attribute__((always_inline)) mcu_init_vector_table()
{
  // this is more complex and cpu specific on the RISC-V
}

inline void __attribute__((always_inline)) mcu_enable_fpu()
{
}

inline void __attribute__((always_inline)) mcu_enable_icache()
{
}

inline void __attribute__((always_inline)) mcu_enable_dcache()
{
}

inline void __attribute__((always_inline)) mcu_disable_icache()
{
}

inline void __attribute__((always_inline)) mcu_disable_dcache()
{
}


#endif
