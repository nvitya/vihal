/*
 *  file:     boards_builtin.h (sg2000)
 *  brief:    some pre-defined boards with CV1800/SG200x chips
 *  date:     2024-04-21
 *  authors:  nvitya
*/

#ifndef _BOARDS_BUILTIN_H_
#define _BOARDS_BUILTIN_H_

#if 0 // to use elif everywhere

#elif defined(BOARD_MILKV_DUO)

  #define BOARD_NAME "Milk-V Duo"
  #define MCU_CV1800
  #define EXTERNAL_XTAL_HZ   25000000

#else

  #error "Unknown board."

#endif

#endif // _BOARDS_BUILTIN_H_
