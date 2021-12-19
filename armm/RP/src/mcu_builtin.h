/*
 *  file:     mcu_builtin.h (RP)
 *  brief:    Built-in RP MCU definitions
 *  version:  1.00
 *  date:     2021-12-18
 *  authors:  nvitya
*/

#ifndef __MCU_BUILTIN_H
#define __MCU_BUILTIN_H

#ifdef LITTLE_ENDIAN
  #undef LITTLE_ENDIAN
#endif

#if 0

//----------------------------------------------------------------------
// Raspberry Pi
//----------------------------------------------------------------------

#elif defined(MCU_RP2040)

  #define MCUF_RP
  #define MCUSF_20

  #include "rp2040.h"

#else

  #error "Unknown MCU"

#endif

#endif
