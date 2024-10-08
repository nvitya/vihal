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
 *  file:     mcu_defs.h (STM32)
 *  brief:    STM32 MCU Family definitions
 *  version:  1.00
 *  date:     2018-02-10
 *  authors:  nvitya
*/

#ifndef __MCU_DEFS_H
#define __MCU_DEFS_H

#if defined(MCUSF_F0)

  #if !defined(MAX_CLOCK_SPEED)
    #define MAX_CLOCK_SPEED         48000000
  #endif

  #define MCU_INTERNAL_RC_SPEED    8000000

#elif defined(MCUSF_G0)

  #if !defined(MAX_CLOCK_SPEED)
    #define MAX_CLOCK_SPEED         64000000
  #endif

  #define MCU_INTERNAL_RC_SPEED    16000000

#elif	defined(MCUSF_L0)

  #define MAX_CLOCK_SPEED  32000000

#elif	defined(MCUSF_F1)

  #if !defined(MAX_CLOCK_SPEED)
    #define MAX_CLOCK_SPEED  72000000
  #endif

#elif	defined(MCUSF_F3)

  #if !defined(MAX_CLOCK_SPEED)
    #define MAX_CLOCK_SPEED  72000000
  #endif

#elif	defined(MCUSF_F4)

  #if !defined(MAX_CLOCK_SPEED)
    #define MAX_CLOCK_SPEED  180000000
  #endif

  #define MCU_INTERNAL_RC_SPEED     16000000

#elif	defined(MCUSF_G4)

  #if !defined(MAX_CLOCK_SPEED)
    #define MAX_CLOCK_SPEED  170000000
  #endif

  #define MCU_INTERNAL_RC_SPEED     16000000

	#define MAX_IRQ_HANDLER_COUNT  101

#elif	defined(MCUSF_WB)

  #if !defined(MAX_CLOCK_SPEED)
    #define MAX_CLOCK_SPEED  64000000
  #endif

  #define MCU_INTERNAL_RC_SPEED     16000000

	#define MAX_IRQ_HANDLER_COUNT  101

#elif	defined(MCUSF_F7)

  #if !defined(MAX_CLOCK_SPEED)
    #define MAX_CLOCK_SPEED  216000000
  #endif

  #define MCU_INTERNAL_RC_SPEED     16000000

#elif defined(MCUSF_H7)

  #if !defined(MAX_CLOCK_SPEED)
    #define MAX_CLOCK_SPEED  400000000
  #endif

  #define MCU_INTERNAL_RC_SPEED     64000000

#endif

#ifndef MCU_INTERNAL_RC_SPEED
  #define MCU_INTERNAL_RC_SPEED     64000000
#endif

#if defined(MCUSF_H7)
  #define HWDMA_MXB
  #define HWDMA_V2
#elif defined(MCUSF_H7RS)
  #define HWDMA_V3
#elif defined(MCUSF_F1) || defined(MCUSF_F0) || defined(MCUSF_G0) || defined(MCUSF_L0) || defined(MCUSF_F3) || defined(MCUSF_G4) || defined(MCUSF_WB)
  #define HWDMA_V1
  #define HW_DMA_REGS   DMA_Channel_TypeDef
#else
  #define HWDMA_V1
  #define HW_DMA_REGS   DMA_Stream_TypeDef
#endif

#if __CORTEX_M < 3
  #if defined(TIM14)
    #define CLOCKCNT16       (TIM14->CNT)      // use the worst timer for clock counting
  #else
    #define CLOCKCNT16       (TIM21->CNT)
  #endif
#endif

inline void __attribute__((always_inline)) mcu_preinit_code()
{
}

#endif // __MCU_DEFS_H
