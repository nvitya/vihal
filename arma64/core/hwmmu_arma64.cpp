/*
 * hwmmu_arma64.cpp
 *
 *  Created on: Jul 23, 2024
 *      Author: vitya
 */

#include "string.h"
#include <hwmmu_arma64.h>

THwMmu  hwmmu;

uint64_t g_hwmmu_16k_L2[HWMMU_MAX_BLKIDX] __attribute__((aligned(512)));

void THwMmu::Init()
{
  ptable = &g_hwmmu_16k_L2[0];
  table_items = sizeof(g_hwmmu_16k_L2) / 8;

  blksize = (1 << HWMMU_BLKSIZE_BITS);
  lowmask = blksize - 1;
  highmask = ~lowmask;

  // prepare to invalid descriptor:
  memset(ptable, 0, 8 * table_items);
}

void THwMmu::MapRange(uintptr_t avaddr, uintptr_t apaddr, uintptr_t asize, uint32_t aflags)
{
  uint32_t blkidx = (avaddr >> HWMMU_BLKSIZE_BITS);
  if (blkidx >= HWMMU_MAX_BLKIDX)
  {
    return;
  }

  apaddr = (apaddr & 0xFFF000000);

  uint64_t desc_base = (0
    | (1  << 10)  // AF: access flag
    | (1  <<  8)  // SH(2): shareability, 1 = inner shareable
    | (1  <<  6)  // AP(2): access permissions, 1 = read+write
    | ((aflags & MMUCFG_MA_MASK) <<  2)  // ATTRIDX(3):
    | (0  <<  1)  // PAGE_DESC: 0 = block descriptor, 1 = page descriptor
    | (1  <<  0)  // VALID: 1 = descriptor valid
  );

  uint32_t blkidx_end = ((asize + lowmask) >> HWMMU_BLKSIZE_BITS);
  while (blkidx < blkidx_end)
  {
    ptable[blkidx] = (desc_base | apaddr);
    ++blkidx;
    apaddr += blksize;
  }

}

inline void __attribute__ ((always_inline)) arma64_mmu_store_ttbr0(uint64_t adata)
{
  register uint64_t data = adata;
  asm volatile ("msr  ttbr0_el2, %[input_a]" :  : [input_a] "r" (data));
}

inline void __attribute__ ((always_inline)) arma64_mmu_store_hcr(uint64_t adata)
{
  register uint64_t data = adata;
  asm volatile ("msr  hcr_el2, %[input_a]" :  : [input_a] "r" (data));
}

inline uint64_t __attribute__ ((always_inline)) arma64_mmu_load_hcr()
{
  register uint64_t data;
  asm volatile ("mrs  %[result], hcr_el2" : [result] "=r" (data) : );
  return data;
}

inline void __attribute__ ((always_inline)) arma64_mmu_store_tcr(uint64_t adata)
{
  register uint64_t data = adata;
  asm volatile ("msr  tcr_el2, %[input_a]" :  : [input_a] "r" (data));
}

inline uint64_t __attribute__ ((always_inline)) arma64_mmu_load_tcr()
{
  register uint64_t data;
  asm volatile ("mrs  %[result], tcr_el2" : [result] "=r" (data) : );
  return data;
}

inline void __attribute__ ((always_inline)) arma64_mmu_store_sctlr(uint64_t adata)
{
  register uint64_t data = adata;
  asm volatile ("msr  sctlr_el2, %[input_a]" :  : [input_a] "r" (data));
}

inline uint64_t __attribute__ ((always_inline)) arma64_mmu_load_sctlr()
{
  register uint64_t data;
  asm volatile ("mrs  %[result], sctlr_el2" : [result] "=r" (data) : );
  return data;
}

inline uint64_t __attribute__ ((always_inline)) arma64_mmu_load_mmfr0()
{
  register uint64_t data;
  asm volatile ("mrs  %[result], id_aa64mmfr0_el1" : [result] "=r" (data) : );
  return data;
}


void THwMmu::Activate()
{
  arma64_mmu_store_ttbr0(uint64_t(ptable));

  PrepareMair();

  uint64_t tcr   = arma64_mmu_load_tcr();
  uint64_t hcr   = arma64_mmu_load_hcr();
  uint64_t sctlr = arma64_mmu_load_sctlr();
  uint64_t mmfr0 = arma64_mmu_load_mmfr0();

  // TCR register Setup

  // bits[4:0]: virtual address size
  tcr &= ~(uint64_t(0x1F));
  tcr |= (64 - HWMMU_PHYADDR_BITS);

  // bits[18:16]: the physical address size, 0 = 32, 1 = 36, 2 = 40, 3 = 42, 4 = 44, 5 = 48, 6 = 52, 7 = 56
  tcr &= ~(uint64_t(0x7) << 16);
  tcr |= (1              << 16);

  // bits[15:14]: TG = granule size:  2 = 16 kByte
  tcr &= ~(uint64_t(0x3) << 14);
  tcr |= (2              << 14);

  // bit[21]: HA, hardware access flag: 0 = disable
  tcr &= ~(uint64_t(0x1) << 21);

  // HCR register setup

  // bit[34]: E2H: disable
  hcr &= ~(uint64_t(1) << 34);

  // SCTLR register setup

  sctlr |= (1 << 0); // M_ENABLE: 1 = enable MMU

  arma64_mmu_store_tcr(tcr);
  arma64_mmu_store_hcr(hcr);
  arma64_mmu_store_sctlr(sctlr);
}

inline void __attribute__ ((always_inline)) arma64_mmu_mair_store(uint64_t adata)
{
  register uint64_t data = adata;
  asm volatile ("msr  mair_el2, %[input_a]" :  : [input_a] "r" (data));
}

void THwMmu::PrepareMair()
{
  uint64_t mair = (0
    | (uint64_t(MMU_MATTR_NORMAL)        <<  0)  // idx 0
    | (uint64_t(0)                       <<  8)  // idx 1 (unused)
    | (uint64_t(MMU_MATTR_NOCACHE)       << 16)  // idx 2
    | (uint64_t(MMU_MATTR_DEVREGS)       << 24)  // idx 3
    | (uint64_t(MMU_MATTR_DEVMEM)        << 32)  // idx 4
    | (uint64_t(0)                       << 40)  // idx 5 (unused)
    | (uint64_t(0)                       << 48)  // idx 6 (unused)
    | (uint64_t(0)                       << 56)  // idx 7 (unused)
  );

  arma64_mmu_mair_store(mair);
}
