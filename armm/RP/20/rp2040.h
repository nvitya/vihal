// rp2040.h

#ifndef _RP2040_H_
#define _RP2040_H_

#include <stdint.h>

// some required Cortex-M defines:

#define __CM0_REV                 0 /*!< Core Revision r0p0                            */
#define __MPU_PRESENT             0 /*!< STM32F0xx do not provide MPU                  */
#define __NVIC_PRIO_BITS          2 /*!< STM32F0xx uses 2 Bits for the Priority Levels */
#define __Vendor_SysTickConfig    0     /*!< Set to 1 if different SysTick Config is used */
#define __VTOR_PRESENT            1
// Mandatory IRQn definition

typedef enum
{
  NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                                        */
  HardFault_IRQn              = -13,    /*!< 3 Cortex-M0 Hard Fault Interrupt                                */
  SVC_IRQn                    = -5,     /*!< 11 Cortex-M0 SV Call Interrupt                                  */
  PendSV_IRQn                 = -2,     /*!< 14 Cortex-M0 Pend SV Interrupt                                  */
  SysTick_IRQn                = -1,     /*!< 15 Cortex-M0 System Tick Interrupt                              */

  TIMER_0_IRQn                 =  0,
  TIMER_1_IRQn                 =  1,
  TIMER_2_IRQn                 =  2,
  TIMER_3_IRQn                 =  3,
  PWM_WRAP_IRQn                =  4,
  USBCTRL_IRQn                 =  5,
  XIP_IRQn                     =  6,
  PIO0_0_IRQn                  =  7,
  PIO0_1_IRQn                  =  8,
  PIO1_0_IRQn                  =  9,
  PIO1_1_IRQn                  = 10,
  DMA_0_IRQn                   = 11,
  DMA_1_IRQn                   = 12,
  IO_BANK0_IRQn                = 13,
  IO_QSPI_IRQn                 = 14,
  SIO_PROC0_IRQn               = 15,
  SIO_PROC1_IRQn               = 16,
  CLOCKS_IRQn                  = 17,
  SPI0_IRQn                    = 18,
  SPI1_IRQn                    = 19,
  UART0_IRQn                   = 20,
  UART1_IRQn                   = 21,
  ADC_FIFO_IRQn                = 22,
  I2C0_IRQn                    = 23,
  I2C1_IRQn                    = 24,
  RTC_IRQn                     = 25
//
} IRQn_Type;


#include "core_cm0plus.h"            /* Cortex-M0 processor and core peripherals */

#define __force_inline   inline __attribute__((always_inline))

// cstddef required for offsetof in the usb.h
#include <cstddef>

#include "hardware/platform_defs.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/dreq.h"

#include "hardware/structs/iobank0.h"
#include "hardware/structs/padsbank0.h"
#include "hardware/structs/sio.h"
#include "hardware/structs/watchdog.h"
#include "hardware/structs/timer.h"
#include "hardware/structs/resets.h"
#include "hardware/structs/clocks.h"
#include "hardware/structs/xosc.h"
#include "hardware/structs/rosc.h"
#include "hardware/structs/pll.h"

#include "hardware/structs/dma.h"
#include "hardware/structs/uart.h"
#include "hardware/structs/spi.h"
#include "hardware/structs/i2c.h"
#include "hardware/structs/adc.h"
#include "hardware/structs/pwm.h"
#include "hardware/structs/pio.h"

#include "hardware/structs/ssi.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/pads_qspi.h"
#include "hardware/structs/xip_ctrl.h"
#include "hardware/structs/usb.h"


#endif /* _RP2040_H_ */

