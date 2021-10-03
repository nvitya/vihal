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
/*
 *  file:     cppinit.cpp
 *  brief:    Standard C++ Initialization, tightly coupled with the linker scripts
 *  version:  1.00
 *  date:     2021-09-28
 *  authors:  nvitya
*/

#include <stdint.h>
#include <sys/types.h>

// These magic symbols are provided by the linker.
extern void (*__preinit_array_start[]) (void) __attribute__((weak));
extern void (*__preinit_array_end[]) (void) __attribute__((weak));
extern void (*__init_array_start[]) (void) __attribute__((weak));
extern void (*__init_array_end[]) (void) __attribute__((weak));

// Iterate over all the preinit/init routines (mainly static constructors).
__attribute__((section(".startup"), used))
void __run_init_array (void)
{
  int count;
  int i;

  count = __preinit_array_end - __preinit_array_start;
  for (i = 0; i < count; i++)
	{
    __preinit_array_start[i] ();
	}

  // If you need to run the code in the .init section, please use
  // the startup files, since this requires the code in crti.o and crtn.o
  // to add the function prologue/epilogue.
  //_init(); // DO NOT ENABE THIS!

  count = __init_array_end - __init_array_start;
  for (i = 0; i < count; i++)
	{
    __init_array_start[i]();
	}
}

__attribute__((section(".startup"), used))
void cppinit(void)
{
	// section initialization based on the .init section tables

  // Call the standard library initialisation (mandatory for C++ to
  // execute the constructors for the static objects).
  __run_init_array();
}

// ----------------------------------------------------------------------------
