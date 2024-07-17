/*
 * clockcnt_bcm.h
 *
 *  Created on: Jul 3, 2024
 *      Author: vitya
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

#endif /* CLOCKCNT_BCM_H_ */
