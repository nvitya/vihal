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
// file:     rv64g_generic.h
// brief:    RV64G Generic definitions
// created:  2022-02-01
// authors:  nvitya

#ifndef _RV64G_GENERIC_H_
#define _RV64G_GENERIC_H_

#define MCU_RV64G

#define MCU_RV32I
#define CPU_RV32I

#define MSTATUS_MIE         0x00000008

#define CSR_MSTATUS         0x300
#define CSR_MIE             0x304
#define CSR_MTVEC           0x305
#define CSR_MTVT            0x307
#define CSR_MEPC            0x341
#define CSR_MCAUSE          0x342
#define CSR_MIP             0x344

#define CSR_MCYCLE          0xB00

// read mtime:
#define CLOCKCNT  (cpu_csr_read(0xB00))  // = 0xB00 = mcycle

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

//extern "C" void (* __isr_vectors [])();

inline void __attribute__((always_inline)) mcu_init_vector_table()
{
}

inline void __attribute__((always_inline)) mcu_enable_fpu()
{
}

inline unsigned __get_PRIMASK()
{
  return 0;
}

inline void __set_PRIMASK(unsigned aprimask)
{

}

#endif // _RV64G_GENERIC_H_
