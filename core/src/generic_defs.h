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
 *  file:     generic_defs.h
 *  brief:    Generic definitions
 *  version:  1.00
 *  date:     2021-09-28
 *  authors:  nvitya
*/

#ifndef __GENERIC_DEFS_H
#define __GENERIC_DEFS_H

#define PROGMEM // some Arduino related stuff might require this

#define WEAK  __attribute__ ((weak))

#define ALWAYS_INLINE  inline __attribute__((always_inline))

#define SETREGPART(areg, ashift, amask, avalue) { areg &= ~(amask << ashift); areg |= (avalue << ashift); }

class TCbClass { };
typedef void (TCbClass::*PCbClassCallback)(void * arg);

extern unsigned SystemCoreClock;  // standard CMSIS variable, this is used almost everywhere

#endif
