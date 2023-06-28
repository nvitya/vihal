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
 *  file:     hwpins_msp.h
 *  brief:    MSP Pin/Pad and GPIO configuration
 *  date:     2023-06-27
 *  authors:  nvitya
*/

#ifndef _HWPINS_MSP_H
#define _HWPINS_MSP_H

#define HWPINS_PRE_ONLY
#include "hwpins.h"

class THwPinCtrl_msp : public THwPinCtrl_pre
{
public:
	// platform specific
	bool PinSetup(int aportnum, int apinnum, unsigned flags);

	HW_GPIO_REGS * GetGpioRegs(int aportnum);

	bool GpioPortEnable(int aportnum);

	void GpioSet(int aportnum, int apinnum, int value);

	inline bool GpioSetup(int aportnum, int apinnum, unsigned flags)  { return PinSetup(aportnum, apinnum, flags); }

	void GpioIrqSetup(int aportnum, int apinnum, int amode); // not implemented yet
};

class TGpioPort_msp : public TGpioPort_pre
{
public:
	void Assign(int aportnum);
	void Set(unsigned value);

public:
	HW_GPIO_REGS *       regs = nullptr;
	volatile unsigned *  portptr = nullptr;
};

class TGpioPin_msp : public TGpioPin_common
{
public:
	HW_GPIO_REGS *   regs = nullptr;
	unsigned *       togglebitptr = nullptr;

	bool Setup(unsigned flags);
	void Assign(int aportnum, int apinnum, bool ainvert);

	inline void Toggle()  { *togglebitptr = setbitvalue; }

	void SwitchDirection(int adirection);
};

#define HWPINCTRL_IMPL   THwPinCtrl_msp
#define HWGPIOPORT_IMPL  TGpioPort_msp
#define HWGPIOPIN_IMPL   TGpioPin_msp

#endif /* HWPINS_MSP_H_ */
