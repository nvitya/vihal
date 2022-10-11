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
 *  file:     lpc_v3_utils.cpp
 *  brief:    LPC v3 (LPC54x, LPC55x) utilities
 *  date:     2022-10-11
 *  authors:  nvitya
*/

#ifndef LPC_V3_UTILS_H_
#define LPC_V3_UTILS_H_

#include "platform.h"

#ifdef SYSCON_FCCLKSELX_COUNT

  #define FLEXCOMM_BASE_SPEED  96000000
  #define FLEXCOMM_COUNT       8

#else

  #define FLEXCOMM_BASE_SPEED  48000000
  #define FLEXCOMM_COUNT       10

#endif

FLEXCOMM_Type * flexcomm_init(int fcnum, int pselid);

#endif /* LPC_V3_UTILS_H_ */
