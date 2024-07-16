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
// file:     hwuscounter_am.h
// brief:    AM24x implementation of the us (microseconds) counter
// created:  2024-07-16
// authors:  nvitya

#ifndef HWUSCOUNTER_AM_H_
#define HWUSCOUNTER_AM_H_

#include "platform.h"

#define HWUSCOUNTER_PRE_ONLY
#include "hwuscounter.h"

typedef struct
{
  volatile uint32_t  CNTCR;     // 00
  volatile uint32_t  CNTSR;     // 04
  volatile uint32_t  CNTCV_LO;  // 08
  volatile uint32_t  CNTCV_HI;  // 0C
  uint32_t _pad_10[4];          // 10
  volatile uint32_t  CNTFID0;   // 20
  volatile uint32_t  CNTFID1;   // 24
//
} am_gtc_regs_t;

class THwUsCounter_am : public THwUsCounter_pre
{
public:
  am_gtc_regs_t *  regs = nullptr;

  bool Init();

  uint32_t        Get32();
  uint64_t        Get64(); // must be called at least every half an hour

protected:
  uint64_t        cnt64_high = 0;
  uint32_t        cnt32_prev = 0;
};

#define HWUSCOUNTER_IMPL THwUsCounter_am

#endif // def HWUSCOUNTER_AM_H_
