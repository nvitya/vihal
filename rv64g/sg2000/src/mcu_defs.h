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
 *  file:     mcu_defs.h (sg2000)
 *  brief:    Cvitek/Sophgo special MCU definitions
 *  created:  2024-04-21
 *  authors:  nvitya
*/

#ifndef _MCU_DEFS_H
#define _MCU_DEFS_H

#include "rv64g_cpu.h"

#define MCUF_SG2000
#define HW_DMA_MAX_COUNT             32764

#define MCU_INTERNAL_RC_SPEED      8000000  // not used

#define L1_CACHE_BYTES  64

// this is valid for C906

#define CSR_MHCR      0x7C1
#define CSR_MCOR      0x7C2

inline void __attribute__((always_inline)) mcu_enable_icache()
{
  // invalidate the I-Cache
  cpu_csr_clrbits(CSR_MCOR, 0x33);
  cpu_csr_setbits(CSR_MCOR, 0x11);
  // Enable the I-Cache
  cpu_csr_setbits(CSR_MHCR, 0x01);
}

inline void __attribute__((always_inline)) mcu_enable_dcache()
{
  // invalidate the D-Cache
  cpu_csr_clrbits(CSR_MCOR, 0x33);
  cpu_csr_setbits(CSR_MCOR, 0x12);
  // Enable the D-Cache
  cpu_csr_setbits(CSR_MHCR, 0x02);
}

inline void __attribute__((always_inline)) mcu_disable_icache()
{
  // clear (flush) the I-Cache
  cpu_csr_clrbits(CSR_MCOR, 0x33);
  cpu_csr_setbits(CSR_MCOR, 0x21);
  // Disable the I-Cache
  cpu_csr_clrbits(CSR_MHCR, 0x01);
}

inline void __attribute__((always_inline)) mcu_disable_dcache()
{
  // clear (flush) the D-Cache
  cpu_csr_clrbits(CSR_MCOR, 0x33);
  cpu_csr_setbits(CSR_MCOR, 0x22);
  // Disable the D-Cache
  cpu_csr_clrbits(CSR_MHCR, 0x02);
}

inline void __attribute__((always_inline)) mcu_preinit_code()
{
  mcu_enable_icache(); // enable instruction cache otherwise it will be very slow
  mcu_enable_dcache(); // enable data cache
}

// Cache handling extensions (e.g. XuanTie C906)

inline void __attribute__ ((always_inline)) cpu_dcache_clear(void * addr)
{
  asm volatile ("dcache.cpa %[input_i]" : : [input_i] "r" (addr));
  asm volatile ("sync.s"); // ensure completion of clean operation.
}

inline void __attribute__ ((always_inline)) cpu_dcache_clear(void * addr, uintptr_t len)
{
  register uintptr_t raddr asm("a0") = intptr_t(addr) & ~(L1_CACHE_BYTES - 1);
  register uintptr_t rend  asm("a1") = raddr + len;
  while (raddr < rend)
  {
    asm volatile ("dcache.cpa a0");
    raddr += L1_CACHE_BYTES;
  }
  asm volatile ("sync.s"); // ensure completion of clean operation.
}


inline void __attribute__ ((always_inline)) cpu_dcache_invalidate(void * addr)
{
  asm volatile ("dcache.ipa %[input_i]" : : [input_i] "r" (addr));
}

inline void __attribute__ ((always_inline)) cpu_dcache_invalidate(void * addr, uintptr_t len)
{
  register uintptr_t raddr asm("a0") = intptr_t(addr) & ~(L1_CACHE_BYTES - 1);
  register uintptr_t rend  asm("a1") = raddr + len;
  while (raddr < rend)
  {
    asm volatile ("dcache.ipa a0");
    raddr += L1_CACHE_BYTES;
  }
  //asm volatile ("sync.s"); // ensure completion of clean operation.
}


#endif
