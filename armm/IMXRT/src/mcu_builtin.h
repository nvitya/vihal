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

#if defined(MCU_IMXRT1020) || defined(MCU_IMXRT1021)

  #define MCUF_IMXRT
  #define MCUSF_1020

  #include "MIMXRT1021.h"

#else

  #error "Unknown MCU"

#endif

#endif
