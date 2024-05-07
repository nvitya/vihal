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
 *  file:     platform_generic.h
 *  brief:    Generic platform definitions / defaults. Included at the end of platform.h
 *  version:  1.00
 *  date:     2022-02-06
 *  authors:  nvitya
*/

#ifndef PLATFORM_GENERIC_H_
#define PLATFORM_GENERIC_H_

#define VIHAL

#if !defined(MAX_CLOCK_SPEED)
  #ifdef MCU_FIXED_SPEED
    #define MAX_CLOCK_SPEED  MCU_FIXED_SPEED
  #else
    #error "MCU Maximal clock speed is not defined!"
  #endif
#endif

#ifndef MCU_CLOCK_SPEED
  #define MCU_CLOCK_SPEED  MAX_CLOCK_SPEED
#endif

#if !defined(SPI_SELF_FLASHING)
  #define SPI_SELF_FLASHING  0
#endif

#if !defined(MCU_NO_DMA)
  #define MCU_NO_DMA   0
#endif

#if !defined(MCU_NO_UNALIGNED)
  #define MCU_NO_UNALIGNED  0
#endif

#endif
