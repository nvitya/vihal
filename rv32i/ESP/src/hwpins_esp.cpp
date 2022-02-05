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
 *  brief:    ESP Risc-V Pin/Pad and GPIO configuration
 *  version:  1.00
 *  date:     2022-02-03
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"
#include "esp_utils.h"

bool THwPinCtrl_esp::PadSetup(unsigned apadnum, unsigned aiomuxidx, unsigned flags)
{
  if ((apadnum < 0) || (apadnum >= ESP_GPIO_COUNT))
  {
    return false;
  }

  // port power control is not necessary ?

  // 1. prepare pad configuration

  uint32_t padcfg = (0
    | (0  << 15)  // FILTER_EN: 1 = enable filter for the input signals
    | (0  << 12)  // MCU_SEL(3): function select
    | (0  << 10)  // FUN_DRV(2): drive strength
    | (1  <<  9)  // FUN_IE: 1 = input enable
    | (0  <<  8)  // FUN_WPU: 1 = internal pull-up
    | (0  <<  7)  // FUN_WPD: 1 = internal pull-down
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

  if (flags & PINCFG_PULLDOWN)
  {
    padcfg |= (1 << 7);
  }

  if (flags & PINCFG_AF_MASK)
  {
    padcfg |= (((flags >> PINCFG_AF_SHIFT) & 7) << 12);  // only 7 functions are supported here !
  }
  else
  {
    padcfg |= (1 << 12);  // 1 = GPIO function
  }

  IO_MUX->CFG[apadnum] = padcfg;

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
  //GPIO->PIN[apinnum] = pincfg;

  // route the GPIO output regs to the output PAD

  if (flags & PINCFG_OUTPUT)
  {
    GPIO->FUNC_OUT_SEL_CFG[apadnum] = (0
      | (aiomuxidx <<  0)  // OUT_SEL(8)
      | (0    <<  8)  // INV_SEL
      | (1    <<  9)  // OEN_SEL
      | (0    << 10)  // OEN_INV_SEL
    );
  }

  // 3. GPIO configuration

  unsigned gpio_bit = (apadnum & 0x1F);
  unsigned gpio_mask = (1 << gpio_bit);

  if (flags & PINCFG_OUTPUT)
  {
    // set the output before enabling
    if (flags & PINCFG_GPIO_INIT_1)
    {
      GPIO->OUT.SET = gpio_mask;
    }
    else
    {
      GPIO->OUT.CLR = gpio_mask;
    }

    GPIO->ENABLE.SET = gpio_mask;
  }
  else
  {
    GPIO->ENABLE.CLR = gpio_mask;
  }

  return true;
}

bool THwPinCtrl_esp::PinSetup(int aportnum, int apinnum, unsigned flags) // pinnum = pad number, portnum is ignored
{
  return PadSetup(apinnum, 0x80, flags);
}

void THwPinCtrl_esp::GpioSet(int aportnum, int apinnum, int value)
{
  unsigned gpio_mask = (1 << (apinnum & 0x1F));

  esp_gpio_out_t * regs = &GPIO->OUT;

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
  regs = &GPIO->OUT;
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

  regs = &GPIO->OUT;

  unsigned gpio_mask = (1 << (apinnum & 0x1F));

	setbitvalue = gpio_mask;
	clrbitvalue = gpio_mask;
  getbitptr    = (unsigned *)&(GPIO->IN);
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
	if (adirection)
	{
		GPIO->ENABLE.SET = setbitvalue;
	}
	else
	{
    GPIO->ENABLE.CLR = setbitvalue;
	}
}
