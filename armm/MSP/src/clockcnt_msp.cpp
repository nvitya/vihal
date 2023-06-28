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
 *  file:     clockcnt_msp.cpp
 *  brief:    MSP Counter for M0 MCUs
 *  date:     2023-06-28
 *  authors:  nvitya
*/

#include "platform.h"

#if __CORTEX_M < 3

// clock timer initialization for Cortex-M0 processors

void clockcnt_init()
{
#warning "implement M0 clock counter!"

#if 0

#if defined(TIM14)
	#if defined(RCC_APBENR2_TIM14EN)
		RCC->APBENR2 |= RCC_APBENR2_TIM14EN;
	#else
	  RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
	#endif
  #define CCTIMER  TIM14

#else
	RCC->APB2ENR |= RCC_APB2ENR_TIM21EN;
  #define CCTIMER  TIM21
#endif

	CCTIMER->CR1 = 0;
	CCTIMER->PSC = 0; // count every clock
	CCTIMER->CR1 = 1;
	CCTIMER->EGR = 1; // reinit, start the timer

#endif
}

#endif

