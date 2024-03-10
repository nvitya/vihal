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
 *  file:     hwmemprot.h
 *  brief:    ARM Cortex-M Memory Protection Unit (MPU) interface
 *  created:  2024-03-09
 *  authors:  nvitya
*/

#ifndef HWMEMPROT_H_
#define HWMEMPROT_H_

#include "platform.h"

#if defined(MPU)

/* ATTR description

 https://developer.arm.com/documentation/dui0646/b/Cortex-M7-Peripherals/Optional-Memory-Protection-Unit/MPU-access-permission-attributes?lang=en#BIHHEDAE

 BITS:   5 |  4 |  3 |  2 |  1 |  0
 Func:  T2 | T1 | T0 |  S |  C |  B
         0    0    0    0    0    0  = Stronggly ordered, non-cacheable, shareable (device mem)
         0    0    0    0    0    1  = Device (non-cacheable)

 cache:  1    B    B    S    A    A  = BB = outer cache policy, AA = inner cache policy, S = shared (for DMA)
         1    0    1    1    0    1  = Cacheable, write+read allocate
         1    0    0    1    0    0  = Non-cacheable memory

Cacheable policies:
  00 = non-cacheable,
  01 = Write back, write and Read- Allocate,
  10 = Write through, no Write-Allocate,
  11 = Write back, no Write-Allocate
*/

// base bit-offset: 16
#define MEMPROT_ATTR_SHARED        (1 << 2)
#define MEMPROT_ATTR_ORDERED       0x00
#define MEMPROT_ATTR_DEVICE        0x01
#define MEMPROT_ATTR_MEM_NOCACHE   0x24
#define MEMPROT_ATTR_MEM_CACHED    0x2D

// base bit-offset: 24
#define MEMPROT_PERM_NOACC   0
#define MEMPROT_PERM_RW      3
#define MEMPROT_PERM_RO      7
#define MEMPROT_PERM_XN      (1 << 4)

class THwMemProt
{
public:
  bool       initialized = false;
  uint8_t    max_regions = 0;

  uint8_t    drcnt = 0;  // defined region count

  void       Init();
  void       Disable();
  void       Enable();

  void       ClearRegions();
  void       AddRegion(uint32_t aaddr, uint32_t asize, uint32_t aattr, uint32_t perm);
};

extern THwMemProt  hwmemprot;

#endif

#endif /* HWMEMPROT_H_ */
