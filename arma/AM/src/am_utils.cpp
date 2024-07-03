/*
 * am_utils.cpp
 *
 *  Created on: Jul 3, 2024
 *      Author: vitya
 */

#include "platform.h"
#include "stdint.h"
#include "hw_utils.h"
#include "am_utils.h"

uint8_t * g_am_cm_regs = nullptr;

volatile uint32_t * am_get_cm_reg(uint32_t aregoffs)
{
	if (!g_am_cm_regs)
	{
		g_am_cm_regs = (uint8_t *)hw_memmap(HW_CM_PER_BASE, 0x400);
	}
	return (volatile uint32_t *)&g_am_cm_regs[aregoffs];
}

void am_enable_per(uint32_t acmregoffs, bool aenable)
{
	volatile uint32_t * cmreg = am_get_cm_reg(acmregoffs);
	if (aenable)
	{
		*cmreg = 0x00002;
	}
	else
	{
		*cmreg = 0x30000;
	}
}
