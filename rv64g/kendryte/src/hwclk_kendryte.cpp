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
 *  file:     hwclk_kendryte.cpp
 *  brief:    Kendryte clock speed implementation
 *  date:     2022-02-01
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwclk.h"

bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
#if 0
  uint32_t ctl  = RCU->CTL;
  uint32_t cfg0 = RCU->CFG0;
  uint32_t cfg1 = RCU->CFG1;

  // set a stable CPU clock source for the case of re-initialization

  ctl |= 1; // enalble the IRC8M (usually never disabled
  RCU->CTL = ctl;

  // reset the cfg0
  cfg0 &= ~3; // bit0..1: 0 = select the internal 8 MHz RC for the SYS clock
  RCU->CFG0 = cfg0;
  while (((RCU->CFG0 >> 2) & 3) != 0)
  {
    // wait until it is set
  }

  // reset all PLL parameters, prepare bus divisors with /1
  cfg0 = 0;
  if (target_speed_hz > 54000000) // the APB1 maximal speed is 54 MHz
  {
    cfg0 |= (4 << 8); // divide the APB1 with 2
  }
  RCU->CFG0 = cfg0;

  cfg1 = 0;
  RCU->CFG1 = cfg1;

  ctl &= 0x0000FFFF; // disable all PLL-s, keep calibration data and IRC8M enable
  RCU->CTL = ctl;

  unsigned basefreq;  // divide the input clock to 4 MHz
  unsigned inputdiv;
  unsigned pllmul;

  if (0 == external_clock_hz) // use the internal RC
  {
    basefreq = 4000000;  // divide the input clock to 4 MHz
    inputdiv = 1; // ignored, fix /2
  }
  else // use the external crystal
  {
    basefreq = 4000000;  // divide the input clock to 4 MHz
    if (external_clock_hz == 25000000)  basefreq = 5000000;
    inputdiv = external_clock_hz / basefreq;

    ctl |= RCU_CTL_HXTALEN;  // enable the external crystal oscillator
    RCU->CTL |= ctl;
    while (0 == (RCU->CTL & RCU_CTL_HXTALSTB))
    {
      //
    }

    cfg0 |= RCU_CFG0_PLLSEL; // select the external XTAL for the PLL input
    cfg1 |= ((inputdiv - 1) << 0);  // select the division
  }

  pllmul = (target_speed_hz / basefreq) - 2;
  cfg0 |= ((pllmul & 15) << 18);
  cfg0 |= ((pllmul & 0x10) << (29-4));

  cfg0 |= (3 << 14); // select /8 for the ADC clock

  uint32_t usbdv = 3; // set the usb div to /2
  if (target_speed_hz <= 48000000)
  {
    usbdv = 1; // set to /1
  }

  cfg0 |= (usbdv << 22);

  // apply the prepared configs then
  RCU->CFG0 = cfg0;
  RCU->CFG1 = cfg1;

  // turn on the PLL
  ctl |= RCU_CTL_PLLEN;
  RCU->CTL = ctl;
  while (0 == (RCU->CTL & RCU_CTL_PLLSTB))
  {
    // wait until the PLL is stable
  }

  cfg0 |= 2; // select the PLL as the SYS clock source
  RCU->CFG0 = cfg0;
  while (((RCU->CFG0 >> 2) & 3) == 2)
  {
    // wait until the PLL activated as system clock
  }

#endif
  // finally set the global variable for the other system parts
  SystemCoreClock = target_speed_hz; // defined as .noinit, so it won't be cleared at section setup

  return true;
}


