/*
 *  file:     mcu_impl.h (AM)
 *  brief:    AM list of implemented VIHAL core peripherals
 *  created:  2024-06-15
 *  authors:  nvitya
*/

#ifdef HWPINS_H_
  #include "hwpins_am.h"
#endif

#ifdef HWUART_H_
  #include "hwuart_am.h"
#endif

#ifdef HWUSCOUNTER_H_
  #include "hwuscounter_am.h"
#endif

#ifdef HWSPI_H_
  //#include "hwspi_am.h"
#endif

#ifdef HWDMA_H_
  //#include "hwdma_am.h"
#endif

#if defined(HWCAN_H_)
  //#include "hwcan_am.h"
#endif

#ifdef HWADC_H_
  //#include "hwadc_am.h"
#endif

#ifdef HWPWM_H_
  //#include "hwpwm_am.h"
#endif

#define SKIP_UNIMPLEMENTED_WARNING
