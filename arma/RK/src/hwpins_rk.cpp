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
	if ((aportnum < 0) || (aportnum > 4))
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

uint8_t *  THwPinCtrl_rk::GetIocRegs(int aportnum)
{
	if ((aportnum < 0) || (aportnum > 4))
	{
		return nullptr;
	}

	uint8_t * regs = mapped_ioc_regs[aportnum];
	if (!regs)
	{
		const uint32_t ioc_base_addresses[5] =
		{
				GPIO0_IOC_BASE, GPIO1_IOC_BASE, GPIO2_IOC_BASE, GPIO3_IOC_BASE, GPIO4_IOC_BASE
		};

		regs = (uint8_t *)map_hw_addr(ioc_base_addresses[aportnum], 4096, (void * *)&mapped_ioc_regs[aportnum]);
	}

	return regs;
}

bool THwPinCtrl_rk::PinSetup(int aportnum, int apinnum, unsigned flags)
{
	if ((aportnum < 0) || (aportnum > 4))
	{
		return false;
	}

	uint8_t * ioc_regs = GetIocRegs(aportnum);
	if (!ioc_regs)
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
	volatile uint32_t * reg_od = (volatile uint32_t *)(&ioc_regs[0x700 + (aportnum << 4) + 4 * (apinnum >> 3)]);
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
	volatile uint32_t * reg_pupd = (volatile uint32_t *)(&ioc_regs[0x200 + (aportnum << 4) + 4 * (apinnum >> 3)]);
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
	volatile uint32_t * reg_sl = (volatile uint32_t *)(&ioc_regs[0x600 + (aportnum << 4) + 4 * (apinnum >> 3)]);
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
	volatile uint32_t * reg_ds = (volatile uint32_t *)(&ioc_regs[0x100 + (aportnum << 6) + 4 * (apinnum >> 1)]);
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
	volatile uint32_t * reg_smt = (volatile uint32_t *)(&ioc_regs[0x400 + (aportnum << 4) + 4 * (apinnum >> 3)]);
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
	volatile uint32_t * reg_iomux = (volatile uint32_t *)(&ioc_regs[(aportnum << 5) | (apinnum & 0x1C)]);
	pinpos = ((apinnum & 3) << 2);
	*reg_iomux = ((0xF << (16 + pinpos)) | (value << pinpos));

  return true;
}

const uint8_t rk3506_rmio_lookup[] = // values here: (gpio_port << 5) | gpio_pin
{
// from GPIO0_A0 -> GPIO0_C7 it is linear

/*  0 */ 0x00, // 0.A0
/*  1 */ 0x01,
/*  2 */ 0x02,
/*  3 */ 0x03,
/*  4 */ 0x04,
/*  5 */ 0x05,
/*  6 */ 0x06,
/*  7 */ 0x07,

/*  8 */ 0x08, // 0.B0
/*  9 */ 0x09,
/* 10 */ 0x0A,
/* 11 */ 0x0B,
/* 12 */ 0x0C,
/* 13 */ 0x0F,
/* 14 */ 0x0E,
/* 15 */ 0x0F,

/* 16 */ 0x10, // 0.C0
/* 17 */ 0x11,
/* 18 */ 0x12,
/* 19 */ 0x13,
/* 20 */ 0x14,
/* 21 */ 0x15,
/* 22 */ 0x16,
/* 23 */ 0x17,

	// from now it is non-linear

/* 24 */ 0x29,  // 1.B1
/* 25 */ 0x2A,  // 1.B2
/* 26 */ 0x2B,	// 1.B3
/* 27 */ 0x32,	// 1.C2
/* 28 */ 0x33,	// 1.C3
/* 29 */ 0x39,	// 1.D1
/* 30 */ 0x3A,	// 1.D2
/* 31 */ 0x3B,	// 1.D3
};

int THwPinCtrl_rk::RmioByPortPin(int aportnum, int apinnum)
{
	uint8_t gpio_code = ((aportnum << 5) | apinnum);
	int rmio_num = 0;
	while (rmio_num < (int)sizeof(rk3506_rmio_lookup))
	{
		if (rk3506_rmio_lookup[rmio_num] == gpio_code)
		{
			return rmio_num;
		}
		++rmio_num;
	}
	return -1;
}

bool THwPinCtrl_rk::PinSetupRmio(int aportnum, int apinnum, int rmio_func, unsigned flags)
{
	int rmionum = RmioByPortPin(aportnum, apinnum);
	if (rmionum < 0)
	{
		return false;
	}

  if (!PinSetup(aportnum, apinnum, (flags & ~(PINCFG_AF_MASK)) | PINCFG_AF_RM_IO))
  {
  	return false;
  }
  return RmioSetup(rmionum, rmio_func);
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
	volatile uint32_t * reg_sel = (volatile uint32_t *)(&mapped_rm_io[0x80 + (rmionum << 2)]);
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
