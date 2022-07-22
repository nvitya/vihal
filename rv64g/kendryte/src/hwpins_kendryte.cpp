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
 *  file:     hwpins_kendryte.cpp
 *  brief:    KendryteV Pin/Pad and GPIO configuration
 *  created:  2022-07-22
 *  authors:  nvitya
*/

#include "platform.h"
//#include "hwpins.h"
#include "hwpins_kendryte.h"

bool THwPinCtrl_kendryte::PadSetup(int apadnum, int afuncnum, unsigned flags)
{
  SYSCTL->CLK_EN_PERI |= (SYSCTL_CLK_EN_PERI_FPIOA | SYSCTL_CLK_EN_PERI_GPIO);

  uint32_t iocfg = (0
    | (afuncnum <<  0)  // CH_SEL(8): Channel select from 256 functions
    | (0        <<  8)  // DS(4): Driving selector
    | (0        << 12)  // OE_EN: Output enable.It can disable or enable IO output
    | (0        << 13)  // OE_INV: Invert the output enable signal
    | (0        << 14)  // DO_SEL: Data output select: 0 for DO, 1 for OE
    | (0        << 15)  // DO_INV: Invert the result of data output select (DO_SEL)
    | (0        << 16)  // PU: pull enable or pull-up enable?
    | (0        << 17)  // PD: pull select or pull-down enable?
    | (0        << 18)  // SPU: strong pull up
    | (0        << 19)  // SL: Slew rate control enable
    | (0        << 20)  // IE_EN: Input enable. It can disable or enable IO input
    | (0        << 21)  // IE_INV: Invert the input enable signal
    | (0        << 22)  // DI_INV: Invert Data input
    | (0        << 23)  // ST: Schmitt trigger enable
  );

  if ((flags & PINCFG_DRIVE_MASK) == PINCFG_DRIVE_WEAK)
  {
    iocfg |= (3 << 8);
  }
  else if ((flags & PINCFG_DRIVE_MASK) == PINCFG_DRIVE_STRONG)
  {
    iocfg |= (15 << 8);
  }
  else
  {
    iocfg |= (8 << 8);
  }

  if (flags & PINCFG_OUTPUT)
  {
    iocfg |= (1 << 12) | (1 << 20) | (1 << 23);  // enable the input too
  }
  else
  {
    iocfg |= (1 << 20);  // enable the input only
    if (flags & PINCFG_ANALOGUE)
    {
      // do not enable the Schmitt trigger !
    }
    else
    {
      iocfg |= (1 << 23);  // enable the Schmitt Trigger
    }
  }

  if (flags & PINCFG_PULLUP)
  {
    iocfg |= (1 << 16);
  }
  else if (flags & PINCFG_PULLDOWN)
  {
    iocfg |= (1 << 17);
  }

  FPIOA->IOCFG[apadnum] = iocfg;

  return true;
}


bool THwPinCtrl_kendryte::PinSetup(int aportnum, int apinnum, unsigned flags)  // gpiohs setup
{
  uint32_t pinmask = (1 << apinnum);

  if (flags & PINCFG_OUTPUT)
  {
    if (flags & PINCFG_GPIO_INIT_1)
    {
      GPIOHS->OUTPUT_VAL  |= pinmask;
    }
    else
    {
      GPIOHS->OUTPUT_VAL  &= ~pinmask;
    }

    if (flags & PINCFG_GPIO_INVERT)
    {
      GPIOHS->OUTPUT_XOR  |= pinmask;
    }
    else
    {
      GPIOHS->OUTPUT_XOR  &= ~pinmask;
    }

    GPIOHS->INPUT_EN  &= ~pinmask;
    GPIOHS->OUTPUT_EN |=  pinmask;
  }
  else
  {
    GPIOHS->OUTPUT_EN &= ~pinmask;
    GPIOHS->INPUT_EN  |=  pinmask;
  }

  return true;
}

void THwPinCtrl_kendryte::GpioSet(int aportnum, int apinnum, int value)
{

}

void THwPinCtrl_kendryte::GpioIrqSetup(int aportnum, int apinnum, int amode)
{
  // todo: implement
}

// GPIO Port

void TGpioPort_kendryte::Assign(int aportnum)
{
  *portptr = GPIOHS->OUTPUT_VAL;
}

void TGpioPort_kendryte::Set(unsigned value)
{
  *portptr = value;
}

// GPIO Pin

static uint32_t g_dummy_u32 = 0;

void TGpioPin_kendryte::InitDummy()
{
  setbitptr = &g_dummy_u32;
  getbitptr = &g_dummy_u32;
}

void TGpioPin_kendryte::Assign(int aportnum, int apinnum, bool ainvert)
{
	portnum = aportnum;
  pinnum = apinnum;
  inverted = ainvert;
  pinmask = (1u << apinnum);
  negpinmask = ~pinmask;
  setbitptr = (uint32_t *)&GPIOHS->OUTPUT_VAL;
  getbitptr = (uint32_t *)&GPIOHS->INPUT_VAL;

  // todo: prepare the output invert at the Setup()
}

void TGpioPin_kendryte::Toggle()
{
  if (*setbitptr & pinmask)
  {
    Set0();
  }
  else
  {
    Set1();
  }
}

void TGpioPin_kendryte::SwitchDirection(int adirection)
{
  // todo: implement
}

