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

// The register accesses are very slow on this platform!

#ifndef GPIO_OUT_USE_SHADOW_REG
// using the shadow registers make the GPIO outputs much faster,
// as they don't have SET and CLEAR registers and the atomic extension
// does not work on the GPIO DR register (lands in an exception)
// but it will cause problems, when the Linux core drives the same GPIO port
  #define GPIO_OUT_USE_SHADOW_REG  0     // 340 ns GPIO set time
  // #define GPIO_OUT_USE_SHADOW_REG  1  // 170 ns GPIO set time
#endif

class THwPinCtrl_sg : public THwPinCtrl_pre
{
public: // platform specific

  bool PadSetup(uint32_t afmuxoffs, uint32_t aioblk, uint32_t agpio, unsigned flags);  // CV1800/SG200x specific PAD setup
  bool PadFuncSetup(uint32_t afmuxoffs, uint32_t aioblk, uint32_t agpio, uint32_t afunc, unsigned flags);  // CV1800/SG200x specific PAD setup with FMUX

public: //
	bool PinSetup(int aportnum, int apinnum, unsigned flags);  // this does only GPIO setup

	void GpioSet(int aportnum, int apinnum, int value);

	inline bool GpioSetup(int aportnum, int apinnum, unsigned flags)  { return PinSetup(aportnum, apinnum, flags); }

	gpio_regs_t *  GetGpioRegs(int aportnum);

public:
	gpio_regs_t *        mapped_gpio_regs[5] = {0};
	uint8_t *            mapped_fmux_regs = nullptr;
	uint8_t *            mapped_pwr_ioblk = nullptr;
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
    #if GPIO_OUT_USE_SHADOW_REG
      cpu_atomic_or32(outshadowptr, pinmask);  // (returns the previous value)
      *setbitptr = *outshadowptr;
    #else
      unsigned intstatus = mcu_interrupts_save_and_disable();
      *setbitptr |= pinmask;  // this read-modify-write takes about 340 ns, and not multi-core safe !
      mcu_interrupts_restore(intstatus);
    #endif
  }

  inline void Set0() // irq-safe read-modify-write
  {
    #if GPIO_OUT_USE_SHADOW_REG
      cpu_atomic_and32(outshadowptr, negpinmask);  // (returns the previous value)
      *setbitptr = *outshadowptr;
    #else
      unsigned intstatus = mcu_interrupts_save_and_disable();
      *setbitptr &= negpinmask; // this read-modify-write takes about 340 ns, and not multi-core safe !
      mcu_interrupts_restore(intstatus);
    #endif
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
	uint32_t *          outshadowptr = nullptr;
};

#define HWPINCTRL_IMPL   THwPinCtrl_sg
#define HWGPIOPORT_IMPL  TGpioPort_sg
#define HWGPIOPIN_IMPL   TGpioPin_sg

extern gpio_regs_t * sg_gpio_regs(int aportnum);


#endif
