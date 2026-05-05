/* -----------------------------------------------------------------------------
 * This file is a part of the NVCM project: https://github.com/nvitya/nvcm
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
 *  file:     hwpwm_py32.cpp
 *  brief:    PY32 PWM Driver
 *  date:     2026-05-04
 *  authors:  nvitya
*/

#include "hwpwm.h"

#include "py32_utils.h"

bool THwPwmChannel_py32::Init(int atimernum, int achnum, int aoutnum) // outnum: 0 = A, 1 = B
{
	initialized = false;
	advanced_timer = false;

	devnum = atimernum;
	chnum = achnum;
	outnum = (aoutnum & 1);

	regs = nullptr;

  if (false)
  {

  }
#ifdef TIM1
	if (1 == devnum)
	{
		regs = TIM1;
		RCC->APBENR2 |= RCC_APBENR2_TIM1EN;
		advanced_timer = true;
	}
#endif
#ifdef TIM2
	else if (2 == devnum)
	{
		regs = TIM2;
		RCC->APBENR1 |= RCC_APBENR1_TIM2EN;
	}
#endif
#ifdef TIM16
	else if (16 == devnum)
	{
		regs = TIM16;
		RCC->APBENR2 |= RCC_APBENR2_TIM16EN;
		advanced_timer = true;
	}
#endif
#ifdef TIM17
	else if (17 == devnum)
	{
		regs = TIM17;
		RCC->APBENR2 |= RCC_APBENR2_TIM17EN;
		advanced_timer = true;
	}
#endif

	if (!regs)
	{
		return false;
	}

	if (1 == chnum)
	{
		valreg = (volatile uint32_t *)&(regs->CCR1);
	}
	else if (2 == chnum)
	{
		valreg = (volatile uint32_t *)&(regs->CCR2);
	}
	else if (3 == chnum)
	{
		valreg = (volatile uint32_t *)&(regs->CCR3);
	}
	else if (4 == chnum)
	{
		valreg = (volatile uint32_t *)&(regs->CCR4);
	}
	else
	{
		return false;
	}

	chpos = (chnum - 1);

	timer_base_speed = py32_bus_speed(1);
	if (timer_base_speed < SystemCoreClock)
	{
	  timer_base_speed = (timer_base_speed << 1);  // the timer clock speed is twice of the APB speed
	}

	regs->SMCR = 0;

	SetFrequency(frequency);

	*valreg = 0;

	// setup the capture and compare mode register

	uint32_t chv = 0
		| (0 <<  7)  // OCxCE: Output compare 1 clear enable
		| (6 <<  4)  // OCxM(3): Output compare 1 mode, 6 = PWM mode 1
		| (1 <<  3)  // OCxPE: 0 = can be written any time, 1 = CCR activated at each update event
		| (0 <<  2)  // OCxFE: Output compare 1 fast enable
		| (0 <<  0)  // CCxS(2): Capture/Compare 1 selection, 0 = CHx is output
	;

	uint32_t vshift = 8 * (chpos & 1);

	volatile uint32_t * ccmr;
	if (chnum >= 3)
	{
		ccmr = (volatile uint32_t *)&regs->CCMR2;
	}
	else
	{
		ccmr = (volatile uint32_t *)&regs->CCMR1;
	}

	*ccmr &= ~(0xFF << vshift);
	*ccmr |=  (chv  << vshift);

	outenbit = (1 << (chpos << 2));


	uint32_t ccer =	regs->CCER;
	ccer &= ~(0xF << (chpos << 2)); // set normal polarity, disable output
	if (inverted)
	{
		ccer |= (2 << (chpos << 2));  // set inverted polarity
	}
	regs->CCER = ccer;

	regs->DCR = 0;
	//regs->AF1 = 0;
	//regs->AF2 = 0;
	//regs->DIER = 0;
	regs->DIER = (0x1F << 8);  // ENABLE DMA events

	if (advanced_timer)
	{
		regs->BDTR = (0
		  | (1 << 15)  // MOE: main output enable
		  | (1 << 14)  // AOE: automatic output enable
		  | (0 << 12)  // BKE: 0 = no brake handling
		);
	}

	regs->CR1 = 0
	  | (0 <<  8)  // CKD(2): clock division
	  | (1 <<  7)  // ARPE: Auto-reload preload enable, 1 = buffered ARR
	  | (0 <<  5)  // CMS(2): Center aligned mode, 0 = edge aligned
	  | (0 <<  4)  // DIR: 0 = upcounter, 1 = downcounter
	  | (0 <<  3)  // OPM: one pulse mode
	  | (0 <<  2)  // URS: Update request source
	  | (0 <<  1)  // UDIS: Update disable
	  | (1 <<  0)  // CEN: Counter enable
	;

	regs->CR2 = 0;

	regs->EGR = 1;

	initialized = true;
	return true;
}

void THwPwmChannel_py32::SetFrequency(uint32_t afrequency)
{
	frequency = afrequency;

	uint32_t prescaler = 0;
	do
	{
		++prescaler;
		periodclocks = timer_base_speed / (prescaler * frequency);
	}
	while (periodclocks > 65535);

	regs->PSC = prescaler - 1; // prescaler
	regs->ARR = periodclocks - 1;
}


void THwPwmChannel_py32::SetOnClocks(uint16_t aclocks)
{
	*valreg = aclocks;
	regs->EGR = (1 << 0);  // UG
}

void THwPwmChannel_py32::Enable()
{
	regs->CCER |= outenbit;
}

void THwPwmChannel_py32::Disable()
{
	regs->CCER &= ~outenbit;
}

