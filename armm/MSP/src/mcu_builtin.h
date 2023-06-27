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

  #define __SAMD51J18A__
  #include "samd51.h"

#else

  #error "Unknown MCU"

#endif

#endif
