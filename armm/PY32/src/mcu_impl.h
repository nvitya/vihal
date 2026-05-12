/*
 *  file:     mcu_impl.h (PY32)
 *  brief:    PY32 HW Driver list
 *  created:  2026-04-20
 *  authors:  nvitya
*/

#ifdef HWPINS_H_
  #include "hwpins_py32.h"
#endif

#ifdef HWUART_H_
  #include "hwuart_py32.h"
#endif

#ifdef HWUSCOUNTER_H_
  #include "hwuscounter_py32.h"
#endif

#ifdef HWSPI_H_
  //#include "hwspi_py32.h"
#endif

#ifdef HWDMA_H_
  #include "hwdma_py32.h"
#endif

#ifdef HWADC_H_
  #include "hwadc_py32.h"
#endif

#ifdef HWPWM_H_
  #include "hwpwm_py32.h"
#endif

#ifdef HWI2CSLAVE_H_
  #include "hwi2cslave_py32.h"
#endif

#ifdef HWINTFLASH_H_
  #include "hwintflash_py32.h"
#endif

#define SKIP_UNIMPLEMENTED_WARNING
