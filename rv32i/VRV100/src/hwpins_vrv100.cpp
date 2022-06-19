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
 *  file:     hwpins_vrv100.cpp
 *  brief:    VRV100 Pin/Pad and GPIO configuration
 *  created:  2021-10-02
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"

#if !defined(MAX_PORT_NUMBER)
  #define MAX_PORT_NUMBER  2
#endif

vexriscv_gpio_t * THwPinCtrl_vrv100::GetGpioRegs(int aportnum)
{
	if ((aportnum < 0) || (aportnum >= MAX_PORT_NUMBER))
	{
		return nullptr;
	}
	else
	{
		return (vexriscv_gpio_t *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(aportnum));
	}
}

bool THwPinCtrl_vrv100::PinSetup(int aportnum, int apinnum, unsigned flags)
{
	// STM32 version
	vexriscv_gpio_t * regs = GetGpioRegs(aportnum);
	if (!regs)
	{
		return false;
	}

	uint32_t pinmask = (1 << apinnum);

  if (flags & PINCFG_GPIO_INIT_1)
  {
  	regs->OUTSET = pinmask;
  }
  else
  {
  	regs->OUTCLR = pinmask;
  }

  if (flags & PINCFG_OUTPUT)
  {
  	regs->DIRSET = pinmask;
  }
  else
  {
  	regs->DIRCLR = pinmask;
  }

  return true;
}

void THwPinCtrl_vrv100::GpioSet(int aportnum, int apinnum, int value)
{
	vexriscv_gpio_t * regs = (vexriscv_gpio_t *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(aportnum));

	uint32_t pinmask = (1 << apinnum);

  if (1 == value)
  {
  	regs->OUTSET = pinmask;
  }
  else if (value & 2) // toggle
  {
  	if (regs->OUTDATA & pinmask)
  	{
    	regs->OUTCLR = pinmask;
  	}
  	else
  	{
  		regs->OUTSET = pinmask;
  	}
  }
  else
  {
  	regs->OUTCLR = pinmask;
  }
}

void THwPinCtrl_vrv100::GpioIrqSetup(int aportnum, int apinnum, int amode)
{

}

// GPIO Port

void TGpioPort_vrv100::Assign(int aportnum)
{
	if ((aportnum < 0) || (aportnum > MAX_PORT_NUMBER))
	{
		regs = nullptr;
		return;
	}

  regs = (vexriscv_gpio_t *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(aportnum));
  portptr = (volatile unsigned *)&(regs->OUTDATA);
}

void TGpioPort_vrv100::Set(unsigned value)
{
  *portptr = value;
}

// GPIO Pin

void TGpioPin_vrv100::Assign(int aportnum, int apinnum, bool ainvert)
{
	portnum = aportnum;
  pinnum = apinnum;
  inverted = ainvert;

	regs = hwpinctrl.GetGpioRegs(aportnum);
	if (!regs)
	{
		return;
	}

	if ((apinnum < 0) || (apinnum > 15))
	{
		regs = nullptr;
		return;
	}

	setbitvalue = (1 << pinnum);
	clrbitvalue = (1 << pinnum);

  getbitptr = (unsigned *)&(regs->INDATA);
  getoutbitptr = (unsigned *)&(regs->OUTDATA);
  getbitshift = apinnum;

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
}

void TGpioPin_vrv100::Toggle()
{
  unsigned pinmask = (1 << pinnum);
	if (regs->OUTDATA & pinmask)
	{
  	regs->OUTCLR = pinmask;
	}
	else
	{
		regs->OUTSET = pinmask;
	}
}

void TGpioPin_vrv100::SwitchDirection(int adirection)
{
  if (adirection)
  {
  	regs->DIRSET = (1 << pinnum);
  }
  else
  {
  	regs->DIRCLR = (1 << pinnum);
  }
}

