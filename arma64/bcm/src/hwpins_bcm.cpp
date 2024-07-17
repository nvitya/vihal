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
#if 0
	if ((aportnum < 0) || (aportnum >= HW_GPIO_COUNT))
	{
		return false;
	}

	uint32_t * confreg = GetConfReg(aconfreg_offs);
	if (!confreg)
	{
		return false;
	}

  uint32_t padcfg = (0
    | (0  <<  6)  // SLEWCTRL: 0 = Fast, 1 = Slow
    | (0  <<  5)  // RX_ACTIVE: 0 = receiver disabled, 1 = receiver enabled
    | (0  <<  4)  // PUTYPESEL: 0 = pulldown, 1 = pullup
    | (0  <<  3)  // PUDEN: 0 = enable pullup/pulldown, 1 = disable pullup/pulldown
    | (0  <<  0)  // MMODE(3): af function select
  );

  if (flags & PINCFG_PULLUP)
  {
    padcfg |= (1 << 4);
  }

  if (flags & PINCFG_PULLDOWN)
  {
    // leave it as is
  }

  if (0 == (flags & (PINCFG_PULLDOWN | PINCFG_PULLUP)))
  {
    padcfg |= (1 << 3);  // disable pulldown
  }

  padcfg |= (1 << 5); // activate RX (input)

  if (flags & PINCFG_SPEED_SLOW)
  {
    padcfg |= (1 << 6);
  }

  if (flags & PINCFG_AF_MASK)
  {
    padcfg |= ((flags >> PINCFG_AF_SHIFT) & 0x7);
  }
  else // GPIO
  {
  	THwGpioRegs * regs = GetGpioRegs(agpio >> 8);
    if (regs)
    {
      uint32_t pinmask = (1 << (agpio & 0x1F));
      if (flags & PINCFG_OUTPUT)
      {
        if (flags & PINCFG_GPIO_INIT_1)
        {
          regs->SETDATAOUT = pinmask;
        }
        else
        {
          regs->CLEARDATAOUT = pinmask;
        }
        regs->OE &= ~pinmask;
      }
      else
      {
        regs->OE |= pinmask;
      }

      padcfg |= 7; // AF7 = GPIO
    }
  }

  *confreg = padcfg;

#endif

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

  if (1 == value)
  {
  	regs->SETDATAOUT = (1 << apinnum);
  }
  else if (value & 2) // toggle
  {
  	if (regs->DATAOUT & (1 << apinnum))
  	{
  		regs->CLEARDATAOUT = (1 << apinnum);
  	}
  	else
  	{
    	regs->SETDATAOUT = (1 << apinnum);
  	}
  }
  else
  {
  	regs->CLEARDATAOUT = (1 << apinnum);
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

  getbitshift = apinnum;
	setbitvalue = (1 << apinnum);
	clrbitvalue = setbitvalue;

  getbitptr = (unsigned *)&(regs->DATAIN);
  getoutbitptr = (unsigned *)&(regs->DATAOUT);

  if (ainvert)
  {
    setbitptr = (unsigned *)&(regs->CLEARDATAOUT);
    clrbitptr = (unsigned *)&(regs->SETDATAOUT);
  }
  else
  {
    setbitptr = (unsigned *)&(regs->SETDATAOUT);
    clrbitptr = (unsigned *)&(regs->CLEARDATAOUT);
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
	if (adirection)
	{
		regs->OE &= ~setbitvalue;
	}
	else
	{
		regs->OE |= setbitvalue;
	}
}
