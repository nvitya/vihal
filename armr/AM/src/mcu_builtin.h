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

#elif defined(MCU_AM2434)

  #define MCUF_AM
  #define MCUSF_24

  #include "am2434.h"

#else

  #error "Unknown MCU"

#endif

#define HW_DMA_MAX_COUNT 65536

#endif
