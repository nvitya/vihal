/*
 *  file:     boards_builtin.h (AM)
 *  brief:    Built-in AM board definitions
 *  date:     2024-06-15
 *  authors:  nvitya
*/

#ifndef BOARDS_BUILTIN_H_
#define BOARDS_BUILTIN_H_

#if 0 // to use elif everywhere

//-------------------------------------------------------------------------------------------------
// TI AM24xx
//-------------------------------------------------------------------------------------------------

#elif defined(BOARD_LP_AM2434)

  #define BOARD_NAME "Launchpad AM2434"
  #define MCU_AM2434
  #define EXTERNAL_XTAL_HZ     48000000

#else

  #error "Unknown board."

#endif


#endif /* BOARDS_BUILTIN_H_ */
