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
 *  file:     hwpins_gd32v.cpp
 *  brief:    GD32V Pin/Pad and GPIO configuration
 *  version:  1.00
 *  date:     2021-10-20
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"

#if !defined(MAX_PORT_NUMBER)
  #define MAX_PORT_NUMBER  5
#endif

gd32v_gpio_t * THwPinCtrl_gd32v::GetGpioRegs(int aportnum)
{
	if ((aportnum < 0) || (aportnum >= MAX_PORT_NUMBER))
	{
		return nullptr;
	}
	else
	{
		return (gd32v_gpio_t *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(aportnum));
	}
}

bool THwPinCtrl_gd32v::PinSetup(int aportnum, int apinnum, unsigned flags)
{
  gd32v_gpio_t * regs = GetGpioRegs(aportnum);
	if (!regs)
	{
		return false;
	}

  // turn on port power
  GpioPortEnable(aportnum);

	uint32_t pinmask = (1 << apinnum);

  unsigned md = 0;
  unsigned ctl = 0;

  if (flags & PINCFG_OUTPUT)
  {
    if ((flags & PINCFG_SPEED_MASK) == PINCFG_SPEED_SLOW)
    {
      md = GPIO_OSPEED_2MHZ;
    }
    else if ((flags & PINCFG_SPEED_MASK) == PINCFG_SPEED_FAST)
    {
      md = GPIO_OSPEED_50MHZ;
    }
    else
    {
      md = GPIO_OSPEED_10MHZ;  // may be too slow !
    }

    if (flags & PINCFG_OPENDRAIN)
    {
      ctl = 1;
    }

    if (flags & PINCFG_AF_MASK)
    {
      ctl = 2;
    }

    if (flags & PINCFG_GPIO_INIT_1)
    {
      regs->BOP = pinmask;
    }
    else
    {
      regs->BOP = (pinmask << 16);
    }
  }
  else  // input
  {
    // leave the mode bits at 0 = input mode
    if (flags & PINCFG_ANALOGUE)
    {
      // ctl = 0; // already set
    }
    else if (flags & PINCFG_PULLUP)
    {
      ctl = 2;
      regs->BOP = pinmask;
    }
    else if (flags & PINCFG_PULLDOWN)
    {
      ctl = 2;
      regs->BOP = pinmask << 16;
    }
    else
    {
      ctl = 1;  // floating input
    }
  }

  unsigned pinconf = ((ctl << 2) | md);

  // set the pinconf reg
  uint32_t tmp;
  uint32_t regidx = (apinnum >> 3);
  uint32_t cfgshift = ((apinnum & 7) << 2);

  tmp = regs->CTL[regidx];
  tmp &= ~(0xF    << cfgshift);
  tmp |= (pinconf << cfgshift);
  regs->CTL[regidx] = tmp;

  return true;
}

bool THwPinCtrl_gd32v::GpioPortEnable(int aportnum)
{
  if ((aportnum < 0) || (aportnum >= MAX_PORT_NUMBER))
  {
    return false;
  }

  RCU->APB2EN |= (RCU_APB2EN_PAEN << aportnum);

  return true;
}

void THwPinCtrl_gd32v::GpioSet(int aportnum, int apinnum, int value)
{
  gd32v_gpio_t * regs = (gd32v_gpio_t *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(aportnum));

	uint32_t pinmask = (1 << apinnum);

  if (1 == value)
  {
  	regs->BOP = pinmask;
  }
  else if (value & 2) // toggle
  {
  	if (regs->ISTAT & pinmask)
  	{
    	regs->BOP = (pinmask << 16);
  	}
  	else
  	{
  		regs->BOP = pinmask;
  	}
  }
  else
  {
  	regs->BOP = (pinmask << 16);
  }
}

void THwPinCtrl_gd32v::GpioIrqSetup(int aportnum, int apinnum, int amode)
{

}

// GPIO Port

void TGpioPort_gd32v::Assign(int aportnum)
{
	if ((aportnum < 0) || (aportnum > MAX_PORT_NUMBER))
	{
		regs = nullptr;
		return;
	}

  regs = (gd32v_gpio_t *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(aportnum));
  portptr = (volatile unsigned *)&(regs->OCTL);
}

void TGpioPort_gd32v::Set(unsigned value)
{
  *portptr = value;
}

// GPIO Pin

void TGpioPin_gd32v::Assign(int aportnum, int apinnum, bool ainvert)
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


  setbitptr = (unsigned *)&(regs->BOP);
  clrbitptr = (unsigned *)&(regs->BOP);
  getbitptr = (unsigned *)&(regs->ISTAT);
  getoutbitptr = (unsigned *)&(regs->OCTL);
  getbitshift = apinnum;

  if (ainvert)
  {
    setbitvalue = (1 << pinnum) << 16;
    clrbitvalue = (1 << pinnum);
  }
  else
  {
    setbitvalue = (1 << pinnum);
    clrbitvalue = (1 << pinnum) << 16;
  }
}

void TGpioPin_gd32v::Toggle()
{
  unsigned pinbit = (1 << pinnum);
  regs->BOP = (pinbit << 16) | (regs->OCTL ^ pinbit);
}

void TGpioPin_gd32v::SwitchDirection(int adirection)
{
  // here we override previous pull-up/down open-drain settings !

  unsigned pinconf;

  if (adirection)
  {
    pinconf = 1;  // push-pull output
  }
  else
  {
    pinconf = 4;  // floating input
  }

  // set the pinconf reg
  uint32_t tmp;
  uint32_t regidx = (pinnum >> 3);
  uint32_t cfgshift = ((pinnum & 7) << 2);

  tmp = regs->CTL[regidx];
  tmp &= ~(0xF    << cfgshift);
  tmp |= (pinconf << cfgshift);
  regs->CTL[regidx] = tmp;
}

