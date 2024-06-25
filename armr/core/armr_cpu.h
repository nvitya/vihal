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
 *  file:     armr_cpu.h
 *  brief:    ARM Cortex-R CPU related definitions
 *  created:  2024-06-15
 *  authors:  nvitya
*/

#ifndef _ARMR_CPU_H
#define _ARMR_CPU_H

#include "stdint.h"

inline uint32_t __attribute__((always_inline)) cpu_mr_read_pmccntr()
{
  register uint32_t res;
  asm volatile ("MRC  p15, #0, %[result], c9, c13, 0" : [result] "=r" (res) : );
  return res;
}

inline uint32_t __attribute__((always_inline)) cpu_mr_read_pmcr()
{
  register uint32_t res;
  asm volatile ("MRC  p15, #0, %[result], c9, c12, 0" : [result] "=r" (res) : );
  return res;
}

inline void __attribute__((always_inline)) cpu_mr_write_pmcr(uint32_t avalue)
{
  asm volatile ("MCR  p15, #0, %[avalue], c9, c12, 0" : : [avalue] "r" (avalue) );
}

inline void __attribute__((always_inline)) cpu_mr_write_pmctenset(uint32_t avalue)
{
  asm volatile ("MCR  p15, #0, %[avalue], c9, c12, 1" : : [avalue] "r" (avalue) );
}

inline void __attribute__((always_inline)) cpu_mr_write_pmctenclr(uint32_t avalue)
{
  asm volatile ("MCR  p15, #0, %[avalue], c9, c12, 2" : : [avalue] "r" (avalue) );
}

inline uint32_t __attribute__((always_inline)) cpu_mr_read_pmovsr()
{
  register uint32_t res;
  asm volatile ("MRC  p15, #0, %[result], c9, c12, 3" : [result] "=r" (res) : );
  return res;
}

inline void __attribute__((always_inline)) cpu_mr_write_pmovsr(uint32_t avalue)
{
  asm volatile ("MCR  p15, #0, %[avalue], c9, c12, 3" : : [avalue] "r" (avalue) );
}

inline uint32_t __attribute__((always_inline)) cpu_mr_read_c9_c12(const int aid)
{
  register uint32_t res;
  asm volatile ("MRC  p15, #0, %[result], c9, c12, %[aid]" : [result] "=r" (res) : [aid] "i" (aid) );
  return res;
}

inline void __attribute__((always_inline)) cpu_mr_write_c9_c12(const int aid, uint32_t avalue)
{
  asm volatile ("MCR  p15, #0, %[avalue], c9, c12, %[aid]" : : [aid] "i" (aid), [avalue] "r" (avalue) );
}

inline uint32_t __attribute__((always_inline)) cpu_mr_read_c9_c13(const int aid)
{
  register uint32_t res;
  asm volatile ("MRC  p15, #0, %[result], c9, c13, %[aid]" : [result] "=r" (res) : [aid] "i" (aid) );
  return res;
}

inline void __attribute__((always_inline)) cpu_mr_write_c9_c13(const int aid, uint32_t avalue)
{
  asm volatile ("MCR  p15, #0, %[avalue], c9, c13, %[aid]" : : [aid] "i" (aid), [avalue] "r" (avalue) );
}


#endif // _ARMR_CPU_H
