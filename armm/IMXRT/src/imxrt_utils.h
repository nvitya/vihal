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
 *  file:     imxrt_utils.h
 *  brief:    some generic IMXRT helper functions
 *  created:  2018-11-23
 *  authors:  nvitya
*/

#ifndef IMXRT_UTILS_H_
#define IMXRT_UTILS_H_

#include "platform.h"

static inline void imxrt_set_clock_gate(unsigned index, unsigned shift, unsigned value)
{
	if (index <= 6)
	{
		volatile uint32_t *reg;
		reg = ((volatile uint32_t *)&CCM->CCGR0) + index;
		*reg = ((*reg) & ~(3U << shift)) | ((value & 3) << shift);
	}
}


#endif /* IMXRT_UTILS_H_ */
