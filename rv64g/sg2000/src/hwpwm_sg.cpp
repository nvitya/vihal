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
 *  file:     hwpwm_sg.cpp
 *  brief:    SG200x/CV1800 (Sophgo) PWM Driver
 *  created:  2024-05-24
 *  authors:  nvitya
*/

#include "hwpwm.h"
#include "traces.h"

#include "sg_utils.h"

bool THwPwmChannel_sg::Init(int adevnum, int achnum)
{
	initialized = false;

	devnum = adevnum;
	chnum = (achnum & 3);
	outnum = chnum;

  if (0 == devnum)
  {
    regs = PWM0;
  }
  else if (1 == devnum)
  {
    regs = PWM1;
  }
  else if (2 == devnum)
  {
    regs = PWM2;
  }
  else if (3 == devnum)
  {
    regs = PWM3;
  }
  else
  {
    regs = nullptr;
  }

  if (!regs)
  {
    return false;
  }

  channel_bit = (1 << chnum);

  reg_period   = &(regs->PERIOD0);
  reg_period   += (2 * chnum);
  reg_hlperiod = &(regs->HLPERIOD0);
  reg_hlperiod += (2 * chnum);

  Disable();  // disable first otherwise the settings sill be ineffective

  base_speed = 100000000;

  uint32_t polreg = regs->POLARITY;
  if (!inverted)
  {
    polreg |=  (channel_bit << 0);
  }
  else
  {
    polreg &= ~(channel_bit << 0);
  }
  polreg &= ~(channel_bit << 8); // 0 = continuous output mode, 1 = fixed count pulse mode
  polreg &= ~(1 << 16);          // bit 16: 0 = general, independent mode, 1 = shifted synchronized output mode
  polreg |=  (1 << 20);          // bit 20: 1 = force APB clock on
  regs->POLARITY = polreg;

  SetFrequency(frequency);
  SetOnClocks(0);

  regs->PWM_OE |= (channel_bit << 0); // Enable the output, but do not start it

	initialized = true;
	return true;
}

bool THwPwmChannel_sg::Init(int apwmnum)
{
  return Init(apwmnum >> 2, apwmnum & 3);
}

void THwPwmChannel_sg::SetFrequency(uint32_t afrequency)
{
	frequency = afrequency;
	periodclocks = base_speed / frequency;
	*reg_period = periodclocks;
	if (chenabled)
	{
	  regs->PWMUPDATE = channel_bit;
	  regs->PWMUPDATE = 0;
	}
}

void THwPwmChannel_sg::SetOnClocks(uint32_t aclocks)
{
  on_clocks = aclocks;
  *reg_hlperiod = aclocks;
  if (chenabled)
  {
    regs->PWMUPDATE = channel_bit;
    regs->PWMUPDATE = 0;
  }
}

void THwPwmChannel_sg::Enable()
{
	regs->PWMSTART |= channel_bit;
	chenabled = true;
}

void THwPwmChannel_sg::Disable()
{
  regs->PWMSTART &= ~channel_bit;
  chenabled = false;
}

