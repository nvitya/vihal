// file:     platform.h (ARMR)
// brief:    Main ARM Cortex-R platform include
// created:  2024-06-14
// authors:  nvitya

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "generic_defs.h"
#include "armr_cpu.h"

// board.h must be provided by the application. It should minimum define the cpu.
#include "board.h"

#if defined(MCU_DEFINED)

  // in the board.h the MCU also can be defined

#else

  #include "mcu_builtin.h"

#endif

#ifndef LITTLE_ENDIAN
	#define LITTLE_ENDIAN
#endif

//------------------------------------------------------------------

// the mcu_defs.h comes from the MCUF directory

#include "mcu_defs.h"


#ifndef CLOCKCNT
  #define CLOCKCNT  cpu_mr_read_pmccntr()
#endif

#include "armr_generic.h"

#include "platform_generic.h"

#endif // _PLATFORM_H_
