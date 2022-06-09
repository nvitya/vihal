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
 *  file:     hwpwm_rp.h
 *  brief:    RP2040 PWM Driver
 *  created:  2022-06-09
 *  authors:  nvitya
*/

#ifndef SRC_HWPWM_RP_H_
#define SRC_HWPWM_RP_H_

#define HWPWM_PRE_ONLY
#include "hwpwm.h"

class THwPwmChannel_rp : public THwPwmChannel_pre
{
public:
	bool          Init(int atimernum, int achnum, int aoutnum);

	void          SetOnClocks(uint16_t aclocks);
	void          Enable();
	void          Disable();
	inline bool   Enabled() { return ((regs->csr & PWM_CH0_CSR_EN_BITS) != 0); }

	void          SetFrequency(uint32_t afrequency);

public:
	pwm_slice_hw_t *        regs = nullptr;
	uint32_t                timer_base_speed = 0;
};

#define HWPWM_IMPL THwPwmChannel_rp

#endif /* SRC_HWPWM_RP_H_ */
