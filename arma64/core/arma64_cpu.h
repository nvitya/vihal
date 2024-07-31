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
 *  file:     arma64_cpu.h
 *  brief:    ARM-A non official CPU related definitions
 *  created:  2024-07-17
 *  authors:  nvitya
*/

#ifndef ARMA64_CPU_H_
#define ARMA64_CPU_H_

#include "stdint.h"

#define MCU_ARMA64
#define CPU_ARMA64

typedef struct
{
  uint64_t  fp;   //   0 = x29 = frame pointer
  uint64_t  lr;   //   8 = x30 = link register
  uint64_t  x0;   //  16
  uint64_t  x1;
  uint64_t  x2;   //  32
  uint64_t  x3;
  uint64_t  x4;   //  48
  uint64_t  x5;
  uint64_t  x6;   //  64
  uint64_t  x7;
  uint64_t  x8;   //  80
  uint64_t  x9;
  uint64_t  x10;  //  96
  uint64_t  x11;
  uint64_t  x12;  // 112
  uint64_t  x13;
  uint64_t  x14;  // 128
  uint64_t  x15;
  uint64_t  x16;  // 144
  uint64_t  x17;
  uint64_t  x18;  // 160
  uint64_t  xzr;
  uint64_t  esr;  // 176
  uint64_t  far;
//
} TInterruptFrame; // 192 bytes

#define __DSB()

inline void __attribute__((always_inline)) mcu_interrupts_disable()
{
}

inline unsigned __attribute__((always_inline)) mcu_interrupts_save_and_disable()
{
  return 0;
}

inline void __attribute__((always_inline)) mcu_interrupts_restore(unsigned prevstate)
{
}

inline void __attribute__((always_inline)) mcu_interrupts_enable()
{
}


inline void __attribute__((always_inline)) mcu_disable_interrupts() // deprecated
{
}

inline void __attribute__((always_inline)) mcu_enable_interrupts() // deprecated
{
}

#endif
