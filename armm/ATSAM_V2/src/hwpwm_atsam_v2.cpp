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
/*
 *  file:     hwpwm_atsam_v2.cpp
 *  brief:    ATSAM V2 PWM Driver
 *  version:  1.00
 *  date:     2021-11-17
 *  authors:  nvitya
*/

#include "hwpwm.h"

#include "atsam_v2_utils.h"

// Parameters:
//   adevnum:  0 - 4: TCC, later 0x100-0x107: using TC
//   achnum:   0 - 5 for TCC
bool THwPwmChannel_atsam_v2::Init(int adevnum, int achnum)
{
  unsigned gclkid;
  uint32_t tmp;

	initialized = false;

	devnum = adevnum;
	regs = nullptr;

	if ((achnum < 0) || (achnum > 5))
	{
	  return false;
	}
	chnum = achnum;

  if (false)
  {
  }
#if defined(TCC0)
  else if (0 == adevnum)
  {
    regs = TCC0;
    gclkid = TCC0_GCLK_ID;
    MCLK->APBBMASK.reg |= MCLK_APBBMASK_TCC0;
  }
#endif
#if defined(TCC1)
  else if (1 == adevnum)
  {
    regs = TCC1;
    gclkid = TCC1_GCLK_ID;
    MCLK->APBBMASK.reg |= MCLK_APBBMASK_TCC1;
  }
#endif
#if defined(TCC2)
  else if (2 == adevnum)
  {
    regs = TCC2;
    gclkid = TCC2_GCLK_ID;
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_TCC2;
  }
#endif
#if defined(TCC3)
  else if (3 == adevnum)
  {
    regs = TCC3;
    gclkid = TCC3_GCLK_ID;
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_TCC3;
  }
#endif
#if defined(TCC4)
  else if (4 == adevnum)
  {
    regs = TCC4;
    gclkid = TCC4_GCLK_ID;
    MCLK->APBDMASK.reg |= MCLK_APBDMASK_TCC4;
  }
#endif

	if (!regs)
	{
		return false;
	}

  atsam2_set_periph_gclk(gclkid, 0);  // use the system clock
  timer_base_speed = SystemCoreClock;

  chbit = (1 << chnum);

  bool wasenabled = TccDisable(); // disable is it was enabled

  regs->CTRLA.reg = (0
    | (0  << 24)  // CPTEN(4)
    | (0  << 23)  // DMAOS
    | (0  << 15)  // MSYNC
    | (0  << 14)  // ALOCK
    | (1  << 12)  // PRESYNC(2): 1 = prescaler clock
    | (0  << 11)  // RUNSTDBY
    | (0  <<  8)  // PRESCALER(3)
    | (0  <<  5)  // RESOLUTION(2): 0 = dithering is disabled
    | (0  <<  1)  // ENABLE
    | (0  <<  0)  // SWRST
  );

  regs->DBGCTRL.reg = ((1 << 2) | (1 << 0));  // enable running during debug

  regs->CTRLBCLR.reg = 0xFF; // clear the CTRLB register

  // rather do not modify, was already zero
  //   regs->WEXCTRL.reg = 0;
  //   regs->FCTRLA.reg = 0;
  //   regs->FCTRLB.reg = 0;
  //   regs->EVCTRL.reg = 0;

  regs->INTENCLR.reg = 0xFFFFFFFF;  // disable interrupts

#if 0
  tmp = regs->DRVCTRL.reg;
  if (inverted)
  {
    tmp |= ((chbit << 16) | (chbit << 20));
  }
  else
  {
    tmp &= ~((chbit << 16) | (chbit << 20));
  }
  regs->DRVCTRL.reg = tmp;
#endif

  tmp = regs->WAVE.reg;
  tmp &= ~(0x00FF0000);  // keep the polarity block
  tmp |= (0
    | (0  <<  7)  // CIPEREN
    | (0  <<  4)  // RAMP(2)
    | (2  <<  0)  // WAVEGEN(3): 2 = normal PWM
  );

  if (inverted)
  {
    tmp |= (chbit << 16);
  }
  else
  {
    tmp &= ~(chbit << 16);
  }

  regs->WAVE.reg = tmp;

  // set up fix pattern
  tmp = regs->PATT.reg;
  if (inverted)  tmp |= (chbit << 8);
  tmp |= chbit; // start with disabled
  regs->PATT.reg = tmp;

	SetFrequency(frequency);

	TccEnable(true);

	initialized = true;
	return true;
}

void THwPwmChannel_atsam_v2::SetFrequency(uint32_t afrequency)
{
	frequency = afrequency;

	uint32_t presc = 0; // warning, special coding, double increment over 4
	uint32_t fdiv = 1;
	while (1)
	{
		//++prescaler;
		periodclocks = timer_base_speed / (fdiv * frequency);
		if (periodclocks <= 65536)
		{
		  break;
		}

		++presc;
		if (presc > 4)  // after 16: 64, 256, 1024
		{
		  fdiv <<= 2;
		}
		else
		{
		  fdiv <<= 1;
		}
	}

	// set the prescaler
	if (presc != regs->CTRLA.bit.PRESCALER)
	{
	  // can be set only when not enabled!
	  unsigned wasenabled = TccDisable();
	  regs->CTRLA.bit.PRESCALER = presc;
	  TccEnable(wasenabled);
	}

	regs->PER.reg = periodclocks;
}

void THwPwmChannel_atsam_v2::SetOnClocks(uint16_t aclocks)
{
  regs->CCBUF[chnum].reg = aclocks;
}

void THwPwmChannel_atsam_v2::Enable()
{
  uint32_t patt = regs->PATT.reg;
  patt &= ~(chbit);
  regs->PATT.reg = patt;
}

void THwPwmChannel_atsam_v2::Disable()
{
  uint32_t patt = regs->PATT.reg;
  patt |= chbit;
  regs->PATT.reg = patt;
}

bool THwPwmChannel_atsam_v2::TccDisable()
{
  uint32_t ctrla = regs->CTRLA.reg;
  bool wasenabled = (0 != (ctrla & TCC_CTRLA_ENABLE));
  if (wasenabled)
  {
    ctrla &= ~TCC_CTRLA_ENABLE;
    regs->CTRLA.reg = ctrla;
    while (regs->SYNCBUSY.bit.ENABLE)
    {
      // wait
    }
  }
  return wasenabled;
}

void THwPwmChannel_atsam_v2::TccEnable(bool aenable)
{
  uint32_t ctrla = regs->CTRLA.reg;
  bool wasenabled = (0 != (ctrla & TCC_CTRLA_ENABLE));
  if (aenable)
  {
    if (!wasenabled)
    {
      ctrla |= TCC_CTRLA_ENABLE;
      regs->CTRLA.reg = ctrla;
      while (regs->SYNCBUSY.bit.ENABLE)
      {
        // wait
      }
    }
  }
  else // disable
  {
    if (wasenabled)
    {
      ctrla &= ~TCC_CTRLA_ENABLE;
      regs->CTRLA.reg = ctrla;
      while (regs->SYNCBUSY.bit.ENABLE)
      {
        // wait
      }
    }
  }
}
