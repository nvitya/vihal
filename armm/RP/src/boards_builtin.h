/*
 *  file:     boards_builtin.h (RP)
 *  brief:    Built-in RP board definitions
 *  version:  1.00
 *  date:     2021-12-18
 *  authors:  nvitya
*/

#ifndef BOARDS_BUILTIN_H_
#define BOARDS_BUILTIN_H_

#if 0 // to use elif everywhere

//-------------------------------------------------------------------------------------------------
// RP (Raspberry Pi)
//-------------------------------------------------------------------------------------------------

#elif defined(BOARD_RPI_PICO)

  #define BOARD_NAME "Raspberry Pi Pico with RP2040"
  #define MCU_RP2040
  #define EXTERNAL_XTAL_HZ   12000000

#elif defined(BOARD_RPI_PICOW)

  #define BOARD_NAME "Raspberry Pi Pico-W with RP2040"
  #define MCU_RP2040
  #define EXTERNAL_XTAL_HZ   12000000

#elif defined(BOARD_RPI_PICO2)

  #define BOARD_NAME "Raspberry Pi Pico-2 with RP2350"
  #define MCU_RP2350
  #define EXTERNAL_XTAL_HZ   12000000

#else

  #error "Unknown board."

#endif


#endif /* BOARDS_BUILTIN_H_ */
