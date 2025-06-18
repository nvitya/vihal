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
 *  file:     boards_builtin.h (IMXRT)
 *  brief:    Built-in IMXRT board definitions
 *  created:  2018-11-23
 *  authors:  nvitya
*/

#ifndef BOARDS_BUILTIN_H_
#define BOARDS_BUILTIN_H_

#if 0 // to use elif everywhere

//-------------------------------------------------------------------------------------------------
// IMXRT
//-------------------------------------------------------------------------------------------------

#elif defined(BOARD_EVK_IMXRT1020)

  #define BOARD_NAME "IMXRT1020-EVK"
  #define MCU_IMXRT1021
  #define EXTERNAL_XTAL_HZ   24000000

#elif defined(BOARD_EVK_IMXRT1024)

  #define BOARD_NAME "IMXRT1024-EVK"
  #define MCU_IMXRT1024
  #define EXTERNAL_XTAL_HZ   24000000

#elif defined(BOARD_EVK_IMXRT1050A)

  #define BOARD_NAME "IMXRT1050-EVK-A"
  #define MCU_IMXRT1052A
  #define EXTERNAL_XTAL_HZ   24000000

#elif defined(BOARD_EVK_IMXRT1050) || defined(BOARD_EVK_IMXRT1050B)

  #define BOARD_NAME "IMXRT1050-EVK-B"
  #define MCU_IMXRT1052
  #define EXTERNAL_XTAL_HZ   24000000

#elif defined(BOARD_EVK_IMXRT1040)

  #define BOARD_NAME "IMXRT1040-EVK"
  #define MCU_IMXRT1042
  #define EXTERNAL_XTAL_HZ   24000000

#else

  #error "Unknown board."

#endif


#endif /* BOARDS_BUILTIN_H_ */
