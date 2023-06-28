/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2023 Viktor Nagy, nvitya
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
 *  file:     hwclkctrl_msp.cpp
 *  brief:    MSP MCU Clock / speed setup
 *  created:  2023-06-27
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwclk.h"
//#include "msp_utils.h"

inline bool hwclk_ext_osc_ready()
{
	//return (SYSCTRL->PCLKSR.bit.XOSCRDY != 0);
	return true;
}

void hwclk_start_ext_osc(unsigned aextspeed)
{

	// ...

	while (!hwclk_ext_osc_ready())
	{
	  // wait
	}
}

bool hwclk_start_inths_osc(unsigned aextspeed)
{
	return true;
}

void hwclk_prepare_hispeed(unsigned acpuspeed)
{
  // !!!!!
}

bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
  SystemCoreClock = MCU_INTERNAL_RC_SPEED;

  hwclk_prepare_hispeed(target_speed_hz);

  unsigned basespeed;
  if (!external_clock_hz)
  {
    // use the internal 48 MHz oscillator
    basespeed = MCU_INTERNAL_RC_SPEED;
  }
  else
  {
    hwclk_start_ext_osc(external_clock_hz);
    basespeed = (external_clock_hz & HWCLK_EXTCLK_MASK);
	}

  //SystemCoreClock = target_speed_hz;
	return true;
}

