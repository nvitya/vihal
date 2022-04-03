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
 *  file:     armm_cpu.h
 *  brief:    ARMM non official CPU related definitions
 *  version:  1.00
 *  date:     2022-02-06
 *  authors:  nvitya
*/

#ifndef ARMM_CPU_H_
#define ARMM_CPU_H_

#include "stdint.h"

#define MCU_ARMM
#define CPU_ARMM

inline void __attribute__((always_inline)) mcu_disable_interrupts()
{
  __asm volatile ("cpsid i" : : : "memory");
}

inline void __attribute__((always_inline)) mcu_enable_interrupts()
{
  __asm volatile ("cpsie i" : : : "memory");
}

class TCriticalSection
{
public:
  uint32_t priMask;
  bool init;

  inline void enter()
  {
    init = true;

    // get priMask
    __asm volatile ("MRS %0, primask" : "=r" (priMask) :: "memory");

    // disable interrupt
    __asm volatile ("cpsid i" : : : "memory");
  }

  inline void leave()
  {
    if(init)
    {
      init = false;

      // set priMask
      __asm volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
    }
  }

  TCriticalSection(bool enter = false)
  {
    init = enter;
    if(enter)
    {
      // get priMask
      __asm volatile ("MRS %0, primask" : "=r" (priMask) :: "memory");

      // disable interrupt
      __asm volatile ("cpsid i" : : : "memory");
    }
  }

  ~TCriticalSection()
  {
    if(init)
    {
      // set priMask
      __asm volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
    }
  }
};

#endif
