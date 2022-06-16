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
 *  file:     hwextirq_gd32v.h
 *  brief:    GD32V Extenal Pin Interrupt
 *  date:     2022-06-16
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwextirq_gd32v.h"

bool THwExtIrq_gd32v::Init(int aportnum, int apinnum, unsigned flags)
{
#if 0
	apinnum = (apinnum & 0x0F);

	irqpend_reg = &REG_EXTI_PR;
	irqack_reg  = &REG_EXTI_PR;
	pin_mask = (1 << apinnum);

	int cridx  = (apinnum >> 2);
	int rshift = ((apinnum & 3) << 2);

#if defined(SYSCFG_EXTICR1_EXTI0)

	#ifdef RCC_APB2ENR_SYSCFGEN
		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	#endif

	volatile uint32_t * pcfgreg = &SYSCFG->EXTICR[cridx];

#elif defined(AFIO_EXTICR1_EXTI0)

	volatile uint32_t * pcfgreg = &AFIO->EXTICR[cridx];

#elif defined(EXTI_EXTICR1_EXTI0)

	volatile uint32_t * pcfgreg = &EXTI->EXTICR[cridx];

#endif

	uint32_t tmp = *pcfgreg;
	tmp &= ~(15 << rshift);
	tmp |= ((aportnum & 15) << rshift);
	*pcfgreg = tmp;

	if (flags & HWEXTIRQ_FALLING)
	{
		REG_EXTI_FTSR |=  pin_mask;
	}
	else
	{
		REG_EXTI_FTSR &= ~pin_mask;
	}

	if (flags & HWEXTIRQ_RISING)
	{
		REG_EXTI_RTSR |=  pin_mask;
	}
	else
	{
		REG_EXTI_RTSR &= ~pin_mask;
	}

#endif

	IrqAck();
	Enable();

  return true;
}

void THwExtIrq_gd32v::Enable()
{
#if 0
	REG_EXTI_EMR |= pin_mask;
	REG_EXTI_IMR |= pin_mask;
#endif
}

void THwExtIrq_gd32v::Disable()
{
#if 0
	REG_EXTI_EMR &= ~pin_mask;
	REG_EXTI_IMR &= ~pin_mask;
#endif
}
