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
 *  brief:    some pre-defined FPGA boards with VRV100 SoC
 *  version:  1.00
 *  date:     2021-10-20
 *  authors:  nvitya
*/

#ifndef BOARDS_BUILTIN_H_
#define BOARDS_BUILTIN_H_

#if 0 // to use elif everywhere

#elif defined(BOARD_VRV1_103)

#define BOARD_NAME "VRV1_103"
#define MCU_VRV1_103
#define MCU_FIXED_SPEED  100000000

#elif defined(BOARD_VRV1_104)

#define BOARD_NAME "VRV1_104"
#define MCU_VRV1_104
#define MCU_FIXED_SPEED  100000000

#elif defined(BOARD_VRV1_241)

#define BOARD_NAME "VRV1_241"
#define MCU_VRV1_241
#define MCU_FIXED_SPEED  100000000

#elif defined(BOARD_VRV1_401)

#define BOARD_NAME "VRV1_401"
#define MCU_VRV1_401
#define MCU_FIXED_SPEED  100000000

#elif defined(BOARD_VRV1_441)

#define BOARD_NAME "VRV1_441"
#define MCU_VRV1_441
#define MCU_FIXED_SPEED  100000000

#elif defined(BOARD_VRV1_443)

#define BOARD_NAME "VRV1_443"
#define MCU_VRV1_443
#define MCU_FIXED_SPEED  100000000

#elif defined(BOARD_VRV1_543)

#define BOARD_NAME "VRV1_543"
#define MCU_VRV1_543
#define MCU_FIXED_SPEED  100000000

#else

  #error "Unknown board."

#endif


#endif /* BOARDS_BUILTIN_H_ */
