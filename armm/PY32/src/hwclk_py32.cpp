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
 *  file:     hwclk_py32.cpp
 *  brief:    PY32 clock speed implementation
 *  date:     2026-04-21
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwclk.h"

void hwclk_start_ext_osc(unsigned aextspeed)
{
}

void hwclk_prepare_hispeed(unsigned acpuspeed)
{
	if (acpuspeed <= 24000000)
	{
		FLASH->ACR = 0;
	}
	else if (acpuspeed <= 48000000)
	{
		FLASH->ACR = 1;
	}
	else
	{
		FLASH->ACR = 2;
	}
}

#define RCC_CFGR_SW_HSI   0  /*!< HSISYS used as system clock */
#define RCC_CFGR_SW_PLL   2  /*!< PLL used as system clock */


bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
  // select the HSI as clock source (required if this is called more times)

	uint32_t tmp;
  uint32_t cfgr;

  cfgr = RCC->CFGR;
  cfgr &= ~7;
  cfgr |= RCC_CFGR_SW_HSI;
  RCC->CFGR = cfgr;
  while (((RCC->CFGR >> 3) & 7) != RCC_CFGR_SW_HSI)
  {
    // wait until it is set
  }

  RCC->CR &= ~RCC_CR_PLLON;  // disable the PLL
  while ((RCC->CR & RCC_CR_PLLRDY) != 0)
  {
    // Wait until the PLL is ready
  }

  SystemCoreClock = MCU_INTERNAL_RC_SPEED; // set the global variable for the fall error happens

  hwclk_prepare_hispeed(target_speed_hz);

  // read the 24 MHz calibration value from the FLASH
  uint32_t hsi_calib_24M = *((volatile uint32_t *)0x1FFF0F10);

  // change the HSI to 24 MHz (starts with 8 MHz by default)
  tmp = RCC->ICSCR;
  tmp &= ~((7 << 13) | (0x1FFF << 0));
  tmp |= ( (4 << 13) | ((hsi_calib_24M & 0x1FFF) << 0) );
  RCC->ICSCR = tmp;

  uint32_t internal_rc_speed = 24000000;
  SystemCoreClock = internal_rc_speed;

  unsigned basespeed;
  unsigned pllsrc;
  if (external_clock_hz)
  {
    return false;  // not implemented yet.

    //hwclk_start_ext_osc(external_clock_hz);
    //basespeed = (external_clock_hz & HWCLK_EXTCLK_MASK);
  }
  else
  {
    // the internal oscillator is usually 8 MHz, but it will be divided by 2 at the PLL input
    basespeed = internal_rc_speed;
  }

  unsigned freqmul = target_speed_hz / basespeed;
  if (freqmul > 3)
  {
  	return false;
  }

  if (freqmul <= 1)
  {
  	freqmul = 1;
  }
  else
  {
    // setup the PLL
    uint32_t pllcode;
    if (freqmul >= 3)
    {
    	freqmul = 3;
    	pllcode = 1;
    }
    else
    {
    	freqmul = 2;
    	pllcode = 0;
    }

    RCC->PLLCFGR = (0
    	| (pllcode << 2)  // PLLMUL(2): 0 = 2x, 1 = 3x, otherwise invalid
			| (0       << 0)  // PLLSRC: 0 = HSI, 1 = HSE
    );

    RCC->CR |= RCC_CR_PLLON;  // enable the PLL
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
      // Wait till PLL is ready
    }

    // Select PLL as system clock source
    cfgr = RCC->CFGR;
    cfgr &= ~7;
    cfgr |= RCC_CFGR_SW_PLL;
    RCC->CFGR = cfgr;
    while (((RCC->CFGR >> 3) & 7) != RCC_CFGR_SW_PLL)
    {
      // wait until it is set
    }
  }

  SystemCoreClock = internal_rc_speed * freqmul;
  return true;
}
