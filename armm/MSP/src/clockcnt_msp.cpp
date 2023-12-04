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
	GPTIMER_Regs * regs = TIMG_CLOCKCNT;

  regs->GPRCM.PWREN = (GPTIMER_PWREN_KEY_UNLOCK_W | GPTIMER_PWREN_ENABLE_ENABLE);
  regs->CLKSEL = GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE;
  regs->CLKDIV = 0; // do not divide
  regs->COMMONREGS.CPS = 0; // no pre-scaling
  regs->COMMONREGS.CCLKCTL = 1;

  regs->COUNTERREGS.LOAD = 0xFFFFFFFF;
  regs->COUNTERREGS.CTRCTL = (0
  	| GPTIMER_CTRCTL_EN_MASK
#if defined(MCU_EARLY_SAMPLE)
		//| GPTIMER_CTRCTL_CM_DOWN  // no upcounting mode yet...
#else
		| GPTIMER_CTRCTL_CM_UP
#endif
    | GPTIMER_CTRCTL_REPEAT_REPEAT_1
  );
}

#endif

