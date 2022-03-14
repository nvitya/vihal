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
  uint32_t crreg = RCC->CR;
#ifdef RCC_CR_HSEBYP
  if (aextspeed & HWCLK_EXTCLK_BYPASS)
  {
    crreg |= RCC_CR_HSEBYP;
    RCC->CR = crreg;
  }
#endif
  crreg |= RCC_CR_HSEON;
  RCC->CR = crreg;

  while (0 == (RCC->CR & RCC_CR_HSERDY))
  {
    // wait
  }
}

static bool is_divisible(unsigned nom, unsigned div)
{
  unsigned res = nom / div;
  return (res * div == nom);
}

#if !defined(RCC_CFGR_PLLMUL) && defined(RCC_CFGR_PLLMULL)
  #define RCC_CFGR_PLLMUL RCC_CFGR_PLLMULL
#endif

#if defined(MCUSF_F0) || defined(MCUSF_L0)

void hwclk_prepare_hispeed(unsigned acpuspeed)
{
  /* Enable Prefetch Buffer and set Flash Latency */
#ifdef FLASH_ACR_PRFTBE
  FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;
#else
  FLASH->ACR = FLASH_ACR_LATENCY | FLASH_ACR_PRFTEN;
#endif
}

#elif defined(MCUSF_G0)

void hwclk_prepare_hispeed(unsigned acpuspeed)
{
	uint32_t tmp;
	// set the voltage scaling to Range 1 (1.2 V)
	tmp = PWR->CR1;
	tmp &= ~(PWR_CR1_VOS);
	tmp |= (1 << PWR_CR1_VOS_Pos); // 1 = range1, 2 = range2
	PWR->CR1 = tmp;

	// wait until the new voltage scaling is in effect
	while (PWR->SR2 & PWR_SR2_VOSF)
	{
		// wait until the VOSF is cleared
	}

  // Enable Prefetch Buffer, instruction cache and set Flash Latency to 2
	tmp = FLASH->ACR;

	tmp &= ~(FLASH_ACR_ICEN | FLASH_ACR_PRFTEN); // disable ICACHE, prefetch
	FLASH->ACR = tmp;
	tmp |= (FLASH_ACR_ICRST); // reset ICACHE
	FLASH->ACR = tmp;
	tmp &= ~(FLASH_ACR_ICRST);
	FLASH->ACR = tmp;
	tmp &= ~(FLASH_ACR_LATENCY);
	tmp |= (2 << FLASH_ACR_LATENCY_Pos);
	FLASH->ACR = tmp;

	tmp |= (FLASH_ACR_ICEN | FLASH_ACR_PRFTEN); // enable I-CACHE,
	FLASH->ACR = tmp;

  //FLASH->ACR = (FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | (2 << FLASH_ACR_LATENCY_Pos));  // set two wait states
}

#elif defined(MCUSF_F1) || defined(MCUSF_F3)

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

#elif defined(MCUSF_F4)

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

#elif defined(MCUSF_G4)

void hwclk_prepare_hispeed(unsigned acpuspeed)
{
  unsigned tmp;

  RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;  // Enable Power Control clock

  if (acpuspeed > 150000000)  // 1.28 V (Boost) required for the core
  {
    PWR->CR5 &= ~PWR_CR5_R1MODE;  // Enable boost
  }
  else // 1.2 V required for the Core (1.0 V not supported (up to 26 MHz))
  {
    PWR->CR5 |= PWR_CR5_R1MODE;   // Disable boost
  }

  // Set Voltage Scaling Range 1 (normal mode), (1.0 V mode (up to 26 MHz) not supported)

  tmp = PWR->CR1;
  tmp &= ~(PWR_CR1_VOS);
  tmp |= PWR_CR1_VOS_0;
  PWR->CR1 = tmp;
  while (PWR->SR2 & PWR_SR2_VOSF)
  {
    // Wait until VOSF is cleared
  }

  // set Flash latency

  unsigned ws;
  if      (acpuspeed <=  60000000)  ws = FLASH_ACR_LATENCY_2WS;  // this is the safest minimum that works always, not the fastest one
  else if (acpuspeed <=  80000000)  ws = FLASH_ACR_LATENCY_3WS;
  else if (acpuspeed <= 100000000)  ws = FLASH_ACR_LATENCY_4WS;
  else if (acpuspeed <= 120000000)  ws = FLASH_ACR_LATENCY_5WS;
  else if (acpuspeed <= 140000000)  ws = FLASH_ACR_LATENCY_6WS;
  else if (acpuspeed <= 160000000)  ws = FLASH_ACR_LATENCY_7WS;
  else
  {
    ws = FLASH_ACR_LATENCY_8WS;
  }

  FLASH->ACR &= ~FLASH_ACR_LATENCY;
  FLASH->ACR |= ws;

  // enable caches and prefetch
  FLASH->ACR |= (FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN);
}

#elif defined(MCUSF_WB)

void hwclk_prepare_hispeed(unsigned acpuspeed)
{
  unsigned tmp;

  // Set Voltage Scaling Range 1 (normal mode), (Range 2 (up to 16 MHz) not supported)

  tmp = PWR->CR1;
  tmp &= ~(PWR_CR1_VOS);
  tmp |= PWR_CR1_VOS_0;  // VOS_0 = Range 1, VOS_1 = Range 2
  PWR->CR1 = tmp;
  while (PWR->SR2 & PWR_SR2_VOSF)
  {
    // Wait until VOSF is cleared
  }

  // set Flash latency

  unsigned ws;
  if      (acpuspeed <= 18000000)  ws = FLASH_ACR_LATENCY_0WS;
  else if (acpuspeed <= 36000000)  ws = FLASH_ACR_LATENCY_1WS;
  else if (acpuspeed <= 54000000)  ws = FLASH_ACR_LATENCY_2WS;
  else
  {
    ws = FLASH_ACR_LATENCY_3WS;
  }

  tmp = FLASH->ACR;
  tmp &= ~FLASH_ACR_LATENCY;
  tmp |= ws;
  FLASH->ACR = ws;

  // enable caches and prefetch
  FLASH->ACR |= (FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN);
}

#elif defined(MCUSF_F7)

void hwclk_prepare_hispeed(unsigned acpuspeed)
{
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;  // Enable Power Control clock

  // The voltage scaling allows optimizing the power consumption when the
  // device is clocked below the maximum system frequency, to update the
  // voltage scaling value regarding system frequency refer to product
  // datasheet.

  // Enable the Over-drive to extend the clock frequency to 216 MHz
  PWR->CR1 |= PWR_CR1_ODEN;
  while((PWR->CSR1 & PWR_CSR1_ODRDY) == 0) { }

  // Enable the Over-drive switch
  PWR->CR1 |= (uint32_t)PWR_CR1_ODSWEN;
  while((PWR->CSR1 & PWR_CSR1_ODRDY) == 0) { }

  PWR->CR1 |= PWR_CR1_VOS;  // voltage scaling for maximum speed (Scale 1)

  // set Flash latency
  FLASH->ACR &= ~FLASH_ACR_LATENCY;
  FLASH->ACR |= FLASH_ACR_LATENCY_6WS;

  FLASH->ACR |= FLASH_ACR_ARTEN | FLASH_ACR_PRFTEN;  // turn on the ART accelerator
}

#elif defined(MCUSF_H7)

// && defined(SYSCFG_PWRCR_ODEN) /* STM32H74xxx and STM32H75xxx lines */

#ifndef MCUSF_H7_V2
  // STM32H74xxx and STM32H75xxx lines
  #define LL_PWR_REGU_VOLTAGE_SCALE0      (PWR_D3CR_VOS_0 | PWR_D3CR_VOS_1) /*!< Select voltage scale 0 */
#else
  // STM32H73xx and newer
  #define LL_PWR_REGU_VOLTAGE_SCALE0      0x00000000U                       /*!< Select voltage scale 0 */
#endif

void hwclk_prepare_hispeed(unsigned acpuspeed)
{
  uint32_t tmp;

  // set LDO
  tmp = PWR->CR3;
  tmp &= ~(PWR_CR3_SCUEN | PWR_CR3_LDOEN | PWR_CR3_BYPASS);
  tmp |= PWR_CR3_LDOEN;
  PWR->CR3 = tmp;

  while ((PWR->CSR1 & PWR_CSR1_ACTVOSRDY) != PWR_CSR1_ACTVOSRDY)
  {
    // wait until ready....
  }

  tmp = PWR->D3CR;
  tmp &= ~PWR_D3CR_VOS_Msk;
  tmp |= (LL_PWR_REGU_VOLTAGE_SCALE0 << PWR_D3CR_VOS_Pos); // VOS0 (=Scale 3) required for maximal speed
  PWR->D3CR = tmp;

  if (PWR->D3CR) { } // some delay
  if (PWR->D3CR) { } // some delay

#ifndef MCUSF_H7_V2
  // Enable the PWR overdrive for the maximal speed
  SYSCFG->PWRCR |= SYSCFG_PWRCR_ODEN;
  if (SYSCFG->PWRCR) { } // some delay
  if (SYSCFG->PWRCR) { } // some delay
  if (SYSCFG->PWRCR) { } // some delay
#endif

  while (0 == (PWR->D3CR & PWR_D3CR_VOSRDY))
  {
    // wait until ready
  }

  // set Flash latency
  tmp = FLASH->ACR;
  tmp &= ~(FLASH_ACR_LATENCY | FLASH_ACR_WRHIGHFREQ);
#ifndef MCUSF_H7_V2
  tmp |= (FLASH_ACR_LATENCY_4WS | FLASH_ACR_WRHIGHFREQ_1);
#else
  tmp |= (FLASH_ACR_LATENCY_3WS | FLASH_ACR_WRHIGHFREQ_1 | FLASH_ACR_WRHIGHFREQ_0);
#endif
  FLASH->ACR = tmp;
}

#else
  #error "STM32 hi-speed setup missing for this subfamily"
#endif

//---------------------------------------------------------------------------------------------------------------------------
#if defined(MCUSF_F0) || defined(MCUSF_L0)
//---------------------------------------------------------------------------------------------------------------------------

bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
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

#if defined(MCUSF_L0)

  // only a few combinations supported, but usually the max speed will be used.

  unsigned plldiv = 1;  // = 2, using this fix value
  unsigned pllmul = 0;
  switch (freqmul)
  {
    case  2:  pllmul = 1; break;
    case  3:  pllmul = 2; break;
    case  4:  pllmul = 3; break;
    case  6:  pllmul = 4; break;
    case  8:  pllmul = 5; break;
    case 12:  pllmul = 6; break;
    case 16:  pllmul = 7; break;
    case 24:  pllmul = 8; break;
    default:  pllmul = 0; break;
  }

  // do not divide the peripheral clocks:
  cfgr &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);

  /* PLL configuration = HSE * 6 = 48 MHz */
  cfgr &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLDIV | RCC_CFGR_PLLMUL));
  cfgr |= (uint32_t)((pllmul << 18) | (plldiv << 22));
  if (external_clock_hz)
  {
    cfgr |= (uint32_t)(RCC_CFGR_PLLSRC_HSE);
  }

#else

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

#endif

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
}

//---------------------------------------------------------------------------------------------------------------------------
#elif defined(MCUSF_G0)
//---------------------------------------------------------------------------------------------------------------------------

#if defined(MCUSF_G0) // add the missing defines
  #define RCC_CFGR_SW_HSI     0
  #define RCC_CFGR_SW_PLL     2

  #define RCC_CFGR_HPRE_DIV1  1
  #define RCC_CFGR_PPRE_DIV1  1
#endif

bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
  // select the HSI as clock source (required if this is called more times)

  uint32_t cfgr;

  cfgr = RCC->CFGR;
  cfgr &= ~RCC_CFGR_SW;
  cfgr |= RCC_CFGR_SW_HSI;
  RCC->CFGR = cfgr;
  while (((RCC->CFGR >> RCC_CFGR_SWS_Pos) & RCC_CFGR_SW) != RCC_CFGR_SW_HSI)
  {
    // wait until it is set
  }

  RCC->CR &= ~(RCC_CR_HSIDIV); // set the HSI16 division to 1 (16 MHz)

  RCC->CR &= ~RCC_CR_PLLON;  // disable the PLL
  while ((RCC->CR & RCC_CR_PLLRDY) != 0)
  {
    // Wait until the PLL is ready
  }

  SystemCoreClock = MCU_INTERNAL_RC_SPEED; // set the global variable for the case an error happens

  hwclk_prepare_hispeed(target_speed_hz);

  unsigned basespeed;
  unsigned pllsrc;

  // external clock source is not implemented !
  pllsrc = 2;
  basespeed = MCU_INTERNAL_RC_SPEED;

  unsigned vcospeed = target_speed_hz * 2;
  unsigned pllr = 2; // divide by 2 to get the final CPU speed

  unsigned pllm = 1;
  unsigned pllq = 2;
  unsigned plln = vcospeed  / basespeed;    // the vco multiplier
  unsigned pllp = vcospeed / 35000000;      // adc speed

  RCC->PLLCFGR = (0
    | (pllsrc     <<  0)  // PLLSRC(2): select PLL source
    | ((pllm - 1) <<  4)  // PLLM(3)
    | (plln       <<  8)  // PLLN(7)
		| (1          << 16)  // PLLPEN
    | ((pllp - 1) << 17)  // PLLP(5)
		| (1          << 24)  // PLLQEN
    | ((pllq - 1) << 25)  // PLLQ(3)
		| (1          << 28)  // PLLREN
    | ((pllr - 1) << 29)  // PLLR(3)
  );

  RCC->CR |= RCC_CR_PLLON;  // enable the PLL
  while((RCC->CR & RCC_CR_PLLRDY) == 0)
  {
    // Wait till PLL is ready
  }

  // setup bus dividers AHB=1, PERIPH=1
  cfgr = RCC->CFGR;
  cfgr &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE);
  cfgr |= (RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE_DIV1);
  RCC->CFGR = cfgr;

  // Select PLL as system clock source
  cfgr &= ~RCC_CFGR_SW;
  cfgr |= RCC_CFGR_SW_PLL;
  RCC->CFGR = cfgr;
  while (((RCC->CFGR >> RCC_CFGR_SWS_Pos) & RCC_CFGR_SW) != RCC_CFGR_SW_PLL)
  {
    // wait until it is set
  }

  SystemCoreClock = target_speed_hz;
  return true;
}


//---------------------------------------------------------------------------------------------------------------------------
#elif defined(MCUSF_F1) || defined(MCUSF_F3)
//---------------------------------------------------------------------------------------------------------------------------

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

  unsigned basespeed;
  if (external_clock_hz)
  {
    hwclk_start_ext_osc(external_clock_hz);
    basespeed = (external_clock_hz & HWCLK_EXTCLK_MASK);
  }
  else
  {
    basespeed = MCU_INTERNAL_RC_SPEED;
  }

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

  cfgr &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |  RCC_CFGR_PLLMUL);

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
#elif defined(MCUSF_G4) || defined(MCUSF_WB)
//---------------------------------------------------------------------------------------------------------------------------

#ifndef RCC_CFGR_SW_PLL  // MCUSF_WB has no such define
  #define RCC_CFGR_SW_PLL  3
#endif

bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
  uint32_t tmp;

  // set back to the internal oscillator
  #if defined(MCUSF_WB)

    // for the WB the MSI will be selected as it is the default source after the reset

    // ensure that the MSI is running at 4 MHz
    tmp = RCC->CR;
    tmp &= ~(RCC_CR_MSIRANGE);
    tmp |= RCC_CR_MSIRANGE_6; // range6 = 4 MHz
    tmp |= RCC_CR_MSION;
    RCC->CR = tmp;
    while (0 == (RCC->CR & RCC_CR_MSIRDY))
    {
      // wait until it is ready
    }

    // select the MSI as the clock source
    #if !defined(RCC_CFGR_SW_MSI)
      #define RCC_CFGR_SW_MSI 0
    #endif
    tmp = RCC->CFGR;
    tmp &= ~3;
    tmp |= RCC_CFGR_SW_MSI;
    RCC->CFGR = tmp;
    while (((RCC->CFGR >> 2) & 3) != RCC_CFGR_SW_MSI)
    {
      // wait until it is set
    }
  #else
    tmp = RCC->CFGR;
    tmp &= ~3;
    tmp |= RCC_CFGR_SW_HSI;
    RCC->CFGR = tmp;
    while (((RCC->CFGR >> 2) & 3) != RCC_CFGR_SW_HSI)
    {
      // wait until it is set
    }
  #endif

  RCC->CR &= ~RCC_CR_PLLON;  // disable the PLL
  while ((RCC->CR & RCC_CR_PLLRDY) != 0)
  {
    // Wait until the PLL is ready
  }

  SystemCoreClock = MCU_INTERNAL_RC_SPEED; // set the global variable for the fall error happens

  #if defined(MCUSF_WB)
    // in STM32WB MCU's the flash is clocked with the same speed as SRAM2 and CPU2
    // CPU2 support only frequencies up to 32Mhz
    if(target_speed_hz > 32000000)
    {
      hwclk_prepare_hispeed(target_speed_hz/2);
    }
    else
    {
      hwclk_prepare_hispeed(target_speed_hz);
    }
  #else
    hwclk_prepare_hispeed(target_speed_hz);
  #endif


  unsigned basespeed;
  unsigned pllsrc;
  if (external_clock_hz)
  {
    hwclk_start_ext_osc(external_clock_hz);
    basespeed = (external_clock_hz & HWCLK_EXTCLK_MASK);
    pllsrc = 3;  // HSE
  }
  else
  {
    #if defined(MCUSF_WB)  // start the HSI
      RCC->CR |= RCC_CR_HSION;
      while (0 == (RCC->CR & RCC_CR_HSIRDY))
      {
        // wait until it is ready
      }
    #endif

    pllsrc = 2; // HSI (16 MHz)
    basespeed = MCU_INTERNAL_RC_SPEED;
  }

  unsigned vcospeed = target_speed_hz * 2;
  unsigned pllp = 2; // divide by 2 to get the final CPU speed

  // try some round frequencies for VCO input:
  unsigned vco_in_hz;
  vco_in_hz = 4000000; // this is the default
  if (!is_divisible(basespeed, vco_in_hz) || !is_divisible(vcospeed, vco_in_hz))
  {
    vco_in_hz = 5000000; // for 25 MHz input cristals
    if (!is_divisible(basespeed, vco_in_hz) || !is_divisible(vcospeed, vco_in_hz))
    {
      vco_in_hz = 3000000;
      if (!is_divisible(basespeed, vco_in_hz) || !is_divisible(vcospeed, vco_in_hz))
      {
        vco_in_hz = 4000000;  // not synthetizable properly, stay with the default
      }
    }
  }

  unsigned pllm = basespeed / vco_in_hz;
  unsigned plln = vcospeed / vco_in_hz;

  if (pllm > 1)  --pllm;

  #if defined(MCUSF_WB)  // the register layout is different
    RCC->PLLCFGR = 0
      | (pllsrc <<  0)  // PLLSRC(2)
      | (pllm   <<  4)  // PLLM(4)
      | (plln   <<  8)  // PLLN(7)
      | (1      << 16)  // PLLPEN
      | (1      << 17)  // PLLP(5): 1 = VCO / 2, same as PCLK
      | (1      << 24)  // PLLQEN
      | (1      << 25)  // PLLQ(3): 1 = VCO / 2, same as PCLK
      | (1      << 28)  // PLLREN
      | (1      << 29)  // PLLR(3): system clock = VCO speed / 2
    ;
  #else
    RCC->PLLCFGR = 0
      | (pllsrc <<  0)  // PLLSRC(2)
      | (pllm   <<  4)  // PLLM(4)
      | (plln   <<  8)  // PLLN(7)
      | (1      << 16)  // PLLPEN
      | (0      << 17)  // PLLP: 0 = use PLLPDIV
      | (1      << 20)  // PLLQEN
      | (2      << 21)  // PLLQ(2): divide by 6, this gives only 48 MHz at 144 MHz CPU speed
      | (1      << 24)  // PLLREN
      | (0      << 25)  // PLLR(2): system clock = VCO speed / 2
      | (2      << 27)  // PLLPDIV(5): divide by 2
    ;
  #endif

  RCC->CR |= RCC_CR_PLLON;  // enable the PLL
  while((RCC->CR & RCC_CR_PLLRDY) == 0)
  {
    // Wait till PLL is ready
  }

  #if defined(MCUSF_G4)

    // on the G4 devices the APB1 and APB2 buses allowed to run with system clock speed
    // select bus dividers AHB=1, ABP1=1, APB2=1
    tmp = RCC->CFGR;
    tmp &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
    tmp |= (RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV1 | RCC_CFGR_PPRE2_DIV1);
    RCC->CFGR = tmp;

  #elif defined(MCUSF_WB)

    // select bus dividers AHB=1, ABP1=1, APB2=1 (no divisions required)
    tmp = RCC->CFGR;
    tmp &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2); // clearing all these means do not divide
    RCC->CFGR = tmp;

    tmp = RCC->EXTCFGR;
    tmp &= ~(RCC_EXTCFGR_C2HPRE | RCC_EXTCFGR_SHDHPRE);
    //tmp |= (8 << RCC_EXTCFGR_C2HPRE_Pos);  // HCLK2: 8 = divide by 2
    RCC->EXTCFGR = tmp;

    // divide clock for CPU2 (cortex M0+) and SRAM2 by 2 if the sysclk is greater than 32Mhz
    if(target_speed_hz > 32000000)
    {
		tmp = RCC->EXTCFGR;
		tmp &= ~(RCC_EXTCFGR_C2HPRE | RCC_EXTCFGR_SHDHPRE);
		tmp |= (8 << RCC_EXTCFGR_C2HPRE_Pos) | (8 << RCC_EXTCFGR_SHDHPRE_Pos);  // HCLK2: 8 = divide by 2
		RCC->EXTCFGR = tmp;
    }

    // select for USART sysclk as clock source
    tmp = RCC->CCIPR;
    tmp &= ~(RCC_CCIPR_USART1SEL_Msk);
    tmp |= (1 << RCC_CCIPR_USART1SEL_Pos);  // Select Sysclk
    RCC->CCIPR = tmp;

  #else
    #error "Unimplemented sub-family handling here"
  #endif

  // Select PLL as system clock source

  tmp &= ~3;
  tmp |= RCC_CFGR_SW_PLL;
  RCC->CFGR = tmp;
  while (((RCC->CFGR >> 2) & 3) != RCC_CFGR_SW_PLL)
  {
    // wait until it is set
  }

  SystemCoreClock = target_speed_hz;
  return true;
}

//---------------------------------------------------------------------------------------------------------------------------
#elif defined(MCUSF_F4) || defined(MCUSF_F7)
//---------------------------------------------------------------------------------------------------------------------------

bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
  // select the HSI as clock source (required if this is called more times)

  uint32_t tmp;

  tmp = RCC->CFGR;
  tmp &= ~3;
  tmp |= RCC_CFGR_SW_HSI;
  RCC->CFGR = tmp;
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

  if (target_speed_hz < 24000000) // minimum speed to use the PLL
  {
    return (target_speed_hz == MCU_INTERNAL_RC_SPEED);
  }

  hwclk_prepare_hispeed(target_speed_hz);

  unsigned basespeed;
  unsigned pllsrc;
  if (external_clock_hz)
  {
    hwclk_start_ext_osc(external_clock_hz);
    basespeed = (external_clock_hz & HWCLK_EXTCLK_MASK);
    pllsrc = 1;
  }
  else
  {
    pllsrc = 0;
    basespeed = MCU_INTERNAL_RC_SPEED;
  }

  unsigned pllp = 2; // divide by 2 to get the final CPU speed
  unsigned vcospeed = target_speed_hz * pllp;

  // use the minimal VCO speed
  while (vcospeed < 192000000)
  {
    pllp += 2;
    vcospeed = target_speed_hz * pllp;
  }

  // try to find a speed for the USB
  while (!is_divisible(vcospeed, 48000000) && (target_speed_hz * (pllp + 2) <= 432000000))
  {
    pllp += 2;
    vcospeed = target_speed_hz * pllp;
  }

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

  // select bus dividers AHB=1, ABP1=4, APB2=2
  tmp = RCC->CFGR;
  tmp &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
  tmp |= (RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2);
  RCC->CFGR = tmp;

  // Select PLL as system clock source

  tmp &= ~3;
  tmp |= RCC_CFGR_SW_PLL;
  RCC->CFGR = tmp;
  while (((RCC->CFGR >> 2) & 3) != RCC_CFGR_SW_PLL)
  {
    // wait until it is set
  }

#ifdef MCUSF_F7
  RCC->DCKCFGR2 &= ~(RCC_DCKCFGR2_CK48MSEL); // select the 48 MHz from the PLL
#endif

  SystemCoreClock = target_speed_hz;
  return true;
}

//---------------------------------------------------------------------------------------------------------------------------
#elif defined(MCUSF_H7)
//---------------------------------------------------------------------------------------------------------------------------

bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
  uint32_t tmp;

  // select the HSI as clock source (required if this is called more times)
  tmp = RCC->CFGR;
  tmp &= ~3;
  tmp |= RCC_CFGR_SW_HSI;
  RCC->CFGR = tmp;
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
    pllsrc = RCC_PLLCKSELR_PLLSRC_HSE;
  }
  else
  {
    pllsrc = RCC_PLLCKSELR_PLLSRC_HSI;
    basespeed = MCU_INTERNAL_RC_SPEED;
  }

  unsigned vcospeed = target_speed_hz;  // no * 2 here like by the other models !

  // try some round frequencies for VCO input:
  unsigned pllrange;
  unsigned vco_in_hz;
  vco_in_hz = 8000000; // this provides lower jitter
  pllrange = 3;
  if (!is_divisible(basespeed, vco_in_hz) || !is_divisible(vcospeed, vco_in_hz))
  {
    vco_in_hz = 5000000; // for 25 MHz input cristals
    pllrange = 2;
    if (!is_divisible(basespeed, vco_in_hz) || !is_divisible(vcospeed, vco_in_hz))
    {
      vco_in_hz = 4000000; // try this one then
      pllrange = 2;
      if (!is_divisible(basespeed, vco_in_hz) || !is_divisible(vcospeed, vco_in_hz))
      {
        vco_in_hz = 3000000;
        pllrange = 1;
        if (!is_divisible(basespeed, vco_in_hz) || !is_divisible(vcospeed, vco_in_hz))
        {
          vco_in_hz = 2000000;  // hoping that works, 2MHz is the minimal PLL input freq.
          pllrange = 0;
        }
      }
    }
  }

  unsigned pllm = basespeed / vco_in_hz;   // vco input pre-divider

  unsigned plln = vcospeed  / vco_in_hz;   // the vco multiplier
  unsigned pllq = vcospeed / 48000000;     // usb speed
  unsigned pllp = 1;  // no processor division
  unsigned pllr = 2;

  tmp = RCC->PLLCFGR;
  tmp &= ~((0xF << 0) | (7 << 16)); // clear PLL1 configuration
  tmp |= (0
    | (7 << 16) // enable all outputs (P, Q, R)
    | (pllrange <<  2) // PLLRGE(2): 0 = 1-2 MHz PLL1 input clock range
    | (0 <<  1) // PLL1VCOSEL: 0 = wide range (192-836 MHz), 1 = medium range (150 - 420 MHz)
    | (0 <<  0) // PLL1FRACEN: 0 = disable fractional divider
  );
  RCC->PLLCFGR = tmp;

  tmp = RCC->PLLCKSELR;
  tmp &= ~(RCC_PLLCKSELR_PLLSRC | RCC_PLLCKSELR_DIVM1);
  tmp |= (0
         | (pllsrc   << RCC_PLLCKSELR_PLLSRC_Pos)
         | (pllm     << RCC_PLLCKSELR_DIVM1_Pos)
  );
  RCC->PLLCKSELR = tmp;

  // configure PLL1 dividers:
  RCC->PLL1DIVR = (0
    | ((plln - 1) << RCC_PLL1DIVR_N1_Pos)
    | ((pllp - 1) << RCC_PLL1DIVR_P1_Pos)
    | ((pllq - 1) << RCC_PLL1DIVR_Q1_Pos)
    | ((pllr - 1) << RCC_PLL1DIVR_R1_Pos)
  );

  RCC->CR |= RCC_CR_PLLON;  // enable the PLL
  while((RCC->CR & RCC_CR_PLLRDY) == 0)
  {
    // Wait till PLL is ready
  }

#ifndef MCUSF_H7_V2  // at the H74x, H75x all the APB buses can run at 200 MHz

  // set CPU divider to 1, and all bus dividers to 2:

  unsigned ahbdiv = 8;
  unsigned apbdiv = 0;  // 0 = same as the AHB bus speed
  if (target_speed_hz <= MAX_CLOCK_SPEED / 2)
  {
    // no bus speed division required.
    ahbdiv = 0;
  }

#else  // newer models, where the APB buses limited to 137.5 MHz

  unsigned ahbdiv = 8;
  unsigned apbdiv = 4;  // 4 = /2 of the AHB
  if (target_speed_hz <= MAX_CLOCK_SPEED / 2)
  {
    // no bus speed division required.
    ahbdiv = 0;
    if (target_speed_hz <= MAX_CLOCK_SPEED / 4)
    {
      apbdiv = 0;
    }
  }

#endif

  RCC->D1CFGR = (0
    | (0       <<  8)  // D1CPRE(4): CPU clock division, 0 = not divided
    | (apbdiv  <<  4)  // D1PPRE(3): APB3 division, 4 = divided by 2
    | (ahbdiv  <<  0)  // HPRE(4):   AHB division, 8 = divided by 2
  );

  RCC->D2CFGR = (0
    | (apbdiv <<  8)  // D2PPRE2(3): APB2 division, 4 = divided by 2
    | (apbdiv <<  4)  // D2PPRE1(3): APB1 division, 4 = divided by 2
  );

  RCC->D3CFGR = (0
    | (apbdiv <<  4)  // D2PPRE1(3): APB4 division, 4 = divided by 2
  );

  // Select PLL as system clock source

  tmp = RCC->CFGR;
  tmp &= ~RCC_CFGR_SW;
  tmp |= RCC_CFGR_SW_PLL1;
  RCC->CFGR = tmp;
  while (((RCC->CFGR & RCC_CFGR_SWS_Msk) >> RCC_CFGR_SWS_Pos) != RCC_CFGR_SW_PLL1)
  {
    // wait until it is set
  }

  // USB !
  //RCC->DCKCFGR2 &= ~(RCC_DCKCFGR2_CK48MSEL); // select the 48 MHz from the PLL

  SystemCoreClock = target_speed_hz;
  return true;
}

#else
  #error "STM32 MCU Clock Setup not implemented for this sub-family"
#endif

#if defined(MCUSF_WB)

  bool hwlsclk_init(bool external_clock)
  {
    // LSI clock must always enabled for correct watch dog operation
    // by default LSI2 is enabled but LSI1 is more accurate and need less power
    // to select LSI1 as LSI clock source LSI2 must disabled

    // enable LSI1
    RCC->CSR |= RCC_CSR_LSI1ON;

    // wait until LSI1 is ready
    while(!(RCC->CSR & RCC_CSR_LSI1RDY))
    {

    }

    // disable LSI2
    RCC->CSR &= ~RCC_CSR_LSI2ON;

    // wait until LSI2 is disabled
    while((RCC->CSR & RCC_CSR_LSI2RDY))
    {

    }

    if(external_clock)
    {
      if(!(PWR->CR1 & PWR_CR1_DBP))
      {
        // enable backup domain access
        PWR->CR1 |= PWR_CR1_DBP;
        // wait for backup domain access
        while(!(PWR->CR1 & PWR_CR1_DBP))
        {

        }
      }

      // enable LSE and set drive strange to low
      uint32_t tmp = RCC->BDCR;
      tmp &= ~RCC_BDCR_LSEDRV;
      tmp |= RCC_BDCR_LSEON;
      RCC->BDCR = tmp;

      // wait until LSE is ready
      while(!(RCC->BDCR & RCC_BDCR_LSERDY))
      {

      }

      // switch RTC to LSE
      tmp = RCC->BDCR;
      tmp &= ~RCC_BDCR_RTCSEL;
      tmp |= (1 << RCC_BDCR_RTCSEL_Pos);
      RCC->BDCR = tmp;
    } else {
      // switch RTC to LSI
      uint32_t tmp = RCC->BDCR;
      tmp &= ~RCC_BDCR_RTCSEL;
      tmp |= (2 << RCC_BDCR_RTCSEL_Pos);
      RCC->BDCR = tmp;
    }

    return true;
  }
#else
  #error "STM32 MCU low speed Clock Setup not implemented for this sub-family"
#endif
