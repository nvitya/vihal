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
 *  file:     hwpins_sg.h
 *  brief:    SG2000 Pin/Pad and GPIO configuration
 *  created:  2024-04-21
 *  authors:  nvitya
*/

#ifndef _HWPINS_SG_H
#define _HWPINS_SG_H

#define HWPINS_PRE_ONLY
#include "hwpins.h"
#include "platform.h"

#define PORTNUM_PWR     4

class THwPinCtrl_sg : public THwPinCtrl_pre
{
public: // platform specific

  bool PadSetup(uint32_t afmuxoffs, uint32_t aioblk, uint32_t agpio, unsigned flags);  // CV1800/SG200x specific PAD setup
  bool PadFuncSetup(uint32_t afmuxoffs, uint32_t aioblk, uint32_t agpio, uint32_t afunc, unsigned flags);  // CV1800/SG200x specific PAD setup with FMUX

public: //
	bool PinSetup(int aportnum, int apinnum, unsigned flags);  // this does only GPIO setup

	void GpioSet(int aportnum, int apinnum, int value);

	inline bool GpioSetup(int aportnum, int apinnum, unsigned flags)  { return PinSetup(aportnum, apinnum, flags); }
};

class TGpioPort_sg : public TGpioPort_pre
{
public:
	void Assign(int aportnum);
	void Set(unsigned value);

public:
	volatile uint32_t *  portptr = nullptr;
};

class TGpioPin_sg : public TGpioPin_pre
{
public:
  void InitDummy(); // for unassigned ports

  // WARNING: the atomic extension does not work here on the DR register
  // the solution is not perfect, because the linux core can access
  // the same gpio register parallel to these !

  inline void Set1() // irq-safe read-modify-write
  {
    unsigned intstatus = mcu_interrupts_save_and_disable();
    *setbitptr |= pinmask;
    mcu_interrupts_restore(intstatus);
  }

  inline void Set0() // irq-safe read-modify-write
  {
    unsigned intstatus = mcu_interrupts_save_and_disable();
    *setbitptr &= negpinmask;
    mcu_interrupts_restore(intstatus);
  }

  inline void SetTo(unsigned value)  { if (value & 1) Set1(); else Set0(); }

  inline unsigned char Value()       { return ((*getbitptr >> pinnum) & 1); }
  inline unsigned char OutValue()    { return ((*setbitptr >> pinnum) & 1); }

	void Assign(int aportnum, int apinnum, bool ainvert);

	void Toggle();

	void SwitchDirection(int adirection);

public:
	gpio_regs_t *       regs;
	uint32_t            pinmask = 0;
	uint32_t            negpinmask = 0;
	uint32_t *          setbitptr = nullptr;
	uint32_t *          getbitptr = nullptr;
	uint32_t *          pindirptr = nullptr;
};

#define HWPINCTRL_IMPL   THwPinCtrl_sg
#define HWGPIOPORT_IMPL  TGpioPort_sg
#define HWGPIOPIN_IMPL   TGpioPin_sg

#endif
