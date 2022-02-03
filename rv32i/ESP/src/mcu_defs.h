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


inline void __attribute__((always_inline)) mcu_preinit_code()
{
}

inline void __attribute__((always_inline)) mcu_postinit_code()
{
}

