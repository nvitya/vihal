//  file:     mcu_impl.h (sg2000)
//  brief:    SG2000 HW Driver list
//  created:  2024-04-21
//  authors:  nvitya

#ifdef HWPINS_H_
  #include "hwpins_sg.h"
#endif

#ifdef HWUSCOUNTER_H_
  #include "hwuscounter_sg.h"
#endif

#ifdef HWUART_H_
  #include "hwuart_sg.h"
#endif

#ifdef HWSPI_H_
  #include "hwspi_sg.h"
#endif

#ifdef HWDMA_H_
  #define SKIP_UNIMPLEMENTED_WARNING
#endif
