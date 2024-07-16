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
// file:     hwuscounter_am.cpp
// brief:    AM24x implementation of the us (microseconds) counter
// created:  2024-07-16
// authors:  nvitya

#include <stdio.h>
#include <stdarg.h>

#include "hwuscounter.h"
#include "hwuscounter_am.h"

bool THwUsCounter_am::Init()
{
  regs = (am_gtc_regs_t *)(SDL_GTC0_GTC_CFG1_BASE);
  regs->CNTCR = 1; // enable
  initialized = true;
  return true;
}

uint32_t THwUsCounter_am::Get32()
{
  uint32_t uslow1;
  uint32_t ushigh1;
  uint32_t ushigh2;
  do
  {
    ushigh1 = regs->CNTCV_HI;
    uslow1  = regs->CNTCV_LO;
    ushigh2 = regs->CNTCV_HI;
  }
  while (ushigh1 != ushigh2);

  uint64_t t64 = (uint64_t(ushigh1) << 32) | uslow1;

  return t64 / 225;
}

uint64_t THwUsCounter_am::Get64()
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
