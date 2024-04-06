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
// file:     hwuscounter.cpp
// brief:    us (microseconds) counter using some internal timer hardware
// created:  2024-04-06
// authors:  nvitya

#include "hwuscounter.h"
#include "clockcnt.h"

THwUsCounter  uscounter;

// fallback implementation using the CLOCKCNT

bool THwUsCounter_noimpl::Init()
{
  clockcnt_init();
  clocks_per_us = SystemCoreClock / 1000000;
  last_clkcnt_ud = CLOCKCNT;
  last_clkcnt_ud -= (last_clkcnt_ud % clocks_per_us);  // remove the remainder
  cnt32 = 0;
  initialized = true;
  return true;
}

uint32_t THwUsCounter_noimpl::Get32()
{
  uint32_t clkcnt = CLOCKCNT;
  uint32_t elapsed_clocks = clkcnt - last_clkcnt_ud;
  uint32_t elapsed_us = elapsed_clocks / clocks_per_us;
  cnt32 += elapsed_us;
  last_clkcnt_ud += elapsed_us * clocks_per_us;
  return cnt32;
}

uint64_t THwUsCounter_noimpl::Get64()
{
  uint32_t cnt32_cur = Get32();
  if (cnt32_cur < cnt32_prev)
  {
    // the 32-bit version overflowed (after 66 minutes)
    cnt64_high += 0x100000000ull;
  }
  cnt32_prev = cnt32_cur;
  return cnt64_high + cnt32_cur;
}


