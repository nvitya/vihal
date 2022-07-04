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
// file:     startup.S (rv64g)
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



  //fence.i  // clear the instruction cache

//-------------------------------------------------
// Setup CPU core CSRs (some of them DO NOT have a dedicated
// reset and need to be explicitly initialized)
//-------------------------------------------------
cpu_csr_init:

  csrw mideleg, 0
  csrw medeleg, 0
  csrw mie, 0
  csrw mip, 0

  la    x10,   dummy_trap_handler // configure early trap handler
  csrw  mtvec, x10
  csrw  mepc,  x10                       // just to init mepc

  li t0, 0x00006000U // = MSTATUS_FS
  csrs mstatus, t0

  csrw  mie, zero                        // absolutely no interrupts sources, thanks

  //li    x11, 7
  //csrw  mcounteren, x11

  //li    x11,   0                         // stop all counters except for [m]cycle[h] and [m]instret[h]
  //csrw  0x320, x11                       // = mcountinhibit (literal address for lagacy toolchain compatibility)

  csrw  mcycle,    zero                  // reset cycle counters
  csrw  minstret,  zero                  // reset instruction counters

  //csrw  fcsr, zero                       // reset floating-point CSR

  csrr a0, mhartid
  beq a0, x0, cpu0_start

cpu1_loop:
  wfi
  j cpu1_loop

cpu0_start:


//-------------------------------------------------
// Copy Regions (data, code ram etc.)
//-------------------------------------------------
copy_regions_begin:

  // all must be 4-byte aligned !
  la    x8, __data_regions_array_start
  la    x9, __data_regions_array_end

  // warning, only  32-bit addresses here !

copy_regions_loop:
  bge   x8, x9, copy_regions_finished
  lw    x11, 0(x8)        // start of data area (copy source)
  lw    x12, 4(x8)        // start of data area (copy destination)
  lw    x13, 8(x8)        // last address of destination data area
  addi  x8, x8, 12

copy_data_loop:
  bge   x12, x13, copy_data_loop_end
  lw    x14, 0(x11)
  sw    x14, 0(x12)
  addi  x11, x11, 4
  addi  x12, x12, 4
  j copy_data_loop

copy_data_loop_end:
  j copy_regions_loop

copy_regions_finished:

//-------------------------------------------------
// Zero Regions (bss)
//-------------------------------------------------
zero_regions_begin:
  // all must be 4-byte aligned !
  la    x8, __bss_regions_array_start
  la    x9, __bss_regions_array_end

zero_regions_loop:
  bge   x8, x9, zero_regions_finished
  lw    x11, 0(x8)        // start of destination area
  lw    x12, 4(x8)        // last address of destination data area
  addi  x8, x8, 8

zero_data_loop:
  bge   x11, x12, zero_data_loop_end
  sw    zero, 0(x11)
  addi  x11,  x11, 4
  j zero_data_loop

zero_data_loop_end:
  j zero_regions_loop

zero_regions_finished:

  // set the global pointer
  // the GP is used for "relaxed" global data access, when the data is within -0x800 ... +0x800 range

  la    gp, __global_pointer$   // global pointer

exec_start_c:

  // initialize the registers:

//addi   x0, x0, 0  // zero: hardwired to zero
//addi   x1, x0, 0  // ra: return address
//addi   x2, x0, 0  // sp: stack pointer
//addi   x3, x0, 0  // gp: gloabl pointer
  addi   x4, x0, 0  // tp: thread pointer
  addi   x5, x0, 0  // t0: temporary
  addi   x6, x0, 0  // t1: temporary
  addi   x7, x0, 0  // t2: temporary
  addi   x8, x0, 0  // s0/fp saved register / frame pointer
  addi   x9, x0, 0  // s1 saved register
  // restore self flashing flag:
  addi  x10, x15, 0 // a0: function argument / return value
  addi  x11, x0, 0  // a1: function argument / return value
  addi  x12, x0, 0  // a2: function argument
  addi  x13, x0, 0  // a3: function argument
  addi  x14, x0, 0  // a4: function argument
  addi  x15, x0, 0  // a5: function argument

  fssr    x0
  fmv.w.x f0,  x0
  fmv.w.x f1,  x0
  fmv.w.x f2,  x0
  fmv.w.x f3,  x0
  fmv.w.x f4,  x0
  fmv.w.x f5,  x0
  fmv.w.x f6,  x0
  fmv.w.x f7,  x0
  fmv.w.x f8,  x0
  fmv.w.x f9,  x0
  fmv.w.x f10, x0
  fmv.w.x f11, x0
  fmv.w.x f12, x0
  fmv.w.x f13, x0
  fmv.w.x f14, x0
  fmv.w.x f15, x0
  fmv.w.x f16, x0
  fmv.w.x f17, x0
  fmv.w.x f18, x0
  fmv.w.x f19, x0
  fmv.w.x f20, x0
  fmv.w.x f21, x0
  fmv.w.x f22, x0
  fmv.w.x f23, x0
  fmv.w.x f24, x0
  fmv.w.x f25, x0
  fmv.w.x f26, x0
  fmv.w.x f27, x0
  fmv.w.x f28, x0
  fmv.w.x f29, x0
  fmv.w.x f30, x0
  fmv.w.x f31, x0

  jal   ra, _start

unexpected_return:
  nop
  j unexpected_return


//------------------------------------------------
// dummy trap handler
//------------------------------------------------

  .balign 4

dummy_trap_handler:
  nop
  j dummy_trap_handler


.cfi_endproc
.end
