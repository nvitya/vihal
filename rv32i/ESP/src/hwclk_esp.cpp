/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     hwclkctrl_esp.cpp
 *  brief:    ESP MCU Clock / speed setup
 *  version:  1.00
 *  date:     2022-01-29
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwclk.h"

bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
  uint32_t tmp;

  if (target_speed_hz > 160000000)  target_speed_hz = 160000000;

  SystemCoreClock = external_clock_hz / 2;  // this is the defalt speed 20 MHz

  // set the CPU clock source to XTAL

  tmp = SYSTEM->SYSCLK_CONF;
  tmp &= ~(3 << 10);
  tmp |= (1 << 10);  // 1 = FOSC
  SYSTEM->SYSCLK_CONF = tmp;

  // set the SYSTEM PLL to 480 MHz

  uint32_t periodsel = (target_speed_hz <= 80000000 ? 0 : 1);

  tmp = SYSTEM->CPU_PER_CONF;
  tmp &= ~0x07;
  tmp |= (0
    | (periodsel << 0)  // CPUPERIOD_SEL(2):  1 = 160 MHz, 0 = 80 MHz
    | (1 << 2)  // PLL_FREQ_SEL: 1 = 480, 0 = 320
  );
  SYSTEM->CPU_PER_CONF = tmp;

  // select the PLL to system clock

  tmp = SYSTEM->SYSCLK_CONF;
  tmp &= ~(3 << 10); // clear the source
  if (target_speed_hz >= 80000000)
  {
    tmp |= (1 << 10);  // 1 = PLL
  }
  else
  {
    // use the divided crystal
    tmp &= ~(0x3FF); // clear the divider
    uint32_t clkdiv = external_clock_hz / target_speed_hz;
    if (clkdiv > 0)  --clkdiv;
    tmp |= ((0 << 10) | (clkdiv << 0));  // 0 = XTAL with divider
  }
  SYSTEM->SYSCLK_CONF = tmp;

  // ???
  //((void (*)(int)) 0x40000588)(160);  // ets_update_cpu_frequency(160)

  SystemCoreClock = target_speed_hz;

  return true;
}

