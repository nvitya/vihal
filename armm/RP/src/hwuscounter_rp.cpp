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
// file:     hwuscounter_rp.cpp
// brief:    RP2040 implementation of the us (microseconds) counter
// created:  2024-04-07
// authors:  nvitya

#include <stdio.h>
#include <stdarg.h>

#include "hwuscounter.h"
#include "hwuscounter_rp.h"

bool THwUsCounter_rp::Init()
{
#if defined(timer0_hw)
  regs = timer0_hw;
#else
  regs = timer_hw;
#endif

  regs->dbgpause = 0;  // do not stop for multi-core debugging
  initialized = true;
  return true;
}
