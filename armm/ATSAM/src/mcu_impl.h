/*
 *  file:     mcu_impl.h (ATSAM)
 *  brief:    ATSAM list of implemented VIHAL core peripherals
 *  version:  1.00
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
