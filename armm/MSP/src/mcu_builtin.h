/*
 *  file:     mcu_builtin.h (MSP)
 *  brief:    Built-in MSP MCU definitions
 *  date:     2023-06-27
 *  authors:  nvitya
*/

#ifndef __MCU_BUILTIN_H
#define __MCU_BUILTIN_H

#ifdef LITTLE_ENDIAN
  #undef LITTLE_ENDIAN
#endif

#if 0

//----------------------------------------------------------------------
// Atmel v2
//----------------------------------------------------------------------

#elif defined(MCU_MSPM0G3505) || defined(MCU_MSPM0G3506) || defined(MCU_MSPM0G3507)

  #define MCUF_MSP
  #define MCUSF_M0G

  #include "mspm0g350x.h"

#elif defined(MCU_MSPM0G3105) || defined(MCU_MSPM0G3106) || defined(MCU_MSPM0G3105)

  #define MCUF_MSP
  #define MCUSF_M0G

  #include "mspm0g310x.h"

#else

  #error "Unknown MCU"

#endif

#endif
