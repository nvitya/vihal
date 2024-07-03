/*
 *  file:     boards_builtin.h (AM)
 *  brief:    Built-in TI AM boards
 *  date:     2024-07-03
 *  authors:  nvitya
*/

#ifndef BOARDS_BUILTIN_H_
#define BOARDS_BUILTIN_H_

#if 0 // to use elif everywhere

//-------------------------------------------------------------------------------------------------
// TI AM
//-------------------------------------------------------------------------------------------------

#elif defined(BOARD_BEAGLEB_BLACK)

  #define BOARD_NAME "BeagleBone Black (AM3558)"
  #define MCU_AM3558

#else

  #error "Unknown board."

#endif


#endif /* BOARDS_BUILTIN_H_ */
