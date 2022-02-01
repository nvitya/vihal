/*
 *  file:     boards_builtin.h (kendryte)
 *  brief:    some pre-defined boards with kendryte chips
 *  date:     2022-02-01
 *  authors:  nvitya
*/

#pragma once

#if 0 // to use elif everywhere

#elif defined(BOARD_MAIX_BIT)

  #define BOARD_NAME "MAiX BiT K210 board"
  #define MCU_K210
  #define EXTERNAL_XTAL_HZ   24000000

#else

  #error "Unknown board."

#endif


