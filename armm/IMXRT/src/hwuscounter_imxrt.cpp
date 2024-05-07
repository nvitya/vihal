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
// file:     hwuscounter_stm32.cpp
// brief:    STM32 implementation of the us (microseconds) counter
// created:  2024-04-06
// authors:  nvitya
// notes:    uses the 32-bit TIM2 or TIM5 timers

#include <stdio.h>
#include <stdarg.h>

#include "platform.h"
//#include "hwuscounter.h"
#include "hwuscounter_imxrt.h"

#include "imxrt_utils.h"

bool THwUsCounter_imxrt::Init()
{
  if (2 == timerdev)
  {
    regs = GPT2;
    imxrt_set_clock_gate(0, 24, 3);  // bus clock
    imxrt_set_clock_gate(0, 26, 3);  // serial clock ?
  }
  else
  {
    regs = GPT1;
    imxrt_set_clock_gate(1, 20, 3);  // bus clock
    imxrt_set_clock_gate(1, 22, 3);  // serial clock ?
  }

  cnt64_high = 0;
  cnt32_prev = 0;

  regs->CR = (1 << 15); // software reset
  while (regs->CR & (1 << 15))
  {
    __NOP();
  }

  // uses the PERCLK_CLK_ROOT which is set to use the 24 MHz oscillator
  uint32_t periphclock = 24000000;
  regs->PR = (0
    | (((periphclock / 1000000) - 1) << 0)
    | (0 << 12) // PRESCALER24M
  );

  regs->CR = (0
    | (0  << 31)  // FO3
    | (0  << 30)  // FO2
    | (0  << 29)  // FO1
    | (0  << 26)  // OM3(3)
    | (0  << 23)  // OM2(3)
    | (0  << 20)  // OM1(3): 0 = output disabled
    | (0  << 18)  // IM2(2)
    | (0  << 16)  // IM1(2): 0 = capture disabled
    | (0  << 15)  // SWR
    | (0  << 10)  // EN_24M: 1 = enable 24 MHz oscillator input
    | (1  <<  9)  // FRR: compare event behaviour: 0 = restart mode, 1 = free run mode
    | (1  <<  6)  // CLKSRC(3): 0 = no clock, 1 = peripheral clock, 2 = ipg_clk_highfreq, 5 = 24 MHz crystal
    | (0  <<  5)  // STOPEN
    | (0  <<  4)  // DOZEEN
    | (0  <<  3)  // WAITEN
    | (1  <<  2)  // DBGEN: 0 = disable in debug mode
    | (0  <<  1)  // ENMOD
    | (1  <<  0)  // EN
  );

  initialized = true;
  return true;
}

uint64_t THwUsCounter_imxrt::Get64()
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

