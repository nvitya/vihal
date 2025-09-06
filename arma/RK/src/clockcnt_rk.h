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
 *  file:     clockcnt_rk.h
 *  brief:    RK3506 ClockCnt Timer
 *  date:     2025-09-06
 *  authors:  nvitya
*/

#ifndef CLOCKCNT_RK_H_
#define CLOCKCNT_RK_H_

#include "stdint.h"

extern uint32_t clockcnt_get();

#ifndef CLOCKCNT_TIMER_NUM
  #define CLOCKCNT_TIMER_NUM  5
#endif
#define CLOCKCNT           clockcnt_get()
#define CLOCKCNT_BITS      32

#endif /* CLOCKCNT_RK_H_ */
