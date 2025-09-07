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
 *  file:     hwpins_rk.cpp
 *  brief:    RK Pin/Pad and GPIO configuration
 *  created:  2025-07-29
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins_rk.h"
#include "hwpins.h"
#include "rk_utils.h"

GPIO_REG * THwPinCtrl_rk::GetGpioRegs(int aportnum)
{
	if ((aportnum < 1) || (aportnum > 5))
	{
		return nullptr;
	}

	GPIO_REG * regs = mapped_gpio_regs[aportnum];
	if (!regs)
	{
		const uint32_t gpio_base_addresses[5] =
		{
				GPIO0_BASE, GPIO1_BASE, GPIO2_BASE, GPIO3_BASE, GPIO4_BASE
		};

		regs = (GPIO_REG *)map_hw_addr(gpio_base_addresses[aportnum], sizeof(GPIO_REG), (void * *)&mapped_gpio_regs[aportnum]);
	}

	return regs;
}

bool THwPinCtrl_rk::PinSetup(int aportnum, int apinnum, unsigned flags)
{
	if ((aportnum < 0) || (aportnum > 4))
	{
		return false;
	}

	if (!map_hw_addr(GPIO0_IOC_BASE, 4096, (void * *)&mapped_ioc))
	{
		return false;
	}

	apinnum &= 0x1F;

	uint32_t pinpos, value;

	// Open-Drain
  if (flags & PINCFG_OPENDRAIN)
  {
  	value = 1;
  }
  else
  {
  	value = 0;
  }
	volatile uint32_t * reg_od = (volatile uint32_t *)(&mapped_ioc[0x700 + (aportnum << 4) + 4 * (apinnum >> 3)]);
	pinpos = (apinnum & 0x7);
	*reg_od = (7 << (16 + pinpos)) | (value << pinpos);

  // Pull-Up/Down
  if (flags & PINCFG_PULLUP)
  {
    value = 1;
  }
  else if (flags & PINCFG_PULLDOWN)
  {
    value = 2;
  }
  else
  {
  	value = 0;
  }
	pinpos = ((apinnum & 0xF) << 1);
	volatile uint32_t * reg_pupd = (volatile uint32_t *)(&mapped_ioc[0x200 + (aportnum << 4) + 4 * (apinnum >> 3)]);
	*reg_pupd = ((3 << (16 + pinpos)) | (value << pinpos));

	// Slew Rate
  if (PINCFG_SPEED_SLOW == (flags & PINCFG_SPEED_MASK))
  {
    value = 0;
  }
  else
  {
  	value = 3;
  }
	volatile uint32_t * reg_sl = (volatile uint32_t *)(&mapped_ioc[0x600 + (aportnum << 4) + 4 * (apinnum >> 3)]);
	pinpos = ((apinnum & 0x7) << 1);
	*reg_sl = (3 << (16 + pinpos)) | (value << pinpos);

	// Drive Strength
  if (PINCFG_DRIVE_WEAK == (flags & PINCFG_DRIVE_MASK))
  {
    value = 1;
  }
  else if (PINCFG_DRIVE_STRONG == (flags & PINCFG_DRIVE_MASK))
  {
    value = 31;
  }
  else
  {
    value = 7;
  }
	volatile uint32_t * reg_ds = (volatile uint32_t *)(&mapped_ioc[0x100 + (aportnum << 6) + 4 * (apinnum >> 1)]);
	pinpos = ((apinnum & 0x1) << 8);
	*reg_ds = (63 << (16 + pinpos)) | (value << pinpos);

  // Disable Schmitt trigger for the analogue
  if (0 == (flags & PINCFG_ANALOGUE))
  {
  	value = 0; // disable Schmitt trigger
  }
  else
  {
  	value = 1; // enable the Schmitt trigger
  }
	volatile uint32_t * reg_smt = (volatile uint32_t *)(&mapped_ioc[0x400 + (aportnum << 4) + 4 * (apinnum >> 3)]);
	pinpos = (apinnum & 0x7);
	*reg_smt = (1 << (16 + pinpos)) | (value << pinpos);

	// IO MUX + GPIO
  if (flags & PINCFG_AF_MASK)
  {
  	value = ((flags & PINCFG_AF_MASK) >> PINCFG_AF_SHIFT);
  }
  else // gpio
  {
  	value = 0;
    //uint32_t pinmask16 = (1 << (apinnum & 0x0F));
		if (flags & PINCFG_OUTPUT)
		{
			if (flags & PINCFG_GPIO_INIT_1)
			{
				GpioSet(aportnum, apinnum, 1);
			}
			else
			{
				GpioSet(aportnum, apinnum, 0);
			}
			GpioDir(aportnum, apinnum, 1);
		}
		else
		{
			GpioDir(aportnum, apinnum, 0);
		}
  }
	volatile uint32_t * reg_iomux = (volatile uint32_t *)(&mapped_ioc[(aportnum << 5) | (apinnum & 0x1C)]);
	pinpos = ((apinnum & 3) << 2);
	*reg_iomux = (0xF << (16 + pinpos)) | (value << pinpos);

  return true;
}

bool THwPinCtrl_rk::RmioSetup(int rmionum, int funcnum)
{
	if ((rmionum < 0) || (rmionum > 31))
	{
		return false;
	}

	if (!map_hw_addr(RM_IO_BASE, 4096, (void * *)&mapped_rm_io))
	{
		return false;
	}

	funcnum &= 0x7F;
	volatile uint32_t * reg_sel = (volatile uint32_t *)(&mapped_rm_io[rmionum << 4]);
	*reg_sel = ((0x7F << 16) | funcnum);

	return true;
}

void THwPinCtrl_rk::GpioSet(int aportnum, int apinnum, int value)
{
  GPIO_REG * regs = GetGpioRegs(aportnum);
  if (regs)
  {
  	uint32_t * setbitptr;
  	if (apinnum >= 16)
  	{
      setbitptr = (uint32_t *)&regs->SWPORT_DR_H;
  	}
  	else
  	{
      setbitptr = (uint32_t *)&regs->SWPORT_DR_L;
  	}

  	uint32_t bitmask = (1 << (apinnum & 0xF));
    if (value)
    {
      *setbitptr = (bitmask << 16) | bitmask;
    }
    else
    {
      *setbitptr = (bitmask << 16);
    }
  }
}

void THwPinCtrl_rk::GpioDir(int aportnum, int apinnum, int value)
{
  GPIO_REG * regs = GetGpioRegs(aportnum);
  if (regs)
  {
  	uint32_t * setbitptr;
  	if (apinnum >= 16)
  	{
      setbitptr = (uint32_t *)&regs->SWPORT_DDR_H;
  	}
  	else
  	{
      setbitptr = (uint32_t *)&regs->SWPORT_DDR_L;
  	}

  	uint32_t bitmask = (1 << (apinnum & 0xF));
    if (value)
    {
      *setbitptr = (bitmask << 16) | bitmask;
    }
    else
    {
      *setbitptr = (bitmask << 16);
    }
  }
}

bool THwPinCtrl_rk::PadFuncSetup(uint32_t afmuxoffs, uint32_t aioblk,
    uint32_t agpio, uint32_t afunc, unsigned flags)
{
	return false;
}

// GPIO Pin

uint32_t g_rk_dummy_u32 = 0;

void TGpioPin_rk::InitDummy()
{
  // Initialize the pointers with harmless target
  setbitptr = &g_rk_dummy_u32;
  clrbitptr = &g_rk_dummy_u32;
  pindirptr = &g_rk_dummy_u32;
  getbitptr = &g_rk_dummy_u32;
  getoutbitptr = &g_rk_dummy_u32;
}

void TGpioPin_rk::Assign(int aportnum, int apinnum, bool ainvert)
{
	apinnum &= 0x1F;

	portnum = aportnum;
  pinnum = apinnum;
  inverted = ainvert;

  getbitshift   = apinnum;
  getbitshift16 = (apinnum & 0xF);

  if (ainvert)
  {
    clrbitvalue = ((1 << (16 + getbitshift16)) | (1 << getbitshift16));
    setbitvalue = ((1 << (16 + getbitshift16)) | 0);
  }
  else
  {
    setbitvalue = ((1 << (16 + getbitshift16)) | (1 << getbitshift16));
    clrbitvalue = ((1 << (16 + getbitshift16)) | 0);
  }

  regs = hwpinctrl.GetGpioRegs(aportnum);
  if (regs)
  {
  	if (apinnum >= 16)
  	{
      setbitptr = (uint32_t *)&regs->SWPORT_DR_H;
      pindirptr = (uint32_t *)&regs->SWPORT_DDR_H;
    	getoutbitptr = (volatile uint32_t *)&regs->SWPORT_DR_H;
  	}
  	else
  	{
      setbitptr = (uint32_t *)&regs->SWPORT_DR_L;
      pindirptr = (uint32_t *)&regs->SWPORT_DDR_L;
    	getoutbitptr = (volatile uint32_t *)&regs->SWPORT_DR_L;
  	}
  	clrbitptr = setbitptr;
    getbitptr = (volatile uint32_t *)&regs->EXT_PORT;
  }
  else
  {
  	InitDummy();
  }
}

void TGpioPin_rk::Toggle()
{
  if (OutValue())
  {
    Set0();
  }
  else
  {
    Set1();
  }
}

void TGpioPin_rk::SwitchDirection(int adirection)
{
  if (adirection)
  {
    *pindirptr = ((1 << (16 + getbitshift16)) | (1 << getbitshift16));
  }
  else
  {
    *pindirptr = ((1 << (16 + getbitshift16)) | 0);
  }
}
