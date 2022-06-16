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
/*
 *  file:     rv32i_cpu.h
 *  brief:    RV32I CPU related definitions
 *  version:  1.00
 *  date:     2022-02-06
 *  authors:  nvitya
*/

#pragma once

#include "stdint.h"

#define MCU_RV32I
#define CPU_RV32I

#define MSTATUS_MIE         0x00000008

#define CSR_MSTATUS         0x300
#define CSR_MIE             0x304
#define CSR_MTVEC           0x305
#define CSR_MTVT            0x307
#define CSR_MEPC            0x341
#define CSR_MCAUSE          0x342

inline uint32_t __attribute__ ((always_inline)) cpu_csr_read(const int csr_id)
{
  register uint32_t csr_data;

  asm volatile ("csrr %[result], %[input_i]" : [result] "=r" (csr_data) : [input_i] "i" (csr_id));

  return csr_data;
}

inline void __attribute__ ((always_inline)) cpu_csr_write(const int csr_id, uint32_t data)
{
  register uint32_t csr_data = data;

  asm volatile ("csrw %[input_i], %[input_j]" :  : [input_i] "i" (csr_id), [input_j] "r" (csr_data));
}

inline void __attribute__ ((always_inline)) cpu_csr_setbits(const int csr_id, uint32_t abits)
{
  register unsigned long __tmp;
  register uint32_t csr_data = abits;
  asm volatile ("csrrs %[result], %[input_i], %[input_j]" : [result] "=r" (__tmp) : [input_i] "i" (csr_id), [input_j] "r" (csr_data));
}

inline void __attribute__ ((always_inline)) cpu_csr_clrbits(const int csr_id, uint32_t abits)
{
  register unsigned long __tmp;
  register uint32_t csr_data = abits;
  asm volatile ("csrrc %[result], %[input_i], %[input_j]" : [result] "=r" (__tmp) : [input_i] "i" (csr_id), [input_j] "r" (csr_data));
}

