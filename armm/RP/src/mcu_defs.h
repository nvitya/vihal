/*
 *  file:     mcu_defs.h (RP)
 *  brief:    RP MCU Family definitions
 *  version:  1.00
 *  date:     2021-12-18
 *  authors:  nvitya
*/

#ifndef __MCU_DEFS_H
#define __MCU_DEFS_H


#define MAX_CLOCK_SPEED  133000000

#define MAX_IRQ_HANDLER_COUNT  64

#define MCU_INTERNAL_RC_SPEED   48000000 // DFLL48M


#define HW_GPIO_REGS      iobank0_status_ctrl_hw_t
//#define HW_UART_REGS      SercomUsart

#if __CORTEX_M < 3
  #define CLOCKCNT_BITS  32
	#define CLOCKCNT       (timer_hw->timelr)
#endif



inline void __attribute__((always_inline)) mcu_preinit_code()
{
}

inline void __attribute__((always_inline)) mcu_postinit_code()
{
}

#endif // __MCU_DEFS_H
