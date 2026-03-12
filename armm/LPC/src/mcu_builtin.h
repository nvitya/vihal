
#ifndef __MCU_BUILTIN_H
#define __MCU_BUILTIN_H

#if defined(MCU_LPC1768) 

  #define MCUF_LPC
  #define MCUSF_17XX

  #include "LPC17xx.h"

#else

  #error "Unknown MCU"

#endif

#endif
