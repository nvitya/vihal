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
 *  file:     hwpwm_sg.h
 *  brief:    SG200x/CV1800 (Sophgo) PWM Driver
 *  created:  2024-05-24
 *  authors:  nvitya
*/

#ifndef SRC_HWPWM_SG_H_
#define SRC_HWPWM_SG_H_

#define HWPWM_PRE_ONLY
#include "hwpwm.h"

class THwPwmChannel_sg : public THwPwmChannel_pre
{
public:
	bool          Init(int adevnum, int achnum);
  bool          Init(int apwmnum);

	void          SetOnClocks(uint32_t aclocks);
	uint32_t      GetOnClocks() { return on_clocks; };
	void          Enable();
	void          Disable();
	inline bool   Enabled() { return ((regs->PWMSTART & channel_bit) != 0); }

	void          SetFrequency(uint32_t afrequency);

public:
	pwm_regs_t *         regs = nullptr;
	volatile uint32_t *  reg_period = nullptr;
	volatile uint32_t *  reg_hlperiod = nullptr;
	uint32_t             base_speed = 0;
	uint32_t             channel_bit = 0;
	uint32_t             on_clocks = 0;
	bool                 chenabled = false;
};

#define HWPWM_IMPL THwPwmChannel_sg

#endif /* SRC_HWPWM_SG_H_ */
