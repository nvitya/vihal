/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     msp_utils.cpp
 *  brief:    MSP Utilities
 *  created:  2023-07-01
 *  authors:  nvitya
*/

#include "msp_utils.h"

uint32_t msp_bus_speed(uint8_t abusid)  // 0 = PD0 (MCLK/2), 1 = PD1 (MCLK)
{
  if ((0 == abusid) && (SYSCTL->SOCLOCK.MCLKCFG & SYSCTL_MCLKCFG_UDIV_MASK))
  {
    return (SystemCoreClock >> 1);
  }

  return SystemCoreClock;
}

