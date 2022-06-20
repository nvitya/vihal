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
 *  file:     mcu_defs.h (GD32V)
 *  brief:    GD32V special MCU definitions
 *  version:  1.00
 *  date:     2021-10-20
 *  authors:  nvitya
*/

#ifndef __MCU_DEFS_H
#define __MCU_DEFS_H

#include "stdint.h"
#include "rv32i_cpu.h"

#define MCUF_GD32V

#define MCU_INTERNAL_RC_SPEED   8000000

#define HW_DMA_MAX_COUNT          32768

inline void __attribute__((always_inline)) mcu_preinit_code()
{
}

inline void mcu_irq_priority_set(unsigned intnum, unsigned priority)
{
  eclic_int_set_level_prio(intnum, priority);
}

inline void mcu_irq_pending_clear(unsigned intnum)
{
  ECLIC->INT[intnum].IP = 0;
}

inline void mcu_irq_pending_set(unsigned intnum)
{
  ECLIC->INT[intnum].IP = 1;
}

inline void mcu_irq_enable(unsigned intnum)
{
  eclic_int_enable(intnum);
}

inline void mcu_irq_disable(unsigned intnum)
{
  eclic_int_disable(intnum);
}

inline bool mcu_irq_enabled(unsigned intnum)
{
  return eclic_int_enabled(intnum);
}


#endif // __MCU_DEFS_H
