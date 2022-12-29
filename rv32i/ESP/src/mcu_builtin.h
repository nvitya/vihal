/*
 *  file:     mcu_builtin.h (ESP)
 *  brief:    Built-in Risc-V Espressif MCU definitions
 *  version:  1.00
 *  date:     2022-02-03
 *  authors:  nvitya
*/

#ifndef _MCU_BUILTIN_H_
#define _MCU_BUILTIN_H_

#ifdef LITTLE_ENDIAN
  #undef LITTLE_ENDIAN
#endif

#if 0

//----------------------------------------------------------------------
// Espressif
//----------------------------------------------------------------------

#elif defined(MCU_ESP32C3) || defined(MCU_ESP8685)

  #define MCUF_ESP
  #define MCUSF_32C3

  #include "esp32c3_vihal.h"


#else

  #error "Unknown MCU"

#endif

#endif // _MCU_BUILTIN_H_
