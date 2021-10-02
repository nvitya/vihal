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
 *  file:     clockcnt.h
 *  brief:    Clock Counter vendor-independent definitions
 *  version:  1.00
 *  date:     2021-09-28
 *  authors:  nvitya
*/

#ifndef CLOCKCNT_H_
#define CLOCKCNT_H_

#include "platform.h"

#define clockcnt_t unsigned

#ifndef CLOCKCNT
  #error "Define CLOCKCNT !"
#endif

#define ELAPSEDCLOCKS(t1, t0) ((clockcnt_t)(t1 - t0))

void clockcnt_init();  // it is platform dependent

void delay_clocks(unsigned aclocks);

inline void delay_us(unsigned aus)
{
	delay_clocks(aus * (SystemCoreClock / 1000000));
}

inline void delay_ms(unsigned ams)
{
	delay_clocks(ams * (SystemCoreClock / 1000));
}

#endif /* CLOCKCNT_H_ */
