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
 *  file:     vectors_c906.S
 *  brief:    Interrupt handling for C906 RV64G CPUs
 *  created:  2024-04-26
 *  authors:  nvitya
 *  description:
 */

#ifndef LINUX

#define MSTATUS_MIE         0x00000008
#define CSR_MSTATUS         0x300
#define CSR_MEPC            0x341
#define CSR_MCAUSE          0x342
#define CSR_MMISC_CTL       0x7d0
#define CSR_MSUBM           0x7c4

  .section  .startup
  .balign   4


//-----------------------------------------------------------------------------------
// INTERRUPT HANDLERS
//-----------------------------------------------------------------------------------

  .macro SAVE_CONTEXT

    addi   sp, sp, -22 * 8  // allocate stack space

    sd     x1,  0*8(sp)
    sd     x4,  1*8(sp)
    sd     x5,  2*8(sp)
    sd     x6,  3*8(sp)
    sd     x7,  4*8(sp)
    sd    x10,  5*8(sp)
    sd    x11,  6*8(sp)
    sd    x12,  7*8(sp)
    sd    x13,  8*8(sp)
    sd    x14,  9*8(sp)
    sd    x15, 10*8(sp)

    sd    x16, 11*8(sp)
    sd    x17, 12*8(sp)
    sd    x28, 13*8(sp)
    sd    x29, 14*8(sp)
    sd    x30, 15*8(sp)
    sd    x31, 16*8(sp)

    // location 17: MEPC
    // location 18: MSTATUS
    // location 19: - (MSUBM)
    // location 20: PLIC EXTI number (32-bit)
    // location 21: -
  .endm

  .macro RESTORE_CONTEXT
    ld     x1,  0*8(sp)
    ld     x4,  1*8(sp)
    ld     x5,  2*8(sp)
    ld     x6,  3*8(sp)
    ld     x7,  4*8(sp)
    ld    x10,  5*8(sp)
    ld    x11,  6*8(sp)
    ld    x12,  7*8(sp)
    ld    x13,  8*8(sp)
    ld    x14,  9*8(sp)
    ld    x15, 10*8(sp)

    ld    x16, 11*8(sp)
    ld    x17, 12*8(sp)
    ld    x28, 13*8(sp)
    ld    x29, 14*8(sp)
    ld    x30, 15*8(sp)
    ld    x31, 16*8(sp)

    addi   sp, sp, 22 * 8  // De-allocate the stack space
  .endm

  .macro SAVE_EPC_STATUS     // Save the mepc and mstatus
    csrr   x5, CSR_MEPC
    sd     x5, 17*8(sp)
    csrr   x5, CSR_MSTATUS
    sd     x5, 18*8(sp)
    //csrr   x5, CSR_MSUBM
    //sw     x5, 19*4(sp)
  .endm

  .macro RESTORE_EPC_STATUS  // Restore the mepc and mstatus
    ld     x5,  17*8(sp)
    csrw   CSR_MEPC, x5
    ld     x5,  18*8(sp)
    csrw   CSR_MSTATUS, x5
    //lw     x5,  19*4(sp)
    //csrw   CSR_MSUBM, x5
  .endm

//-----------------------------------------------------------------------------------
// IRQ Setup function, called from startup.S
//-----------------------------------------------------------------------------------



  .global riscv_setup_irq
  .type   riscv_setup_irq, @function
  .weak   riscv_setup_irq
riscv_setup_irq:
  .cfi_startproc

  la    t0, riscv_trap_entry
  csrw  mtvec, t0
  csrw  mepc, t0

  ret
  .cfi_endproc


//-----------------------------------------------------------------------------------
// Trap entry, entry point for all irqs and exceptions
//-----------------------------------------------------------------------------------

  .align  6           // The trap entry (address) must be aligned to 64 bytes !
  .global riscv_trap_entry
  .type   riscv_trap_entry, @function
  .weak   riscv_trap_entry
riscv_trap_entry:
  .cfi_startproc

  SAVE_CONTEXT        // allocate stack space for 20 registers, save 17 registers, 3 locations are additionally available
  SAVE_EPC_STATUS     // use locations 17, 18 to save the MEPC/Mstatus regs

  csrr   t0, mcause    // get the index of exception / interrupt
  blt    t0, x0, riscv_trapcause_irq  // the highest bit set = negative = irq
riscv_trapcause_exception:
  andi   t0, t0, 0x0F
  slli   t0, t0, 3     // shift left by 3 = * 8
  la     t1, __isr_exc_vectors
  add    t0, t0, t1    // add to the vector table address
  ld     t0, 0(t0)     // get the address
  jalr   t0            // jump (and link) to the handler
  j riscv_trap_end

riscv_trapcause_irq:
  andi   t0, t0, 0x0F
  li     t1, 11
  beq    t0, t1, riscv_trap_extirq

  slli   t0, t0, 3     // shift left by 3 = * 8
  la     t1, __isr_irq_vectors
  add    t0, t0, t1    // add to the vector table address
  ld     t0, 0(t0)     // get the address
  jalr   t0            // jump (and link) to the handler

riscv_trap_extirq:
  lui    t1, 0x70200    // PLIC claim register - 4 = 0x70200_000
  lw     t0, 4(t1)      // +4 offset = PLIC claim register
  beq    t0, x0, riscv_trap_end

  sw     t0, 20*8(sp)   // save PLIC EXTI number
  slli   t0, t0, 3      // shift left by 3 = * 8
  la     t1, __isr_irq_vectors
  add    t0, t0, t1    // add to the vector table address
  ld     t0, 0(t0)     // get the address
  jalr   t0            // jump (and link) to the handler

  lw     t0, 20*8(sp)   // load the PLIC EXTI number
  lui    t1, 0x70200    // PLIC claim register - 4 = 0x70200_000
  sw     t0, 4(t1)      // +4 offset = PLIC claim register
  j riscv_trap_extirq   // check for the next pending EXT IRQ

riscv_trap_end:

  RESTORE_EPC_STATUS  // Restore the MEPC/Mstatus/Msubm reg
  RESTORE_CONTEXT     // Restore the registers, and stack pointer
  mret                // Return to regular code
  .cfi_endproc

#endif

//-----------------------------------------------------------------------------------
// END OF FILE
