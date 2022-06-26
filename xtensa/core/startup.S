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
// file:     startup.S (xtensa)
// brief:    VIHAL xtensa (ESP32) startup code, heavily tied to the linker script
// created:  2021-10-03
// authors:  nvitya

/***************************************************************************
                           !! !WARNING !!!

 This MCU is very special and requires special initializations,
 which is not known to me yet, so at the time the
 ESP32 is not useable with  VIHAL!

 Window overflow exception handling required:

 https://sachin0x18.github.io/posts/demystifying-xtensa-isa/

****************************************************************************/

#include "xtensa_vihal_asm.h"

  .section .startup

  // register usage: a0 = return address, a1 = stack pointer, a2..a7 = function arguments

  .align      4
  .literal_position
  .align      4

//-------------------------------------------------
// Reset entry point
//-------------------------------------------------

  .global _cold_entry
  .type   _cold_entry,@function
_cold_entry:
  movi   a2, 0  // keep the contents of a2 here !
  j xtensa_cpu_init

//-------------------------------------------------
// Development entry point for self-flashing
//-------------------------------------------------
  .global _soft_entry
  .type   _soft_entry,@function
_soft_entry:
  movi   a2, 1  // keep the contents of a2 here !
  j xtensa_cpu_init

xtensa_cpu_init:

  // preserve the contents of the a2 !

  // initialize the PS with INTLEVEL=3, without WOE!
  movi             a0, (PS_INTLEVEL(3) | PS_UM)  //  = PS_INTLEVEL(3) | PS_UM | PS_WOE
  wsr              a0, PS
  rsync

  // reset the window base
  rsr.windowbase   a0
  ssl              a0
  movi             a0, 1
  sll              a0, a0
  wsr.windowstart  a0
  rsync

  // setup the stack
  movi             a0, __stack   // will be split up to two instructions and using the literal pool
  mov              sp, a0        // sp = a1 !

  // enable the window overflow interrupt
  rsr.ps           a0
  movi             a3, (PS_WOE)
  or               a0, a0, a3
  wsr.ps           a0
  rsync

  // setup the vector table
  movi             a0, _vector_table
  wsr              a0, vecbase

  // jump to startup_c in C
  movi             a0, startup_c
  jx               a0



