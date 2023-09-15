/*
 *  file:     mcu_defs.h (IMXRT)
 *  brief:    IMXRT MCU Family definitions
 *  created:  2018-11-23
 *  authors:  nvitya
*/

#ifndef __MCU_DEFS_H
#define __MCU_DEFS_H

#if defined(MCUSF_1020)

  #define MAX_CLOCK_SPEED  500000000

#elif defined(MCUSF_1050)

  //#define MAX_CLOCK_SPEED  600000000
  #define MAX_CLOCK_SPEED  528000000  // safer speed, 600 MHz does not work yet

#endif

#define HW_GPIO_REGS      GPIO_Type
#define HW_UART_REGS      LPUART_Type
#define HW_SPI_REGS       LPSPI_Type
#define HW_QSPI_REGS		  FLEXSPI_Type

inline void __attribute__((always_inline)) mcu_preinit_code()
{
  // Disable Watchdog
  if (WDOG1->WCR & WDOG_WCR_WDE_MASK)
  {
      WDOG1->WCR &= ~WDOG_WCR_WDE_MASK;
  }
  if (WDOG2->WCR & WDOG_WCR_WDE_MASK)
  {
      WDOG2->WCR &= ~WDOG_WCR_WDE_MASK;
  }
  RTWDOG->CNT = 0xD928C520U; // 0xD928C520U is the update key
  RTWDOG->TOVAL = 0xFFFF;
  RTWDOG->CS = (uint32_t) ((RTWDOG->CS) & ~RTWDOG_CS_EN_MASK) | RTWDOG_CS_UPDATE_MASK;
}

#endif // __MCU_DEFS_H
