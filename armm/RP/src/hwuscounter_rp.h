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
// file:     hwuscounter_rp.h
// brief:    RP2040 implementation of the us (microseconds) counter
// created:  2024-04-07
// authors:  nvitya

#ifndef HWUSCOUNTER_RP_H_
#define HWUSCOUNTER_RP_H_

#define HWUSCOUNTER_PRE_ONLY
#include "hwuscounter.h"

class THwUsCounter_rp : public THwUsCounter_pre
{
public:
  int             timerdev = 2;  // 2 = TIM2 by default, alternate: 5 = TIM5
  timer_hw_t *    regs = nullptr;

  bool Init();

  inline uint32_t Get32()
  {
    return regs->timelr;
  }

  inline uint64_t Get64()
  {
    uint32_t lowreg = regs->timelr;  // reading the low latches the high part
    return (uint64_t(regs->timehr) << 32) | lowreg;
  }
};

#define HWUSCOUNTER_IMPL THwUsCounter_rp

#endif // def HWUSCOUNTER_RP_H_
