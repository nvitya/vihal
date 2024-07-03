/*
 *  file:     mcu_builtin.h (TI AM)
 *  brief:    Built-in MSP MCU definitions
 *  date:     2024-07-03
 *  authors:  nvitya
*/

#ifndef __MCU_BUILTIN_H
#define __MCU_BUILTIN_H

#ifdef LITTLE_ENDIAN
  #undef LITTLE_ENDIAN
#endif

#if 0

//----------------------------------------------------------------------
// TI AM
//----------------------------------------------------------------------

#elif defined(MCU_AM3558)

  #define MCUF_AM
  #define MCUSF_35

  #include "am3558_vihal.h"

#else

  #error "Unknown MCU"

#endif

#endif
