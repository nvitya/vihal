/*
 *  file:     mcu_builtin.h (LPC_V3)
 *  brief:    Built-in LPC_V3 MCU definitions
 *  version:  1.00
 *  date:     2018-02-10
 *  authors:  nvitya
*/

#ifndef __MCU_BUILTIN_H
#define __MCU_BUILTIN_H

#if 0

//----------------------------------------------------------------------
// NXP
//----------------------------------------------------------------------

#elif defined(MCU_LPC54606) || defined(MCU_LPC54608)

  #define MCUF_LPC_V3
  #define MCUSF_546XX

  #include "LPC54608.h"

#elif defined(MCU_LPC54016)

  #define MCUF_LPC_V3
  #define MCUSF_540XX

  #include "LPC54016.h"

#elif defined(MCU_LPC54018)

  #define MCUF_LPC_V3
  #define MCUSF_540XX

  #include "LPC54018.h"

#elif defined(MCU_LPC55S28)

  #define MCUF_LPC_V3
  #define MCUSF_552X

  #define MAX_CLOCK_SPEED  150000000

  #include "LPC55S28.h"

#else

  #error "Unknown MCU"

#endif

#endif
