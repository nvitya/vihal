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

.section .resetentry  // goes to the very beginning

.balign 4


.extern __stack

//-------------------------------------------------
// Reset entry point
//-------------------------------------------------
.global _reset_entry
_reset_entry:
.global _cold_entry
_cold_entry:

  mov    x15, 0
  b init_start

//-------------------------------------------------
// Development entry point
//-------------------------------------------------
.global _soft_entry
_soft_entry:

  mov    x15, 1

init_start:
  msr    SCTLR_EL1, xzr  // ARM v8 example: "Set SCTLRs for EL1 to safe values"
                         // Disables MMU + caches, but also disables some instructions

  mov    x0, #0
  //orr    x0, #(1 << 29)  // E0POE: enable access to POR_EL0
  orr    x0, x0, #(3 << 24)  // SME instructions enable
  orr    x0, x0, #(3 << 20)  // FPU enable
  orr    x0, x0, #(3 << 16)  // ZEN enable
  msr    CPACR_EL1, x0


  ldr    x0, =vector_table
  msr    VBAR_EL1, x0    // Install vector table

  ldr    x0, =__stack     // initialize the stack pointer
  mov    sp, x0

cpu0_start:

  //la    t0, riscv_setup_irq     // setup interrupts, this is interrupt controlled dependent
  //jalr  x1, t0                  // call the function

  b     startup_c_entry  // continue with C++ code


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
  b        .                    //        Synchronous

  .balign 128
irq_current_el_sp0:
  b        .                    //        IRQ

  .balign 128
fiq_current_el_sp0:
  b        .                    //        FIQ

  .balign 128
serror_current_el_sp0:
  b        .                    //        SError

// ------------------------------------------------------------
// Current EL with SPx
// ------------------------------------------------------------

  .balign 128
sync_current_el_spx:
  b        .                    //        Synchronous

  .balign 128
irq_current_el_spx:
  b        .                    //        IRQ

  .balign 128
fiq_current_el_spx:
  b        .                    //        FIQ

  .balign 128
serror_current_el_spx:
  b        .                    //        SError

// ------------------------------------------------------------
// Lower EL using AArch64
// ------------------------------------------------------------

  .balign 128
sync_lower_el_aarch64:
   b        .

  .balign 128
irq_lower_el_aarch64:
  b        .                    //        IRQ

  .balign 128
fiq_lower_el_aarch64:
  b        .                    //        FIQ

  .balign 128
serror_lower_el_aarch64:
  b        .                    //        SError

// ------------------------------------------------------------
// Lower EL using AArch32
// ------------------------------------------------------------

  .balign 128
sync_lower_el_aarch32:
   b        .

  .balign 128
irq_lower_el_aarch32:
  b        .                    //        IRQ

  .balign 128
fiq_lower_el_aarch32:
  b        .                    //        FIQ

  .balign 128
serror_lower_el_aarch32:
  b        .                    //        SError

  .end
