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
 *  file:     mcu_builtin.h (SG2000)
 *  brief:    Built-in CV1800/SG200x MCU definitions
 *  date:     2024-04-21
 *  authors:  nvitya
*/

#ifndef _MCU_BUILTIN_H_
#define _MCU_BUILTIN_H_

#if 0

#elif defined(MCU_RK3506)

  #define MCUSF_35
  #define MAX_CLOCK_SPEED  200000000

  #include "rk3506_vihal.h"

#else

  #error "Unknown RK MCU"

#endif

#endif // _MCU_BUILTIN_H_
