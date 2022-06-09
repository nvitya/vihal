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
 *  file:     hwpwm_rp.cpp
 *  brief:    RP2040 PWM Driver
 *  created:  2022-06-09
 *  authors:  nvitya
*/

#include "hwpwm.h"

#include "rp_utils.h"

bool THwPwmChannel_rp::Init(int atimernum, int achnum, int aoutnum) // outnum: 0 = A, 1 = B
{
	initialized = false;

  rp_reset_control(RESETS_RESET_PWM_BITS, false); // remove reset

	devnum = atimernum;
	chnum = (achnum & 7);
	outnum = (aoutnum & 1);

  timer_base_speed = SystemCoreClock;

	regs = &pwm_hw->slice[chnum];

	uint32_t cr = (0
	  | (0  <<  7)  // PH_ADV: phase correction forwards
	  | (0  <<  6)  // PH_RET: phase correction downwards
	  | (0  <<  4)  // DIVMODE(2): 0 = normal free running mode, 1-3 = channel B input clock
	  | (0  <<  3)  // B_INV: 1 = invert output B
	  | (0  <<  2)  // A_INV: 1 = invert output A
	  | (0  <<  1)  // PH_CORRECT
	  | (0  <<  0)  // EN: 1 = enable
	);

	if (inverted)
	{
	  cr |= (1 << (2 + outnum));
	}

	regs->csr = cr;

	SetFrequency(frequency);

	uint32_t cc = regs->cc;
	if (outnum)
	{
	  cc &= 0x0000FFFF;  // reset B
	}
	else
	{
    cc &= 0xFFFF0000;  // reset A
	}

	initialized = true;
	return true;
}

void THwPwmChannel_rp::SetFrequency(uint32_t afrequency)
{
	frequency = afrequency;

	uint32_t intdiv = 0;
	do
	{
		++intdiv;
		periodclocks = timer_base_speed / (intdiv * frequency);
	}
	while (periodclocks > 65535);

	regs->div = (intdiv << 4); // use only the integer part
	regs->top = periodclocks;
}

void THwPwmChannel_rp::SetOnClocks(uint16_t aclocks)
{
  uint32_t cc = regs->cc;
  if (outnum)
  {
    cc &= 0x0000FFFF;  // reset B
    cc |= (aclocks << 16);
  }
  else
  {
    cc &= 0xFFFF0000;  // reset A
    cc |= aclocks;
  }
	regs->cc = cc;
}

void THwPwmChannel_rp::Enable()
{
	regs->csr |= PWM_CH0_CSR_EN_BITS;
}

void THwPwmChannel_rp::Disable()
{
  regs->csr &= ~PWM_CH0_CSR_EN_BITS;
}

