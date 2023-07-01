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

  tmp = SYSCTL->SOCLOCK.MCLKCFG;
  tmp &= ~(SYSCTL_MCLKCFG_USEHSCLK_ENABLE | SYSCTL_MCLKCFG_UDIV_MASK | SYSCTL_SYSOSCCFG_FREQ_MASK);
  tmp |= (1 << SYSCTL_MCLKCFG_UDIV_OFS);  // Set the ULPCLK to half of the MCLK
  SYSCTL->SOCLOCK.MCLKCFG = tmp;

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


#if 1
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

	unsigned pdiv = 0;
	unsigned mul = 2;  // = qdiv
  unsigned rdiv2x = 1;
	unsigned vcospeed;  // The VCO frequency must be within 60 and 400 MHz

	while (1)
	{
	  vcospeed = (basespeed >> pdiv) * mul;
	  if (vcospeed < 60000000)  // minimum 60 MHz
	  {
	    ++mul;
	  }
	  else if (vcospeed > 400000000) // maximum 400 MHz
	  {
	    if (3 == pdiv)
	    {
	      return false;  // no solution found !
	    }
	    ++pdiv;
	    mul = 2;
	  }
	  else
	  {
	    rdiv2x = (vcospeed * 2) / target_speed_hz;
	    if ( rdiv2x * target_speed_hz == vcospeed * 2 )  // integer divisible ?
	    {
	      break; // found it
	    }
	    else
	    {
	      ++mul;
	    }
	  }
	}

  unsigned rdiv0 = (rdiv2x >> 1); // results to the half frequency as the 2x output
	unsigned rdiv1 = ((vcospeed >> 1) / 40000000);  // try to achieve 40 MHz, >> 1 comes from the divisor coding (/2)
  //unsigned rdiv1 = rdiv0;  // to check the ULPCLK

  SYSCTL->SOCLOCK.SYSPLLCFG0 = (0
    | ((rdiv2x - 1) << 16)  // RDIVCLK2X(4)
    | ((rdiv1 - 1)  << 12)  // RDIVCLK1(4)
    | ((rdiv0 - 1)  <<  8)  // RDIVCLK0(4)
    | (1  <<  6)  // ENABLECLK2X
    | (1  <<  5)  // ENABLECLK1
    | (1  <<  4)  // ENABLECLK0
    | (1  <<  1)  // MCLK2XVCO: 0 = CLK0 will be used as HSCLK, 1 = CLK2X will be used as HSCLK
    | (1  <<  0)  // SYSPLLREF: 1 = use the HFLCK as the reference
	);

  SYSCTL->SOCLOCK.SYSPLLCFG1 = (0
    | ((mul - 1)  <<  8)  // QDIV(7):
    | (pdiv <<  0)  // PDIV(2): 0 = /1, 1 = /2, 2 = /4, 3 = /8
	);

  // enable SYSPLL
  SYSCTL->SOCLOCK.HSCLKEN |= SYSCTL_HSCLKEN_SYSPLLEN_ENABLE;
  while ((SYSCTL->SOCLOCK.CLKSTATUS & SYSCTL_CLKSTATUS_SYSPLLGOOD_MASK) == 0)
  {
    // wait until SYSPLL startup is stabilized
  }

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

  SystemCoreClock = target_speed_hz;
  return true;

#else

  // just use the external clock directly

  SYSCTL->SOCLOCK.HSCLKCFG = SYSCTL_HSCLKCFG_HSCLKSEL_HFCLKCLK;  // set the SYSPLL for the HFCLK (external crystal freq)

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

  SystemCoreClock = basespeed;
  return true;

#endif
}

