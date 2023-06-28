/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     hwpins_msp.cpp
 *  brief:    MSP Pin/Pad and GPIO configuration
 *  date:     2023-06-27
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"

#define MAX_PORT_NUMBER   2

uint32_t atsam_port_pull_dir[MAX_PORT_NUMBER];

HW_GPIO_REGS * THwPinCtrl_msp::GetGpioRegs(int aportnum)
{
	if ((aportnum < 0) || (aportnum >= MAX_PORT_NUMBER))
	{
		return nullptr;
	}
	else
	{
		if (aportnum == PORTNUM_B)
		{
			return (GPIO_Regs *)(GPIOB_BASE);
		}
		else
		{
			return (GPIO_Regs *)(GPIOA_BASE);
		}
	}
}


bool THwPinCtrl_msp::PinSetup(int aportnum, int apinnum, unsigned flags)
{
	HW_GPIO_REGS * regs = GetGpioRegs(aportnum);
	if (!regs)
	{
		return false;
	}

	if ((apinnum < 0) || (apinnum > 31))
	{
		return false;
	}

	// 1. turn on port power
	GpioPortEnable(aportnum);

#if 0

	// prepare pin configuration
	unsigned n = (1 << 1); // enable input

	if (flags & PINCFG_DRIVE_STRONG)
	{
		n |= (1 << 6);
	}

	if (flags & PINCFG_AF_MASK)
	{
		n |= (1 << 0);

		unsigned pmidx = (apinnum >> 1);
		unsigned pmshift = (apinnum & 1) * 4;

		regs->PMUX[pmidx].reg &= ~(0xF << pmshift);
		regs->PMUX[pmidx].reg |= (((flags >> PINCFG_AF_SHIFT) & 0xF) << pmshift);
	}
	else
	{
		// GPIO

		if (flags & PINCFG_OUTPUT)
		{
		  if (flags & PINCFG_GPIO_INIT_1)
		  {
		  	regs->OUTSET.reg = (1 << apinnum);
		  }
		  else
		  {
		  	regs->OUTCLR.reg = (1 << apinnum);
		  }
			regs->DIRSET.reg = (1 << apinnum);
		}
		else
		{
			regs->DIRCLR.reg = (1 << apinnum);
		}
	}

  regs->PINCFG[apinnum].reg = n;

  if (flags & PINCFG_PULLUP)
  {
  	regs->PINCFG[apinnum].reg |= (1 << 2); // pull enable, unfortunately the direction comes from the output reg
  	regs->OUTSET.reg = (1 << apinnum);
  	atsam_port_pull_dir[aportnum] |= (1 << apinnum); // save for later
  }

  if (flags & PINCFG_PULLDOWN)
  {
  	regs->PINCFG[apinnum].reg |= (1 << 2);
  	regs->OUTCLR.reg = (1 << apinnum);
  	atsam_port_pull_dir[aportnum] &= ~(1 << apinnum);  // save for later
  }

#endif

  return true;
}

bool THwPinCtrl_msp::GpioPortEnable(int aportnum)
{
	if ((aportnum < 0) || (aportnum >= MAX_PORT_NUMBER))
	{
		return false;
	}

	//PM->APBBMASK.reg |= PM_APBBMASK_PORT;

  return true;
}

void THwPinCtrl_msp::GpioSet(int aportnum, int apinnum, int value)
{
#if 0
	HW_GPIO_REGS * regs = (HW_GPIO_REGS *)&(PORT->Group[aportnum]);

  if (1 == value)
  {
  	regs->OUTSET.reg = (1 << apinnum);
  }
  else if (value & 2) // toggle
  {
  	regs->OUTTGL.reg = (1 << apinnum);
  }
  else
  {
  	regs->OUTCLR.reg = (1 << apinnum);
  }
#endif
}

void THwPinCtrl_msp::GpioIrqSetup(int aportnum, int apinnum, int amode)
{

}

// GPIO Port

void TGpioPort_msp::Assign(int aportnum)
{
	regs = hwpinctrl.GetGpioRegs(aportnum);
}

void TGpioPort_msp::Set(unsigned value)
{
	//regs->OUT.reg = value;
}

// GPIO Pin

void TGpioPin_msp::Assign(int aportnum, int apinnum, bool ainvert)
{
	portnum = aportnum;
  pinnum = apinnum;
  inverted = ainvert;

	regs = hwpinctrl.GetGpioRegs(aportnum);
	if (!regs)
	{
		return;
	}

	setbitvalue = (1 << pinnum);
	clrbitvalue = (1 << pinnum);
#if 0
  getbitptr    = (unsigned *)&(regs->IN);
  getoutbitptr = (unsigned *)&(regs->OUT);
  getbitshift = pinnum;
  togglebitptr = (unsigned *)&(regs->OUTTGL.reg);

  if (ainvert)
  {
    setbitptr = (unsigned *)&(regs->OUTCLR);
    clrbitptr = (unsigned *)&(regs->OUTSET);
  }
  else
  {
    setbitptr = (unsigned *)&(regs->OUTSET);
    clrbitptr = (unsigned *)&(regs->OUTCLR);
  }
#endif
}

void TGpioPin_msp::SwitchDirection(int adirection)
{
#if 0
	if (adirection)
	{
		regs->DIRSET.reg = setbitvalue;
	}
	else
	{
		regs->DIRCLR.reg = setbitvalue;
		if (regs->PINCFG[pinnum].bit.PULLEN)
		{
			if (atsam_port_pull_dir[portnum] & setbitvalue)
			{
				regs->OUTSET.reg = setbitvalue;  // pull up
			}
			else
			{
				regs->OUTCLR.reg = setbitvalue;  // pull down
			}
		}
	}
#endif
}
