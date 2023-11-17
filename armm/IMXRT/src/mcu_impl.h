/*
 *  file:     mcu_impl.h (IMXRT)
 *  brief:    IMXRT list of implemented NVCM core peripherals
 *  created:  2018-11-23
 *  authors:  nvitya
*/

#ifdef HWCLKCTRL_H_
  #include "hwclkctrl_imxrt.h"
#endif

#ifdef HWPINS_H_
  #include "hwpins_imxrt.h"
#endif

#ifdef HWUART_H_
  #include "hwuart_imxrt.h"
#endif

#ifdef HWSPI_H_
  #include "hwspi_imxrt.h"
#endif

#ifdef HWI2C_H_
  #include "hwi2c_imxrt.h"
#endif

#ifdef HWQSPI_H_
  #include "hwqspi_imxrt.h"
#endif

#ifdef HWDMA_H_
  #include "hwdma_imxrt.h"
#endif

#if defined(HWSDRAM_H_)
  #include "hwsdram_imxrt.h"
#endif

#ifdef HWETH_H_
  #include "hweth_imxrt.h"
#endif
