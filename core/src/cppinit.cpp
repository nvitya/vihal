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

#include "platform.h"
#include <stdint.h>
#include <sys/types.h>

#ifndef LINUX

extern uintptr_t __data_regions_array_start;
extern uintptr_t __data_regions_array_end;

extern uintptr_t __bss_regions_array_start;
extern uintptr_t __bss_regions_array_end;

// ----------------------------------------------------------------------------

inline void __attribute__((always_inline)) __initialize_data(uintptr_t * from, uintptr_t * region_begin, uintptr_t * region_end)
{
  // Iterate and copy word by word.
  // It is assumed that the pointers are word aligned.
  uintptr_t * p = region_begin;
  while (p < region_end)
  {
    *p++ = *from++;
  }
}

inline void __attribute__((always_inline)) __initialize_bss(uintptr_t * region_begin, uintptr_t * region_end)
{
  // Iterate and clear word by word.
  // It is assumed that the pointers are word aligned.
  uintptr_t * p = region_begin;
  while (p < region_end)
  {
    *p++ = 0;
  }
}

__attribute__((section(".startup")))
void memory_region_setup(void)
{
  uintptr_t * recp;
  uintptr_t * loadaddr;
  uintptr_t * destaddrbegin;
  uintptr_t * destaddrend;

  // section initialization based on the .init section tables

  // 1. Copy preinitialized data sections

  recp =  (uintptr_t *)&__data_regions_array_start;
  while (recp < &__data_regions_array_end)
  {
    loadaddr = (uintptr_t *)(*recp);
    ++recp;
    destaddrbegin = (uintptr_t *)(*recp);
    ++recp;
    destaddrend = (uintptr_t *)(*recp);
    ++recp;
    __initialize_data(loadaddr, destaddrbegin, destaddrend);
  }

  // 2. Zero BSS data sections
  recp =  (uintptr_t *)&__bss_regions_array_start;
  while (recp < &__bss_regions_array_end)
  {
    destaddrbegin = (uintptr_t *)(*recp);
    ++recp;
    destaddrend = (uintptr_t *)(*recp);
    ++recp;
    __initialize_bss(destaddrbegin, destaddrend);
  }
}

// linker symbols, their value is invalid, only their addresses can be used !

extern uintptr_t __preinit_array_start;
extern uintptr_t __preinit_array_end;
extern uintptr_t __init_array_start;
extern uintptr_t __init_array_end;

typedef void (init_array_func_t)(void);

__attribute__((section(".startup"), used))
void __run_init_func_array(uintptr_t * pstart, uintptr_t * pend)
{
  while (pstart < pend)
  {
    init_array_func_t * pfunc = (init_array_func_t *)(*pstart);
    (*pfunc)();
    ++pstart;
  }
}

__attribute__((section(".startup"), used))
void cppinit(void)
{
  // section initialization based on the .init section tables

  // Call the standard library initialisation (mandatory for C++ to
  // execute the constructors for the static objects).
  __run_init_func_array(&__preinit_array_start, &__preinit_array_end);
  __run_init_func_array(&__init_array_start,    &__init_array_end);
}

#endif

// ----------------------------------------------------------------------------
