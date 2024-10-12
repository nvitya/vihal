/*
 *  file:     mcu_builtin.h (RP)
 *  brief:    Built-in RP MCU definitions
 *  created:  2021-12-18
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
  #define MAX_CLOCK_SPEED  132000000

  #include "rp2040_vihal.h"

#elif defined(MCU_RP2350)

  #define MCUF_RP
  #define MCUSF_23
  #define MAX_CLOCK_SPEED  150000000

  #include "rp2350_vihal.h"

#else

  #error "Unknown MCU"

#endif

#endif
