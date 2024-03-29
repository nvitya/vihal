/*
 *  file:     boards_builtin.h (kendryte)
 *  brief:    some pre-defined boards with kendryte chips
 *  date:     2022-02-01
 *  authors:  nvitya
*/

#ifndef _BOARDS_BUILTIN_H_
#define _BOARDS_BUILTIN_H_

#if 0 // to use elif everywhere

#elif defined(BOARD_MAIX_BIT)

  #define BOARD_NAME "MAiX BiT K210 board"
  #define MCU_K210
  #define EXTERNAL_XTAL_HZ   26000000

#else

  #error "Unknown board."

#endif

#endif // _BOARDS_BUILTIN_H_
