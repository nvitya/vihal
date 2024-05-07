/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2018 Viktor Nagy, nvitya
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
 *  file:     hwpwm_msp.h
 *  brief:    MSPM0G PWM Driver
 *  date:     2024-05-07
 *  authors:  nvitya
 *  note:
 *    only the TIMG units are supported
*/


#ifndef SRC_HWPWM_MSP_H_
#define SRC_HWPWM_MSP_H_

#define HWPWM_PRE_ONLY
#include "hwpwm.h"

class THwPwmChannel_msp : public THwPwmChannel_pre
{
public:
	bool          Init(int atimernum, int achnum);

	void          SetOnClocks(uint16_t aclocks);
	void          Enable();
	void          Disable();
	inline bool   Enabled() { return false; } //return ((regs->CCER & outenbit) != 0); }

	void          SetFrequency(uint32_t afrequency);

public:
	GPTIMER_Regs *          regs = nullptr;
	bool                    advanced_timer = false;

	uint32_t						    outenbit = 0;
	uint32_t                timer_base_speed = 0;
	volatile uint32_t *     valreg = nullptr;
};

#define HWPWM_IMPL THwPwmChannel_msp

#endif /* SRC_HWPWM_MSP_H_ */
