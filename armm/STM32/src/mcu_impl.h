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

#ifdef HWUSCOUNTER_H_
  #include "hwuscounter_stm32.h"
#endif

#ifdef HWSPI_H_
  #include "hwspi_stm32.h"
#endif

#ifdef HWDMA_H_
  #include "hwdma_stm32.h"
  #include "hwdma_stm32_v2.h"
  #include "hwdma_stm32_v3.h"
#endif

#if defined(HWQSPI_H_)
  #include "hwqspi_stm32.h"
  #include "hwqspi_stm32_xspi.h"
#endif

#ifdef HWI2C_H_
  #include "hwi2c_stm32.h"
#endif

#ifdef HWI2CSLAVE_H_
  #include "hwi2cslave_stm32.h"
#endif

#ifdef HWINTFLASH_H_
  #include "hwintflash_stm32.h"
#endif

#ifdef HWEXTIRQ_H_
  #include "hwextirq_stm32.h"
#endif

#ifdef HWADC_H_
  #include "hwadc_stm32.h"
#endif

#ifdef HWDAC_H_
  #include "hwdac_stm32.h"
#endif

#ifdef HWPWM_H_
  #include "hwpwm_stm32.h"
#endif

#if defined(HWCAN_H_)
  #include "hwcan_stm32.h"
#endif

#ifdef HWETH_H_
  #include "hweth_stm32.h"
  #include "hweth_stm32_v2.h"
#endif

#if (defined(FMC_Bank5_6_R) || defined(FMC_Bank5_6))  && defined(HWSDRAM_H_)
  #include "hwsdram_stm32.h"
#endif

#ifdef HWUSBCTRL_H_
  #include "hwusbctrl_stm32.h"
  #include "hwusbctrl_stm32_otg.h"
#endif

#if defined(LTDC_SRCR_IMR) && defined(HWLCDCTRL_H_)
  #include "hwlcdctrl_stm32.h"
#endif

#if defined(HWSDMMC_H_) && (defined(SDIO) || defined(SDMMC) || defined(SDMMC1))
  #include "hwsdmmc_stm32.h"
#endif
