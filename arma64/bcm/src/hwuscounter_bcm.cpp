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
// file:     hwuscounter_bcm.cpp
// brief:    BCM implementation of the us (microseconds) counter
// created:  2024-07-17
// authors:  nvitya

#include <hwuscounter_bcm.h>
#include <stdio.h>
#include <stdarg.h>
#include "bcm_utils.h"

#include "hwuscounter.h"

bool THwUsCounter_bcm::Init()
{
	regs = (systimer_regs_t *)map_hw_addr(HW_SYSTIMER_BASE, sizeof(systimer_regs_t), (void * *)&regs);
  initialized = true;
  return true;
}

uint64_t THwUsCounter_bcm::Get64()
{
  uint32_t uslow1;
  uint32_t ushigh1;
  uint32_t ushigh2;
  do
  {
    ushigh1 = regs->CHI;
    uslow1  = regs->CLO;
    ushigh2 = regs->CHI;
  }
  while (ushigh1 != ushigh2);

  return (uint64_t(ushigh1) << 32) | uslow1;
}
