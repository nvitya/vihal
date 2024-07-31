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
// file:     arma64_startup.S
// brief:    VIHAL 64-bit ARM-A (AArch64) startup code, heavily tied to the linker script
// created:  2024-07-28
// authors:  nvitya

/*

  The Non-Initialized core starts in EL1 on the Raspberry Pi 4.
  So initializations in EL2 or EL3 is not possible.

*/

  .extern exception_handler_c
  .extern irq_handler_c
  .extern serror_handler_c

  .global exception_entry_asm

exception_entry_asmx:

  stp     x29, x30, [sp, #-192]!

  //sub     sp,  sp, #48
  //stp     x30, x29, [sp]

  mov     x29, sp
  bl exception_handler_c

  add     sp, sp, #192
  ldp     x29, x30, [sp, #0]
  //ldp     x29, x30, [sp], #48
  ret


exception_entry_asm:
  stp     x29, x30, [sp, #-192]!

  //sub   sp,  sp, #192
  //stp   x29, x30, [sp, #0]

  stp   x0,  x1,  [sp, #16]
  stp   x2,  x3,  [sp, #32]
  stp   x4,  x5,  [sp, #48]
  stp   x6,  x7,  [sp, #64]
  stp   x8,  x9,  [sp, #80]
  stp   x10, x11, [sp, #96]
  stp   x12, x13, [sp, #112]
  stp   x14, x15, [sp, #128]
  stp   x16, x17, [sp, #144]
  stp   x18, xzr, [sp, #160]

  mrs   x0, ESR_EL1
  mrs   x1, FAR_EL1
  stp   x0, x1, [sp, #176]

  mov   x29, sp  // set the frame pointer
  mov   x0, x29

  //ldr   x1, =exception_handler_c
  //blr   x1
  bl exception_handler_c
//1:
//  b 1b

  ldp   x29, x30, [sp, #0]
  ldp   x0,  x1,  [sp, #16]
  ldp   x2,  x3,  [sp, #32]
  ldp   x4,  x5,  [sp, #48]
  ldp   x6,  x7,  [sp, #64]
  ldp   x8,  x9,  [sp, #80]
  ldp   x10, x11, [sp, #96]
  ldp   x12, x13, [sp, #112]
  ldp   x14, x15, [sp, #128]
  ldp   x16, x17, [sp, #144]
  ldp   x18, xzr, [sp, #160]
  add   sp, sp, #192
  eret

  .global irq_entry_asm
irq_entry_asm:
  sub   sp,  sp, #192
  stp   x0,  x1,  [sp, #0]
  stp   x2,  x3,  [sp, #16]
  stp   x4,  x5,  [sp, #32]
  stp   x6,  x7,  [sp, #48]
  stp   x8,  x9,  [sp, #64]
  stp   x10, x11, [sp, #80]
  stp   x12, x13, [sp, #96]
  stp   x14, x15, [sp, #112]
  stp   x16, x17, [sp, #128]
  stp   x18, x29, [sp, #144]
  stp   x30, xzr, [sp, #160]

  mrs   x0, ESR_EL1
  mrs   x1, FAR_EL1
  stp   x0, x1, [sp, #176]

  mov   x0, sp
  ldr   x1, =irq_handler_c
  blr   x1
  //bl irq_handler_c

  ldp   x0,  x1,  [sp, #0]
  ldp   x2,  x3,  [sp, #16]
  ldp   x4,  x5,  [sp, #32]
  ldp   x6,  x7,  [sp, #48]
  ldp   x8,  x9,  [sp, #64]
  ldp   x10, x11, [sp, #80]
  ldp   x12, x13, [sp, #96]
  ldp   x14, x15, [sp, #112]
  ldp   x16, x17, [sp, #128]
  ldp   x18, x29, [sp, #144]
  ldp   x30, xzr, [sp, #160]
  add   sp, sp, #192
  eret

  .global serror_entry_asm
serror_entry_asm:
  sub   sp,  sp, #192
  stp   x0,  x1,  [sp, #0]
  stp   x2,  x3,  [sp, #16]
  stp   x4,  x5,  [sp, #32]
  stp   x6,  x7,  [sp, #48]
  stp   x8,  x9,  [sp, #64]
  stp   x10, x11, [sp, #80]
  stp   x12, x13, [sp, #96]
  stp   x14, x15, [sp, #112]
  stp   x16, x17, [sp, #128]
  stp   x18, x29, [sp, #144]
  stp   x30, xzr, [sp, #160]

  mrs   x0, ESR_EL1
  mrs   x1, FAR_EL1
  stp   x0, x1, [sp, #176]

  mov   x0, sp
  ldr   x1, =serror_handler_c
  blr   x1
  //bl serror_handler_c

  ldp   x0,  x1,  [sp, #0]
  ldp   x2,  x3,  [sp, #16]
  ldp   x4,  x5,  [sp, #32]
  ldp   x6,  x7,  [sp, #48]
  ldp   x8,  x9,  [sp, #64]
  ldp   x10, x11, [sp, #80]
  ldp   x12, x13, [sp, #96]
  ldp   x14, x15, [sp, #112]
  ldp   x16, x17, [sp, #128]
  ldp   x18, x29, [sp, #144]
  ldp   x30, xzr, [sp, #160]
  add   sp, sp, #192
  eret


// ------------------------------------------------------------
// Vector Table
// ------------------------------------------------------------

  .align 12
  .global vector_table
vector_table:

// ------------------------------------------------------------
// Current EL with SP0
// ------------------------------------------------------------
  .balign 128
sync_current_el_sp0:
  b exception_entry_asm

  .balign 128
irq_current_el_sp0:
  b irq_entry_asm

  .balign 128
fiq_current_el_sp0:
  b irq_entry_asm

  .balign 128
serror_current_el_sp0:
  b serror_entry_asm

// ------------------------------------------------------------
// Current EL with SPx
// ------------------------------------------------------------

  .balign 128
sync_current_el_spx:
  b exception_entry_asm
  //bl exception_handler_c

  .balign 128
irq_current_el_spx:
  b irq_entry_asm

  .balign 128
fiq_current_el_spx:
  b irq_entry_asm

  .balign 128
serror_current_el_spx:
  b serror_entry_asm

// ------------------------------------------------------------
// Lower EL using AArch64
// ------------------------------------------------------------

  .balign 128
sync_lower_el_aarch64:
  b exception_entry_asm

  .balign 128
irq_lower_el_aarch64:
  b irq_entry_asm

  .balign 128
fiq_lower_el_aarch64:
  b irq_entry_asm

  .balign 128
serror_lower_el_aarch64:
  b serror_entry_asm

// ------------------------------------------------------------
// Lower EL using AArch32
// ------------------------------------------------------------

  .balign 128
sync_lower_el_aarch32:
  b exception_entry_asm

  .balign 128
irq_lower_el_aarch32:
  b irq_entry_asm

  .balign 128
fiq_lower_el_aarch32:
  b irq_entry_asm

  .balign 128
serror_lower_el_aarch32:
  b serror_entry_asm

  .end
