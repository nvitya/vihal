/*
 *  file:     mcu_impl.h (ESP32)
 *  brief:    ESP32 list of implemented VIHAL core peripherals
 *  created:  2022-02-03
 *  authors:  nvitya
*/

#ifdef HWPINS_H_
  #include "hwpins_esp.h"
#endif

#ifdef HWUART_H_
  #include "hwuart_esp.h"
#endif

#ifdef HWDMA_H_
  #include "hwdma_esp.h"
#endif

#ifdef HWSPI_H_
  #include "hwspi_esp.h"
#endif

#ifdef HWQSPI_H_
  #include "hwqspi_esp.h"
#endif

#ifdef HWI2C_H_
  #include "hwi2c_esp.h"
#endif

#ifdef HWADC_H_
  #include "hwadc_esp.h"
#endif
