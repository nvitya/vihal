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

#include "platform.h"
#include <hwmemprot.h>

#if defined(MPU) && (__CORTEX_M != 33)

THwMemProt   hwmemprot;

void THwMemProt::Init()
{
  unsigned n;

  Disable();
  max_regions = (MPU->TYPE >> 8);
  ClearRegions();

  initialized = true;
}

void THwMemProt::Disable()
{
  MPU->CTRL = 0; // disable the MPU
}

void THwMemProt::Enable()
{
  MPU->CTRL = (1 << 0) | (1 << 2);
}

void THwMemProt::ClearRegions()
{
  unsigned n;
  for (n = 0; n < max_regions; ++n)
  {
    MPU->RNR = n;
    MPU->RBAR = 0;
    MPU->RASR = 0;
  }
  drcnt = 0;
}

void THwMemProt::AddRegion(uint32_t aaddr, uint32_t asize, uint32_t attr, uint32_t perm)
{
  if (drcnt >= max_regions)
  {
    return;
  }

  MPU->RNR = drcnt;
  MPU->RBAR = aaddr;

  uint32_t sizebytes = 32;
  uint32_t sizecode  = 4;
  while ((sizebytes < asize) && (sizecode < 31))
  {
    ++sizecode;
    sizebytes = (sizebytes << 1);
  }

  uint32_t rasr = (0
    | (1        <<  0) // ENABLE
    | (sizecode <<  1) // SIZE(5): region size = 2^(1 + SIZE),  2^28 = 256 MByte
    | (0        <<  8) // SRD(8): subregion disable bits
    | (attr     << 16) // attributes(6), B, S, C, TEX
    | (perm     << 24) // AP(3): permissions, 3 = RW/RW (full access)
    | (0        << 28) // (included in "perm") XN: 1 = code execution disabled
  );
  MPU->RASR = rasr;

  ++drcnt;
}

#endif // defined MPU
