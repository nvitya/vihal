/*
 *  file:     mcu_defs.h (RP)
 *  brief:    RP MCU Family definitions
 *  version:  1.00
 *  date:     2021-12-18
 *  authors:  nvitya
*/

#ifndef __MCU_DEFS_H
#define __MCU_DEFS_H

#ifndef MAX_CLOCK_SPEED
  #define MAX_CLOCK_SPEED  132000000
#endif

#define MAX_IRQ_HANDLER_COUNT   64

#define MCU_INTERNAL_RC_SPEED      6000000 // ring oscillator

#define HW_GPIO_REGS      io_bank0_status_ctrl_hw_t
//#define HW_UART_REGS      SercomUsart

#if __CORTEX_M < 3
  #define CLOCKCNT16     (pwm_hw->slice[7].ctr)
#endif


inline void __attribute__((always_inline)) mcu_preinit_code()
{
}

inline void __attribute__((always_inline)) mcu_postinit_code()
{
}

#endif // __MCU_DEFS_H
