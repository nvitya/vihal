/*
 *  file:     mcu_impl.h (ATSAM_V2)
 *  brief:    ATSAM_V2 list of implemented VIHAL core peripherals
 *  version:  1.00
 *  date:     2018-02-10
 *  authors:  nvitya
*/

#ifdef HWPINS_H_
  #include "hwpins_atsam_v2.h"
#endif

#ifdef HWUART_H_
  #include "hwuart_atsam_v2.h"
#endif

#ifdef HWDMA_H_
  #include "hwdma_atsam_v2.h"
#endif

#ifdef HWSPI_H_
  #include "hwspi_atsam_v2.h"
#endif

#ifdef HWQSPI_H_
  #include "hwqspi_atsam_v2.h"
#endif

#ifdef HWI2C_H_
  #include "hwi2c_atsam_v2.h"
#endif

#ifdef HWI2CSLAVE_H_
  #include "hwi2cslave_atsam_v2.h"
#endif

#ifdef HWINTFLASH_H_
  #include "hwintflash_atsam_v2.h"
#endif

#ifdef HWEXTIRQ_H_
  #include "hwextirq_atsam_v2.h"
#endif

#ifdef HWCAN_H_
  #include "hwcan_atsam_v2.h"
#endif

/* not migrated yet from NVCM:

#if defined(HWUSBCTRL_H_)
  #include "hwusbctrl_atsam_v2.h"
#endif

*/