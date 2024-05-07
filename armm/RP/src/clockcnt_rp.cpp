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
 *  file:     clockcnt_rp.cpp
 *  brief:    RP2040 Clock Counter using the PWM7
 *  date:     2018-02-10
 *  authors:  nvitya
*/

#include "platform.h"
#include "rp_utils.h"

#if __CORTEX_M < 3

// clock timer initialization for Cortex-M0 processors

void clockcnt_init()
{
  rp_reset_control(RESETS_RESET_PWM_BITS, false); // remove reset

  pwm_slice_hw_t *  regs = &pwm_hw->slice[7];

  regs->div = (1 << 4); // no division
  regs->top = 0xFFFF;
  regs->csr = (0
    | (0  <<  7)  // PH_ADV: phase correction forwards
    | (0  <<  6)  // PH_RET: phase correction downwards
    | (0  <<  4)  // DIVMODE(2): 0 = normal free running mode, 1-3 = channel B input clock
    | (0  <<  3)  // B_INV: 1 = invert output B
    | (0  <<  2)  // A_INV: 1 = invert output A
    | (0  <<  1)  // PH_CORRECT
    | (1  <<  0)  // EN: 1 = enable
  );
}

#endif

