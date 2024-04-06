/*
 *  file:     mcu_impl.h (ATSAM)
 *  brief:    ATSAM list of implemented VIHAL core peripherals
 *  date:     2021-10-27
 *  authors:  nvitya
*/

#ifdef HWCLKCTRL_H_
  #include "hwclkctrl_atsam.h"
#endif

#ifdef HWPINS_H_
  #include "hwpins_atsam.h"
#endif

#ifdef HWDMA_H_
  #include "hwdma_atsam.h"
#endif

#ifdef HWUART_H_
  #include "hwuart_atsam.h"
#endif

#ifdef HWUSCOUNTER_H_
  #include "hwuscounter_atsam.h"
#endif

#ifdef HWSPI_H_
  #include "hwspi_atsam.h"
#endif

#ifdef QSPI
	#ifdef HWQSPI_H_
		#include "hwqspi_atsam.h"
	#endif
#endif

#ifdef HWI2C_H_
  #include "hwi2c_atsam.h"
#endif

#ifdef HWI2CSLAVE_H_
  #include "hwi2cslave_atsam.h"
#endif

#ifdef HWINTFLASH_H_
  #include "hwintflash_atsam.h"
#endif

#ifdef HWEXTIRQ_H_
  #include "hwextirq_atsam.h"
#endif

#ifdef HWADC_H_
  #include "hwadc_atsam.h"
#endif

#ifdef HWPWM_H_
  #include "hwpwm_atsam.h"
#endif

#if defined(HWCAN_H_)
  #include "hwcan_atsam.h"
#endif

#if defined(HWETH_H_) && defined(GMAC)
  #include "hweth_atsam.h"
#endif

#if defined(SDRAMC) && defined(HWSDRAM_H_)
  #include "hwsdram_atsam.h"
#endif

#if defined(HWUSBCTRL_H_)
  #include "hwusbctrl_atsam.h"
  #include "hwusbctrl_atsam_hs.h"
#endif

#if defined(HWSDMMC_H_) && defined(HSMCI)
  #include "hwsdmmc_atsam.h"
#endif
