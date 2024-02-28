/*
 *  file:     mcu_builtin.h (VRV100)
 *  brief:    Built-In VRV100 (VexRiscV FPGA) Variants
 *  version:  1.00
 *  date:     2021-10-02
 *  authors:  nvitya
*/

#ifndef __MCU_BUILTIN_H
#define __MCU_BUILTIN_H

#if 0

#elif defined(MCU_VRV100_104) || defined(MCU_VRV100_103) || defined(MCU_VRV100_241) \
  || defined(MCU_VRV100_365) \
  || defined(MCU_VRV100_403) || defined(MCU_VRV100_441) || defined(MCU_VRV100_443) || defined(MCU_VRV100_456) \
  || defined(MCU_VRV100_543)

  #define MCUF_VRV100

  #include "VRV100.h"

#else

  #error "Unknown MCU"

#endif

#endif
