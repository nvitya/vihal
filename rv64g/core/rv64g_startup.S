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
// file:     rv64g_startup.S
// brief:    VIHAL RV64G startup code, heavily tied to the linker script
// created:  2022-02-01
// authors:  nvitya

.section .resetentry  // goes to the very beginning

.option  norelax   // important, because the GP register is not set yet

.balign 4

//-------------------------------------------------
// Reset entry point
//-------------------------------------------------
.global _reset_entry
_reset_entry:
.global _cold_entry
_cold_entry:

.cfi_startproc
.cfi_undefined ra

  j 1f
  .word 0xdeadbeef
  .align 3
  .global g_wake_up
  g_wake_up:
      .dword 1
      .dword 0
1:

  li    x15, 0
  j init_start

//-------------------------------------------------
// Development entry point for self-flashing
//-------------------------------------------------
.global _soft_entry
_soft_entry:

  li    x15, 1

init_start:

  la    sp, __stack  // stack pointer (x2)

  fence.i  // clear the instruction cache

//-------------------------------------------------
// Setup CPU core CSRs (some of them DO NOT have a dedicated
// reset and need to be explicitly initialized)
//-------------------------------------------------

  csrc  mstatus, 0xC                     // disable machine interrupts
  csrw  mie, zero                        // no machine interrupt sources

  li    t0, 0x00006000U // = MSTATUS_FS
  csrs  mstatus, t0

  csrw  mideleg, 0
  csrw  medeleg, 0
  csrw  mip, 0
  //csrw  fcsr, zero                       // reset floating-point CSR

  csrw  mcycle,    zero                  // reset cycle counters
  csrw  minstret,  zero                  // reset instruction counters

  // set the global pointer
  // the GP is used for "relaxed" global data access, when the data is within -0x800 ... +0x800 range

  la    gp, __global_pointer$   // global pointer

  // multi-core not handled here, just stop the other cores
  csrr a0, mhartid
  beq a0, x0, cpu0_start
cpu1_loop:
  wfi
  j cpu1_loop

cpu0_start:

  la    t0, riscv_setup_irq     // setup interrupts, this is interrupt controlled dependent
  jalr  x1, t0                  // call the function

  j     startup_c_entry  // continue with C++ code


.cfi_endproc
.end
