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
 *  file:     armm_generic.h
 *  brief:    ARM Cortex-M Generic definitions
 *  version:  1.00
 *  date:     2021-10-02
 *  authors:  nvitya
*/

#ifndef __ARMM_GENERIC_H
#define __ARMM_GENERIC_H

inline void __attribute__((always_inline)) mcu_disable_interrupts()
{
  __asm volatile ("cpsid i");
}

inline void __attribute__((always_inline)) mcu_enable_interrupts()
{
  __asm volatile ("cpsie i");
}

extern "C" void (* __isr_vectors [])();

inline void __attribute__((always_inline)) mcu_init_vector_table()
{
	#if (__CORTEX_M >= 3) || __VTOR_PRESENT
		SCB->VTOR = (unsigned)(&__isr_vectors);
	#endif
}

inline void __attribute__((always_inline)) mcu_enable_fpu()
{
	#if __FPU_PRESENT
		SCB->CPACR |= (0xF << 20); // enable the FPU
	#endif
}

inline void __attribute__((always_inline)) mcu_enable_icache()
{
	#if __ICACHE_PRESENT
		SCB_EnableICache();  // Instruction cache is always safe to enable, if present
	#endif
}

inline void __attribute__((always_inline)) mcu_enable_dcache()
{
	#if __DCACHE_PRESENT
		SCB_EnableDCache();  // Instruction cache is always safe to enable, if present
	#endif
}

inline void __attribute__((always_inline)) mcu_disable_icache()
{
	#if __ICACHE_PRESENT
		SCB_DisableICache();  // Instruction cache is always safe to enable, if present
	#endif
}

inline void __attribute__((always_inline)) mcu_disable_dcache()
{
	#if __DCACHE_PRESENT
		SCB_DisableDCache();  // Instruction cache is always safe to enable, if present
	#endif
}

#endif
