/*
 *  file:     mcu_impl.h (bcm)
 *  brief:    BCM list of implemented VIHAL peripherals
 *  date:     2024-07-17
 *  authors:  nvitya
*/

#ifdef HWPINS_H_
  #include "hwpins_bcm.h"
#endif

#ifdef HWUSCOUNTER_H_
  #include "hwuscounter_bcm.h"
#endif

#ifdef HWSPI_H_
  //#include "hwspi_am.h"
#endif

#define SKIP_UNIMPLEMENTED_WARNING
