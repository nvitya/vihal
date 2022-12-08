/*
 *  file:     boards_builtin.h (GD32V)
 *  brief:    some pre-defined boards with GD32V
 *  date:     2021-10-20
 *  authors:  nvitya
*/

#ifndef BOARDS_BUILTIN_H_
#define BOARDS_BUILTIN_H_

#if 0 // to use elif everywhere

#elif defined(BOARD_LONGAN_NANO)

  #define BOARD_NAME "Longan Nano GD32VF103CB board"
  #define MCU_GD32VF103CB
  #define EXTERNAL_XTAL_HZ   8000000

#elif defined(BOARD_WIO_LITE_RV)

  #define BOARD_NAME "Wio Lite RISC-V (GD32VF103CB)"
  #define MCU_GD32VF103CB
  #define EXTERNAL_XTAL_HZ   8000000

#else

  #error "Unknown board."

#endif


#endif /* BOARDS_BUILTIN_H_ */
