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
// file:     hwuscounter_bcm.h
// brief:    BCM implementation of the us (microseconds) counter
// created:  2024-07-16
// authors:  nvitya

#ifndef HWUSCOUNTER_BCM_H_
#define HWUSCOUNTER_BCM_H_

#include "platform.h"

#define HWUSCOUNTER_PRE_ONLY
#include "hwuscounter.h"

class THwUsCounter_bcm : public THwUsCounter_pre
{
public:
	systimer_regs_t *  regs = nullptr;  // native 1 us system timer

  bool Init();

  inline uint32_t Get32() { return regs->CLO; }
  uint64_t        Get64();
};

#define HWUSCOUNTER_IMPL THwUsCounter_bcm

#endif // def HWUSCOUNTER_BCM_H_
