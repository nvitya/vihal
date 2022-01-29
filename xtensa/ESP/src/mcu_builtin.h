/*
 *  file:     mcu_builtin.h (ESP)
 *  brief:    Built-in ESP MCU definitions
 *  version:  1.00
 *  date:     2022-01-29
 *  authors:  nvitya
*/

#pragma once

#ifdef LITTLE_ENDIAN
  #undef LITTLE_ENDIAN
#endif

#if 0

//----------------------------------------------------------------------
// Espressif
//----------------------------------------------------------------------

#elif defined(MCU_ESP32)

  #define MCUF_ESP
  #define MCUSF_ESP32

  #include "esp32_vihal.h"

#else

  #error "Unknown MCU"

#endif


