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
 *  file:     hwpins_sg.cpp
 *  brief:    SG2000 Pin/Pad and GPIO configuration
 *  created:  2024-04-21
 *  authors:  nvitya
*/

#include "platform.h"
//#include "hwpins.h"
#include "hwpins_sg.h"

bool THwPinCtrl_sg::PadSetup(int apadnum, int afuncnum, unsigned flags)
{
  return true;
}


bool THwPinCtrl_sg::PinSetup(int aportnum, int apinnum, unsigned flags)  // gpiohs setup
{
  return true;
}

void THwPinCtrl_sg::GpioSet(int aportnum, int apinnum, int value)
{

}

void THwPinCtrl_sg::GpioIrqSetup(int aportnum, int apinnum, int amode)
{
  // todo: implement
}

// GPIO Port

void TGpioPort_sg::Assign(int aportnum)
{
  *portptr = GPIOHS->OUTPUT_VAL;
}

void TGpioPort_sg::Set(unsigned value)
{
  *portptr = value;
}

// GPIO Pin

static uint32_t g_dummy_u32 = 0;

void TGpioPin_sg::InitDummy()
{
  setbitptr = &g_dummy_u32;
  getbitptr = &g_dummy_u32;
}

void TGpioPin_sg::Assign(int aportnum, int apinnum, bool ainvert)
{
	portnum = aportnum;
  pinnum = apinnum;
  inverted = ainvert;
  pinmask = (1u << apinnum);
  negpinmask = ~pinmask;

  //setbitptr = (uint32_t *)&GPIOHS->OUTPUT_VAL;
  //getbitptr = (uint32_t *)&GPIOHS->INPUT_VAL;


  setbitptr = &g_dummy_u32;
  getbitptr = &g_dummy_u32;


  // todo: prepare the output invert at the Setup()
}

void TGpioPin_sg::Toggle()
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

void TGpioPin_sg::SwitchDirection(int adirection)
{
  // todo: implement
}

