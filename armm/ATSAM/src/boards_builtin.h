/*
 *  file:     boards_builtin.h (ATSAM)
 *  brief:    Built-in ATSAM board definitions
 *  version:  1.00
 *  date:     2018-02-10
 *  authors:  nvitya
*/

#ifndef BOARDS_BUILTIN_H_
#define BOARDS_BUILTIN_H_

#if 0 // to use elif everywhere

//-------------------------------------------------------------------------------------------------
// ATMEL
//-------------------------------------------------------------------------------------------------

#elif defined(BOARD_ARDUINO_DUE)

  #define BOARD_NAME "Arduino DUE"
  #define MCU_ATSAM3X8E
  #define EXTERNAL_XTAL_HZ   12000000

#elif defined(BOARD_XPLAINED_SAME70)

  #define BOARD_NAME "SAME70 XPlained"
  #define MCU_ATSAME70Q21
  #define EXTERNAL_XTAL_HZ   12000000

//-----------------------------------------------------
// boards made by nvitya
//-----------------------------------------------------

#elif defined(BOARD_MIBO100_ATSAME70)

  #define BOARD_NAME "MIBO-100 ATSAME70N by nvitya"
	#define MCU_ATSAME70N20
  #define EXTERNAL_XTAL_HZ   12000000

#elif defined(BOARD_MIBO64_ATSAM4S)

  #define BOARD_NAME "MIBO-64 ATSAM4S by nvitya"
  #define MCU_ATSAM4S2B
  #define EXTERNAL_XTAL_HZ   12000000

#elif defined(BOARD_VERTIBO_A)

  #define BOARD_NAME "VERTIBO-A by nvitya"
  #define MCU_ATSAME70Q20
  #define EXTERNAL_XTAL_HZ  12000000
  #define MCU_CLOCK_SPEED  288000000  // because the SDRAM shares the data bus with the FPGA

#elif defined(BOARD_ENEBO_A)

  #define BOARD_NAME "ENEBO-A by nvitya"
  #define MCU_ATSAME70N20
  #define EXTERNAL_XTAL_HZ  12000000

#else

  #error "Unknown board."

#endif


#endif /* BOARDS_BUILTIN_H_ */
