/*
 *  file:     mcu_defs.h (ESP)
 *  brief:    ESP MCU Family definitions
 *  version:  1.00
 *  date:     2022-01-29
 *  authors:  nvitya
*/

#pragma once

#define MAX_CLOCK_SPEED         240000000

#define MCU_INTERNAL_RC_SPEED     8000000 // ring oscillator

#define HW_DMA_MAX_COUNT            32764


//#define HW_GPIO_REGS      iobank0_status_ctrl_hw_t
//#define HW_UART_REGS      SercomUsart


inline void __attribute__((always_inline)) mcu_preinit_code()
{
	// set the PS_INTLEVEL lower for proper JTAG debugging:

	asm("movi    a0, (3 | 0x00040000 | 0x00000020)");  //  = PS_INTLEVEL(3) | PS_UM | PS_WOE
	asm("wsr     a0, PS");
}

inline void __attribute__((always_inline)) mcu_postinit_code()
{
}

