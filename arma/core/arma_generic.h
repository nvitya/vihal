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
 *  file:     arma_generic.h
 *  brief:    ARM Cortex-A Generic definitions
 *  date:     2024-07-03
 *  authors:  nvitya
*/

#ifndef __ARMA_GENERIC_H
#define __ARMA_GENERIC_H

#define __DEBUG_BKPT()

#define MCU_NO_UNALIGNED 0

inline void mcu_irq_priority_set(unsigned intnum, unsigned priority)
{
}

inline void mcu_irq_pending_clear(unsigned intnum)
{
}

inline void mcu_irq_pending_set(unsigned intnum)
{
}

inline void mcu_irq_enable(unsigned intnum)
{
}

inline void mcu_irq_disable(unsigned intnum)
{
}

inline bool mcu_irq_enabled(unsigned intnum)
{
	return false;
}

inline void __attribute__((always_inline)) mcu_init_vector_table()
{
}

inline void __attribute__((always_inline)) mcu_enable_fpu()
{
}

inline void __attribute__((always_inline)) mcu_enable_icache()
{
}

inline void __attribute__((always_inline)) mcu_enable_dcache()
{
}

inline void __attribute__((always_inline)) mcu_disable_icache()
{
}

inline void __attribute__((always_inline)) mcu_disable_dcache()
{
}

#endif
