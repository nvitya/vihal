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
 *  file:     gd32v_utils.h
 *  brief:    GD32V Utilities
 *  version:  1.00
 *  date:     2021-10-21
 *  authors:  nvitya
*/

#ifndef GD32V_UTILS_H_
#define GD32V_UTILS_H_

#include "platform.h"

// constants helping determine peripheral bus base frequencies
#define GD32V_BUSID_AHB     0
#define GD32V_BUSID_APB1    1
#define GD32V_BUSID_APB2    2

uint32_t gd32v_bus_speed(uint8_t abusid);

#endif /* GD32V_UTILS_H_ */
