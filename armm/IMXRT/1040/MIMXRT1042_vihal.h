/*
 * Slightly modified IMXRT1042 processor include for VIHAL usage
*/

/*
** ###################################################################
**     Processors:          MIMXRT1042DFP6B
**                          MIMXRT1042DJM6B
**                          MIMXRT1042XFP5B
**                          MIMXRT1042XJM5B
**
**     Compilers:           Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**                          Keil ARM C/C++ Compiler
**                          MCUXpresso Compiler
**
**     Reference manual:    IMXRT1040RM Rev.1, 09/2022
**     Version:             rev. 0.1, 2021-07-20
**     Build:               b241021
**
**     Abstract:
**         CMSIS Peripheral Access Layer for MIMXRT1042
**
**     Copyright 1997-2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2024 NXP
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 0.1 (2021-07-20)
**         Initial version.
**
** ###################################################################
*/


#ifndef MIMXRT1042_VIHAL_H_
#define MIMXRT1042_VIHAL_H_

#define CPU_MIMXRT1042XJM5B

#include "MIMXRT1042_COMMON.h"

#include "periph/PERI_ADC.h"
#include "periph/PERI_ADC_ETC.h"
#include "periph/PERI_AIPSTZ.h"
#include "periph/PERI_AOI.h"
#include "periph/PERI_BEE.h"
#include "periph/PERI_CAN.h"
#include "periph/PERI_CCM.h"
#include "periph/PERI_CCM_ANALOG.h"
#include "periph/PERI_CM7_MCM.h"
#include "periph/PERI_CMP.h"
#include "periph/PERI_CSU.h"
#include "periph/PERI_DCDC.h"
#include "periph/PERI_DCP.h"
#include "periph/PERI_DMA.h"
#include "periph/PERI_DMAMUX.h"
#include "periph/PERI_ENC.h"
#include "periph/PERI_ENET.h"
#include "periph/PERI_EWM.h"
#include "periph/PERI_FLEXIO.h"
#include "periph/PERI_FLEXRAM.h"
#include "periph/PERI_FLEXSPI.h"
#include "periph/PERI_GPC.h"
#include "periph/PERI_GPIO.h"
#include "periph/PERI_GPT.h"
#include "periph/PERI_I2S.h"
#include "periph/PERI_IOMUXC.h"
#include "periph/PERI_IOMUXC_GPR.h"
#include "periph/PERI_IOMUXC_SNVS.h"
#include "periph/PERI_IOMUXC_SNVS_GPR.h"
#include "periph/PERI_LCDIF.h"
#include "periph/PERI_LPI2C.h"
#include "periph/PERI_LPSPI.h"
#include "periph/PERI_LPUART.h"
#include "periph/PERI_OCOTP.h"
#include "periph/PERI_PGC.h"
#include "periph/PERI_PIT.h"
#include "periph/PERI_PMU.h"
#include "periph/PERI_PWM.h"
#include "periph/PERI_PXP.h"
#include "periph/PERI_RTWDOG.h"
#include "periph/PERI_SEMC.h"
#include "periph/PERI_SNVS.h"
#include "periph/PERI_SPDIF.h"
#include "periph/PERI_SRC.h"
#include "periph/PERI_TEMPMON.h"
#include "periph/PERI_TMR.h"
#include "periph/PERI_TRNG.h"
#include "periph/PERI_USB.h"
#include "periph/PERI_USBNC.h"
#include "periph/PERI_USBPHY.h"
#include "periph/PERI_USB_ANALOG.h"
#include "periph/PERI_USDHC.h"
#include "periph/PERI_WDOG.h"
#include "periph/PERI_XBARA.h"
#include "periph/PERI_XBARB.h"
#include "periph/PERI_XTALOSC24M.h"

#endif  /* #ifndef MIMXRT1042_VIHAL_H_ */
