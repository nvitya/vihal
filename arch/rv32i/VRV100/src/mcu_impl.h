/*
 *  file:     mcu_impl.h (VRV100)
 *  brief:    VRV100 HW Driver list
 *  version:  1.00
 *  date:     2021-10-02
 *  authors:  nvitya
*/

#ifdef HWCLKCTRL_H_
  // #include "hwclkctrl_.h"
#endif

#ifdef HWPINS_H_
  #include "hwpins_vrv100.h"
#endif

#ifdef HWUART_H_
  #include "hwuart_vrv100.h"
#endif

#ifdef HWSPI_H_
  // #include "hwspi_vrv100.h"
#endif

#ifdef HWI2C_H_
  // #include "hwi2c_.h"
#endif

#ifdef HWDMA_H_
  #define SKIP_UNIMPLEMENTED_WARNING
#endif
