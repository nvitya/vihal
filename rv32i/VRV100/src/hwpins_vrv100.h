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
 *  file:     hwpins_vrv100.h
 *  brief:    VRV100 Pin/Pad and GPIO configuration
 *  version:  1.00
 *  date:     2021-10-02
 *  authors:  nvitya
*/

#ifndef _HWPINS_VRV100_H
#define _HWPINS_VRV100_H

#define HWPINS_PRE_ONLY
#include "hwpins.h"
#include "platform.h"

class THwPinCtrl_vrv100 : public THwPinCtrl_pre
{
public:
	// platform specific
	bool PinSetup(int aportnum, int apinnum, unsigned flags);

	vexriscv_gpio_t * GetGpioRegs(int aportnum);

	void GpioSet(int aportnum, int apinnum, int value);

	inline bool GpioSetup(int aportnum, int apinnum, unsigned flags)  { return PinSetup(aportnum, apinnum, flags); }

	void GpioIrqSetup(int aportnum, int apinnum, int amode); // not implemented yet
};

class TGpioPort_vrv100 : public TGpioPort_pre
{
public:
	void Assign(int aportnum);
	void Set(unsigned value);

public:
	vexriscv_gpio_t *       regs = nullptr;
	volatile unsigned *  portptr = nullptr;
};

class TGpioPin_vrv100 : public TGpioPin_common
{
public:
	vexriscv_gpio_t *   regs = nullptr;

	bool Setup(unsigned flags);
	void Assign(int aportnum, int apinnum, bool ainvert);

	void Toggle();

	void SwitchDirection(int adirection);
};

#define HWPINCTRL_IMPL   THwPinCtrl_vrv100
#define HWGPIOPORT_IMPL  TGpioPort_vrv100
#define HWGPIOPIN_IMPL   TGpioPin_vrv100

#endif /* _HWPINS_VRV100_H */
