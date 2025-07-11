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
 *  file:     sg_utils.h
 *  brief:    Sophgo Utilities
 *  created:  2024-04-21
 *  authors:  nvitya
*/

#ifndef _SG_UTILS_H_
#define _SG_UTILS_H_

#include "platform.h"

uint32_t sg_bus_speed(uint8_t abusid);

void * map_hw_addr(uintptr_t aaddr, unsigned asize, void * * aptrvar);

void set_periph_clock_enable(uint32_t en_reg_idx, uint32_t en_bit, uint32_t aenable);

#endif // _SG_UTILS_H_
