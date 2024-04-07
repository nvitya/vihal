/*
 *  file:     mcu_impl.h (RP)
 *  brief:    RP list of implemented VIHAL core peripherals
 *  date:     2021-12-18
 *  authors:  nvitya
*/

#ifdef HWPINS_H_
  #include "hwpins_rp.h"
#endif

#ifdef HWUART_H_
  #include "hwuart_rp.h"
#endif

#ifdef HWUSCOUNTER_H_
  #include "hwuscounter_rp.h"
#endif

#ifdef HWDMA_H_
  #include "hwdma_rp.h"
#endif

#ifdef HWSPI_H_
  #include "hwspi_rp.h"
#endif

#ifdef HWI2C_H_
  #include "hwi2c_rp.h"
#endif

#ifdef HWQSPI_H_
  #include "hwqspi_rp.h"
#endif

#ifdef HWADC_H_
  #include "hwadc_rp.h"
#endif

#ifdef HWPWM_H_
  #include "hwpwm_rp.h"
#endif

#ifdef HWEXTIRQ_H_
  #include "hwextirq_rp.h"
#endif

#if defined(HWUSBCTRL_H_)
  #include "hwusbctrl_rp.h"
#endif
