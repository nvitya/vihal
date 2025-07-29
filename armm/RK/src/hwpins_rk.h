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
 *  file:     hwpins_rk.h
 *  brief:    RK Pin/Pad and GPIO configuration
 *  created:  2025-07-29
 *  authors:  nvitya
*/

#ifndef _HWPINS_RK_H
#define _HWPINS_RK_H

#define HWPINS_PRE_ONLY
#include "hwpins.h"
#include "platform.h"

// The register accesses are very slow on this platform!

class THwPinCtrl_rk : public THwPinCtrl_pre
{
public: // platform specific

  bool PadSetup(uint32_t afmuxoffs, uint32_t aioblk, uint32_t agpio, unsigned flags);  // CV1800/SG200x specific PAD setup
  bool PadFuncSetup(uint32_t afmuxoffs, uint32_t aioblk, uint32_t agpio, uint32_t afunc, unsigned flags);  // CV1800/SG200x specific PAD setup with FMUX

public: //
	bool PinSetup(int aportnum, int apinnum, unsigned flags);  // this does only GPIO setup

	void GpioSet(int aportnum, int apinnum, int value);

	inline bool GpioSetup(int aportnum, int apinnum, unsigned flags)  { return PinSetup(aportnum, apinnum, flags); }

	GPIO_REG *  GetGpioRegs(int aportnum);

public:
	GPIO_REG *           mapped_gpio_regs[5] = {0};
	uint8_t *            mapped_fmux_regs = nullptr;
	uint8_t *            mapped_pwr_ioblk = nullptr;
};

class TGpioPin_rk : public TGpioPin_pre
{
public:
  void InitDummy(); // for unassigned ports

  // WARNING: the atomic extension does not work here on the DR register
  // the solution is not perfect, because the linux core can access
  // the same gpio register parallel to these !

  inline void Set1() // irq-safe read-modify-write
  {
    *setbitptr |= pinmask;  // this read-modify-write takes about 340 ns, and not multi-core safe !
  }

  inline void Set0() // irq-safe read-modify-write
  {
    *setbitptr &= negpinmask; // this read-modify-write takes about 340 ns, and not multi-core safe !
  }

  inline void SetTo(unsigned value)  { if (value & 1) Set1(); else Set0(); }

  inline unsigned char Value()       { return ((*getbitptr >> pinnum) & 1); }
  inline unsigned char OutValue()    { return ((*setbitptr >> pinnum) & 1); }

	void Assign(int aportnum, int apinnum, bool ainvert);

	void Toggle();

	void SwitchDirection(int adirection);

public:
	GPIO_REG *             regs;
	uint32_t               pinmask = 0;
	uint32_t               negpinmask = 0;
	volatile uint32_t *    setbitptr = nullptr;
	volatile uint32_t *    getbitptr = nullptr;
	volatile uint32_t *    pindirptr = nullptr;
	volatile uint32_t *    outshadowptr = nullptr;
};

#define HWPINCTRL_IMPL   THwPinCtrl_rk
#define HWGPIOPORT_IMPL  TGpioPort_rk
#define HWGPIOPIN_IMPL   TGpioPin_rk

//extern GPIO_REG * rk_gpio_regs(int aportnum);


#endif
