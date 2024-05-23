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
 *  file:     hwclk_sg.cpp
 *  brief:    SG2000 clock speed implementation
 *  date:     2024-04-01
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwclk.h"

bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
  // This is set fixed by the Linux

#if defined(MCU_CV1800)

  SystemCoreClock = 594000000;

#else

  SystemCoreClock = 786431995;  // measured using the 25 MHz reference clock
  //SystemCoreClock = 786000000;
  //SystemCoreClock = 750000000;

#endif

  // finally set the global variable for the other system parts
  //SystemCoreClock = target_speed_hz; // defined as .noinit, so it won't be cleared at section setup

  return true;
}


