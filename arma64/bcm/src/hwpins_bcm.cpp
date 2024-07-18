/*
 *  file:     hwpins_bcm.cpp
 *  brief:    Broadcom Pin/Pad and GPIO configuration
 *  date:     2024-07-17
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"
#include "hwpins_bcm.h"
#include "bcm_utils.h"

#include "stdio.h"
#include "stddef.h"

gpio_regs_t * THwPinCtrl_bcm::GetGpioRegs()
{
	return (gpio_regs_t *)map_hw_addr(HW_GPIO_BASE, sizeof(gpio_regs_t), (void * *)&mapped_gpio_regs);
}

bool THwPinCtrl_bcm::PinSetup(int aportnum, int apinnum, unsigned flags)
{
	if ((apinnum < 0) || (apinnum >= HW_GPIO_MAX_PINS))
	{
		return false;
	}

	gpio_regs_t *  regs = GetGpioRegs();
	if (!regs)
	{
		return false;
	}

  uint32_t tmp, ridx, bshift;

  // 1. pull-up/down setup

  uint32_t pudcode = 0;
  if (flags & PINCFG_PULLUP)
  {
    pudcode = 1;
  }
  else if (flags & PINCFG_PULLDOWN)
  {
    pudcode = 2;
  }
  ridx   = (apinnum >> 4);
  bshift = (apinnum & 0xF) << 1;
  tmp = regs->PUP_PDN_CNTRL[ridx];
  tmp &= ~(3 << bshift);
  tmp |= (pudcode << bshift);
  regs->PUP_PDN_CNTRL[ridx] = tmp;

  // 2. Function Setup

  uint32_t func = 0;
  if (flags & PINCFG_AF_MASK)
  {
    func = ( (2 + (flags >> PINCFG_AF_SHIFT)) & 0x7 );
  }
  else // GPIO
  {
		if (flags & PINCFG_OUTPUT)
		{
			func = 1;
			if (flags & PINCFG_GPIO_INIT_1)
			{
				regs->SET[apinnum >> 5] = (1 << (apinnum & 0x1F));
			}
			else
			{
				regs->CLR[apinnum >> 5] = (1 << (apinnum & 0x1F));
			}
		}
		else
		{
			func = 0;
		}
  }
	ridx   = (apinnum / 10);
	bshift = (apinnum % 10) * 3;
	tmp = regs->FSEL[ridx];
	tmp &= ~(7 << bshift);
	tmp |= (func << bshift);
	regs->FSEL[ridx] = tmp;

  return true;
}

bool THwPinCtrl_bcm::GpioPortEnable(int aportnum)
{
  return true;
}

void THwPinCtrl_bcm::GpioSet(int aportnum, int apinnum, int value)
{
	gpio_regs_t * regs = GetGpioRegs(); // set the regs member
	if (!regs)
	{
		return;
	}

	uint32_t regidx  = (apinnum >> 5);
	uint32_t bitmask = (1 << (apinnum & 0x1F));

  if (1 == value)
  {
		regs->SET[regidx] = bitmask;
  }
  else if (value & 2) // toggle
  {
  	if (regs->LEV[regidx] & bitmask)
  	{
  		regs->CLR[regidx] = bitmask;
  	}
  	else
  	{
  		regs->SET[regidx] = bitmask;
  	}
  }
  else
  {
		regs->CLR[regidx] = bitmask;
  }
}


// GPIO Pin

void TGpioPin_bcm::Assign(int aportnum, int apinnum, bool ainvert)
{
	portnum = aportnum;
  pinnum = apinnum;
  inverted = ainvert;

	regs = hwpinctrl.GetGpioRegs();
	if (!regs)
	{
		return;
	}

	if ((apinnum < 0) || (apinnum > 31))
	{
		regs = nullptr;
		return;
	}

	uint32_t regidx  = (apinnum >> 5);
  getbitshift = (apinnum & 0x1F);
	setbitvalue = (1 << getbitshift);
	clrbitvalue = setbitvalue;

  getbitptr = (unsigned *)&(regs->LEV[regidx]);
  getoutbitptr = (unsigned *)&(regs->LEV[regidx]);

  if (ainvert)
  {
    setbitptr = (unsigned *)&(regs->CLR[regidx]);
    clrbitptr = (unsigned *)&(regs->SET[regidx]);
  }
  else
  {
    setbitptr = (unsigned *)&(regs->SET[regidx]);
    clrbitptr = (unsigned *)&(regs->CLR[regidx]);
  }
}

void TGpioPin_bcm::Toggle()
{
	// generic code without hw toggle support:
	if (Value())
	{
		Set0();
	}
	else
	{
		Set1();
	}
}

void TGpioPin_bcm::SwitchDirection(int adirection)
{
  uint32_t func;
	if (adirection)
	{
		func = 1;
	}
	else
	{
		func = 0;
	}
	uint32_t ridx   = (pinnum / 10);
	uint32_t bshift = (pinnum % 10) * 3;
	uint32_t tmp = regs->FSEL[ridx];
	tmp &= ~(7 << bshift);
	tmp |= (func << bshift);
	regs->FSEL[ridx] = tmp;
}
