/*
 *  file:     boards_builtin.h (ATSAM_V2)
 *  brief:    Built-in ATSAM_V2 board definitions
 *  version:  1.00
 *  date:     2018-02-10
 *  authors:  nvitya
*/

#ifndef BOARDS_BUILTIN_H_
#define BOARDS_BUILTIN_H_

#if 0 // to use elif everywhere

//-------------------------------------------------------------------------------------------------
// ATMEL v2
//-------------------------------------------------------------------------------------------------

#elif defined(BOARD_MIBO64_ATSAME5X)

  #define BOARD_NAME "MIBO-64 ATSAME5X by nvitya"
  #define MCU_ATSAME51J20
  #define EXTERNAL_XTAL_HZ   12000000

#else

  #error "Unknown board."

#endif


#endif /* BOARDS_BUILTIN_H_ */
