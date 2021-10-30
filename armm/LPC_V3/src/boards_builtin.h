/*
 *  file:     boards_builtin.h (LPC_V3)
 *  brief:    Built-in LPC_V3 board definitions
 *  version:  1.00
 *  date:     2018-02-10
 *  authors:  nvitya
*/

#ifndef BOARDS_BUILTIN_H_
#define BOARDS_BUILTIN_H_

#if 0 // to use elif everywhere

//-------------------------------------------------------------------------------------------------
// NXP
//-------------------------------------------------------------------------------------------------

#elif defined(BOARD_MIBO100_LPC546XX) ||  defined(BOARD_MIBO100_LPC546)

  #define BOARD_NAME "MIBO-100 LPC546xx by nvitya"
  #define MCU_LPC54606
  #define EXTERNAL_XTAL_HZ   12000000

#elif defined(BOARD_MIBO100_LPC540)

  #define BOARD_NAME "MIBO-100 LPC540xx by nvitya"
  #define MCU_LPC54016
  #define EXTERNAL_XTAL_HZ   12000000

#elif defined(BOARD_XPRESSO_LPC54608)

  #define BOARD_NAME "LPCXpresso54608"
  #define MCU_LPC54608
  #define EXTERNAL_XTAL_HZ   12000000

#else

  #error "Unknown board."

#endif


#endif /* BOARDS_BUILTIN_H_ */
