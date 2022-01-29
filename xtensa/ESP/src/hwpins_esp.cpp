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
 *  file:     hwpins_esp.cpp
 *  brief:    ESP Pin/Pad and GPIO configuration
 *  version:  1.00
 *  date:     2022-01-29
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"
#include "esp_utils.h"

#define MAX_GPIOPIN   39

bool THwPinCtrl_esp::PinSetup(int aportnum, int apinnum, unsigned flags)
{
  if ((apinnum < 0) || (apinnum >= MAX_GPIOPIN))
  {
    return false;
  }

#if 0

  // remove resets
  rp_reset_control(RESETS_RESET_PADS_BANK0_BITS | RESETS_RESET_IO_BANK0_BITS, false);

	// port power control is not necessary ?

	uint32_t padcfg;

	padcfg = padsbank0_hw->io[apinnum];

	padcfg &= 0xFFFFFF00; // clear bits
	padcfg |= (0
	  | (0 << 7)  // OD: output disable
	  | (1 << 6)  // IE: input enable
	  | (0 << 4)  // DRIVE(2): 0 = 2mA, 1 = 4mA, 2 = 8mA, 3 = 12mA
	  | (0 << 3)  // PUE: 1 = pull-up enable
	  | (0 << 2)  // PDE: 1 = pull-down enable
	  | (0 << 1)  // SCHMITT: 1 = enable Smitt trigger
	  | (0 << 0)  // SLEWFAST: 1 = fast slew rate
	);

	// prepare pin configuration
	if (flags & PINCFG_DRIVE_STRONG)
	{
		padcfg |= (2 << 4);  // set to 8 mA
	}
	else
	{
    padcfg |= (1 << 4);  // set to 4 mA
	}

  if (flags & PINCFG_SPEED_FAST)
  {
    padcfg |= (1 << 0);  // set to 8 mA
  }

  if (flags & PINCFG_PULLUP)
  {
    padcfg |= (1 << 3);
  }

  if (flags & PINCFG_PULLDOWN)
  {
    padcfg |= (1 << 2);
  }

  if (0 == (flags & PINCFG_ANALOGUE))
  {
    padcfg |= (1 << 1); // enable the Smitt trigger
  }

  padsbank0_hw->io[apinnum] = padcfg;

  uint32_t ctrl = iobank0_hw->io[apinnum].ctrl;
  uint32_t pinmask = (1 << apinnum);

  //ctrl &= 0xCFFCCCE0; // reset all to defaults
  ctrl = 0x00000000; // reset all to defaults
	if (flags & PINCFG_AF_MASK)
	{
	  ctrl |= ((flags >> PINCFG_AF_SHIFT) & 0xF);  // only 16 functions are supported here !
	}
	else
	{
		// GPIO
    ctrl |= 5;  // 5 = SIO (single clock IO)

		if (flags & PINCFG_OUTPUT)
		{
		  if (flags & PINCFG_GPIO_INIT_1)
		  {
		    sio_hw->gpio_set = pinmask;
		  }
		  else
		  {
        sio_hw->gpio_clr = pinmask;
		  }

      sio_hw->gpio_oe_set = pinmask;
		}
		else
		{
      sio_hw->gpio_oe_clr = pinmask;
		}
	}

	iobank0_hw->io[apinnum].ctrl = ctrl;

#endif

  return true;
}

void THwPinCtrl_esp::GpioSet(int aportnum, int apinnum, int value)
{
  uint32_t pinmask = (1 << apinnum);

#if 0

  if (1 == value)
  {
    sio_hw->gpio_set = pinmask;
  }
  else if (value & 2) // toggle
  {
    sio_hw->gpio_togl = pinmask;
  }
  else
  {
    sio_hw->gpio_clr = pinmask;
  }
#endif
}

void THwPinCtrl_esp::GpioIrqSetup(int aportnum, int apinnum, int amode)
{

}

// GPIO Port

void TGpioPort_esp::Assign(int aportnum)
{
	//regs = sio_hw;
}

void TGpioPort_esp::Set(unsigned value)
{
  regs->OUT = value;
}

// GPIO Pin

void TGpioPin_esp::Assign(int aportnum, int apinnum, bool ainvert)
{
	portnum = aportnum;
  pinnum = apinnum;
  inverted = ainvert;

	//regs = sio_hw;

#if 0

	setbitvalue = (1 << pinnum);
	clrbitvalue = (1 << pinnum);
  getbitptr    = (unsigned *)&(regs->gpio_in);
  getoutbitptr = (unsigned *)&(regs->gpio_out);
  getbitshift = pinnum;

  if (ainvert)
  {
    setbitptr = (unsigned *)&(regs->gpio_clr);
    clrbitptr = (unsigned *)&(regs->gpio_set);
  }
  else
  {
    setbitptr = (unsigned *)&(regs->gpio_set);
    clrbitptr = (unsigned *)&(regs->gpio_clr);
  }
#endif
}

void TGpioPin_esp::Toggle()
{

}

void TGpioPin_esp::SwitchDirection(int adirection)
{
#if 0
	if (adirection)
	{
		regs->gpio_oe_set = setbitvalue;
	}
	else
	{
		regs->gpio_oe_clr = setbitvalue;
	}
#endif
}
