/*
 *  file:     mcu_builtin.h (PY32)
 *  brief:    Built-in PY32 MCU definitions
 *  created:  2026-04-20
 *  authors:  nvitya
*/

#ifndef __MCU_BUILTIN_H
#define __MCU_BUILTIN_H

#if 0

//----------------------------------------------------------------------
// PY32
//----------------------------------------------------------------------

// STM32F0: Cortex-M0

#elif defined(MCU_PY32F031)

  #define MCUF_PY32
  #define MCUSF_F0

  #include "py32f031x8.h"

#else

  #error "Unknown PY32 MCU"

#endif

#endif
