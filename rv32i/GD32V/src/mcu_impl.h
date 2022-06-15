//  file:     mcu_impl.h (GD32V)
//  brief:    GD32V HW Driver list
//  created:  2021-10-20
//  authors:  nvitya

#ifdef HWPINS_H_
  #include "hwpins_gd32v.h"
#endif

#ifdef HWUART_H_
  #include "hwuart_gd32v.h"
#endif

#ifdef HWDMA_H_
  #include "hwdma_gd32v.h"
#endif

#ifdef HWSPI_H_
  //#include "hwspi_gd32v.h"
#endif

#ifdef HWI2C_H_
  // #include "hwi2c_gd32v.h"
#endif

#ifdef HWDMA_H_
  #define SKIP_UNIMPLEMENTED_WARNING
#endif
