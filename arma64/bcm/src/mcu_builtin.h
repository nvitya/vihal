/*
 *  file:     mcu_builtin.h (BCM)
 *  brief:    Built-in BCM MCU definitions
 *  date:     2024-07-17
 *  authors:  nvitya
*/

#ifndef __MCU_BUILTIN_H
#define __MCU_BUILTIN_H

#ifdef LITTLE_ENDIAN
  #undef LITTLE_ENDIAN
#endif

#if 0

//----------------------------------------------------------------------
// BCM
//----------------------------------------------------------------------

#elif defined(MCU_BCM2711)

  #define MCUF_BCM
  #define MCUSF_27

  #include "bcm2711_vihal.h"

#else

  #error "Unknown MCU"

#endif

#endif
