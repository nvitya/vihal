// rp2350.h

#ifndef _RP2350_H_
#define _RP2350_H_

#include <stdint.h>

/* =========================================================================================================================== */
/* ================                                Interrupt Number Definition                                ================ */
/* =========================================================================================================================== */

typedef enum {
/* =======================================  ARM Cortex-M33 Specific Interrupt Numbers  ======================================= */
  Reset_IRQn                = -15,              /*!< -15  Reset Vector, invoked on Power up and warm reset                     */
  NonMaskableInt_IRQn       = -14,              /*!< -14  Non maskable Interrupt, cannot be stopped or preempted               */
  HardFault_IRQn            = -13,              /*!< -13  Hard Fault, all classes of Fault                                     */
  MemoryManagement_IRQn     = -12,              /*!< -12  Memory Management, MPU mismatch, including Access Violation
                                                     and No Match                                                              */
  BusFault_IRQn             = -11,              /*!< -11  Bus Fault, Pre-Fetch-, Memory Access Fault, other address/memory
                                                     related Fault                                                             */
  UsageFault_IRQn           = -10,              /*!< -10  Usage Fault, i.e. Undef Instruction, Illegal State Transition        */
  SecureFault_IRQn          =  -9,              /*!< -9 Secure Fault Handler                                                   */
  SVCall_IRQn               =  -5,              /*!< -5 System Service Call via SVC instruction                                */
  DebugMonitor_IRQn         =  -4,              /*!< -4 Debug Monitor                                                          */
  PendSV_IRQn               =  -2,              /*!< -2 Pendable request for system service                                    */
  SysTick_IRQn              =  -1,              /*!< -1 System Tick Timer                                                      */
/* ===========================================  RP2350 Specific Interrupt Numbers  =========================================== */
  TIMER0_IRQ_0_IRQn         =   0,              /*!< 0  TIMER0_IRQ_0                                                           */
  TIMER0_IRQ_1_IRQn         =   1,              /*!< 1  TIMER0_IRQ_1                                                           */
  TIMER0_IRQ_2_IRQn         =   2,              /*!< 2  TIMER0_IRQ_2                                                           */
  TIMER0_IRQ_3_IRQn         =   3,              /*!< 3  TIMER0_IRQ_3                                                           */
  TIMER1_IRQ_0_IRQn         =   4,              /*!< 4  TIMER1_IRQ_0                                                           */
  TIMER1_IRQ_1_IRQn         =   5,              /*!< 5  TIMER1_IRQ_1                                                           */
  TIMER1_IRQ_2_IRQn         =   6,              /*!< 6  TIMER1_IRQ_2                                                           */
  TIMER1_IRQ_3_IRQn         =   7,              /*!< 7  TIMER1_IRQ_3                                                           */
  PWM_IRQ_WRAP_0_IRQn       =   8,              /*!< 8  PWM_IRQ_WRAP_0                                                         */
  PWM_IRQ_WRAP_1_IRQn       =   9,              /*!< 9  PWM_IRQ_WRAP_1                                                         */
  DMA_IRQ_0_IRQn            =  10,              /*!< 10 DMA_IRQ_0                                                              */
  DMA_IRQ_1_IRQn            =  11,              /*!< 11 DMA_IRQ_1                                                              */
  DMA_IRQ_2_IRQn            =  12,              /*!< 12 DMA_IRQ_2                                                              */
  DMA_IRQ_3_IRQn            =  13,              /*!< 13 DMA_IRQ_3                                                              */
  USBCTRL_IRQ_IRQn          =  14,              /*!< 14 USBCTRL_IRQ                                                            */
  PIO0_IRQ_0_IRQn           =  15,              /*!< 15 PIO0_IRQ_0                                                             */
  PIO0_IRQ_1_IRQn           =  16,              /*!< 16 PIO0_IRQ_1                                                             */
  PIO1_IRQ_0_IRQn           =  17,              /*!< 17 PIO1_IRQ_0                                                             */
  PIO1_IRQ_1_IRQn           =  18,              /*!< 18 PIO1_IRQ_1                                                             */
  PIO2_IRQ_0_IRQn           =  19,              /*!< 19 PIO2_IRQ_0                                                             */
  PIO2_IRQ_1_IRQn           =  20,              /*!< 20 PIO2_IRQ_1                                                             */
  IO_IRQ_BANK0_IRQn         =  21,              /*!< 21 IO_IRQ_BANK0                                                           */
  IO_IRQ_BANK0_NS_IRQn      =  22,              /*!< 22 IO_IRQ_BANK0_NS                                                        */
  IO_IRQ_QSPI_IRQn          =  23,              /*!< 23 IO_IRQ_QSPI                                                            */
  IO_IRQ_QSPI_NS_IRQn       =  24,              /*!< 24 IO_IRQ_QSPI_NS                                                         */
  SIO_IRQ_FIFO_IRQn         =  25,              /*!< 25 SIO_IRQ_FIFO                                                           */
  SIO_IRQ_BELL_IRQn         =  26,              /*!< 26 SIO_IRQ_BELL                                                           */
  SIO_IRQ_FIFO_NS_IRQn      =  27,              /*!< 27 SIO_IRQ_FIFO_NS                                                        */
  SIO_IRQ_BELL_NS_IRQn      =  28,              /*!< 28 SIO_IRQ_BELL_NS                                                        */
  SIO_IRQ_MTIMECMP_IRQn     =  29,              /*!< 29 SIO_IRQ_MTIMECMP                                                       */
  CLOCKS_IRQ_IRQn           =  30,              /*!< 30 CLOCKS_IRQ                                                             */
  SPI0_IRQ_IRQn             =  31,              /*!< 31 SPI0_IRQ                                                               */
  SPI1_IRQ_IRQn             =  32,              /*!< 32 SPI1_IRQ                                                               */
  UART0_IRQ_IRQn            =  33,              /*!< 33 UART0_IRQ                                                              */
  UART1_IRQ_IRQn            =  34,              /*!< 34 UART1_IRQ                                                              */
  ADC_IRQ_FIFO_IRQn         =  35,              /*!< 35 ADC_IRQ_FIFO                                                           */
  I2C0_IRQ_IRQn             =  36,              /*!< 36 I2C0_IRQ                                                               */
  I2C1_IRQ_IRQn             =  37,              /*!< 37 I2C1_IRQ                                                               */
  OTP_IRQ_IRQn              =  38,              /*!< 38 OTP_IRQ                                                                */
  TRNG_IRQ_IRQn             =  39,              /*!< 39 TRNG_IRQ                                                               */
  PLL_SYS_IRQ_IRQn          =  42,              /*!< 42 PLL_SYS_IRQ                                                            */
  PLL_USB_IRQ_IRQn          =  43,              /*!< 43 PLL_USB_IRQ                                                            */
  POWMAN_IRQ_POW_IRQn       =  44,              /*!< 44 POWMAN_IRQ_POW                                                         */
  POWMAN_IRQ_TIMER_IRQn     =  45               /*!< 45 POWMAN_IRQ_TIMER                                                       */
} IRQn_Type;

/* =========================================================================================================================== */
/* ================                           Processor and Core Peripheral Section                           ================ */
/* =========================================================================================================================== */

/* ==========================  Configuration of the ARM Cortex-M33 Processor and Core Peripherals  =========================== */
#define __CM33_REV                 0x0100U      /*!< CM33 Core Revision                                                        */
#define __NVIC_PRIO_BITS               4        /*!< Number of Bits used for Priority Levels                                   */
#define __Vendor_SysTickConfig         0        /*!< Set to 1 if different SysTick Config is used                              */
#define __VTOR_PRESENT                 1        /*!< Set to 1 if CPU supports Vector Table Offset Register                     */
#define __MPU_PRESENT                  1        /*!< MPU present                                                               */
#define __FPU_PRESENT                  1        /*!< FPU present                                                               */
#define __FPU_DP                       0        /*!< Double Precision FPU                                                      */
#define __DSP_PRESENT                  1        /*!< DSP extension present                                                     */
#define __SAUREGION_PRESENT            1        /*!< SAU region present                                                        */


/** @} */ /* End of group Configuration_of_CMSIS */

#include "core_cm33.h"                          /*!< ARM Cortex-M33 processor and core peripherals                             */
#include "system_RP2350.h"                      /*!< RP2350 System                                                             */

#ifndef __IM                                    /*!< Fallback for older CMSIS versions                                         */
  #define __IM   __I
#endif
#ifndef __OM                                    /*!< Fallback for older CMSIS versions                                         */
  #define __OM   __O
#endif
#ifndef __IOM                                   /*!< Fallback for older CMSIS versions                                         */
  #define __IOM  __IO
#endif

#define __force_inline   inline __attribute__((always_inline))

#define SELF_FLASHING_SPI_ADDR    0x001000
#define SELF_FLASHING_RAM_ADDR  0x20000000

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
#include "hardware/structs/ticks.h"

#include "hardware/structs/dma.h"
#include "hardware/structs/uart.h"
#include "hardware/structs/spi.h"
#include "hardware/structs/i2c.h"
#include "hardware/structs/adc.h"
#include "hardware/structs/pwm.h"
#include "hardware/structs/pio.h"

#include "hardware/structs/qmi.h"  // SSI -> QMI !
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/pads_qspi.h"
#include "hardware/structs/xip_ctrl.h"
#include "hardware/structs/usb.h"


#endif /* _RP2350_H_ */

