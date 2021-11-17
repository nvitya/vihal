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
 *  file:     hwpwm_atsam_v2.h
 *  brief:    ATSAM V2 PWM Driver
 *  version:  1.00
 *  date:     2021-11-17
 *  authors:  nvitya
*/


#ifndef SRC_HWPWM_ATSAM_V2_H_
#define SRC_HWPWM_ATSAM_V2_H_

#define HWPWM_PRE_ONLY
#include "hwpwm.h"

class THwPwmChannel_atsam_v2 : public THwPwmChannel_pre
{
public:
	bool          Init(int adevnum, int achnum);

	void          SetOnClocks(uint16_t aclocks);
	void          Enable();
	void          Disable();
	//inline bool   Enabled() { return ((regs->CCER & outenbit) != 0); }

	void          SetFrequency(uint32_t afrequency);

public:
	//Pwm *                   commonregs = nullptr;
	Tcc *                   regs = nullptr;
	//PwmCmp *                cmpregs = nullptr;

	uint32_t						    chbit = 0;
	uint32_t                timer_base_speed = 0;
	volatile uint32_t *     valreg = nullptr;

protected:
	bool          TccDisable(); // returns the previous state
	void          TccEnable(bool aenable);
};

#define HWPWM_IMPL THwPwmChannel_atsam_v2

#endif /* SRC_HWPWM_ATSAM_V2_H_ */
