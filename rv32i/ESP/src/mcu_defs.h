/*
 *  file:     mcu_defs.h (ESP)
 *  brief:    ESP Risc-V MCU Family definitions
 *  version:  1.00
 *  date:     2022-02-03
 *  authors:  nvitya
*/

#pragma once

#define MAX_CLOCK_SPEED         160000000

#define MCU_INTERNAL_RC_SPEED     4000000 // ring oscillator

#define HW_DMA_MAX_COUNT            32764


//#define HW_GPIO_REGS      iobank0_status_ctrl_hw_t
//#define HW_UART_REGS      SercomUsart

#define CLOCKCNT  (riscv_cpu_csr_read(0x7E2))  // use the mpccr instead of the mtime

inline void __attribute__((always_inline)) mcu_preinit_code()
{
  asm("li    t0, 1");
  asm("csrw  0x7E0, t0");  // = mpcer[0] = count clock cycles
  asm("li    t0, 1");
  asm("csrw  0x7E1, t0");  // = mpcmer[0] = enable counting
}

inline void __attribute__((always_inline)) mcu_postinit_code()
{
}

