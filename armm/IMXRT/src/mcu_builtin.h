/*
 *  file:     mcu_builtin.h (IMXRT)
 *  brief:    Built-in IMXRT MCU definitions
 *  created:  2018-11-23
 *  authors:  nvitya
*/

#ifndef __MCU_BUILTIN_H
#define __MCU_BUILTIN_H

//----------------------------------------------------------------------
// IMXRT
//----------------------------------------------------------------------

#if defined(MCU_IMXRT1020) || defined(MCU_IMXRT1021) || defined(MCU_IMXRT1024)

  #define MCUF_IMXRT
  #define MCUSF_1020

  #include "MIMXRT1021.h"

#elif defined(MCU_IMXRT1050) || defined(MCU_IMXRT1051) || defined(MCU_IMXRT1052)

  #define MCUF_IMXRT
  #define MCUSF_1050

  #include "MIMXRT1052.h"

#elif defined(MCU_IMXRT1040) || defined(MCU_IMXRT1041) || defined(MCU_IMXRT1042)

  #define MCUF_IMXRT
  #define MCUSF_1040

  #include "MIMXRT1042_vihal.h"

#elif defined(MCU_IMXRT1052A) // old eval board?

  #define MCUF_IMXRT
  #define MCUSF_1050

  #ifndef MCUREV_1050A
    #define MCUREV_1050A
  #endif

  #include "MIMXRT1052A.h"

#else

  #error "Unknown MCU"

#endif

#endif
