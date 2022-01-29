// file:     platform.h (xtensa)
// brief:    Main xtensa platform include
// created:  2022-01-29
// authors:  nvitya

#ifndef __PLATFORM_H
#define __PLATFORM_H

#include "stdint.h"

#include "generic_defs.h"

// board.h must be provided by the application. It should minimum define the cpu.
#include "board.h"

#if defined(MCU_DEFINED)

  // in the board.h the MCU also can be defined

#else

  #include "mcu_builtin.h"

#endif

#ifndef LITTLE_ENDIAN
	#define LITTLE_ENDIAN
#endif

//------------------------------------------------------------------

// the mcu_defs.h comes from the MCUF directory

#include "mcu_defs.h"

// handle some required definitions

#if !defined(MAX_CLOCK_SPEED)
  #ifdef MCU_FIXED_SPEED
    #define MAX_CLOCK_SPEED  MCU_FIXED_SPEED
  #else
    #error "MCU Maximal clock speed is not defined!"
  #endif
#endif

#ifndef MCU_CLOCK_SPEED
  #define MCU_CLOCK_SPEED  MAX_CLOCK_SPEED
#endif

#if !defined(SPI_SELF_FLASHING)
  #define SPI_SELF_FLASHING 0
#endif

#include "xtensa_generic.h"

#endif
