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
// file:     arma64_interrupts.S
// brief:    VIHAL 64-bit ARM-A (AArch64) ASM interrupt entry points
// created:  2024-07-28
// authors:  nvitya
/*
Notes:
  Proper stack frame must be kept and created, otherwise the openocd/gdb
  stops working properly.

  Register save code is copied 16 times, because this code space would be
  wasted anyway (fixed gap for 32 instructions)
  This solution worked well with my debugger setup, while other identical
  versions sometimes can not recover the full stack trace.

  After register save the assembly code jump to the C exception handlers.

  Only the mandatory general purpose registers are saved:
    x0-x18, x29-x30

  The floating point registers, when they are used must be saved separately
  in the IRQ handlers.
*/
  .equ     STACKFRAME_SIZE, 192

  .extern  exception_handler_c
  .extern  irq_handler_c
  .extern  serror_handler_c

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
  stp     x29, x30, [sp, #-STACKFRAME_SIZE]!
  mov     x29, sp
  stp     x0,  x1,  [sp, #16]
  stp     x2,  x3,  [sp, #32]
  stp     x4,  x5,  [sp, #48]
  stp     x6,  x7,  [sp, #64]
  stp     x8,  x9,  [sp, #80]
  stp     x10, x11, [sp, #96]
  stp     x12, x13, [sp, #112]
  stp     x14, x15, [sp, #128]
  stp     x16, x17, [sp, #144]
  stp     x18, xzr, [sp, #160]
  mrs     x2, ESR_EL1
  mrs     x3, FAR_EL1
  stp     x2, x3, [sp, #176]
  mov     x0, sp
  mov     x1, 0
  bl exception_handler_c
  b  exception_irq_return

  .balign 128
irq_current_el_sp0:
  stp     x29, x30, [sp, #-STACKFRAME_SIZE]!
  mov     x29, sp
  stp     x0,  x1,  [sp, #16]
  stp     x2,  x3,  [sp, #32]
  stp     x4,  x5,  [sp, #48]
  stp     x6,  x7,  [sp, #64]
  stp     x8,  x9,  [sp, #80]
  stp     x10, x11, [sp, #96]
  stp     x12, x13, [sp, #112]
  stp     x14, x15, [sp, #128]
  stp     x16, x17, [sp, #144]
  stp     x18, xzr, [sp, #160]
  mrs     x2, ESR_EL1
  mrs     x3, FAR_EL1
  stp     x2, x3, [sp, #176]
  mov     x0, sp
  mov     x1, 1
  bl irq_handler_c
  b  exception_irq_return

  .balign 128
fiq_current_el_sp0:
  stp     x29, x30, [sp, #-STACKFRAME_SIZE]!
  mov     x29, sp
  stp     x0,  x1,  [sp, #16]
  stp     x2,  x3,  [sp, #32]
  stp     x4,  x5,  [sp, #48]
  stp     x6,  x7,  [sp, #64]
  stp     x8,  x9,  [sp, #80]
  stp     x10, x11, [sp, #96]
  stp     x12, x13, [sp, #112]
  stp     x14, x15, [sp, #128]
  stp     x16, x17, [sp, #144]
  stp     x18, xzr, [sp, #160]
  mrs     x2, ESR_EL1
  mrs     x3, FAR_EL1
  stp     x2, x3, [sp, #176]
  mov     x0, sp
  mov     x1, 2
  bl irq_handler_c
  b  exception_irq_return

  .balign 128
serror_current_el_sp0:
  stp     x29, x30, [sp, #-STACKFRAME_SIZE]!
  mov     x29, sp
  stp     x0,  x1,  [sp, #16]
  stp     x2,  x3,  [sp, #32]
  stp     x4,  x5,  [sp, #48]
  stp     x6,  x7,  [sp, #64]
  stp     x8,  x9,  [sp, #80]
  stp     x10, x11, [sp, #96]
  stp     x12, x13, [sp, #112]
  stp     x14, x15, [sp, #128]
  stp     x16, x17, [sp, #144]
  stp     x18, xzr, [sp, #160]
  mrs     x2, ESR_EL1
  mrs     x3, FAR_EL1
  stp     x2, x3, [sp, #176]
  mov     x0, sp
  mov     x1, 3
  bl serror_handler_c
  b  exception_irq_return

// ------------------------------------------------------------
// Current EL with SPx
// ------------------------------------------------------------

  .balign 128
sync_current_el_spx:
  stp     x29, x30, [sp, #-STACKFRAME_SIZE]!
  mov     x29, sp
  stp     x0,  x1,  [sp, #16]
  stp     x2,  x3,  [sp, #32]
  stp     x4,  x5,  [sp, #48]
  stp     x6,  x7,  [sp, #64]
  stp     x8,  x9,  [sp, #80]
  stp     x10, x11, [sp, #96]
  stp     x12, x13, [sp, #112]
  stp     x14, x15, [sp, #128]
  stp     x16, x17, [sp, #144]
  stp     x18, xzr, [sp, #160]
  mrs     x2, ESR_EL1
  mrs     x3, FAR_EL1
  stp     x2, x3, [sp, #176]
  mov     x0, sp
  mov     x1, 4
  bl exception_handler_c
  b  exception_irq_return

  .balign 128
irq_current_el_spx:
  stp     x29, x30, [sp, #-STACKFRAME_SIZE]!
  mov     x29, sp
  stp     x0,  x1,  [sp, #16]
  stp     x2,  x3,  [sp, #32]
  stp     x4,  x5,  [sp, #48]
  stp     x6,  x7,  [sp, #64]
  stp     x8,  x9,  [sp, #80]
  stp     x10, x11, [sp, #96]
  stp     x12, x13, [sp, #112]
  stp     x14, x15, [sp, #128]
  stp     x16, x17, [sp, #144]
  stp     x18, xzr, [sp, #160]
  mrs     x2, ESR_EL1
  mrs     x3, FAR_EL1
  stp     x2, x3, [sp, #176]
  mov     x0, sp
  mov     x1, 5
  bl irq_handler_c
  b  exception_irq_return

  .balign 128
fiq_current_el_spx:
  stp     x29, x30, [sp, #-STACKFRAME_SIZE]!
  mov     x29, sp
  stp     x0,  x1,  [sp, #16]
  stp     x2,  x3,  [sp, #32]
  stp     x4,  x5,  [sp, #48]
  stp     x6,  x7,  [sp, #64]
  stp     x8,  x9,  [sp, #80]
  stp     x10, x11, [sp, #96]
  stp     x12, x13, [sp, #112]
  stp     x14, x15, [sp, #128]
  stp     x16, x17, [sp, #144]
  stp     x18, xzr, [sp, #160]
  mrs     x2, ESR_EL1
  mrs     x3, FAR_EL1
  stp     x2, x3, [sp, #176]
  mov     x0, sp
  mov     x1, 6
  bl irq_handler_c
  b  exception_irq_return

  .balign 128
serror_current_el_spx:
  stp     x29, x30, [sp, #-STACKFRAME_SIZE]!
  mov     x29, sp
  stp     x0,  x1,  [sp, #16]
  stp     x2,  x3,  [sp, #32]
  stp     x4,  x5,  [sp, #48]
  stp     x6,  x7,  [sp, #64]
  stp     x8,  x9,  [sp, #80]
  stp     x10, x11, [sp, #96]
  stp     x12, x13, [sp, #112]
  stp     x14, x15, [sp, #128]
  stp     x16, x17, [sp, #144]
  stp     x18, xzr, [sp, #160]
  mrs     x2, ESR_EL1
  mrs     x3, FAR_EL1
  stp     x2, x3, [sp, #176]
  mov     x0, sp
  mov     x1, 7
  bl serror_handler_c
  b  exception_irq_return

// ------------------------------------------------------------
// Lower EL using AArch64
// ------------------------------------------------------------

  .balign 128
sync_lower_el_aarch64:
  stp     x29, x30, [sp, #-STACKFRAME_SIZE]!
  mov     x29, sp
  stp     x0,  x1,  [sp, #16]
  stp     x2,  x3,  [sp, #32]
  stp     x4,  x5,  [sp, #48]
  stp     x6,  x7,  [sp, #64]
  stp     x8,  x9,  [sp, #80]
  stp     x10, x11, [sp, #96]
  stp     x12, x13, [sp, #112]
  stp     x14, x15, [sp, #128]
  stp     x16, x17, [sp, #144]
  stp     x18, xzr, [sp, #160]
  mrs     x2, ESR_EL1
  mrs     x3, FAR_EL1
  stp     x2, x3, [sp, #176]
  mov     x0, sp
  mov     x1, 8
  bl exception_handler_c
  b  exception_irq_return

  .balign 128
irq_lower_el_aarch64:
  stp     x29, x30, [sp, #-STACKFRAME_SIZE]!
  mov     x29, sp
  stp     x0,  x1,  [sp, #16]
  stp     x2,  x3,  [sp, #32]
  stp     x4,  x5,  [sp, #48]
  stp     x6,  x7,  [sp, #64]
  stp     x8,  x9,  [sp, #80]
  stp     x10, x11, [sp, #96]
  stp     x12, x13, [sp, #112]
  stp     x14, x15, [sp, #128]
  stp     x16, x17, [sp, #144]
  stp     x18, xzr, [sp, #160]
  mrs     x2, ESR_EL1
  mrs     x3, FAR_EL1
  stp     x2, x3, [sp, #176]
  mov     x0, sp
  mov     x1, 9
  bl irq_handler_c
  b  exception_irq_return

  .balign 128
fiq_lower_el_aarch64:
  stp     x29, x30, [sp, #-STACKFRAME_SIZE]!
  mov     x29, sp
  stp     x0,  x1,  [sp, #16]
  stp     x2,  x3,  [sp, #32]
  stp     x4,  x5,  [sp, #48]
  stp     x6,  x7,  [sp, #64]
  stp     x8,  x9,  [sp, #80]
  stp     x10, x11, [sp, #96]
  stp     x12, x13, [sp, #112]
  stp     x14, x15, [sp, #128]
  stp     x16, x17, [sp, #144]
  stp     x18, xzr, [sp, #160]
  mrs     x2, ESR_EL1
  mrs     x3, FAR_EL1
  stp     x2, x3, [sp, #176]
  mov     x0, sp
  mov     x1, 10
  bl irq_handler_c
  b  exception_irq_return

  .balign 128
serror_lower_el_aarch64:
  stp     x29, x30, [sp, #-STACKFRAME_SIZE]!
  mov     x29, sp
  stp     x0,  x1,  [sp, #16]
  stp     x2,  x3,  [sp, #32]
  stp     x4,  x5,  [sp, #48]
  stp     x6,  x7,  [sp, #64]
  stp     x8,  x9,  [sp, #80]
  stp     x10, x11, [sp, #96]
  stp     x12, x13, [sp, #112]
  stp     x14, x15, [sp, #128]
  stp     x16, x17, [sp, #144]
  stp     x18, xzr, [sp, #160]
  mrs     x2, ESR_EL1
  mrs     x3, FAR_EL1
  stp     x2, x3, [sp, #176]
  mov     x0, sp
  mov     x1, 11
  bl serror_handler_c
  b  exception_irq_return

// ------------------------------------------------------------
// Lower EL using AArch32
// ------------------------------------------------------------

  .balign 128
sync_lower_el_aarch32:
  stp     x29, x30, [sp, #-STACKFRAME_SIZE]!
  mov     x29, sp
  stp     x0,  x1,  [sp, #16]
  stp     x2,  x3,  [sp, #32]
  stp     x4,  x5,  [sp, #48]
  stp     x6,  x7,  [sp, #64]
  stp     x8,  x9,  [sp, #80]
  stp     x10, x11, [sp, #96]
  stp     x12, x13, [sp, #112]
  stp     x14, x15, [sp, #128]
  stp     x16, x17, [sp, #144]
  stp     x18, xzr, [sp, #160]
  mrs     x2, ESR_EL1
  mrs     x3, FAR_EL1
  stp     x2, x3, [sp, #176]
  mov     x0, sp
  mov     x1, 12
  bl exception_handler_c
  b  exception_irq_return

  .balign 128
irq_lower_el_aarch32:
  stp     x29, x30, [sp, #-STACKFRAME_SIZE]!
  mov     x29, sp
  stp     x0,  x1,  [sp, #16]
  stp     x2,  x3,  [sp, #32]
  stp     x4,  x5,  [sp, #48]
  stp     x6,  x7,  [sp, #64]
  stp     x8,  x9,  [sp, #80]
  stp     x10, x11, [sp, #96]
  stp     x12, x13, [sp, #112]
  stp     x14, x15, [sp, #128]
  stp     x16, x17, [sp, #144]
  stp     x18, xzr, [sp, #160]
  mrs     x2, ESR_EL1
  mrs     x3, FAR_EL1
  stp     x2, x3, [sp, #176]
  mov     x0, sp
  mov     x1, 13
  bl irq_handler_c
  b  exception_irq_return

  .balign 128
fiq_lower_el_aarch32:
  stp     x29, x30, [sp, #-STACKFRAME_SIZE]!
  mov     x29, sp
  stp     x0,  x1,  [sp, #16]
  stp     x2,  x3,  [sp, #32]
  stp     x4,  x5,  [sp, #48]
  stp     x6,  x7,  [sp, #64]
  stp     x8,  x9,  [sp, #80]
  stp     x10, x11, [sp, #96]
  stp     x12, x13, [sp, #112]
  stp     x14, x15, [sp, #128]
  stp     x16, x17, [sp, #144]
  stp     x18, xzr, [sp, #160]
  mrs     x2, ESR_EL1
  mrs     x3, FAR_EL1
  stp     x2, x3, [sp, #176]
  mov     x0, sp
  mov     x1, 14
  bl irq_handler_c
  b  exception_irq_return

  .balign 128
serror_lower_el_aarch32:
  stp     x29, x30, [sp, #-STACKFRAME_SIZE]!
  mov     x29, sp
  stp     x0,  x1,  [sp, #16]
  stp     x2,  x3,  [sp, #32]
  stp     x4,  x5,  [sp, #48]
  stp     x6,  x7,  [sp, #64]
  stp     x8,  x9,  [sp, #80]
  stp     x10, x11, [sp, #96]
  stp     x12, x13, [sp, #112]
  stp     x14, x15, [sp, #128]
  stp     x16, x17, [sp, #144]
  stp     x18, xzr, [sp, #160]
  mrs     x2, ESR_EL1
  mrs     x3, FAR_EL1
  stp     x2, x3, [sp, #176]
  mov     x0, sp
  mov     x1, 15
  bl serror_handler_c
  b  exception_irq_return


// ------------------------------------------------------------
// Common IRQ / Exception Return code
// ------------------------------------------------------------

  .balign 128
exception_irq_return:
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
  add   sp, sp, #STACKFRAME_SIZE
  eret

  .end
