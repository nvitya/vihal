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
 *  file:     hwclk_stm32.cpp
 *  brief:    STM32 clock speed implementation
 *  date:     2021-10-23
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwclk.h"

void hwclk_start_ext_osc(unsigned aextspeed)
{
}

static bool is_divisible(unsigned nom, unsigned div)
{
  unsigned res = nom / div;
  return (res * div == nom);
}

void hwclk_prepare_hispeed(unsigned acpuspeed)
{
  /* Enable Prefetch Buffer and set Flash Latency */
//  FLASH->ACR = FLASH_ACR_LATENCY | FLASH_ACR_PRFTEN;
}

bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
#if 0

  // select the HSI as clock source (required if this is called more times)

  uint32_t cfgr;

  cfgr = RCC->CFGR;
  cfgr &= ~3;
  cfgr |= RCC_CFGR_SW_HSI;
  RCC->CFGR = cfgr;
  while (((RCC->CFGR >> 2) & 3) != RCC_CFGR_SW_HSI)
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

  unsigned basespeed;
  unsigned pllsrc;
  if (external_clock_hz)
  {
    hwclk_start_ext_osc(external_clock_hz);
    basespeed = (external_clock_hz & HWCLK_EXTCLK_MASK);
  }
  else
  {
    // the internal oscillator is usually 8 MHz, but it will be divided by 2 at the PLL input
    basespeed = (MCU_INTERNAL_RC_SPEED >> 1);
  }

  unsigned freqmul = target_speed_hz / basespeed;
  if ((freqmul < 2) or (freqmul > 16))
  {
    return false;
  }

  cfgr = RCC->CFGR;

  unsigned pllmul = ((freqmul - 2) & 0xF);  // STM32F0 multiplyer code

  // setup bus dividers AHB=1, PERIPH=1
  cfgr &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE);
  cfgr |= (RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE_DIV1);

  cfgr &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMUL);
  cfgr |= (pllmul << 18);

  if (external_clock_hz)
  {
    cfgr |= (uint32_t)(RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV1);
  }
  else
  {
#ifdef RCC_CFGR_PLLSRC_HSI_DIV2
    cfgr |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_DIV2);
#else
    cfgr |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_PREDIV);
#endif
  }

  RCC->CFGR = cfgr;

  RCC->CR |= RCC_CR_PLLON;  // enable the PLL
  while((RCC->CR & RCC_CR_PLLRDY) == 0)
  {
    // Wait till PLL is ready
  }

  // Select PLL as system clock source
  cfgr &= ~3;
  cfgr |= RCC_CFGR_SW_PLL;
  RCC->CFGR = cfgr;
  while (((RCC->CFGR >> 2) & 3) != RCC_CFGR_SW_PLL)
  {
    // wait until it is set
  }

  SystemCoreClock = target_speed_hz;
  return true;

#else
  SystemCoreClock = MCU_INTERNAL_RC_SPEED;
  return true;
#endif
}
