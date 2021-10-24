/*
 *  file:     mcu_impl.h (STM32)
 *  brief:    STM32 HW Driver list
 *  created:  2021-10-23
 *  authors:  nvitya
*/

#ifdef HWPINS_H_
  #include "hwpins_stm32.h"
#endif

#ifdef HWUART_H_
  #include "hwuart_stm32.h"
#endif

#ifdef HWSPI_H_
  #include "hwspi_stm32.h"
#endif

#ifdef HWDMA_H_
  #define SKIP_UNIMPLEMENTED_WARNING

  #include "hwdma_stm32.h"
  //#include "hwdma_stm32_mxb.h"
#endif

