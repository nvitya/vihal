// rp2040.h

#ifndef _RP2040_H_
#define _RP2040_H_

#include <stdint.h>

// some required Cortex-M defines:

#define __CM0_REV                 0 /*!< Core Revision r0p0                            */
#define __MPU_PRESENT             0 /*!< STM32F0xx do not provide MPU                  */
#define __NVIC_PRIO_BITS          2 /*!< STM32F0xx uses 2 Bits for the Priority Levels */
#define __Vendor_SysTickConfig    0     /*!< Set to 1 if different SysTick Config is used */

// Mandatory IRQn definition

typedef enum
{
  NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                                        */
  HardFault_IRQn              = -13,    /*!< 3 Cortex-M0 Hard Fault Interrupt                                */
  SVC_IRQn                    = -5,     /*!< 11 Cortex-M0 SV Call Interrupt                                  */
  PendSV_IRQn                 = -2,     /*!< 14 Cortex-M0 Pend SV Interrupt                                  */
  SysTick_IRQn                = -1,     /*!< 15 Cortex-M0 System Tick Interrupt                              */

  Device_Irq_00               = 0,
  Device_Irq_01               = 1,
  Device_Irq_02               = 2,
  Device_Irq_03               = 3,
  Device_Irq_04               = 4,
  Device_Irq_05               = 5,
  Device_Irq_06               = 6,
  Device_Irq_07               = 7,
  Device_Irq_08               = 8,
  Device_Irq_09               = 9,
  Device_Irq_10               = 10,
  Device_Irq_11               = 11,
  Device_Irq_12               = 12,
  Device_Irq_13               = 13,
  Device_Irq_14               = 14,
  Device_Irq_15               = 15
//
} IRQn_Type;


#include "core_cm0.h"            /* Cortex-M0 processor and core peripherals */

#define __force_inline   inline __attribute__((always_inline))

#include "hardware/platform_defs.h"
#include "hardware/regs/addressmap.h"

#include "hardware/structs/iobank0.h"
#include "hardware/structs/padsbank0.h"
#include "hardware/structs/sio.h"
#include "hardware/structs/watchdog.h"
#include "hardware/structs/timer.h"
#include "hardware/structs/resets.h"

#endif /* _RP2040_H_ */

