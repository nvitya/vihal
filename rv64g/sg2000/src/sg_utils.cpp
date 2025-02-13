/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     sg_utils.cpp
 *  brief:    Sophgo Utilities
 *  created:  2024-04-21
 *  authors:  nvitya
*/

#include "sg_utils.h"
#include "sg2000_common_vihal.h"

clk_div_regs_t *  mapped_clk_div_regs = nullptr;

uint32_t sg_bus_speed(uint8_t abusid)
{
	return (SystemCoreClock >> 2);
}

#ifdef LINUX

#include "hw_utils.h"

void * map_hw_addr(uintptr_t aaddr, unsigned asize, void * * aptrvar)
{
  if (!aptrvar)
  {
  	return hw_memmap(aaddr, asize);
  }

  if (*aptrvar) // is already set ?
  {
  	return *aptrvar;
  }

  *aptrvar = hw_memmap(aaddr, asize);
	return *aptrvar;
}

#else

void * map_hw_addr(uintptr_t aaddr, unsigned asize, void * * aptrvar)
{
  if (!aptrvar)
  {
  	return (void *)aaddr;
  }

  if (*aptrvar) // is already set ?
  {
  	return *aptrvar;
  }

  *aptrvar = (void *)aaddr;
	return *aptrvar;
}

#endif

void set_periph_clock_enable(uint32_t en_reg_idx, uint32_t en_bit, uint32_t aenable)
{
	clk_div_regs_t *  regs = (clk_div_regs_t *)map_hw_addr(CLK_DIV_BASE_ADDR, 0x1000, (void * *)&mapped_clk_div_regs);
	if (en_reg_idx > 4)
	{
		return;
	}
	if (aenable)
	{
		regs->CLK_EN[en_reg_idx] |= (1 << en_bit);
	}
	else
	{
		regs->CLK_EN[en_reg_idx] &= ~(1 << en_bit);
	}
}
