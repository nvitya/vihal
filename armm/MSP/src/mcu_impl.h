/*
 *  file:     mcu_impl.h (MSP)
 *  brief:    MSP list of implemented VIHAL core peripherals
 *  created:  2023-06-27
 *  authors:  nvitya
*/

#ifdef HWPINS_H_
  #include "hwpins_msp.h"
#endif

#ifdef HWUART_H_
  #include "hwuart_msp.h"
#endif

#if defined(HWCAN_H_)
  #include "hwcan_msp.h"
#endif

#ifdef HWADC_H_
  #include "hwadc_msp.h"
#endif

#ifdef HWDMA_H_
  #include "hwdma_msp.h"
#endif

#ifdef HWINTFLASH_H_
  #include "hwintflash_msp.h"
#endif

#define SKIP_UNIMPLEMENTED_WARNING
