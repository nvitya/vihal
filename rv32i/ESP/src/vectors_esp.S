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
 *  file:     vectors_esp.S
 *  brief:    Interrupt handling for ESP32C3
 *  created:  2022-06-19
 *  authors:  nvitya
 *  description:
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

  .section  .startup
  .balign   4


//-----------------------------------------------------------------------------------
// INTERRUPT HANDLERS
//-----------------------------------------------------------------------------------

  .macro SAVE_CONTEXT

    addi   sp, sp, -20 * 4  // allocate stack space

    sw     x1,  0*4(sp)
    sw     x4,  1*4(sp)  // migt not required
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
    lw     x4,  1*4(sp)  // migt not required
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
    //csrr   x5, CSR_MSUBM
    //sw     x5, 19*4(sp)
  .endm

  .macro RESTORE_EPC_STATUS  // Restore the mepc and mstatus
    lw     x5,  17*4(sp)
    csrw   CSR_MEPC, x5
    lw     x5,  18*4(sp)
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

  la    t0, __isr_vectors
  csrw  mtvec, t0
  csrw  mepc, t0

  ret
  .cfi_endproc


//-----------------------------------------------------------------------------------
// Trap entry, entry point for all irqs
//-----------------------------------------------------------------------------------

  .align  6           // The trap entry (address) must be aligned to 64 bytes !
  .global riscv_trap_entry
  .type   riscv_trap_entry, @function
  .weak   riscv_trap_entry
riscv_trap_entry:
  .cfi_startproc

  SAVE_CONTEXT        // allocate stack space for 20 registers, save 17 registers, 3 locations are additionally available
  SAVE_EPC_STATUS     // use locations 17, 18 to save Save the MEPC/Mstatus regs

  csrr   t0, mcause    // get the index
  srli   t1, t0, 27    // move the interrupt bit from position 31 to position 4.
  andi   t1, t1, 0x10  // keep only the interrupt bit in t1
  andi   t0, t0, 0x0F
  or     t0, t0, t1    // combine: bit4 = interrupt, bit3-0: cause
  slli   t0, t0, 2     // shift left by 2 = * 4

  la     t1, __isr_vectors
  add    t0, t0, t1    // add to the vector table address
  lw     t0, 0(t0)     // get the address
  jalr   t0            // jump (and link) to the handler

  RESTORE_EPC_STATUS  // Restore the MEPC/Mstatus/Msubm reg
  RESTORE_CONTEXT     // Restore the registers, and stack pointer
  mret                // Return to regular code
  .cfi_endproc


//-----------------------------------------------------------------------------------
// END OF FILE
