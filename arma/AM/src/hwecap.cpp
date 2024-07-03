/*
 * hwecap.cpp
 *
 *  Created on: Jun 30, 2024
 *      Author: vitya
 */

#include <hwecap.h>
#include "hw_utils.h"

bool THwEcap::Init(uint32_t adevnum)
{
	if (0 == adevnum)
	{
		regs = (THwEcapRegs *)hw_memmap(HW_ECAP0_BASE, sizeof(THwEcapRegs));
	}
	else if (1 == adevnum)
	{
		regs = (THwEcapRegs *)hw_memmap(HW_ECAP1_BASE, sizeof(THwEcapRegs));
	}
	else if (2 == adevnum)
	{
		regs = (THwEcapRegs *)hw_memmap(HW_ECAP2_BASE, sizeof(THwEcapRegs));
	}
	else
	{
		regs = nullptr;
		return false;
	}

	regs->ECCTL1 = (0
		| (3  << 14)  // FREE_SOFT(2)
		| (0  <<  9)  // PRESCALE(5): 0 = bypass the prescaler
		| (0  <<  8)  // CAPLDEN
		| (0  <<  7)  // CTRRST4
		| (0  <<  6)  // CAP4POL
		| (0  <<  5)  // CTRRST3
		| (0  <<  4)  // CAP3POL
		| (0  <<  3)  // CTRRST2
		| (0  <<  2)  // CAP2POL
		| (0  <<  1)  // CTRRST0
		| (0  <<  0)  // CAP0POL
	);

	regs->ECCTL2 = (0
		| (0  << 10)  // APWMPOL
		| (0  <<  9)  // CAP_APWM: 0 = capture mode, 1 = APWM mode
		| (0  <<  8)  // SWSYNC
		| (0  <<  6)  // SYNCO_SEL
		| (0  <<  5)  // SYNCI_EN
		| (1  <<  4)  // TSCTRSTOP: 1 = free running mode
		| (0  <<  3)  // RE-ARM
		| (0  <<  1)  // STOP_WRAP(2)
		| (0  <<  0)  // CONT_ONESHT
	);

	regs->ECEINT = 0; // disable interrupts

	return true;
}

bool THwEcap::InitPwm(uint32_t adevnum, uint32_t aperclocks, uint32_t adutyclocks)
{
	if (0 == adevnum)
	{
		regs = (THwEcapRegs *)hw_memmap(HW_ECAP0_BASE, sizeof(THwEcapRegs));
	}
	else if (1 == adevnum)
	{
		regs = (THwEcapRegs *)hw_memmap(HW_ECAP1_BASE, sizeof(THwEcapRegs));
	}
	else if (2 == adevnum)
	{
		regs = (THwEcapRegs *)hw_memmap(HW_ECAP2_BASE, sizeof(THwEcapRegs));
	}
	else
	{
		regs = nullptr;
		return false;
	}

	regs->ECEINT = 0; // disable interrupts

	regs->ECCTL1 = (0
		| (3  << 14)  // FREE_SOFT(2): 3 = do not stop on debugger
		| (0  <<  9)  // PRESCALE(5): 0 = bypass the prescaler
		| (0  <<  8)  // CAPLDEN
		| (0  <<  7)  // CTRRST4
		| (0  <<  6)  // CAP4POL
		| (0  <<  5)  // CTRRST3
		| (0  <<  4)  // CAP3POL
		| (0  <<  3)  // CTRRST2
		| (0  <<  2)  // CAP2POL
		| (0  <<  1)  // CTRRST0
		| (0  <<  0)  // CAP0POL
	);

	regs->ECCTL2 = (0
		| (0  << 10)  // APWMPOL
		| (1  <<  9)  // CAP_APWM: 0 = capture mode, 1 = APWM mode
		| (0  <<  8)  // SWSYNC
		| (0  <<  6)  // SYNCO_SEL
		| (0  <<  5)  // SYNCI_EN
		| (0  <<  4)  // TSCTRSTOP: 1 = free running mode
		| (0  <<  3)  // RE-ARM
		| (0  <<  1)  // STOP_WRAP(2)
		| (0  <<  0)  // CONT_ONESHT
	);

	regs->CAP1 = aperclocks-1;
	regs->CAP2 = adutyclocks;

	regs->ECCTL2 |= (1 << 4); // start the counter

	return true;
}
