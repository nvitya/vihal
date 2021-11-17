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
 *  file:     hwpwm_atsam.cpp
 *  brief:    ATSAM PWM Driver
 *  version:  1.00
 *  date:     2021-11-17
 *  authors:  nvitya
*/

#include "hwpwm.h"

#include "atsam_utils.h"

bool THwPwmChannel_atsam::Init(int adevnum, int achnum)
{
  unsigned perid;

	initialized = false;

	devnum = adevnum;
	chnum = (achnum & 7);

	unsigned busid = 1;

	commonregs = nullptr;

  if (false)
  {

  }
#if defined(PWM)
	if (0 == devnum)
	{
	  commonregs = PWM;
    perid = ID_PWM;
	}
#endif
#if defined(PWM0)
	else if (0 == devnum)
	{
	  commonregs = PWM0;
    perid = ID_PWM0;
	}
#endif
#ifdef PWM1
	else if (1 == devnum)
	{
	  commonregs = PWM1;
    perid = ID_PWM1;
	}
#endif

	if (!commonregs)
	{
		return false;
	}

  chbit = (1 << chnum);

  // Enable the peripheral
  atsam_enable_peripheral(perid);
  timer_base_speed = atsam_peripheral_clock();

  regs = &commonregs->PWM_CH_NUM[chnum];
  cmpregs = &commonregs->PWM_CMP[chnum];

	commonregs->PWM_CLK = 0; // disable DIVA, DIVB clocks

	// disable interrutps
	commonregs->PWM_IDR1 = (chbit | (chbit << 16));
	commonregs->PWM_IDR2 = ((chbit << 7) | (chbit << 16));

	commonregs->PWM_SCM &= ~(chbit); // make sure that this is not synchronous

	commonregs->PWM_OS &= ~(chbit | (chbit << 16)); // select the deadtime outputs
	commonregs->PWM_OSC = (chbit | (chbit << 16)); // clear override

	//commonregs->PWM_ELMR[0] |= chbit;
	//commonregs->PWM_ELMR[1] |= chbit;

	// set channel regs

	uint32_t cmr = (0
	  | (0  <<  0)  // CPRE(4):  clock prescaler 0 = /1, 1 = /2, 2 = /4, 3 = /8 etc
	  | (0  <<  8)  // CALG: 0 = left aligned, 1 = center aligned
	  | (0  <<  9)  // CPOL: 0 = start with low level, 1 = start with high level
	  | (0  << 10)  // CES:
	  | (1  << 16)  // DTE: 1 = enable dead time generator
	  | (0  << 17)  // DTHI: 1 = invert high output
	  | (0  << 18)  // DTLI: 1 = invert low output
	);

	if (inverted)
	{
	  cmr |= (1 << 9);
	}

	regs->PWM_CMR = cmr;

	regs->PWM_DT = 0; // disable dead time
	regs->PWM_CDTY = 0;

	SetFrequency(frequency);

	initialized = true;
	return true;
}

void THwPwmChannel_atsam::SetFrequency(uint32_t afrequency)
{
	frequency = afrequency;

	uint32_t cpre = 0;
	while (1)
	{
		//++prescaler;
		periodclocks = timer_base_speed / ((1 << cpre) * frequency);
		if (periodclocks <= 65536)
		{
		  break;
		}
		++cpre;
	}

	if (cpre > 15)  cpre = 15;

	// set the CPRE
	uint32_t tmp = regs->PWM_CMR;
	tmp &= ~0x0F;
	tmp |= cpre;
	regs->PWM_CMR = tmp;

	regs->PWM_CPRD = periodclocks;
}

void THwPwmChannel_atsam::SetOnClocks(uint16_t aclocks)
{
  regs->PWM_CDTYUPD = aclocks;
}

void THwPwmChannel_atsam::Enable()
{
  commonregs->PWM_ENA = chbit;
}

void THwPwmChannel_atsam::Disable()
{
  commonregs->PWM_DIS = chbit;
}

