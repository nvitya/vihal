/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2023 Viktor Nagy, nvitya
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
 *  file:     mcu_defs.h (MSP)
 *  brief:    MSP MCU Family definitions
 *  created:  2023-06-27
 *  authors:  nvitya
*/

#ifndef __MCU_DEFS_H
#define __MCU_DEFS_H

#if defined(MCUSF_M0C)

  #if !defined(MAX_CLOCK_SPEED)
    #define MAX_CLOCK_SPEED         24000000
  #endif

  #define MCU_INTERNAL_RC_SPEED    24000000

#elif defined(MCUSF_M0G)

  #if !defined(MAX_CLOCK_SPEED)
    #define MAX_CLOCK_SPEED         80000000
  #endif

  #define MCU_INTERNAL_RC_SPEED   32000000

#else

  #error "unhandled MCU family"

#endif

#define HW_GPIO_REGS      GPIO_Regs
#define HW_UART_REGS      UART_Regs

#if defined(MCUSF_M0C)

#define TIMG_CLOCKCNT     TIMG8

#if __CORTEX_M < 3
  #define CLOCKCNT16       (TIMG_CLOCKCNT->COUNTERREGS.CTR)
#endif

#else

#define TIMG_CLOCKCNT     TIMG12

#if __CORTEX_M < 3
  #define CLOCKCNT       (TIMG_CLOCKCNT->COUNTERREGS.CTR)
#endif

#endif

inline void __attribute__((always_inline)) mcu_preinit_code()
{
}

inline void __attribute__((always_inline)) mcu_postinit_code()
{
}

#endif // __MCU_DEFS_H
