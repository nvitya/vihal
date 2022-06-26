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
/* file:     startup.cpp (xtensa)
 * brief:    VIHAL xtensa startup code
 * created:  2022-06-25
 * authors:  nvitya
 * notes:
 *   Xtensta initialization steps
 *     - setup some mandatory CPU special registers (like intlevel)
 *     - setup some usable stack
 *     - setup the vector table
*/

/***************************************************************************
                           !! !WARNING !!!

 This MCU is very special and requires special initializations,
 which is not known to me yet, so at the time the
 ESP32 is not useable with  VIHAL!

 Window overflow exception handling required:

 https://sachin0x18.github.io/posts/demystifying-xtensa-isa/

****************************************************************************/


#include "platform.h"
#include "cppinit.h"
#include "xtensa/xt_instr_macros.h"

extern "C" __attribute__((noreturn)) void _start(unsigned self_flashing);

volatile uint32_t g_counter = 0;

extern "C" __attribute__((section(".startup"),used,noreturn))
void startup_c(unsigned self_flashing)
{
  volatile unsigned lcounter = 0;  // unhandled exception comes when this is here

  memory_region_setup();  // copy code to ram, initialize .data, zero .bss sections

  // _start(self_flashing);

  g_counter = 0;

  // here is a test only to check if the processor actually does something:
  while (true)
  {
    ++lcounter;
    ++g_counter;
  }
}
