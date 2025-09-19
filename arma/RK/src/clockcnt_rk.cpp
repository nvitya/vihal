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
 *  file:     clockcnt_rk.cpp
 *  brief:    Clock Counter for RK3506
 *  date:     2025-09-06
 *  authors:  nvitya
*/

#include "platform.h"
#include "clockcnt.h"
#include "rk_utils.h"

struct TIMER_REG *  g_clkcnt_timer = nullptr;

void clockcnt_init()
{
	map_hw_addr(TIMER5_BASE, sizeof(struct TIMER_REG), (void * *)&g_clkcnt_timer);

	// enable the clk_gpll_div_100m = 100MHz clock
	cru_reg_set_field(0x800, 6, 1, 0);  // 0x800 = CRU_GATE_CON00
  // select the 100 MHz clock for the TIMER5:
	cru_reg_set_field(0x35C, 6, 3, 1);  // 0x35C = CRU_CLKSEL_CON23

	// enable the pclk_timer0:
	cru_reg_set_field(0x818, 2, 1, 0);  // 0x818 = CRU_GATE_CON6
	// enable the timer0_c5:
	cru_reg_set_field(0x818, 8, 1, 0);  // 0x818 = CRU_GATE_CON6

	// start this timer in free running mode
	g_clkcnt_timer->CONTROLREG = 0;
  g_clkcnt_timer->LOAD_COUNT[0] = 0xFFFFFFFFU;
  g_clkcnt_timer->LOAD_COUNT[1] = 0xFFFFFFFFU;
	g_clkcnt_timer->CONTROLREG = 1;

	SystemCoreClock = 93750000;  // probably GPLL / 16;
}

uint32_t clockcnt_get()
{
	return g_clkcnt_timer->CURRENT_VALUE[0];
}
