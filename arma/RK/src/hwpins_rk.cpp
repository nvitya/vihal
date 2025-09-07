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
		uint32_t addr;

		const uint32_t gpio_base_addresses[5] =
		{
				GPIO0_BASE, GPIO1_BASE, GPIO2_BASE, GPIO3_BASE, GPIO4_BASE
		};

		regs = (GPIO_REG *)map_hw_addr(gpio_base_addresses[aportnum], sizeof(GPIO_REG), (void * *)&mapped_gpio_regs[aportnum]);
	}

	return regs;
}


bool THwPinCtrl_rk::PadSetup(uint32_t afmuxoffs, uint32_t aioblk, uint32_t agpio, unsigned flags)
{
#if 0
	if (!mapped_fmux_regs)
	{
		mapped_fmux_regs = (uint8_t *)map_hw_addr(0x03001000, 0x1000, (void * *)&mapped_fmux_regs);
		mapped_pwr_ioblk = (uint8_t *)map_hw_addr(0x05027000, 0x1000, (void * *)&mapped_pwr_ioblk);
	}
  volatile uint32_t * reg_fmux  = (volatile uint32_t *)(mapped_fmux_regs + afmuxoffs);

  volatile uint32_t * reg_ioblk = nullptr;
  if (aioblk)
  {
  	if (((aioblk >> 24) & 0x0F) == 0x05)
  	{
  		reg_ioblk = (volatile uint32_t *)(mapped_pwr_ioblk + (aioblk & 0xFFF));
  	}
  	else
  	{
  		reg_ioblk = (volatile uint32_t *)(mapped_fmux_regs + (aioblk & 0xFFF));
  	}
  }

  if (reg_ioblk)  // not always present!
  {
    uint32_t ioblk = (0
      | (0  <<  2)  // PU: 1 = pull-up
      | (0  <<  3)  // PD: 1 = pull-down
      | (0  <<  5)  // DS(3): drive strength, default = 2
      | (0  <<  8)  // ST(2): Schmitt trigger level
      | (0  << 10)  // HE: bus-holder enable
      | (0  << 11)  // SL: slew rate limiter
    );

    if (flags & PINCFG_PULLUP)
    {
      ioblk |= (1 << 2);
    }

    if (flags & PINCFG_PULLDOWN)
    {
      ioblk |= (1 << 3);
    }

    if (PINCFG_SPEED_SLOW == (flags & PINCFG_SPEED_MASK))
    {
      ioblk |= (1 << 11);
    }

    if (PINCFG_DRIVE_WEAK == (flags & PINCFG_DRIVE_MASK))
    {
      ioblk |= (0 <<  5);
    }
    else if (PINCFG_DRIVE_STRONG == (flags & PINCFG_DRIVE_MASK))
    {
      ioblk |= (3 <<  5);
    }
    else
    {
      ioblk |= (2 <<  5);  // drive medium (default)
    }

    if (0 == (flags & PINCFG_ANALOGUE))
    {
      ioblk |= (1 << 8);  // activate the Schmitt trigger
    }

    *reg_ioblk = ioblk;
  }

  if (flags & PINCFG_AF_MASK)
  {
    *reg_fmux = ((flags >> PINCFG_AF_SHIFT) & 0x7);
  }
  else // GPIO
  {
    gpio_regs_t * regs = GetGpioRegs(agpio >> 8);
    if (agpio && regs)
    {
      uint32_t pinmask = (1 << (agpio & 0x1F));
      if (flags & PINCFG_OUTPUT)
      {
        if (flags & PINCFG_GPIO_INIT_1)
        {
          regs->SWPORTA_DR |= pinmask;
        }
        else
        {
          regs->SWPORTA_DR &= ~pinmask;
        }
        regs->SWPORTA_DDR |= pinmask;
      }
      else
      {
        regs->SWPORTA_DDR &= ~pinmask;
      }
      if ((0x400 <= agpio) && (agpio <= 0x402))
      {
      	*reg_fmux = 0; // GPIO for PWR_GPIO0-2 on SG2002
      }
      else
      {
      	*reg_fmux = 3; // GPIO
      }

      sg_gpio_shadow_regs[agpio >> 8] = regs->SWPORTA_DR; // update the shadow reg
    }
  }
#endif

  return true;
}

bool THwPinCtrl_rk::PinSetup(int aportnum, int apinnum, unsigned flags)
{
#if 0
  const gpio_pad_table_item_t * pitem = &gpio_pad_table[0];
  uint32_t gpio_code = aportnum * 0x100 + apinnum;
  while (pitem->fmux_offs | pitem->gpio | pitem->ioblk_reg)
  {
    if (pitem->gpio == gpio_code)
    {
      PadSetup(pitem->fmux_offs,  pitem->ioblk_reg,  pitem->gpio, flags);
      return true;
    }
    ++pitem;
  }
  return false;
#else
  return false;
#endif
}

void THwPinCtrl_rk::GpioSet(int aportnum, int apinnum, int value)
{
  GPIO_REG * regs = GetGpioRegs(aportnum);
  if (regs)
  {
#if 0
    if (value)
    {
      regs->SWPORTA_DR |= (1 << apinnum);
    }
    else
    {
      regs->SWPORTA_DR &= ~(1 << apinnum);
    }
#endif
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
