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

.section .startup

//.option  norelax   // ???

.balign 4

//-------------------------------------------------
// Reset entry point
//-------------------------------------------------
.global asoft_entry
asoft_entry:

  movi    a0, (3 | 0x00040000 | 0x00000020)  //  = PS_INTLEVEL(3) | PS_UM | PS_WOE
  wsr     a0, PS

  nop

endlessloop:

  nop

  j endlessloop

  .literal_position


//.end
