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
	return (0 != (SYSCTL->SOCLOCK.CLKSTATUS & SYSCTL_CLKSTATUS_HFCLKGOOD_MASK));
}

void hwclk_start_ext_osc(unsigned aextspeed)
{
	unsigned rsel;
	if      (aextspeed >= 32000000)  rsel = 3;
	else if (aextspeed >= 16000000)  rsel = 2;
	else if (aextspeed >=  8000000)	 rsel = 1;
	else                             rsel = 0;

	SYSCTL->SOCLOCK.HFCLKCLKCFG = (0
	  | (1    << 28) // HFCLKFLTCHK: 1 = check startup time
		| (rsel << 12) // HFXTRSEL(2): xtal frequency range select
		| (0xFF <<  0) // HFXTTIME(8)
	);

	SYSCTL->SOCLOCK.HSCLKEN |= SYSCTL_HSCLKEN_HFXTEN_MASK;

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
	unsigned fws;
	if (acpuspeed <= 24000000)
	{
		fws = 0;
	}
	else if (acpuspeed <= 48000000)
	{
		fws = 1;
	}
	else // for 80 MHz.
	{
		fws = 2;
	}

	unsigned tmp;
	tmp = SYSCTL->SOCLOCK.MCLKCFG;
	tmp &= ~SYSCTL_MCLKCFG_FLASHWAIT_MASK;
	tmp |= (fws << SYSCTL_MCLKCFG_FLASHWAIT_OFS);
	SYSCTL->SOCLOCK.MCLKCFG = tmp;
}

static bool is_divisible(unsigned nom, unsigned div)
{
  unsigned res = nom / div;
  return (res * div == nom);
}

bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
	unsigned tmp;

  SystemCoreClock = MCU_INTERNAL_RC_SPEED;

  // ??? SYSCTL->SOCLOCK.HSCLKCFG = 1; // use the

  // set the system oscillator to 32 MHz
  tmp = SYSCTL->SOCLOCK.SYSOSCCFG;
  tmp &= ~SYSCTL_SYSOSCCFG_FREQ_MASK;
  tmp |= 0;
  SYSCTL->SOCLOCK.SYSOSCCFG = tmp;

  SYSCTL->SOCLOCK.MCLKCFG &= ~SYSCTL_MCLKCFG_USEHSCLK_ENABLE;

  // Verify SYSOSC -> MCLK
  while ((SYSCTL->SOCLOCK.CLKSTATUS & SYSCTL_CLKSTATUS_HSCLKMUX_MASK) != 0)
  {
    // wait until it is changed to SYSOSC
  }

  // now, it is safe to manipulate the HSCLOCK

  hwclk_prepare_hispeed(target_speed_hz);

  unsigned basespeed;
  if (!external_clock_hz)
  {
    // use the internal 48 MHz oscillator
    basespeed = MCU_INTERNAL_RC_SPEED;

    //TODO: implement
    return true; // NOT implemented yet !
  }
  else
  {
    hwclk_start_ext_osc(external_clock_hz);
    basespeed = (external_clock_hz & HWCLK_EXTCLK_MASK);
	}

  __NOP();

  // setup the SYSPLL

  SYSCTL->SOCLOCK.HSCLKEN &= ~SYSCTL_HSCLKEN_SYSPLLEN_ENABLE; // disable the system PLL

  // these constants were taken from TI SDK:
  // populate SYSPLLPARAM0/1 tuning registers from flash, based on input freq
  if      (basespeed >= 32000000)  tmp = 0x41C40034;
  else if (basespeed >= 16000000)  tmp = 0x41C4002C;
  else if (basespeed >= 16000000)  tmp = 0x41C40024;
  else                             tmp = 0x41C4001C;
	SYSCTL->SOCLOCK.SYSPLLPARAM0 = *(volatile uint32_t *)(tmp);
	SYSCTL->SOCLOCK.SYSPLLPARAM1 = *(volatile uint32_t *)(tmp + 4);

	// the CLK0 and CLK1 clock output will be set to the half of the CLK2X

	unsigned pdiv = 1;
	unsigned mul = 1;  // = qdiv
	unsigned rdiv = 1;
	unsigned rdiv2x = 1;
	unsigned vcotarget = target_speed_hz * 2;  // The VCO frequency must be within 60 and 400 MHz


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




	unsigned refdiv = basespeed / (2 * 1000000);  // divisor for 1 MHz
	if (refdiv < 1)  refdiv = 1;

	uint32_t pllmul = target_speed_hz / 1000000;

	// do not go out of the frequency range of the PLL DSO, the upper limit is not checked.
	while (pllmul * 1000000 < HWPLL_OSC_MIN_FREQ)
	{
		pllmul <<= 1;
		cpu_gclk_div <<= 1;
	}


  SYSCTL->SOCLOCK.SYSPLLCFG0 = (0
    | (0  << 16)  // RDIVCLK2X(4)
    | (0  << 12)  // RDIVCLK1(4)
    | (0  <<  8)  // RDIVCLK0(4)
    | (1  <<  6)  // ENABLECLK2X
    | (1  <<  5)  // ENABLECLK1
    | (1  <<  4)  // ENABLECLK0
    | (1  <<  1)  // MCLK2XVCO
    | (1  <<  0)  // SYSPLLREF: 1 = use the HFLCK as the reference
	);

  SYSCTL->SOCLOCK.SYSPLLCFG1 = (0
    | ((mul - 1)  <<  8)  // QDIV(7):
    | ((pdiv - 1) <<  0)  // PDIV(2): 0 = /1, 1 = /2, 2 = /4, 3 = /8
	);

#if 0
  static const DL_SYSCTL_SYSPLLConfig gSYSPLLConfig = {
      .inputFreq              = DL_SYSCTL_SYSPLL_INPUT_FREQ_16_32_MHZ,
  	.rDivClk2x              = 3,
  	.rDivClk1               = 1,
  	.rDivClk0               = 0,
  	.enableCLK2x            = DL_SYSCTL_SYSPLL_CLK2X_ENABLE,
  	.enableCLK1             = DL_SYSCTL_SYSPLL_CLK1_ENABLE,
  	.enableCLK0             = DL_SYSCTL_SYSPLL_CLK0_DISABLE,
  	.sysPLLMCLK             = DL_SYSCTL_SYSPLL_MCLK_CLK2X,
  	.sysPLLRef              = DL_SYSCTL_SYSPLL_REF_SYSOSC,
  	.qDiv                   = 9,
  	.pDiv                   = DL_SYSCTL_SYSPLL_PDIV_2
  };


  // set predivider PDIV (divides reference clock)
  DL_Common_updateReg(&SYSCTL->SOCLOCK.SYSPLLCFG1, ((uint32_t) config->pDiv),
      SYSCTL_SYSPLLCFG1_PDIV_MASK);

  // populate SYSPLLPARAM0/1 tuning registers from flash, based on input freq
  SYSCTL->SOCLOCK.SYSPLLPARAM0 =
      *(volatile uint32_t *) ((uint32_t) config->inputFreq);
  SYSCTL->SOCLOCK.SYSPLLPARAM1 =
      *(volatile uint32_t *) ((uint32_t) config->inputFreq + (uint32_t) 0x4);


  // set feedback divider QDIV (multiplies to give output frequency)
  DL_Common_updateReg(&SYSCTL->SOCLOCK.SYSPLLCFG1,
      ((config->qDiv << SYSCTL_SYSPLLCFG1_QDIV_OFS) &
          SYSCTL_SYSPLLCFG1_QDIV_MASK),
      SYSCTL_SYSPLLCFG1_QDIV_MASK);

  // write clock output dividers, enable outputs, and MCLK source to SYSPLLCFG0
  DL_Common_updateReg(&SYSCTL->SOCLOCK.SYSPLLCFG0,
      (((config->rDivClk2x << SYSCTL_SYSPLLCFG0_RDIVCLK2X_OFS) &
           SYSCTL_SYSPLLCFG0_RDIVCLK2X_MASK) |
          ((config->rDivClk1 << SYSCTL_SYSPLLCFG0_RDIVCLK1_OFS) &
              SYSCTL_SYSPLLCFG0_RDIVCLK1_MASK) |
          ((config->rDivClk0 << SYSCTL_SYSPLLCFG0_RDIVCLK0_OFS) &
              SYSCTL_SYSPLLCFG0_RDIVCLK0_MASK) |
          config->enableCLK2x | config->enableCLK1 | config->enableCLK0 |
          (uint32_t) config->sysPLLMCLK),
      (SYSCTL_SYSPLLCFG0_RDIVCLK2X_MASK | SYSCTL_SYSPLLCFG0_RDIVCLK1_MASK |
          SYSCTL_SYSPLLCFG0_RDIVCLK0_MASK |
          SYSCTL_SYSPLLCFG0_ENABLECLK2X_MASK |
          SYSCTL_SYSPLLCFG0_ENABLECLK1_MASK |
          SYSCTL_SYSPLLCFG0_ENABLECLK0_MASK |
          SYSCTL_SYSPLLCFG0_MCLK2XVCO_MASK));
#endif


  // enable SYSPLL
  SYSCTL->SOCLOCK.HSCLKEN |= SYSCTL_HSCLKEN_SYSPLLEN_ENABLE;
  while ((SYSCTL->SOCLOCK.CLKSTATUS & SYSCTL_CLKSTATUS_SYSPLLGOOD_MASK) == 0)
  {
    // wait until SYSPLL startup is stabilized
  }


#if 1
  // switching from SYSOSC to HSCLK

  SYSCTL->SOCLOCK.HSCLKCFG = SYSCTL_HSCLKCFG_HSCLKSEL_SYSPLL;  // set the SYSPLL for the HSCLK source

  // Verify HSCLK source is valid
  while ((SYSCTL->SOCLOCK.CLKSTATUS & SYSCTL_CLKSTATUS_HSCLKGOOD_MASK) == 0)
  {
    // wait until HSCLKGOOD
  }

  // Switch MCLK to HSCLK
  SYSCTL->SOCLOCK.MCLKCFG |= SYSCTL_MCLKCFG_USEHSCLK_ENABLE;

  // Verify HSCLK -> MCLK
  while ((SYSCTL->SOCLOCK.CLKSTATUS & SYSCTL_CLKSTATUS_HSCLKMUX_MASK) == 0)
  {
  	// wait until MCLK is sourced from the HSCLK
  }

#endif


  //SystemCoreClock = target_speed_hz;
	return true;
}

