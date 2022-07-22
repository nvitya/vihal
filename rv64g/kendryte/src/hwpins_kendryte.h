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
 *  file:     hwpins_kendryte.h
 *  brief:    GD32V Pin/Pad and GPIO configuration
 *  version:  1.00
 *  date:     2021-10-20
 *  authors:  nvitya
*/

#ifndef _HWPINS_KENDRYTE_H
#define _HWPINS_KENDRYTE_H

#define HWPINS_PRE_ONLY
#include "hwpins.h"
#include "platform.h"

class THwPinCtrl_kendryte : public THwPinCtrl_pre
{
public: // platform specific

  bool PadSetup(int apadnum, int afuncnum, unsigned flags);  // Kendrytye specific FPIOA routing / configuration

public: //
	bool PinSetup(int aportnum, int apinnum, unsigned flags);  // this does only GPIO setup

	void GpioSet(int aportnum, int apinnum, int value);

	inline bool GpioSetup(int aportnum, int apinnum, unsigned flags)  { return PinSetup(aportnum, apinnum, flags); }

	void GpioIrqSetup(int aportnum, int apinnum, int amode); // not implemented yet
};

class TGpioPort_kendryte : public TGpioPort_pre
{
public:
	void Assign(int aportnum);
	void Set(unsigned value);

public:
	volatile uint32_t *  portptr = nullptr;
};

class TGpioPin_kendryte : public TGpioPin_pre
{
public:
  void InitDummy(); // for unassigned ports

  inline void Set1()                 { *setbitptr |= pinmask; }
  inline void Set0()                 { *setbitptr &= negpinmask; }
  inline void SetTo(unsigned value)  { if (value & 1) Set1(); else Set0(); }

  inline unsigned char Value()       { return ((*getbitptr >> pinnum) & 1); }
  inline unsigned char OutValue()    { return ((*setbitptr >> pinnum) & 1); }

	void Assign(int aportnum, int apinnum, bool ainvert);

	void Toggle();

	void SwitchDirection(int adirection);

public:
	uint32_t            pinmask = 0;
	uint32_t            negpinmask = 0;
	uint32_t *          setbitptr = nullptr;
	uint32_t *          getbitptr = nullptr;
};

#define HWPINCTRL_IMPL   THwPinCtrl_kendryte
#define HWGPIOPORT_IMPL  TGpioPort_kendryte
#define HWGPIOPIN_IMPL   TGpioPin_kendryte

#endif
