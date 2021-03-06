/*
 *  file:     mcu_impl.h (ESP)
 *  brief:    ESP list of implemented VIHAL core peripherals
 *  created:  2022-01-29
 *  authors:  nvitya
*/

#ifdef HWPINS_H_
  #include "hwpins_esp.h"
#endif

#ifdef HWUART_H_
  #include "hwuart_esp.h"
#endif

#ifdef HWSPI_H_
  #include "hwspi_esp.h"
#endif

#ifdef HWDMA_H_
  #define SKIP_UNIMPLEMENTED_WARNING
#endif
