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

void hwclk_start_ext_osc()
{
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);

  while (0 == (RCC->CR & RCC_CR_HSERDY))
  {
    // wait
  }
}

//---------------------------------------------------------------------------------------------------------------------------
#if defined(MCUSF_F1) || defined(MCUSF_F3)
//---------------------------------------------------------------------------------------------------------------------------

void hwclk_prepare_hispeed(unsigned acpuspeed)
{
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;  // Enable Power Control clock

  /* Enable Prefetch Buffer */
  FLASH->ACR |= FLASH_ACR_PRFTBE;

  unsigned ws;
  if (acpuspeed <= 24000000)
  {
    ws = FLASH_ACR_LATENCY_0;
  }
  else if (acpuspeed <= 48000000)
  {
    ws = FLASH_ACR_LATENCY_1;
  }
  else
  {
    ws = FLASH_ACR_LATENCY_2;
  }

  FLASH->ACR &= ~FLASH_ACR_LATENCY;
  FLASH->ACR |= ws;

  //FLASH->ACR |= (1 << 3);
}

bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
  unsigned cfgr;

  // set back to the internal oscillator
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
    // Wait till the PLL is not ready
  }

  SystemCoreClock = MCU_INTERNAL_RC_SPEED; // set the global variable for the fall error happens

  hwclk_prepare_hispeed(target_speed_hz);

  if (external_clock_hz)
  {
    hwclk_start_ext_osc();
  }

  unsigned basespeed = (external_clock_hz ? external_clock_hz : MCU_INTERNAL_RC_SPEED);

  unsigned freqmul = target_speed_hz / basespeed;
  if ((freqmul < 2) or (freqmul > 16))
  {
    return false;
  }

  cfgr = RCC->CFGR;

  // setup bus divisors

  cfgr &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
  cfgr |= RCC_CFGR_HPRE_DIV1;   // HCLK = SYSCLK
  cfgr |= RCC_CFGR_PPRE2_DIV1;  // PCLK2 = HCLK
  cfgr |= RCC_CFGR_PPRE1_DIV2;  // PCLK1 = HCLK / 2

  // setup PLL

  cfgr &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |  RCC_CFGR_PLLMULL);

#if defined(MCUSF_F1)

  if (external_clock_hz)
  {
    unsigned pllmul = ((freqmul - 2) & 0xF);  // STM32F1 multiplyer code
    cfgr |= (RCC_CFGR_PLLSRC | (pllmul << 18));
  }
  else
  {
    // src bit cleared = HSI / 2 is the clock source for PLL
    // With internal RC clock the maximal speed is 64 MHz !!!
    unsigned pllmul = ((freqmul*2 - 2) & 0xF);
    cfgr |= (pllmul << 18);
  }

#elif defined(MCUSF_F3)

  if (external_clock_hz)
  {
    // no prediv
    unsigned pllmul = ((freqmul - 2) & 0xF);  // STM32F3 multiplyer code
    cfgr |= (uint32_t)(pllmul << 18);
    cfgr |= (uint32_t)(RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV1);
  }
  else
  {
    // With internal RC clock the maximal speed is 64 MHz !!!
    // we have a /2 division
    unsigned pllmul = freqmul*2;
    if (pllmul >= 16)
    {
      pllmul = 15;
    }
    else
    {
      pllmul -= 2;
    }
    cfgr |= (uint32_t)(pllmul << 18);
    cfgr |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_DIV2);
  }

#endif

  RCC->CFGR = cfgr;

  RCC->CR |= RCC_CR_PLLON;  // Enable PLL
  while ((RCC->CR & RCC_CR_PLLRDY) == 0)
  {
    // Wait until the PLL is ready
  }

  cfgr &= ~(RCC_CFGR_SW);
  cfgr |= RCC_CFGR_SW_PLL;  // Select the PLL as the system clock source
  RCC->CFGR = cfgr;

  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
  {
    // Wait until the PLL is used as the system clock source
  }

#if defined(RCC_APB2ENR_SYSCFGEN)
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
#endif

  SystemCoreClock = target_speed_hz;
  return true;
}

//---------------------------------------------------------------------------------------------------------------------------
#elif defined(MCUSF_F4)
//---------------------------------------------------------------------------------------------------------------------------

void hwclk_prepare_hispeed(unsigned acpuspeed)
{
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;  // Enable Power Control clock

  // set voltage scaling for maximal speed
#if defined(PWR_CR_ODEN)

  // enabling overdrive
  PWR->CR |= PWR_CR_ODEN;
  while (!(PWR->CSR & PWR_CSR_ODRDY)) { }  // wait for status flag

  PWR->CR |= PWR_CR_ODSWEN;
  while (!(PWR->CSR & PWR_CSR_ODSWRDY)) { }  // wait for status flag

  PWR->CR |= PWR_CR_VOS; // set voltage scaling

#else
  PWR->CR |= PWR_CR_VOS; // set voltage scaling
#endif

  // set Flash latency
  FLASH->ACR &= ~FLASH_ACR_LATENCY;
  FLASH->ACR |= FLASH_ACR_LATENCY_5WS;

  // enable caches and prefetch
  FLASH->ACR |= (FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN);
}

bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
  // select the HSI as clock source (required if this is called more times)

  RCC->CFGR &= ~3;
  RCC->CFGR |= RCC_CFGR_SW_HSI;
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
    hwclk_start_ext_osc();
    basespeed = external_clock_hz;
    pllsrc = 1;
  }
  else
  {
    pllsrc = 0;
    basespeed = MCU_INTERNAL_RC_SPEED;
  }

  unsigned vcospeed = target_speed_hz * 2;
  unsigned pllp = 2; // divide by 2 to get the final CPU speed

  unsigned pll_input_freq = 2000000;
  if ((external_clock_hz / 1000000) & 1)  // is the input MHz divisible by 2 ?
  {
    pll_input_freq = 1000000;
  }
  unsigned pllm = basespeed / pll_input_freq;   // generate 1-2 MHz VCO input
  unsigned plln = vcospeed  / pll_input_freq;   // the vco multiplier
  unsigned pllq = vcospeed / 48000000;          // usb speed

  RCC->PLLCFGR =
      (pllsrc << 22)  // select PLL source
    | (pllm <<  0)
    | (plln <<  6)
    | (((pllp >> 1) - 1) << 16)
    | (pllq << 24)
  ;

  RCC->CR |= RCC_CR_PLLON;  // enable the PLL
  while((RCC->CR & RCC_CR_PLLRDY) == 0)
  {
    // Wait till PLL is ready
  }

  // Set AHBCLK divider:
  RCC->CFGR &= ~RCC_CFGR_HPRE;
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

  // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
  // clocks dividers

  RCC->CFGR &= ~3;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while (((RCC->CFGR >> 2) & 3) != RCC_CFGR_SW_PLL)
  {
    // wait until it is set
  }

  // Set APB1CLK Divider:
  RCC->CFGR &= ~RCC_CFGR_PPRE1;
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

  // Set APB2CLK Divider:
  RCC->CFGR &= ~RCC_CFGR_PPRE2;
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

  SystemCoreClock = target_speed_hz;
  return true;
}

#else
  #error "STM32 MCU Clock Setup not implemented for this sub-family"
#endif

