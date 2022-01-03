/*
 *  file:     boards_builtin.h (STM32)
 *  brief:    Built-in STM32 board definitions
 *  created:  2018-02-10
 *  authors:  nvitya
*/

#ifndef BOARDS_BUILTIN_H_
#define BOARDS_BUILTIN_H_

#if 0 // to use elif everywhere

//-------------------------------------------------------------------------------------------------
// STM32
//-------------------------------------------------------------------------------------------------

#elif defined(BOARD_MIN_F103)

  #define BOARD_NAME "STM32F103C8 Minimum Develompent Board"
  #define MCU_STM32F103C8
  #define EXTERNAL_XTAL_HZ   8000000

#elif defined(BOARD_MIN_F401)

  #define BOARD_NAME "STM32F401CC Minimum Develompent Board" // aka. Black Pill
  #define MCU_STM32F401CC
  #define EXTERNAL_XTAL_HZ   25000000

#elif defined(BOARD_MIN_F411)

  #define BOARD_NAME "STM32F411CE Minimum Develompent Board" // aka. Black Pill
  #define MCU_STM32F411CE
  #define EXTERNAL_XTAL_HZ   25000000

#elif defined(BOARD_MIBO48_STM32F303)

  #define BOARD_NAME "STM32F303Cx 48-pin Develompent Board by nvitya"
  #define MCU_STM32F303CB
  #define EXTERNAL_XTAL_HZ   8000000

#elif defined(BOARD_MIBO48_STM32G473)

  #define BOARD_NAME "STM32G473Cx 48-pin Develompent Board by nvitya"
  #define MCU_STM32G473CB
  #define EXTERNAL_XTAL_HZ  25000000

#elif defined(BOARD_MIBO20_STM32F070)

  #define BOARD_NAME "STM32F070F6 20-pin Develompent Board by WF"
  #define MCU_STM32F070F6
  #define EXTERNAL_XTAL_HZ   8000000

#elif defined(BOARD_MIBO20_STM32F030)

  #define BOARD_NAME "STM32F030F4 20-pin Develompent Board by WF"
  #define MCU_STM32F030F4
  #define EXTERNAL_XTAL_HZ         0

#elif defined(BOARD_MIBO64_STM32F070)

  #define BOARD_NAME "STM32F070RB 64-pin Develompent Board by nvitya"
  #define MCU_STM32F070RB
  #define EXTERNAL_XTAL_HZ   8000000

#elif defined(BOARD_MIBO64_STM32F405)

  #define BOARD_NAME "STM32F405RG 64-pin Develompent Board by nvitya"
  #define MCU_STM32F405RG
  #define EXTERNAL_XTAL_HZ   8000000

#elif defined(BOARD_DISCOVERY_F072)

  #define BOARD_NAME "STM32F072 Discovery"
  #define MCU_STM32F072RB
  #define EXTERNAL_XTAL_HZ         0

#elif defined(BOARD_DEV_STM32F407VG)

  #define BOARD_NAME "STM32F407VG Minimal Board"
  #define MCU_STM32F407VG
  #define EXTERNAL_XTAL_HZ   8000000

#elif defined(BOARD_DEV_STM32F407ZE)

  #define BOARD_NAME "STM32F407ZE Development Board"
  #define MCU_STM32F407ZE
  #define EXTERNAL_XTAL_HZ   8000000

#elif defined(BOARD_NUCLEO_F446)

  #define BOARD_NAME "STM32F446 Nucleo-144"
  #define MCU_STM32F446ZE
  #define EXTERNAL_XTAL_HZ   8000000

#elif defined(BOARD_NUCLEO_G474RE)

  #define BOARD_NAME "STM32G474RE Nucleo-64"
  #define MCU_STM32G474RE
  #define EXTERNAL_XTAL_HZ   24000000

#elif defined(BOARD_NUCLEO_G431KB)

  #define BOARD_NAME "STM32G431KB Nucleo-32"
  #define MCU_STM32G431KB
  // the external crystal is not connected by default
  #define EXTERNAL_XTAL_HZ         0
  //#define EXTERNAL_XTAL_HZ   24000000

#elif defined(BOARD_DISCOVERY_WB5M)

  #define BOARD_NAME "STM32WB5M Discovery"
  #define MCU_STM32WB55VG
  #define EXTERNAL_XTAL_HZ   32000000

#elif defined(BOARD_NUCLEO_F746)

  #define BOARD_NAME "STM32F746 Nucleo-144"
  #define MCU_STM32F746ZG
  #define EXTERNAL_XTAL_HZ   (8000000 | HWCLK_EXTCLK_BYPASS)

#elif defined(BOARD_DISCOVERY_F746)

  #define BOARD_NAME "STM32F746 Discovery"
  #define MCU_STM32F746NG
  #define EXTERNAL_XTAL_HZ   25000000

  #define MAX_CLOCK_SPEED  200000000

#elif defined(BOARD_DISCOVERY_F429)

  #define BOARD_NAME "STM32F429 Discovery"
  #define MCU_STM32F429ZI
  #define EXTERNAL_XTAL_HZ   8000000

#elif defined(BOARD_NUCLEO_H723)

  #define BOARD_NAME "STM32H723 Nucleo-144"
  #define MCU_STM32H723ZG
  #define EXTERNAL_XTAL_HZ   (8000000 | HWCLK_EXTCLK_BYPASS)

#elif defined(BOARD_NUCLEO_H743)

  #define BOARD_NAME "STM32H743 Nucleo-144"
  #define MCU_STM32H743ZI
  #define EXTERNAL_XTAL_HZ   (8000000 | HWCLK_EXTCLK_BYPASS)

#else

  #error "Unknown board."

#endif


#endif /* BOARDS_BUILTIN_H_ */
