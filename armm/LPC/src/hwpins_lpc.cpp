
#include "platform.h"
#include "hwpins.h"

#if defined(MCUSF_17XX)
	#define MAX_PORT_NUMBER         5
	#define MAX_GPIO_PORT_NUMBER    5
#else
  #error "Unknown subfamily"
#endif

HW_GPIO_REGS * THwPinCtrl_lpc::GetGpioRegs(int aportnum)
{
	if ((aportnum < 0) || (aportnum >= MAX_GPIO_PORT_NUMBER))
	{
		return 0;
	}

	return (HW_GPIO_REGS *)(LPC_GPIO0_BASE + (LPC_GPIO1_BASE - LPC_GPIO0_BASE) * aportnum);
}

bool THwPinCtrl_lpc::PinSetup(int aportnum, int apinnum, unsigned flags)
{

	if ((aportnum < 0) || (aportnum >= MAX_PORT_NUMBER))
	{
		return false;
	}

	if ((apinnum < 0) || (apinnum > 31))
	{
		return false;
	}

	unsigned n;
	volatile uint32_t * preg;
	unsigned regidx;
	unsigned regshift;
	unsigned pinmask = (1u << apinnum);

	GpioPortEnable();

	// 1. setup pullup-pulldown in PINMODEx

	if (flags & PINCFG_PULLDOWN)
	{
		n = 3;
	}
	else if (flags & PINCFG_PULLUP)
	{
		n = 0;
	}
	else
	{
		n = 1; // use the repeater mode (pull up oder pull down depending of the sensed state)
	}
	regidx = (aportnum << 1) | (apinnum >> 4);
	regshift = ((apinnum << 1) & 0x1F);
	preg = &LPC_PINCON->PINMODE0;
	preg += regidx;
	*preg &= ~(3 << regshift);
	*preg |=  (n << regshift);

	// 2. setup open drain

	preg = &LPC_PINCON->PINMODE_OD0;
	preg += aportnum;
	if (flags & PINCFG_OPENDRAIN)
	{
		*preg |= (1 << apinnum);
	}
	else
	{
		*preg &= ~(1 << apinnum);
	}

	// 3. setup AF
	if (flags & PINCFG_AF_MASK)
	{
		n = ((flags >> PINCFG_AF_SHIFT) & 0x03);
	}
	else
	{
		n = 0;
	}
	regidx = (aportnum << 1) | (apinnum >> 4);
	regshift = ((apinnum << 1) & 0x1F);
	preg = &LPC_PINCON->PINSEL0;
	preg += regidx;
	*preg &= ~(3 << regshift);
	*preg |=  (n << regshift);

	// 4. prepare GPIO
	HW_GPIO_REGS * gpioregs = GetGpioRegs(aportnum);
	if (0 == (flags & PINCFG_AF_MASK))
	{
		gpioregs->FIOMASK &= ~pinmask;

		if (flags & PINCFG_OUTPUT)
		{
			if (flags & PINCFG_GPIO_INIT_1)
			{
				gpioregs->FIOSET = pinmask;
			}
			else
			{
				gpioregs->FIOCLR = pinmask;
			}

			gpioregs->FIODIR |= pinmask;
		}
		else
		{
			gpioregs->FIODIR &= ~pinmask;
		}
	}

	return true;
}

bool THwPinCtrl_lpc::GpioPortEnable()
{
	//LPC_CCU1->CLKCCU[CLK_MX_GPIO].CFG |= 1;
  return true;
}

void THwPinCtrl_lpc::GpioSet(int aportnum, int apinnum, int value)
{
	HW_GPIO_REGS * gpioregs = GetGpioRegs(aportnum);

	unsigned bm = (1 << apinnum);

  if (1 == value)
  {
  	gpioregs->FIOSET = bm;
  }
  else if (value & 2) // toggle
  {
  	if (gpioregs->FIOPIN & bm)
  	{
    	gpioregs->FIOCLR = bm;
  	}
  	else
  	{
    	gpioregs->FIOSET = bm;
  	}
  }
  else
  {
  	gpioregs->FIOCLR = bm;
  }
}

void THwPinCtrl_lpc::GpioIrqSetup(int aportnum, int apinnum, int amode)
{

}

// GPIO Port

void TGpioPort_lpc::Assign(int aportnum)
{
	portnum = aportnum;
	HW_GPIO_REGS * gpioregs = hwpinctrl.GetGpioRegs(aportnum);
	if (gpioregs)
	{
		portptr = (volatile unsigned *)(gpioregs->FIOPIN);
	}
}

void TGpioPort_lpc::Set(unsigned value)
{
	*portptr = value;
}

// GPIO Pin

void TGpioPin_lpc::Assign(int aportnum, int apinnum, bool ainvert)
{
	pinnum = 0xFF;

	if ((aportnum < 0) || (aportnum >= MAX_GPIO_PORT_NUMBER))
	{
		return;
	}

	if ((apinnum < 0) || (apinnum > 31))
	{
		return;
	}

	regs = hwpinctrl.GetGpioRegs(aportnum);

  portnum = aportnum;
  pinnum = apinnum;
  inverted = ainvert;

	setbitvalue = (1u << pinnum);
	clrbitvalue = (1u << pinnum);

  getbitptr = (unsigned *)&(regs->FIOPIN);
  getoutbitptr = (unsigned *)&(regs->FIOPIN);
  getbitshift = apinnum;

  if (ainvert)
  {
    setbitptr = (unsigned *)&(regs->FIOCLR);
    clrbitptr = (unsigned *)&(regs->FIOSET);
  }
  else
  {
    setbitptr = (unsigned *)&(regs->FIOSET);
    clrbitptr = (unsigned *)&(regs->FIOCLR);
  }
}

void TGpioPin_lpc::Toggle()
{
	if (*getbitptr & setbitvalue)
	{
  	*clrbitptr = setbitvalue;
	}
	else
	{
  	*setbitptr = setbitvalue;
	}
}

void TGpioPin_lpc::SwitchDirection(int adirection)
{
  if (adirection)
  {
		regs->FIODIR |= setbitvalue;
	}
	else
	{
		regs->FIODIR &= ~setbitvalue;
	}
}
