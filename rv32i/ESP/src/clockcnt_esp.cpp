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
 *  file:     clockcnt_esp.cpp
 *  brief:    ESP Clock Counter
 *  version:  1.00
 *  date:     2022-01-29
 *  authors:  nvitya
*/

#include "clockcnt.h"

void clockcnt_init()
{
  cpu_csr_write(0x7E0, 1);  // = mpcer[0] = count clock cycles
  cpu_csr_write(0x7E1, 1);  // = mpcmer[0] = enable counting

  //asm("li    t0, 1");
  //asm("csrw  0x7E0, t0");
  //asm("li    t0, 1");
  //asm("csrw  0x7E1, t0");
}
