/*
 *  file:     boards_builtin.h (ESP)
 *  brief:    Built-in ESP(32) board definitions
 *  version:  1.00
 *  date:     2022-01-29
 *  authors:  nvitya
*/

#pragma once

#if 0 // to use elif everywhere

//-------------------------------------------------------------------------------------------------
// ESP32
//-------------------------------------------------------------------------------------------------

#elif defined(BOARD_ESP32_DEVKIT)

  #define BOARD_NAME "ESP32 Development Kit"
  #define MCU_ESP32
  #define EXTERNAL_XTAL_HZ   24000000

#else

  #error "Unknown board."

#endif

