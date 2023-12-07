/*
 *  file:     boards_builtin.h (MSP)
 *  brief:    Built-in MSP board definitions
 *  date:     2023-06-27
 *  authors:  nvitya
*/

#ifndef BOARDS_BUILTIN_H_
#define BOARDS_BUILTIN_H_

#if 0 // to use elif everywhere

//-------------------------------------------------------------------------------------------------
// TI MSPM0G
//-------------------------------------------------------------------------------------------------

#elif defined(BOARD_LP_MSPM0G3507)

  #define BOARD_NAME "Launchpad MSPM0G3507"
  #define MCU_MSPM0G3507
  #define EXTERNAL_XTAL_HZ     48000000

  #define MCU_EARLY_SAMPLE

#else

  #error "Unknown board."

#endif


#endif /* BOARDS_BUILTIN_H_ */
