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
 *  file:     hwpwm_rk.cpp
 *  brief:    RK3506 PWM Driver
 *  created:  2025-09-09
 *  authors:  nvitya
*/

#include "hwpwm.h"
#include "traces.h"

#include "rk_utils.h"

bool THwPwmChannel_rk::Init(int adevnum, int achnum)
{
	uint32_t tmp;

	initialized = false;

	devnum = adevnum;
	chnum = (achnum & 7);
	outnum = chnum;

	if (0 == devnum)
	{
		map_hw_addr(PWM0_BASE + 0x1000 * chnum, sizeof(PWM_REG), (void * *)&regs);
    cru_pmu_reg_set_field(0x800, 15, 1, 0); // pclk_pwm0
    cru_pmu_reg_set_field(0x804,  0, 1, 0); // clk_pwm0
	}
	else if (1 == devnum)
  {
		map_hw_addr(PWM1_BASE + 0x1000 * chnum, sizeof(PWM_REG), (void * *)&regs);
    cru_reg_set_field(0x830,  4, 6, 0x38); // enable clk_pwm1 + pclk_pwm1 + osc_pwm1
  }
  else
  {
    regs = nullptr;
  }

  if (!regs)
  {
    return false;
  }

  Disable();  // disable first otherwise the settings sill be ineffective

  base_speed = 24000000;

  regs->CLK_CTRL = (0
  	| (0xFFFF << 16)  // write bits
  	| (0 << 15) // clk_global_sel: 0 = left_alignedpositive
  	| (1 << 13) // clk_src_sel: 1 = osc
  	| (0 <<  4) // sclale(9): /2
  	| (0 <<  0) // prescale(3)
  );

  tmp = (0
  	| (0x003F << 16)  // write bits
  	| (1 << 5) // aligned_mode_valid
  	| (0 << 4) // output_mode: 0 = left_aligned
  	| (0 << 3) // inactive_pol: 1 = positive
  	| (0 << 2) // duty_pol: 1 = positive
  	| (1 << 0) // pwm_mode(2): 1 = continuous
  );
  if (!inverted)
  {
  	tmp |= (1 << 2);
  }
  else
  {
  	tmp |= (1 << 3);
  }
  regs->CTRL = tmp;

  SetFrequency(frequency);
  SetOnClocks(0);

	initialized = true;
	return true;
}

void THwPwmChannel_rk::SetFrequency(uint32_t afrequency)
{
	frequency = afrequency;
	periodclocks = base_speed / frequency;
	regs->PERIOD = periodclocks;
	if (chenabled)
	{
	  regs->ENABLE = ((1 << 18) | (1 << 2));  // PWM_CTL_ENABLE
	}
}

void THwPwmChannel_rk::SetOnClocks(uint32_t aclocks)
{
  on_clocks = aclocks;
  regs->DUTY = aclocks;
  if (chenabled)
  {
	  regs->ENABLE = ((1 << 18) | (1 << 2));  // PWM_CTL_ENABLE
  }
}

void THwPwmChannel_rk::Enable()
{
	regs->ENABLE = 0x00030003;
	chenabled = true;
}

void THwPwmChannel_rk::Disable()
{
	regs->ENABLE = 0x00030000;
  chenabled = false;
}

