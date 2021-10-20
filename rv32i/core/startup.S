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
// file:     startup.S (rv32i)
// brief:    VIHAL RV32I startup code, heavily tied to the linker script
// created:  2021-10-03
// authors:  nvitya
// notes:
//   The NEORV32 (https://github.com/stnolting/neorv32) startup code was used as an example,
//   but at the end only a few lines was kept from there

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

  nop
  li    x15, 0
  j init_start

//-------------------------------------------------
// Development entry point for self-flashing
//-------------------------------------------------
.global _self_flashing_entry
_self_flashing_entry:

  nop
  li    x15, 1

init_start:

  la    sp, __stack  // stack pointer

  //fence.i  // clear the instruction cache

//-------------------------------------------------
// Setup CPU core CSRs (some of them DO NOT have a dedicated
// reset and need to be explicitly initialized)
//-------------------------------------------------
cpu_csr_init:

  la    x10,   dummy_trap_handler // configure early trap handler
  csrw  mtvec, x10
  csrw  mepc,  x10                       // just to init mepc

  csrw  mstatus, zero                    // disable global IRQ
  csrw  mie, zero                        // absolutely no interrupts sources, thanks

  li    x11, 7
  csrw  mcounteren, x11

  li    x11,   0                         // stop all counters except for [m]cycle[h] and [m]instret[h]
  csrw  0x320, x11                       // = mcountinhibit (literal address for lagacy toolchain compatibility)

  csrw  mcycle,    zero                  // reset cycle counters
  csrw  mcycleh,   zero
  csrw  minstret,  zero                  // reset instruction counters
  csrw  minstreth, zero

#if defined(__riscv_flen)
  csrw  fcsr, zero                       // reset floating-point CSR
#endif

//-------------------------------------------------
// Copy Regions (data, code ram etc.)
//-------------------------------------------------
copy_regions_begin:

  // all must be 4-byte aligned !
  la    x8, __data_regions_array_start
  la    x9, __data_regions_array_end

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
