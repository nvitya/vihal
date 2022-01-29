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
// file:     startup.cpp (xtensa)
// brief:    VIHAL xtensa startup code
// created:  2022-01-29
// authors:  nvitya

#include "platform.h"
#include "cppinit.h"

extern "C" __attribute__((noreturn)) void _start(unsigned self_flashing);


extern "C" __attribute__((section(".startup"),used,noreturn))
void xtensa_startup(unsigned self_flashing)
{
  mcu_preinit_code();

  // the stack might not be set properly so set it
  //asm("ldr  r0, =__stack");
  //asm("mov  sp, r0");

  memory_region_setup();  // copy code to ram, initialize .data, zero .bss sections

  _start(self_flashing);

  while (true)
  {
    //
  }
}

extern "C" __attribute__((section(".resetentry"),used,noreturn))
void _reset_entry()
{
  xtensa_startup(0);

	while (true)
	{
		//
	}
}


extern "C" __attribute__((section(".startup"),used,noreturn))
void soft_entry()
{
  mcu_preinit_code();  // this must be here !

  xtensa_startup(1);

  while (true)
  {
    //
  }
}
