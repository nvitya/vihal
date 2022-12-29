// file:     platform.h (rv32i)
// brief:    Main RV32i platform include
// created:  2021-10-02
// authors:  nvitya

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "generic_defs.h"
#include "rv32i_cpu.h"

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
  // read mtime by default:
  #define CLOCKCNT  (cpu_csr_read(0xB00))
#endif

#include "rv32i_generic.h"

#include "platform_generic.h"

#endif // _PLATFORM_H_
