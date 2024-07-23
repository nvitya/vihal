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
 *  file:     clockcnt_bcm.h
 *  brief:    Clock Counter for 64-bit ARM-A MPUs
 *  date:     2024-07-17
 *  authors:  nvitya
*/

#ifndef CLOCKCNT_BCM_H_
#define CLOCKCNT_BCM_H_

void clockcnt_pmu_init();
inline uint64_t clockcnt_pmu_get(void)
{
	uint64_t val;
	asm volatile("mrs %0, pmccntr_el0" : "=r"(val));  // if you get an illegal instruction here, the PMU is not enabled in the kernel.
	return val;
}

uint64_t clockcnt_get(void);

#define CLOCKCNT           clockcnt_get()
#define CLOCKCNT_BITS      64

#define PMUCLKCNT          clockcnt_pmu_get()  // speed varies with the frequency scaling of the linux cores!

#endif /* CLOCKCNT_BCM_H_ */
