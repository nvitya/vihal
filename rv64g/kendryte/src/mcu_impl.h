//  file:     mcu_impl.h (Kendryte)
//  brief:    Kendryte HW Driver list
//  created:  2022-02-01
//  authors:  nvitya

#ifdef HWPINS_H_
  #include "hwpins_kendryte.h"
#endif

#ifdef HWUART_H_
  #include "hwuart_kendryte.h"
#endif

#ifdef HWSPI_H_
  //#include "hwspi_kendryte.h"
#endif

#ifdef HWI2C_H_
  // #include "hwi2c_kendryte.h"
#endif

#ifdef HWDMA_H_
  #define SKIP_UNIMPLEMENTED_WARNING
#endif
