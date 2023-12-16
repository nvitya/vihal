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
 *  file:     hwpwm_imxrt.cpp
 *  brief:    IMXRT PWM controller
 *  date:     2023-12-16
 *  authors:  nvitya
*/

#include "hwpwm.h"
#include "imxrt_utils.h"

bool THwPwmChannel_imxrt::Init(int adevnum, int achnum, int aoutnum)
{
	initialized = false;

	devnum = adevnum;
	chnum = (achnum & 3);
	outnum = (aoutnum & 1);


	regs = nullptr;

	if (1 == devnum)
	{
		commonregs = PWM1;
		imxrt_set_clock_gate(4, 16, 3);
	}
	else if (2 == devnum)
	{
		commonregs = PWM2;
		imxrt_set_clock_gate(4, 18, 3);
	}
	else if (3 == devnum)
	{
		commonregs = PWM3;
		imxrt_set_clock_gate(4, 20, 3);
	}
	else if (4 == devnum)
	{
		commonregs = PWM4;
		imxrt_set_clock_gate(4, 22, 3);
	}

	if (!commonregs)
	{
		return false;
	}

	regs = (PWM_SubModule_Type *)&(commonregs->SM[chnum]);

	regs->INIT = 0; // initial count register
	regs->CTRL = 0
	  | (0    <<  0)  // DBLEN: 0 = double switching disabled
	  | (0    <<  1)  // DBLX: 0 = PWMX double pulse disabled
	  | (0    <<  2)  // LDMOD: 1 = immediately reload on MCTRL.LDOK
	  | (0    <<  3)  // SPLIT:
	  | (0    <<  4)  // PRSC(3): 0 = /1, 1 = /2, 2 = /4, 3 = /8 ... 7 = /128  = 1/(2^(PRSC))  will be set later
	  | (0    <<  7)  // COMPMODE: 0 = equal, 1 = equal or greater than
	  | (0    <<  8)  // DT(2):
	  | (1    << 10)  // FULL: 1 = full cycle reload
	  | (0    << 11)  // HALF: 1 = half cycle reload
	  | (0    << 12)  // LDFQ(4): 0 = load happens every PWM opportunity
	;

	regs->CTRL2 = 0
	  | (0 <<  0)  // CLK_SEL(2): 0 = use ipbus clock
	  | (0 <<  2)  // RELOAD_SEL: 0 = local reload, 1 = master reload
	  | (0 <<  3)  // FORCE_SEL(3):
	  | (0 <<  6)  // FORCE:
	  | (0 <<  7)  // FORCEN:
	  | (0 <<  8)  // INIT_SEL(2):
	  | (0 << 10)  // PWMX_INIT: initial value of PWMX
	  | (0 << 11)  // PWM45_INIT: initial value of PWM45
	  | (0 << 12)  // PWM23_INIT: initial value of PWM23
	  | (1 << 13)  // INDEP: 0 = complementary outputs, 1 = independent outputs
	  | (1 << 14)  // WAITEN: 1 = run in wait mode too
	  | (1 << 15)  // DBGEN: 1 = run during debugger stop too
	;

	regs->FRCTRL = 0;  // turn off the fractional logic
	regs->OCTRL = 0;   // do not invert signals, 0 on fault

	regs->DMAEN = 0;   // do not enable the DMA yet

	regs->TCTRL = 0;
	regs->DISMAP[0] = 0; // do not disable on fault
	regs->DISMAP[1] = 0; // do not disable on fault

	regs->FRACVAL1 = 0;
	regs->FRACVAL2 = 0;
	regs->FRACVAL3 = 0;
	regs->FRACVAL4 = 0;
	regs->FRACVAL5 = 0;

	// we use only VAL3, and VAL5 for PWM generation

	regs->VAL0 = 0; // set to half period
	regs->VAL1 = 0;
	regs->VAL2 = 0x0000;
	regs->VAL3 = 0x0000;
	regs->VAL4 = 0x0000;
	regs->VAL5 = 0x0000;

	commonregs->DTSRCSEL = 0;
	commonregs->MASK = 0;

	if (outnum) // B (PWM45)
	{
		valreg = &regs->VAL5;
		valregfrac = &regs->FRACVAL5;
		outenbit = (0x0010 << chnum);
	}
	else // A (PWM23)
	{
		valreg = &regs->VAL3;
		valregfrac = &regs->FRACVAL3;
		outenbit = (0x0100 << chnum);
	}

	*valreg = 0; // set the default to 0%

	commonregs->MCTRL2 = 0;

	SetFrequency(frequency);

	// start the submodule
	commonregs->MCTRL |= (0x0111 << chnum);

	initialized = true;
	return true;
}

void THwPwmChannel_imxrt::SetFrequency(uint32_t afrequency)
{
	bool isrunning = ((commonregs->MCTRL & (0x100 << chnum)) != 0);

	if (isrunning) commonregs->MCTRL |= (0x0010 << chnum); // clear load ok

	frequency = afrequency;

	uint32_t prsc = 0;
	unsigned baseclock = SystemCoreClock / 4;
	cpu_clock_shifts = 2;
	periodclocks = baseclock / frequency;

	while (periodclocks > 65535)
	{
		++prsc;
		periodclocks = (periodclocks >> 1);
		++cpu_clock_shifts;
	}

	// update the prescaler
	uint32_t tmp = regs->CTRL;
	tmp &= ~(7 << 4);
	tmp |= (prsc << 4);
	regs->CTRL = tmp;

	regs->VAL0 = (periodclocks >> 1); // set to half period (reload point)
	regs->VAL1 = periodclocks-1;

	if (isrunning)  commonregs->MCTRL |= (0x0001 << chnum);
}


void THwPwmChannel_imxrt::SetOnClocks(uint16_t aclocks)
{
	commonregs->MCTRL |= (0x0010 << chnum); // clear load ok
	*valreg = aclocks;
	commonregs->MCTRL |= (0x0001 << chnum);
}

void THwPwmChannel_imxrt::EnableFracMode(bool aenable)
{
	commonregs->MCTRL |= (0x0010 << chnum); // clear load ok
	if (aenable)
	{
		regs->FRCTRL |= ((1 << 4) | (1 << 2) | (1 << 1));
	}
	else
	{
		regs->FRCTRL &= ~((1 << 4) | (1 << 2) | (1 << 1));
	}
}

void THwPwmChannel_imxrt::SetOnClocksFrac(uint32_t aclocks_x16)
{
	commonregs->MCTRL |= (0x0010 << chnum); // clear load ok
	*(uint32_t *)valregfrac = aclocks_x16;  // write both registers
	commonregs->MCTRL |= (0x0001 << chnum);
}

void THwPwmChannel_imxrt::Enable()
{
	commonregs->OUTEN |= outenbit;
}

void THwPwmChannel_imxrt::Disable()
{
	commonregs->OUTEN &= ~outenbit;
}

