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
 *  file:     vectors_gd32v.S
 *  brief:    Generic vector table for all Cortex-M MCUs
 *  created:  2022-06-15
 *  authors:  nvitya
 *  description:
 *    Interrupt vector definitions for the ECLIC Interrupt Controller
*/

#define MSTATUS_MIE         0x00000008
#define CSR_MSTATUS         0x300
#define CSR_MEPC            0x341
#define CSR_MCAUSE          0x342
#define CSR_MMISC_CTL       0x7d0
#define CSR_MSUBM           0x7c4

#define CSR_MTVT            0x307
#define CSR_MTVT2           0x7EC
#define CSR_JALMNXTI        0x7ED
#define CSR_PUSHMCAUSE      0x7EE
#define CSR_PUSHMEPC        0x7EF
#define CSR_PUSHMSUBM       0x7EB

  .section .startup

//-----------------------------------------------------------------------------------
// INTERRUPT HANDLERS
//-----------------------------------------------------------------------------------

  .macro SAVE_CONTEXT

    addi   sp, sp, -20 * 4  // allocate stack space

    sw     x1,  0*4(sp)
    sw     x4,  1*4(sp)
    sw     x5,  2*4(sp)
    sw     x6,  3*4(sp)
    sw     x7,  4*4(sp)
    sw    x10,  5*4(sp)
    sw    x11,  6*4(sp)
    sw    x12,  7*4(sp)
    sw    x13,  8*4(sp)
    sw    x14,  9*4(sp)
    sw    x15, 10*4(sp)

    sw    x16, 11*4(sp)
    sw    x17, 12*4(sp)
    sw    x28, 13*4(sp)
    sw    x29, 14*4(sp)
    sw    x30, 15*4(sp)
    sw    x31, 16*4(sp)

    // locations 17,18,18 are used differently
  .endm

  .macro RESTORE_CONTEXT
    lw     x1,  0*4(sp)
    lw     x4,  1*4(sp)
    lw     x5,  2*4(sp)
    lw     x6,  3*4(sp)
    lw     x7,  4*4(sp)
    lw    x10,  5*4(sp)
    lw    x11,  6*4(sp)
    lw    x12,  7*4(sp)
    lw    x13,  8*4(sp)
    lw    x14,  9*4(sp)
    lw    x15, 10*4(sp)

    lw    x16, 11*4(sp)
    lw    x17, 12*4(sp)
    lw    x28, 13*4(sp)
    lw    x29, 14*4(sp)
    lw    x30, 15*4(sp)
    lw    x31, 16*4(sp)

    addi   sp, sp, 20 * 4  // De-allocate the stack space
  .endm

  .macro SAVE_EPC_STATUS     // Save the mepc and mstatus
    csrr   x5, CSR_MEPC
    sw     x5, 17*4(sp)
    csrr   x5, CSR_MSTATUS
    sw     x5, 18*4(sp)
    csrr   x5, CSR_MSUBM
    sw     x5, 19*4(sp)
  .endm

  .macro RESTORE_EPC_STATUS  // Restore the mepc and mstatus
    lw     x5,  17*4(sp)
    csrw   CSR_MEPC, x5
    lw     x5,  18*4(sp)
    csrw   CSR_MSTATUS, x5
    lw     x5,  19*4(sp)
    csrw   CSR_MSUBM, x5
  .endm

//-----------------------------------------------------------------------------------
// IRQ Setup function, called from startup.S
//-----------------------------------------------------------------------------------

  .global riscv_setup_irq
  .type   riscv_setup_irq, @function
  .weak   riscv_setup_irq
riscv_setup_irq:

  li    t0, 0x200           // Set the the NMI base to share with mtvec by setting CSR_MMISC_CTL
  csrs  CSR_MMISC_CTL, t0

  la    t0, __isr_vectors
  csrw  mtvt, t0

  la    t0, riscv_irq_entry
  csrw  CSR_MTVT2, t0
  csrs  CSR_MTVT2, 0x1

  la    t0, riscv_trap_entry
  csrw  mtvec, t0
  ret

//-----------------------------------------------------------------------------------
// Trap entry point
//-----------------------------------------------------------------------------------

  .align  6           // In CLIC mode, the trap entry must be 64bytes aligned
  .global riscv_trap_entry
  .type   riscv_trap_entry, @function
  .weak   riscv_trap_entry
riscv_trap_entry:

  SAVE_CONTEXT        // allocate stack space for 20 registers, save 17 registers, 3 locations are additionally available
  SAVE_EPC_STATUS     // use locations 17, 18, 19 to save Save the MEPC/Mstatus/Msubm regs

  csrr  a0, mcause    // Set the function arguments
  mv    a1, sp
  call Trap_Handler

  RESTORE_EPC_STATUS  // Restore the MEPC/Mstatus/Msubm reg
  RESTORE_CONTEXT     // Restore the registers, and stack pointer
  mret                // Return to regular code

//-----------------------------------------------------------------------------------
// IRQ entry point
//-----------------------------------------------------------------------------------

  .align  2
  .global riscv_irq_entry
  .type   riscv_irq_entry, @function
  .weak   riscv_irq_entry
riscv_irq_entry: // -------------> This label will be set to MTVT2 register

  SAVE_CONTEXT        // allocate stack space for 20 registers, save 17 registers, 3 locations are additionally available

  //------This special CSR read operation, which is actually use mcause as operand to directly store it to memory
  csrrwi  x0, CSR_PUSHMCAUSE, 17
  //------This special CSR read operation, which is actually use mepc as operand to directly store it to memory
  csrrwi  x0, CSR_PUSHMEPC, 18
  //------This special CSR read operation, which is actually use Msubm as operand to directly store it to memory
  csrrwi  x0, CSR_PUSHMSUBM, 19

  //------This special CSR read/write operation, which is actually Claim the CLIC to find its pending highest
  // ID, if the ID is not 0, then automatically enable the mstatus.MIE, and jump to its vector-entry-label, and
  // update the link register
  csrrw   ra, CSR_JALMNXTI, ra

  #---- Critical section with interrupts disabled -----------------------
  csrc    CSR_MSTATUS, MSTATUS_MIE

  lw      x5,  19*4(sp)
  csrw    CSR_MSUBM, x5
  lw      x5,  18*4(sp)
  csrw    CSR_MEPC, x5
  lw      x5,  17*4(sp)
  csrw    CSR_MCAUSE, x5

  RESTORE_CONTEXT     // Restore the registers, and stack pointer
  mret                // Return to regular code

//-----------------------------------------------------------------------------------
// END OF FILE
