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
// file:     arma64_startup_c.cpp
// brief:    VIHAL 64-bit ARM-A startup code written in C
// created:  2024-04-26
// authors:  nvitya

#include "platform.h"
#include "cppinit.h"

#ifndef LINUX

extern "C" __attribute__((noreturn)) void _start(unsigned self_flashing);

extern "C" __attribute__((section(".startup"), naked, used, noreturn))
void startup_c_entry(unsigned self_flashing)
{
  // early entry, stack is working, IRQs disabled

  mcu_preinit_code();

  // setup memory regions
  memory_region_setup();

  _start(self_flashing);
}

#endif
