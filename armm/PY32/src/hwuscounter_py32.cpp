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
// file:     hwuscounter_py32.cpp
// brief:    PY32 implementation of the us (microseconds) counter
// created:  2026-04-20
// authors:  nvitya
// notes:    uses the 32-bit TIM2 timer

#include <stdio.h>
#include <stdarg.h>

//#include "hwuscounter.h"
#include "platform.h"
#include "hwuscounter_py32.h"

#if defined(HWUSCOUNTER_IMPL)

#include "py32_utils.h"

bool THwUsCounter_py32::Init()
{
  // only TIM5 and TIM2 are 32-bit timers, but not on all STM32 HW

  regs = TIM2;
  RCC->APBENR1 |= RCC_APBENR1_TIM2EN;

  uint32_t periphclock;
  periphclock = py32_bus_speed(PY32_BUSID_APB1);

  cnt64_high = 0;
  cnt32_prev = 0;

  regs->CR1 = 0; // stop
  regs->CR2 = 0;
  regs->PSC = (periphclock / 1000000) - 1;  // for 1 us clock
  //regs->CNT = 0; // reset the counter
  regs->CR1 = 1; // enable the timer
  regs->EGR = 1; // reinit, start the timer (maybe not required?)

  initialized = true;
  return true;
}

uint64_t THwUsCounter_py32::Get64()
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

#endif
