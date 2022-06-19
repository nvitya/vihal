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
// file:     rv32i_startup.S
// brief:    VIHAL RV32I startup code, heavily tied to the linker script
// created:  2021-10-03
// authors:  nvitya

.section .resetentry  // goes to the very beginning

.option  norelax   // important, because the GP register is not set yet

.balign 4

//-------------------------------------------------
// Reset entry point
//-------------------------------------------------
.global _cold_entry
_cold_entry:

.cfi_startproc
.cfi_undefined ra

  nop
  li    a0, 0
  j init_start

//-------------------------------------------------
// Development entry point for self-flashing
//-------------------------------------------------
.global _soft_entry
_soft_entry:

  nop
  li    a0, 1

init_start:

  li     t0, (3 << 11)    // set mpp=11 => machine-mode, clear all remaining bits,
  csrw   mstatus, t0      // also disables IRQs globally (MIE)

  // The GD32V MCU starts at address 0x00000000 while the code was targeted to 0x08000000.
  // This leads false immediates for instructions that use the PC partially (like luipc)

  // This jump ensures that the execution continues there where the original code was targeted:
  lui    t0, %hi(_start0800)
  li     t1, %lo(_start0800)
  or     t0, t0, t1
  jr     t0

_start0800:

  la    sp, __stack  // stack pointer

  // set the global pointer
  // the GP is used for "relaxed" global data access, when the data is within -0x800 ... +0x800 range

  la    gp, __global_pointer$   // global pointer, must be set under "norelax" option

  la    t0, riscv_setup_irq     // setup interrupts, this is interrupt controlled dependent
  jalr  x1, t0                  // call the function

  j     startup_c_entry  // continue with C++ code

.cfi_endproc
.end
