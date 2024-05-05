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

extern uint32_t __data_regions_array_start;
extern uint32_t __data_regions_array_end;

extern uint32_t __bss_regions_array_start;
extern uint32_t __bss_regions_array_end;

// ----------------------------------------------------------------------------

inline void __attribute__((always_inline)) __initialize_data(uint32_t * from, uint32_t * region_begin, uint32_t * region_end)
{
  // Iterate and copy word by word.
  // It is assumed that the pointers are word aligned.
  uint32_t * p = region_begin;
  while (p < region_end)
  {
    *p++ = *from++;
  }
}

inline void __attribute__((always_inline)) __initialize_bss(uint32_t * region_begin, uint32_t * region_end)
{
  // Iterate and clear word by word.
  // It is assumed that the pointers are word aligned.
  uint32_t * p = region_begin;
  while (p < region_end)
  {
    *p++ = 0;
  }
}

__attribute__((section(".startup")))
void memory_region_setup(void)
{
  uint32_t * recp;
  uint32_t * loadaddr;
  uint32_t * destaddrbegin;
  uint32_t * destaddrend;

  // section initialization based on the .init section tables

  // 1. Copy preinitialized data sections

  recp =  (uint32_t *)&__data_regions_array_start;
  while (recp < &__data_regions_array_end)
  {
    loadaddr = (uint32_t *)(intptr_t(*recp));
    ++recp;
    destaddrbegin = (uint32_t *)(intptr_t(*recp));
    ++recp;
    destaddrend = (uint32_t *)(intptr_t(*recp));
    ++recp;
    __initialize_data(loadaddr, destaddrbegin, destaddrend);
  }

  // 2. Zero BSS data sections
  recp =  (uint32_t *)&__bss_regions_array_start;
  while (recp < &__bss_regions_array_end)
  {
    destaddrbegin = (uint32_t *)(intptr_t(*recp));
    ++recp;
    destaddrend = (uint32_t *)(intptr_t(*recp));
    ++recp;
    __initialize_bss(destaddrbegin, destaddrend);
  }
}


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
