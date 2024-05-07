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
// file:     hwuscounter_imxrt.h
// brief:    IMXRT implementation of the us (microseconds) counter
// created:  2024-04-06
// authors:  nvitya
// notes:    uses the 32-bit GPT timer, reading the GPT counter is slow,
//           takes about 80 CPU clocks !

#ifndef HWUSCOUNTER_IMXRT_H_
#define HWUSCOUNTER_IMXRT_H_

#define HWUSCOUNTER_PRE_ONLY
#include "hwuscounter.h"

class THwUsCounter_imxrt : public THwUsCounter_pre
{
public:
  int             timerdev = 2;  // 2 = GPT2 by default
  GPT_Type *      regs = nullptr;

  bool Init();

  ALWAYS_INLINE uint32_t Get32()
  {
    return regs->CNT;  // takes about 80 CPU Clocks ! (the peripheral runs with 24 MHz)
  }

  uint64_t        Get64(); // must be called at least every half an hour

protected:
  uint64_t        cnt64_high = 0;
  uint32_t        cnt32_prev = 0;
};

#define HWUSCOUNTER_IMPL THwUsCounter_imxrt

#endif // def HWUSCOUNTER_IMXRT_H_
