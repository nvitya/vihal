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
 *  created:  2022-01-29
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"
#include "esp_utils.h"

// This table contains the IO_MUX pad configuration register indexes for the PAD / GPIO ids:
const uint8_t esp_io_mux_pad_map[40] =
{
  16, 33, 15, 32, 17, 26, 23, 24, 25, 20,  // 0-9
  21, 22, 12, 13, 11, 14, 18, 19, 27, 28,  // 10-19
  29, 30, 31, 34, 35,  8,  9, 10, 99, 99,  // 20-29
  99, 99,  6,  7,  4,  5,  0,  1,  2,  3   // 30-39
};

bool THwPinCtrl_esp::PadSetup(unsigned apadnum, unsigned aiomuxidx, unsigned flags)
{
  if ((apadnum < 0) || (apadnum >= ESP_GPIO_COUNT))
  {
    return false;
  }

  // 1. prepare pad configuration

  uint8_t padcfgidx = esp_io_mux_pad_map[apadnum];
  if (padcfgidx >= sizeof(esp_io_mux_pad_map))
  {
    return false;
  }

  uint32_t padcfg = (0
    | (0  << 12)  // MCU_SEL(3): function select
    | (0  << 10)  // FUN_DRV(2): drive strength
    | (1  <<  9)  // FUN_IE: 1 = input enable
    | (0  <<  8)  // FUN_WPU: 1 = internal pull-up
    | (0  <<  7)  // FUN_WPD: 1 = internal pull-down
    | (0  <<  5)  // MCU_DRV(2): drive strength in sleep mode
    | (0  <<  4)  // MCU_IE: 1 = input enable in sleep mode
    | (0  <<  3)  // MCU_WPU: 1 = pull-up in sleep mode
    | (0  <<  2)  // MCU_WPD: 1 = pull-down in sleep mode
    | (0  <<  1)  // SLP_SEL: 1 = put the pad in sleep mode
    | (0  <<  0)  // MCU_OE: 1 = enable output in sleep mode
  );

  if (flags & (PINCFG_DRIVE_STRONG | PINCFG_SPEED_FAST))
  {
    padcfg |= (3 << 10);
  }
  else if (flags & PINCFG_DRIVE_WEAK)
  {
    padcfg |= (0 << 10);
  }
  else
  {
    padcfg |= (2 << 10);
  }

  if (flags & PINCFG_PULLUP)
  {
    padcfg |= (1 << 8);
  }

  if (flags & PINCFG_AF_MASK)
  {
    padcfg |= (((flags >> PINCFG_AF_SHIFT) & 7) << 12);  // only 7 functions are supported here !
  }
  else
  {
    padcfg |= (2 << 12);  // 2 = GPIO function
  }

  IO_MUX->CFG[padcfgidx] = padcfg;

  // 2. GPIO PIN configuration (open-drain)

  uint32_t pincfg = GPIO->PIN[apadnum];
  if (flags & PINCFG_OPENDRAIN)
  {
    pincfg |= (1 << 2); // PAD_DRIVER = 1: open drain
  }
  else
  {
    pincfg &= ~(1 << 2); // 0 = normal
  }
  GPIO->PIN[apadnum] = pincfg;

  // route the GPIO output regs to the output PAD

  GPIO->FUNC_OUT_SEL_CFG[apadnum] = (0
    | (aiomuxidx  <<  0)  // OUT_SEL(9)
    | (0  <<  9)  // INV_SEL
    | (1  << 10)  // OEN_SEL
    | (0  << 10)  // OEN_INV_SEL
  );

  // 3. GPIO configuration

  unsigned gpio_idx = (apadnum > 31 ? 1 : 0);
  unsigned gpio_bit = (apadnum & 0x1F);
  unsigned gpio_mask = (1 << gpio_bit);

  if (flags & PINCFG_OUTPUT)
  {
    // set the output before enabling
    if (flags & PINCFG_GPIO_INIT_1)
    {
      GPIO->OUT[gpio_idx].SET = gpio_mask;
    }
    else
    {
      GPIO->OUT[gpio_idx].CLR = gpio_mask;
    }

    GPIO->ENABLE[gpio_idx].SET = gpio_mask;
  }
  else
  {
    GPIO->ENABLE[gpio_idx].CLR = gpio_mask;
  }

  return true;
}

bool THwPinCtrl_esp::PinSetup(int aportnum, int apinnum, unsigned flags) // pinnum = pad number, portnum is ignored
{
  return PadSetup(apinnum, 0x100, flags);
}

void THwPinCtrl_esp::GpioSet(int aportnum, int apinnum, int value)
{
  unsigned gpio_idx = (apinnum > 31 ? 1 : 0);
  unsigned gpio_mask = (1 << (apinnum & 0x1F));

  esp_gpio_out_t * regs = &GPIO->OUT[gpio_idx];

  if (1 == value)
  {
    regs->SET = gpio_mask;
  }
  else if (value & 2) // toggle
  {
    if (regs->REG & gpio_mask)
    {
      regs->SET = gpio_mask;
    }
    else
    {
      regs->SET = gpio_mask;
    }
  }
  else
  {
    regs->CLR = gpio_mask;
  }
}

void THwPinCtrl_esp::GpioIrqSetup(int aportnum, int apinnum, int amode)
{

}

// GPIO Port

void TGpioPort_esp::Assign(int aportnum)
{
  regs = &GPIO->OUT[aportnum & 1];
}

void TGpioPort_esp::Set(unsigned value)
{
  regs->REG = value;
}

// GPIO Pin

void TGpioPin_esp::Assign(int aportnum, int apinnum, bool ainvert)
{
	portnum = aportnum;
  pinnum = apinnum;
  inverted = ainvert;

  unsigned gpio_idx = (apinnum > 31 ? 1 : 0);

  regs = &GPIO->OUT[gpio_idx];

  unsigned gpio_mask = (1 << (apinnum & 0x1F));

	setbitvalue = gpio_mask;
	clrbitvalue = gpio_mask;
  getbitptr    = (unsigned *)&(GPIO->IN[gpio_idx]);
  getoutbitptr = (unsigned *)&(regs->REG);
  getbitshift = pinnum;

  if (ainvert)
  {
    setbitptr = (unsigned *)&(regs->CLR);
    clrbitptr = (unsigned *)&(regs->SET);
  }
  else
  {
    setbitptr = (unsigned *)&(regs->SET);
    clrbitptr = (unsigned *)&(regs->CLR);
  }
}

void TGpioPin_esp::Toggle()
{
  if (*getoutbitptr & setbitvalue)
  {
    Set0();
  }
  else
  {
    Set1();
  }
}

void TGpioPin_esp::SwitchDirection(int adirection)
{
  unsigned gpio_idx = (pinnum > 31 ? 1 : 0);

	if (adirection)
	{
		GPIO->ENABLE[gpio_idx].SET = setbitvalue;
	}
	else
	{
    GPIO->ENABLE[gpio_idx].CLR = setbitvalue;
	}
}
