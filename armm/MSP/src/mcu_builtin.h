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

#elif defined(MCU_MSPM0G3507)

  #define MCUF_MSP
  #define MCUSF_M0G

  //#define ???
  #include "mspm0g310x.h"

#else

  #error "Unknown MCU"

#endif

#endif
