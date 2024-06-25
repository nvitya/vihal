/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2023 Viktor Nagy, nvitya
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
 *  file:     hwpins_am.h
 *  brief:    AM Pin/Pad and GPIO configuration
 *  date:     2024-06-15
 *  authors:  nvitya
*/

#ifndef _HWPINS_AM_H
#define _HWPINS_AM_H

#define HWPINS_PRE_ONLY
#include "hwpins.h"

#define PORTNUM_MCU     0xC

typedef CSL_GpioRegs                GPIO_Unit_Regs;
typedef CSL_GpioBank_registersRegs  GPIO_Bank_Regs;

class THwPinCtrl_am : public THwPinCtrl_pre
{
public:
	// platform specific
  bool PadSetup(uint32_t apadctl, uint32_t agpio, unsigned flags);


  bool PinSetup(int aportnum, int apinnum, unsigned flags);
	bool GpioPortEnable(int aportnum);

	void GpioSet(int aportnum, int apinnum, int value);

	inline bool GpioSetup(int aportnum, int apinnum, unsigned flags)  { return PinSetup(aportnum, apinnum, flags); }

public: // utility
  GPIO_Unit_Regs *  GetGpioUnit(int aportnum);
  GPIO_Bank_Regs *  GetGpioBankRegs(GPIO_Unit_Regs * aunit, int apinnum);
  GPIO_Bank_Regs *  GetGpioBankRegs(int aportnum, int apinnum);

  void              UnlockPadConfig(volatile uint32_t * apadctlreg);
};

class TGpioPort_am : public TGpioPort_pre
{
public:
	void Assign(int aportnum);
	void Assign(int aportnum, int apinnum);
	void Set(unsigned value);

public:
	GPIO_Bank_Regs *          regs = nullptr;
	volatile unsigned *       portptr = nullptr;
};

class TGpioPin_am : public TGpioPin_common
{
public:
  GPIO_Bank_Regs *      regs = nullptr;
  uint32_t              bitmask = 0;

	bool Setup(unsigned flags);
	void Assign(int aportnum, int apinnum, bool ainvert);

	void Toggle();

	void SwitchDirection(int adirection);
};

#define HWPINCTRL_IMPL   THwPinCtrl_am
#define HWGPIOPORT_IMPL  TGpioPort_am
#define HWGPIOPIN_IMPL   TGpioPin_am

#endif /* HWPINS_AM_H_ */
