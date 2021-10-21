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
 *  file:     boards_builti.h
 *  brief:    some pre-defined boards with GD32V
 *  version:  1.00
 *  date:     2021-10-20
 *  authors:  nvitya
*/

#ifndef BOARDS_BUILTIN_H_
#define BOARDS_BUILTIN_H_

#if 0 // to use elif everywhere

#elif defined(BOARD_LONGAN_NANO)

  #define BOARD_NAME "Longan Nano GD32VF103CB board"
  #define MCU_GD32VF103CB
  #define EXTERNAL_XTAL_HZ   8000000

#else

  #error "Unknown board."

#endif


#endif /* BOARDS_BUILTIN_H_ */
