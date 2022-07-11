/*
 *  file:     boards_builti.h
 *  brief:    some pre-defined FPGA boards with VRV100 SoC
 *  version:  1.00
 *  date:     2021-10-20
 *  authors:  nvitya
*/

#ifndef BOARDS_BUILTIN_H_
#define BOARDS_BUILTIN_H_

#if 0 // to use elif everywhere

#elif defined(BOARD_VRV100_103)

#define BOARD_NAME "VRV100_103"
#define MCU_VRV100_103
#ifndef MCU_FIXED_SPEED
  #define MCU_FIXED_SPEED  100000000
#endif

#elif defined(BOARD_VRV100_104)

#define BOARD_NAME "VRV100_104"
#define MCU_VRV100_104
#ifndef MCU_FIXED_SPEED
  #define MCU_FIXED_SPEED  100000000
#endif

#elif defined(BOARD_VRV100_241)

#define BOARD_NAME "VRV100_241"
#define MCU_VRV100_241
#ifndef MCU_FIXED_SPEED
  #define MCU_FIXED_SPEED  100000000
#endif

#elif defined(BOARD_VRV100_365)

#define BOARD_NAME "VRV100_365"
#define MCU_VRV100_365
#ifndef MCU_FIXED_SPEED
  #define MCU_FIXED_SPEED  100000000
#endif

#elif defined(BOARD_VRV100_401)

#define BOARD_NAME "VRV100_401"
#define MCU_VRV100_401
#ifndef MCU_FIXED_SPEED
  #define MCU_FIXED_SPEED  100000000
#endif

#elif defined(BOARD_VRV100_441)

#define BOARD_NAME "VRV100_441"
#define MCU_VRV100_441
#ifndef MCU_FIXED_SPEED
  #define MCU_FIXED_SPEED  100000000
#endif

#elif defined(BOARD_VRV100_443)

#define BOARD_NAME "VRV100_443"
#define MCU_VRV100_443
#ifndef MCU_FIXED_SPEED
  #define MCU_FIXED_SPEED  100000000
#endif

#elif defined(BOARD_VRV100_543)

#define BOARD_NAME "VRV100_543"
#define MCU_VRV100_543
#ifndef MCU_FIXED_SPEED
  #define MCU_FIXED_SPEED  100000000
#endif

#else

  #error "Unknown board."

#endif


#endif /* BOARDS_BUILTIN_H_ */
