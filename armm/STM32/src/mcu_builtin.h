/*
 *  file:     mcu_builtin.h (STM32)
 *  brief:    Built-in STM32 MCU definitions
 *  created:  2018-02-10
 *  authors:  nvitya
*/

#ifndef __MCU_BUILTIN_H
#define __MCU_BUILTIN_H

#if 0

//----------------------------------------------------------------------
// STM32
//----------------------------------------------------------------------

// STM32F0: Cortex-M0

#elif defined(MCU_STM32F030F4)

  #define MCUF_STM32
  #define MCUSF_F0

  #include "stm32f030x6.h"

#elif defined(MCU_STM32F072RB)

  #define MCUF_STM32
  #define MCUSF_F0

  #include "stm32f072xb.h"

#elif defined(MCU_STM32F070F6)

  #define MCUF_STM32
  #define MCUSF_F0

  #include "stm32f070x6.h"

#elif defined(MCU_STM32F070RB)

  #define MCUF_STM32
  #define MCUSF_F0

  #include "stm32f070xb.h"

#elif defined(MCU_STM32G030F6)

  #define MCUF_STM32
  #define MCUSF_G0

  #include "stm32g030xx.h"

#elif defined(MCU_STM32G0B1RE)

  #define MCUF_STM32
  #define MCUSF_G0

  #include "stm32g0b1xx.h"

// STM32F1: Cortex-M3

#elif defined(MCU_STM32F103C8)

  #define MCUF_STM32
  #define MCUSF_F1
  #include "stm32f103xb.h"

// STM32F3: Cortex-M4F

#elif defined(MCU_STM32F301C8) || defined(MCU_STM32F301K6)

  #define MCUF_STM32
  #define MCUSF_F3

  #include "stm32f301x8.h"

#elif defined(MCU_STM32F303CB) || defined(MCU_STM32F303CC)

  #define MCUF_STM32
  #define MCUSF_F3

  #include "stm32f303xc.h"

// STM32F4: Cortex-M4F

#elif defined(MCU_STM32F446ZE)

	#define MCUF_STM32
  #define MCUSF_F4
  #define MAX_CLOCK_SPEED  180000000

  #include "stm32f446xx.h"

#elif defined(MCU_STM32F429ZI)

	#define MCUF_STM32
  #define MCUSF_F4
  #define MAX_CLOCK_SPEED  180000000

  #include "stm32f429xx.h"

#elif defined(MCU_STM32F401CC) || defined(MCU_STM32F401BC) || defined(MCU_STM32F401VC) || defined(MCU_STM32F401VB)

  #define MCUF_STM32
  #define MCUSF_F4
  #define MAX_CLOCK_SPEED  84000000

  #include "stm32f401xc.h"

#elif defined(MCU_STM32F411CE) || defined(MCU_STM32F411VE) || defined(MCU_STM32F411CC) || defined(MCU_STM32F411VC)

  #define MCUF_STM32
  #define MCUSF_F4
  #define MAX_CLOCK_SPEED  96000000 // actually 100 MHz, but the USB works only with this

  #include "stm32f411xe.h"

#elif defined(MCU_STM32F405RG) || defined(MCU_STM32F405VE) || defined(MCU_STM32F405VG) || defined(MCU_STM32F405ZE) || defined(MCU_STM32F405ZG)

	#define MCUF_STM32
  #define MCUSF_F4
  #define MAX_CLOCK_SPEED  168000000

  #include "stm32f405xx.h"

#elif defined(MCU_STM32F407VE) || defined(MCU_STM32F407VE) || defined(MCU_STM32F407VG) || defined(MCU_STM32F407ZE) || defined(MCU_STM32F407ZG)

  #define MCUF_STM32
  #define MCUSF_F4
  #define MAX_CLOCK_SPEED  168000000

  #include "stm32f407xx.h"

#elif defined(MCU_STM32G474RE) || defined(MCU_STM32G473RE) || defined(MCU_STM32G473CB) || defined(MCU_STM32G473CE)

  #define MCUF_STM32
  #define MCUSF_G4

  #include "stm32g474xx.h"

#elif defined(MCU_STM32G431CB) || defined(MCU_STM32G431KB)

  #define MCUF_STM32
  #define MCUSF_G4

  #include "stm32g431xx.h"

#elif defined(MCU_STM32WB55VG) || defined(MCU_STM32WB5MMG)

  #define MCUF_STM32
  #define MCUSF_WB

  #include "stm32wb55xx.h"

// STM32F7: Cortex-M7

#elif defined(MCU_STM32F746ZG) || defined(MCU_STM32F746NG)

  #define MCUF_STM32
  #define MCUSF_F7

  #include "stm32f746xx.h"

#elif defined(MCU_STM32F750N8)

  #define MCUF_STM32
  #define MCUSF_F7

  #include "stm32f750xx.h"

// STM32H7: Cortex-M7 (400 MHz +)

#elif defined(MCU_STM32H743ZI)

  #define MCUF_STM32
  #define MCUSF_H7
  #define MAX_CLOCK_SPEED  400000000

  #include "stm32h743xx.h"

#elif defined(MCU_STM32H747XI)

  #define MCUF_STM32
  #define MCUSF_H7
  #define MAX_CLOCK_SPEED  480000000

  #include "stm32h747xx.h"

#elif defined(MCU_STM32H750VB)

  #define MCUF_STM32
  #define MCUSF_H7
  #define MAX_CLOCK_SPEED  400000000

  #include "stm32h750xx.h"

// STM32H7 v2: Cortex-M7 (520/550 MHz)

#elif defined(MCU_STM32H723ZG)

  #define MCUF_STM32
  #define MCUSF_H7
  #define MCUSF_H7_V2
  #define MAX_CLOCK_SPEED  520000000  // actually 550 MHz with FREQ_BOOST

  #include "stm32h723xx.h"

#elif defined(MCU_STM32H7S3L8)

  #define MCUF_STM32
  #define MCUSF_H7RS
  #define MAX_CLOCK_SPEED  600000000

  #include "stm32h7s3xx.h"

#else

  #error "Unknown STM32 MCU"

#endif

#endif
