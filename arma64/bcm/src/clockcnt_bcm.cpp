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
 *  file:     clockcnt_bcm.cpp
 *  brief:    Clock Counter for 64-bit ARM-A MPUs
 *  date:     2024-07-17
 *  authors:  nvitya
*/

#include "platform.h"
#include "clockcnt.h"
#include "bcm_utils.h"

systimer_regs_t * g_systimer_regs = nullptr;

void clockcnt_init(void)
{
	map_hw_addr(HW_SYSTIMER_BASE, sizeof(systimer_regs_t), (void * *)&g_systimer_regs);
	SystemCoreClock = 1000000; // 1 us system timer !
}

uint64_t clockcnt_get()
{
	systimer_regs_t * regs = g_systimer_regs;

  uint32_t uslow1;
  uint32_t ushigh1;
  uint32_t ushigh2;
  do
  {
    ushigh1 = regs->CHI;
    uslow1  = regs->CLO;
    ushigh2 = regs->CHI;
  }
  while (ushigh1 != ushigh2);

  return (uint64_t(ushigh1) << 32) | uslow1;
}

// PMU is very fast to read out, but there are two problems with it:
//  1. the access to PMU registers must be enabled in kernel, for example with the kernel module
//  2. the cycle counters are local to every cpu core, and they are not synchronized
//     it is not useable as a global timer, and might provide very bad results on CPU migration

void clockcnt_pmu_init()
{
	uint64_t val;

  #ifndef LINUX
    // these two must be done in kernel:
    /* Disable cycle counter overflow interrupt */
    asm volatile("msr pmintenset_el1, %0" : : "r" ((uint64_t)(0 << 31)));

    /* Enable user-mode access to cycle counters. */
    asm volatile("msr pmuserenr_el0, %0" :: "r" ((1 << 0) | (1 << 2)));
  #endif

	/* Enable cycle counter */
	asm volatile("msr pmcntenset_el0, %0" :: "r" (1 << 31));

	/* Start cycle counter, do not reset and start */
	asm volatile("mrs %0, pmcr_el0" : "=r" (val));
  #ifndef LINUX
	  val |= ((1 << 0) | (1 << 2));  // with reset on Bare-Metal
  #else
	  val |= ((1 << 0)); // without reset on Linux
  #endif
	asm volatile("isb");
	asm volatile("msr pmcr_el0, %0" :: "r" (val));

	val = (1 << 27);
	asm volatile("msr pmccfiltr_el0, %0" :: "r" (val));
}
