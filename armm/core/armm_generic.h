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

#if defined(DEBUG)
  #define __DEBUG_BKPT()  asm volatile ("bkpt 0")
#else
  #define __DEBUG_BKPT()
#endif

#ifndef CLOCKCNT

  #if __CORTEX_M >= 3

    // from Cortex-M3 use the DWT_CYCCNT:
    #define CLOCKCNT (*((volatile unsigned *)0xE0001004))
    #define CLOCKCNT_BITS  32

  #elif defined(CLOCKCNT16)

    // !!! WARNING !!!
    // providing the 32 bit CLOCKCNT for the Cortex-M0 platforms has the following disadvantages:
    //   - works properly only when it is called repeatedly within 0.683 ms
    //   - not safe to call from interrupt context

    #define ELAPSEDCLOCKS16(t1, t0) ((uint16_t)(t1 - t0))

    extern uint32_t clockcnt32_high;
    extern uint16_t last_clockcnt16;

    inline unsigned clockcnt16_to_32()
    {
      uint16_t t0 = CLOCKCNT16;
      if (t0 < last_clockcnt16)
      {
        clockcnt32_high += 0x10000;
      }
      last_clockcnt16 = t0;
      return clockcnt32_high + t0;
    }

    #define CLOCKCNT clockcnt16_to_32()

  #else
    // On Cortex-M0 a 16 or 32 bit timer hw required
    #error "Define CLOCKCNT_BITS for Cortex-M0 processors (hw timer required)"
  #endif

#endif

inline void mcu_irq_priority_set(unsigned intnum, unsigned priority)
{
  NVIC_SetPriority(IRQn_Type(intnum), priority);
}

inline void mcu_irq_pending_clear(unsigned intnum)
{
  NVIC_ClearPendingIRQ(IRQn_Type(intnum));
}

inline void mcu_irq_pending_set(unsigned intnum)
{
  NVIC_SetPendingIRQ(IRQn_Type(intnum));
}

inline void mcu_irq_enable(unsigned intnum)
{
  NVIC_EnableIRQ(IRQn_Type(intnum));
}

inline void mcu_irq_disable(unsigned intnum)
{
  NVIC_DisableIRQ(IRQn_Type(intnum));
}

inline bool mcu_irq_enabled(unsigned intnum)
{
  return (NVIC_GetEnableIRQ(IRQn_Type(intnum)) != 0);
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
