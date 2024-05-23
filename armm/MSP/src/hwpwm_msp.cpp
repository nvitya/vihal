/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2018 Viktor Nagy, nvitya
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
 *  file:     hwpwm_msp.cpp
 *  brief:    MSPM0G PWM Driver
 *  date:     2024-05-07
 *  authors:  nvitya
 *  note:
 *    only the TIMG units are supported
*/

#include "platform.h"
#include "hwpwm.h"
#include "hwpwm_msp.h"

#include "msp_utils.h"

bool THwPwmChannel_msp::Init(int atimernum, int achnum)
{
	initialized = false;
	advanced_timer = false;
	devnum = atimernum;
	chnum = achnum;

	unsigned busid = 1;

	regs = nullptr;

  if (false)
  {

  }
#ifdef TIMG0_BASE
	if (0 == devnum)
	{
		regs = TIMG0;
		busid = 0;
	}
#endif
#ifdef TIMG6_BASE
	else if (6 == devnum)
	{
		regs = TIMG6;
		busid = 1;
	}
#endif
#ifdef TIMG7_BASE
	else if (7 == devnum)
	{
		regs = TIMG7;
		busid = 1;
	}
#endif
#ifdef TIMG8_BASE
	else if (8 == devnum)
	{
		regs = TIMG8;
		busid = 0;
	}
#endif
#ifdef TIMA0_BASE
	if (0x80 == devnum)
	{
		regs = TIMA0;
		busid = 1;
		advanced_timer = true;
	}
#endif
#ifdef TIMA1_BASE
	if (0x81 == devnum)
	{
		regs = TIMA1;
		busid = 1;
		advanced_timer = true;
	}
#endif

	if (!regs)
	{
		return false;
	}

	if (0 == chnum)
	{
		valreg = (volatile uint32_t *)&(regs->COUNTERREGS.CC_01[0]);
	}
	else if (1 == chnum)
	{
		valreg = (volatile uint32_t *)&(regs->COUNTERREGS.CC_01[1]);
	}
	else if (2 == chnum)
	{
		valreg = (volatile uint32_t *)&(regs->COUNTERREGS.CC_23[0]);
	}
	else if (3 == chnum)
	{
		valreg = (volatile uint32_t *)&(regs->COUNTERREGS.CC_23[1]);
	}
	else
	{
		return false;
	}

	outenbit = (1 << chnum);


  regs->GPRCM.PWREN = (GPTIMER_PWREN_KEY_UNLOCK_W | GPTIMER_PWREN_ENABLE_ENABLE);
  if (regs->GPRCM.PWREN) { }
	regs->GPRCM.RSTCTL = 0xB1000002; // clear sticky reset
	if (regs->GPRCM.PWREN) {}  // wait a little
  regs->CLKSEL = GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE;

  regs->CLKDIV = 0; // do not divide
  regs->COMMONREGS.CCLKCTL = 1;

	timer_base_speed = msp_bus_speed(busid);
	SetFrequency(frequency);

	*valreg = 0;

	uint32_t ccctl = 0
		| (0  << 29)  // CC2SELD(3)
		| (0  << 26)  // CCACTUPD(3)
		| (0  << 25)  // SCERCNEZ
		| (0  << 22)  // CC2SELU(3)
		| (0  << 18)  // CCUPD(3): 0 = new CC value takes effect immediately, 4 = new CC value effective in the next cycle only
		| (0  << 17)  // COC: 0 = compare (output) mode, 1 = capture
		| (1  << 12)  // ZCOND(3)
		| (1  <<  8)  // LCOND(3)
		| (0  <<  4)  // ACOND(3)
		| (3  <<  0)  // CCOND(3): 1 = rising edge
	;
	regs->COUNTERREGS.CCCTL_01[chnum] = ccctl;

	uint32_t ccact = 0
		| (0  << 30)  // SWFRCACT_CMPL(2)
		| (0  << 28)  // SWFRCACT(2)
		| (0  << 25)  // FEXACT(3)
		| (0  << 22)  // FENACT(3)
		| (0  << 15)  // CC2UACT(2)
		| (0  << 12)  // CC2DACT(2)
		| (2  <<  9)  // CUACT(2): compare event: 2 = set output to 0
		| (0  <<  6)  // CDACT(2): downconting compare event
		| (0  <<  3)  // LACT(2): load event
		| (1  <<  0)  // ZACT(2): zero event: 1 = set output to 1, 3 = toggle
	;
	regs->COUNTERREGS.CCACT_01[chnum] = ccact;

	uint32_t octl = (0
		| (0  <<  5)  // CCPIV: 0 = low by default, 1 = high by default
		| (0  <<  4)  // CCPOINV: 0 = not inverted, 1 = inverted
		| (0  <<  0)  // CCPO(4): 0 = signal generator
	);
	regs->COUNTERREGS.OCTL_01[chnum] = octl;

	regs->COMMONREGS.CCPD |= outenbit; // configure this channel as output
	regs->COMMONREGS.ODIS |= outenbit; // disable this channel

  regs->COUNTERREGS.CTRCTL = (0
  	| GPTIMER_CTRCTL_EN_MASK
		| GPTIMER_CTRCTL_CM_UP
    | GPTIMER_CTRCTL_REPEAT_REPEAT_1
  );

	initialized = true;
	return true;
}

void THwPwmChannel_msp::SetFrequency(uint32_t afrequency)
{
	frequency = afrequency;

	uint32_t prescaler = 0;
	do
	{
		++prescaler;
		periodclocks = timer_base_speed / (prescaler * frequency);
	}
	while (periodclocks > 65535);

  regs->COUNTERREGS.LOAD = periodclocks - 1;
  regs->COMMONREGS.CPS   = prescaler - 1; // prescaler
}


void THwPwmChannel_msp::SetOnClocks(uint16_t aclocks)
{
	*valreg = aclocks;
}

void THwPwmChannel_msp::Enable()
{
	regs->COMMONREGS.ODIS &= ~outenbit; // enable this channel
}

void THwPwmChannel_msp::Disable()
{
	regs->COMMONREGS.ODIS |= outenbit; // disable this channel
}

