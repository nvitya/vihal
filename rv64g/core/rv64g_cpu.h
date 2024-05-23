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
 *  file:     rv64g_cpu.h
 *  brief:    RV64G CPU related definitions
 *  version:  1.00
 *  date:     2022-02-06
 *  authors:  nvitya
*/

#ifndef _RV64G_CPU_H
#define _RV64G_CPU_H

#include "stdint.h"

inline uint64_t __attribute__ ((always_inline)) cpu_csr_read(const int csr_id)
{
  register uint64_t csr_data;

  asm volatile ("csrr %[result], %[input_i]" : [result] "=r" (csr_data) : [input_i] "i" (csr_id));

  return csr_data;
}

inline void __attribute__ ((always_inline)) cpu_csr_write(const int csr_id, uint64_t data)
{
  register uint64_t csr_data = data;

  asm volatile ("csrw %[input_i], %[input_j]" :  : [input_i] "i" (csr_id), [input_j] "r" (csr_data));
}

inline void __attribute__ ((always_inline)) cpu_csr_setbits(const int csr_id, uint64_t abits)
{
  register uint64_t __tmp;
  register uint64_t csr_data = abits;
  asm volatile ("csrrs %[result], %[input_i], %[input_j]" : [result] "=r" (__tmp) : [input_i] "i" (csr_id), [input_j] "r" (csr_data));
}

inline void __attribute__ ((always_inline)) cpu_csr_clrbits(const int csr_id, uint32_t abits)
{
  register uint64_t __tmp;
  register uint64_t csr_data = abits;
  asm volatile ("csrrc %[result], %[input_i], %[input_j]" : [result] "=r" (__tmp) : [input_i] "i" (csr_id), [input_j] "r" (csr_data));
}

// atomic extension

inline uint32_t __attribute__ ((always_inline)) cpu_atomic_swap32(void * aaddr, uint32_t abits)
{
  register uint64_t __res;
  asm volatile ("amoswap.w %[result], %[input_j], (%[input_i])" : [result] "=r" (__res) : [input_i] "r" (aaddr), [input_j] "r" (abits));
  return __res;
}

inline uint32_t __attribute__ ((always_inline)) cpu_atomic_add32(void * aaddr, uint32_t abits)
{
  register uint64_t __res;
  asm volatile ("amoadd.w %[result], %[input_j], (%[input_i])" : [result] "=r" (__res) : [input_i] "r" (aaddr), [input_j] "r" (abits));
  return __res;
}

inline uint32_t __attribute__ ((always_inline)) cpu_atomic_or32(void * aaddr, uint32_t abits)
{
  register uint64_t __res;
  asm volatile ("amoor.w %[result], %[input_j], (%[input_i])" : [result] "=r" (__res) : [input_i] "r" (aaddr), [input_j] "r" (abits));
  return __res;
}

inline uint32_t __attribute__ ((always_inline)) cpu_atomic_and32(void * aaddr, uint32_t abits)
{
  register uint64_t __res;
  asm volatile ("amoand.w %[result], %[input_j], (%[input_i])" : [result] "=r" (__res) : [input_i] "r" (aaddr), [input_j] "r" (abits));
  return __res;
}

inline uint32_t __attribute__ ((always_inline)) cpu_atomic_xor32(void * aaddr, uint32_t abits)
{
  register uint64_t __res;
  asm volatile ("amoxor.w %[result], %[input_j], (%[input_i])" : [result] "=r" (__res) : [input_i] "r" (aaddr), [input_j] "r" (abits));
  return __res;
}

// 64-bit versions

inline uint64_t __attribute__ ((always_inline)) cpu_atomic_swap64(void * aaddr, uint64_t abits)
{
  register uint64_t __res;
  asm volatile ("amoswap.d %[result], %[input_j], (%[input_i])" : [result] "=r" (__res) : [input_i] "r" (aaddr), [input_j] "r" (abits));
  return __res;
}

inline uint64_t __attribute__ ((always_inline)) cpu_atomic_add64(void * aaddr, uint64_t abits)
{
  register uint64_t __res;
  asm volatile ("amoadd.d %[result], %[input_j], (%[input_i])" : [result] "=r" (__res) : [input_i] "r" (aaddr), [input_j] "r" (abits));
  return __res;
}

inline uint64_t __attribute__ ((always_inline)) cpu_atomic_or64(void * aaddr, uint64_t abits)
{
  register uint64_t __res;
  asm volatile ("amoor.d %[result], %[input_j], (%[input_i])" : [result] "=r" (__res) : [input_i] "r" (aaddr), [input_j] "r" (abits));
  return __res;
}

inline uint64_t __attribute__ ((always_inline)) cpu_atomic_and64(void * aaddr, uint64_t abits)
{
  register uint64_t __res;
  asm volatile ("amoand.d %[result], %[input_j], (%[input_i])" : [result] "=r" (__res) : [input_i] "r" (aaddr), [input_j] "r" (abits));
  return __res;
}

inline uint64_t __attribute__ ((always_inline)) cpu_atomic_xor64(void * aaddr, uint64_t abits)
{
  register uint64_t __res;
  asm volatile ("amoxor.d %[result], %[input_j], (%[input_i])" : [result] "=r" (__res) : [input_i] "r" (aaddr), [input_j] "r" (abits));
  return __res;
}

inline void __attribute__ ((always_inline)) __ISB(void)
{
  //asm volatile ("isb 0xF":::"memory");
}

inline void __attribute__ ((always_inline)) __DSB(void)
{
  //asm volatile ("dsb 0xF":::"memory");
}

// Cache handling extensions (e.g. XuanTie C906)

inline void __attribute__ ((always_inline)) cpu_dcache_cpa(intptr_t addr)
{
  asm volatile ("dcache.cpa %[input_i]" : : [input_i] "r" (addr));
}

inline void __attribute__ ((always_inline)) cpu_dcache_ipa(intptr_t addr)
{
  asm volatile ("dcache.ipa %[input_i]" : : [input_i] "r" (addr));
}


#endif // _RV64G_CPU_H
