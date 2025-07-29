
#ifndef __RK3506_VIHAL_H
#define __RK3506_VIHAL_H

#ifdef __cplusplus
  extern "C" {
#endif

#define __CM0_REV                 0
#define __MPU_PRESENT             0
#define __NVIC_PRIO_BITS          2
#define __Vendor_SysTickConfig    0

#define INTMUX_NUM_INT_PER_CON    128
#define INTMUX_NUM_OUT_PER_CON    4
#define INTMUX_NUM_INT_PER_OUT    32
#define INTMUX_NUM_GROUP_PER_OUT  1
#define INTMUX_NUM_GROUP_PER_CON  4
#define INTMUX_NUM_INT_PER_GROUP  32
/* INTMUX IRQ start from GIC SPI(Shared Peripheral Interrupt) */
#define INTMUX_IRQ_START_NUM      0
#define INTMUX_IRQ_OUT0
#define INTMUX_IRQ_OUT1
#define INTMUX_IRQ_OUT2
#define INTMUX_IRQ_OUT3
#define INTMUX_OUT_IRQ_START_NUM  28

#define NUM_EXT_INTERRUPTS        128

#ifndef __ASSEMBLY__
typedef enum {
/* -------------------  Processor Exceptions Numbers  ----------------------------- */
  NonMaskableInt_IRQn       = -14,     /*  2 Non Maskable Interrupt */
  HardFault_IRQn            = -13,     /*  3 HardFault Interrupt */

  SVCall_IRQn               =  -5,     /* 11 SV Call Interrupt */

  PendSV_IRQn               =  -2,     /* 14 Pend SV Interrupt */
  SysTick_IRQn              =  -1,     /* 15 System Tick Interrupt */

/******  Platform Exceptions Numbers ***************************************************/
  GPIO0_1_IRQn              =  0,      /*!< GPIO0_1 Interrupt         */
  GPIO0_2_IRQn              =  1,      /*!< GPIO0_2 Interrupt         */
  GPIO0_3_IRQn              =  2,      /*!< GPIO0_3 Interrupt         */
  GPIO1_1_IRQn              =  3,      /*!< GPIO1_1 Interrupt in pd_core which response faster than GPIO1_SHADOW_1*/
  GPIO1_SHADOW_1_IRQn       =  3,      /*!< GPIO1_SHADOW_1 Interrupt in pd_pmu which response slower than GPIO1_1 */
  GPIO1_2_IRQn              =  4,      /*!< GPIO1_2 Interrupt in pd_core which response faster than GPIO1_SHADOW_2*/
  GPIO1_SHADOW_2_IRQn       =  4,      /*!< GPIO1_SHADOW_2 Interrupt in pd_pmu which response slower than GPIO1_2 */
  GPIO1_3_IRQn              =  5,      /*!< GPIO1_3 Interrupt in pd_core which response faster than GPIO1_SHADOW_3*/
  GPIO1_SHADOW_3_IRQn       =  5,      /*!< GPIO1_SHADOW_3 Interrupt in pd_pmu which response slower than GPIO1_3 */
  GPIO2_3_IRQn              =  6,      /*!< GPIO2_3 Interrupt         */
  GPIO3_3_IRQn              =  7,      /*!< GPIO3_3 Interrupt         */
  TOUCH_KEY_POS_IRQn        =  8,      /*!< TOUCH_KEY_POS Interrupt   */
  TOUCH_KEY_NEG_IRQn        =  9,      /*!< TOUCH_KEY_NEG Interrupt   */
  PWM1_CH6_IRQn             =  10,     /*!< PWM1_CH6 Interrupt        */
  PWM1_CH7_IRQn             =  11,     /*!< PWM1_CH7 Interrupt        */
  UART3_IRQn                =  12,     /*!< UART3 Interrupt           */
  UART4_IRQn                =  13,     /*!< UART4 Interrupt           */
  I2C2_IRQn                 =  14,     /*!< I2C2 Interrupt            */
  SPI1_IRQn                 =  15,     /*!< SPI1 Interrupt            */
  SARADC_IRQn               =  16,     /*!< SARADC Interrupt          */
  SDMMC_IRQn                =  17,     /*!< SDMMC Interrupt           */
  TIMER4_IRQn               =  18,     /*!< TIMER0_4 Interrupt        */
  TIMER5_IRQn               =  19,     /*!< TIMER0_5 Interrupt        */
  TIMER10_IRQn              =  20,     /*!< TIMER1_4 Interrupt        */
  TIMER11_IRQn              =  21,     /*!< TIMER1_5 and DSMC use the same Interrupt ID */
  DSMC_IRQn                 =  21,     /*!< DSMC and TIMER1_5 use the same Interrupt ID */
  MAILBOX_8MUX1_IRQn        =  22,     /*!< MAILBOX_8MUX1 Interrupt   */
  DMAC0_IRQn                =  23,     /*!< DMAC0 Interrupt           */
  DMAC0_ABORT_IRQn          =  24,     /*!< DMAC0_ABORT Interrupt     */
  DMAC1_IRQn                =  25,     /*!< DMAC1 Interrupt           */
  DMAC1_ABORT_IRQn          =  26,     /*!< DMAC1_ABORT Interrupt     */
  FLEXBUS_IRQn              =  27,     /*!< FLEXBUS Interrupt         */
  INTMUX_OUT0_IRQn          =  28,     /*!< INTMUX0 Interrupt         */
  INTMUX_OUT1_IRQn          =  29,     /*!< INTMUX1 Interrupt         */
  INTMUX_OUT2_IRQn          =  30,     /*!< INTMUX2 Interrupt         */
  INTMUX_OUT3_IRQn          =  31,     /*!< INTMUX3 Interrupt         */
  NUM_INTERRUPTS            =  32,     /*!< Number of internal IRQ    */
  GPIO0_0_IRQn              =  0 + NUM_INTERRUPTS,
  GPIO1_0_IRQn              =  1 + NUM_INTERRUPTS,
  GPIO2_0_IRQn              =  2 + NUM_INTERRUPTS,
  GPIO2_1_IRQn              =  3 + NUM_INTERRUPTS,
  GPIO2_2_IRQn              =  4 + NUM_INTERRUPTS,
  GPIO3_0_IRQn              =  5 + NUM_INTERRUPTS,
  GPIO3_1_IRQn              =  6 + NUM_INTERRUPTS,
  GPIO3_2_IRQn              =  7 + NUM_INTERRUPTS,
  GPIO4_0_IRQn              =  8 + NUM_INTERRUPTS,
  GPIO4_1_IRQn              =  9 + NUM_INTERRUPTS,
  GPIO4_2_IRQn              =  10 + NUM_INTERRUPTS,
  GPIO4_3_IRQn              =  11 + NUM_INTERRUPTS,
  PWM0_CH0_IRQn             =  12 + NUM_INTERRUPTS,
  PWM0_CH1_IRQn             =  13 + NUM_INTERRUPTS,
  PWM0_CH2_IRQn             =  14 + NUM_INTERRUPTS,
  PWM0_CH3_IRQn             =  15 + NUM_INTERRUPTS,
  PWM1_CH0_IRQn             =  16 + NUM_INTERRUPTS,
  PWM1_CH1_IRQn             =  17 + NUM_INTERRUPTS,
  PWM1_CH2_IRQn             =  18 + NUM_INTERRUPTS,
  PWM1_CH3_IRQn             =  19 + NUM_INTERRUPTS,
  PWM1_CH4_IRQn             =  20 + NUM_INTERRUPTS,
  PWM1_CH5_IRQn             =  21 + NUM_INTERRUPTS,
  UART0_IRQn                =  22 + NUM_INTERRUPTS,
  UART1_IRQn                =  23 + NUM_INTERRUPTS,
  UART2_IRQn                =  24 + NUM_INTERRUPTS,
  UART5_IRQn                =  25 + NUM_INTERRUPTS,
  I2C0_IRQn                 =  26 + NUM_INTERRUPTS,
  I2C1_IRQn                 =  27 + NUM_INTERRUPTS,
  SPI0_IRQn                 =  28 + NUM_INTERRUPTS,
  CAN0_IRQn                 =  29 + NUM_INTERRUPTS,
  CAN1_IRQn                 =  30 + NUM_INTERRUPTS,
  SPDIF_TX_IRQn             =  31 + NUM_INTERRUPTS,
  SPDIF_RX_IRQn             =  32 + NUM_INTERRUPTS,
  PDM_IRQn                  =  33 + NUM_INTERRUPTS,
  SAI0_IRQn                 =  34 + NUM_INTERRUPTS,
  SAI1_IRQn                 =  35 + NUM_INTERRUPTS,
  SAI2_IRQn                 =  36 + NUM_INTERRUPTS,
  SAI3_IRQn                 =  37 + NUM_INTERRUPTS,
  SAI4_IRQn                 =  38 + NUM_INTERRUPTS,
  ASRC0_IRQn                =  39 + NUM_INTERRUPTS,
  ASRC1_IRQn                =  40 + NUM_INTERRUPTS,
  TSADC_IRQn                =  41 + NUM_INTERRUPTS,
  VOP_IRQn                  =  42 + NUM_INTERRUPTS,
  MIPI_DSIHOST_IRQn         =  43 + NUM_INTERRUPTS,
  RGA_IRQn                  =  44 + NUM_INTERRUPTS,
  OTPC_NS_IRQn              =  45 + NUM_INTERRUPTS,
  OTPC_S_IRQn               =  46 + NUM_INTERRUPTS,
  KEY_READER_IRQn           =  47 + NUM_INTERRUPTS,
  OTPC_MASK_IRQn            =  48 + NUM_INTERRUPTS,
  MAC0_SBD_IRQn             =  49 + NUM_INTERRUPTS,
  MAC0_SBD_TX_IRQn          =  50 + NUM_INTERRUPTS,
  MAC0_SBD_RX_IRQn          =  51 + NUM_INTERRUPTS,
  MAC0_PMT_IRQn             =  52 + NUM_INTERRUPTS,
  MAC1_SBD_IRQn             =  53 + NUM_INTERRUPTS,
  MAC1_SBD_TX_IRQn          =  54 + NUM_INTERRUPTS,
  MAC1_SBD_RX_IRQn          =  55 + NUM_INTERRUPTS,
  MAC1_PMT_IRQn             =  56 + NUM_INTERRUPTS,
  OTG0_IRQn                 =  57 + NUM_INTERRUPTS,
  OTG0_BVALID_IRQn          =  58 + NUM_INTERRUPTS,
  OTG0_ID_IRQn              =  59 + NUM_INTERRUPTS,
  OTG0_LINESTATE_IRQn       =  60 + NUM_INTERRUPTS,
  OTG0_DISCONNECT_IRQn      =  61 + NUM_INTERRUPTS,
  OTG1_IRQn                 =  62 + NUM_INTERRUPTS,
  OTG1_BVALID_IRQn          =  63 + NUM_INTERRUPTS,
  OTG1_ID_IRQn              =  64 + NUM_INTERRUPTS,
  OTG1_LINESTATE_IRQn       =  65 + NUM_INTERRUPTS,
  OTG1_DISCONNECT_IRQn      =  66 + NUM_INTERRUPTS,
  SPI2APB_IRQn              =  67 + NUM_INTERRUPTS,
  FSPI_IRQn                 =  68 + NUM_INTERRUPTS,
  DDRC_AWPOISON0_IRQn       =  69 + NUM_INTERRUPTS,
  DDRC_AWPOISON1_IRQn       =  70 + NUM_INTERRUPTS,
  DDRC_ARPOISON0_IRQn       =  71 + NUM_INTERRUPTS,
  DDRC_ARPOISON1_IRQn       =  72 + NUM_INTERRUPTS,
  DDRC_DFI_ALERT_ERR_IRQn   =  73 + NUM_INTERRUPTS,
  DDR_MONITOR_IRQn          =  74 + NUM_INTERRUPTS,
  DDRPHY_IRQn               =  75 + NUM_INTERRUPTS,
  TIMER0_IRQn               =  76 + NUM_INTERRUPTS,
  TIMER1_IRQn               =  77 + NUM_INTERRUPTS,
  TIMER2_IRQn               =  78 + NUM_INTERRUPTS,
  TIMER3_IRQn               =  79 + NUM_INTERRUPTS,
  TIMER6_IRQn               =  80 + NUM_INTERRUPTS,
  TIMER7_IRQn               =  81 + NUM_INTERRUPTS,
  TIMER8_IRQn               =  82 + NUM_INTERRUPTS,
  TIMER9_IRQn               =  83 + NUM_INTERRUPTS,
  HPTIMER_IRQn              =  84 + NUM_INTERRUPTS,
  WDT0_IRQn                 =  85 + NUM_INTERRUPTS,
  WDT1_IRQn                 =  86 + NUM_INTERRUPTS,
  Reserved_IRQn             =  87 + NUM_INTERRUPTS,
  CRYPTO_IRQn               =  88 + NUM_INTERRUPTS,
  CRYPTO_KLAD_IRQn          =  89 + NUM_INTERRUPTS,
  CRYPTO_SC_IRQn            =  90 + NUM_INTERRUPTS,
  NS_TRNG_IRQn              =  91 + NUM_INTERRUPTS,
  S_TRNG_IRQn               =  92 + NUM_INTERRUPTS,
  DSMC_SLAVE_IRQn           =  93 + NUM_INTERRUPTS,
  A7_PMUIRQ_0_IRQn          =  94 + NUM_INTERRUPTS,
  A7_PMUIRQ_1_IRQn          =  95 + NUM_INTERRUPTS,
  A7_PMUIRQ_2_IRQn          =  96 + NUM_INTERRUPTS,
  A7_PMUIRQ_3_IRQn          =  97 + NUM_INTERRUPTS,
  A7_AXIERRIRQ_IRQn         =  98 + NUM_INTERRUPTS,
  TIMER1_5_IRQn             =  99 + NUM_INTERRUPTS,      /*!< TIMER1_5 and DSMC use the same Interrupt ID */
  PMU_IRQn                  =  100 + NUM_INTERRUPTS,
  NPOR_POWERGOOD_IRQn       =  101 + NUM_INTERRUPTS,
  GPIO1_SHADOW_0_IRQn       =  102 + NUM_INTERRUPTS,
  OTG0_VBUSVALID_IRQn       =  106 + NUM_INTERRUPTS,
  OTG1_VBUSVALID_IRQn       =  107 + NUM_INTERRUPTS,
  VOP_LB_INTR_IRQn          =  108 + NUM_INTERRUPTS,
  DMA2DDR_IRQn              =  109 + NUM_INTERRUPTS,
  MAILBOX_AP_0_IRQn         =  110 + NUM_INTERRUPTS,
  MAILBOX_AP_1_IRQn         =  111 + NUM_INTERRUPTS,
  MAILBOX_AP_2_IRQn         =  112 + NUM_INTERRUPTS,
  MAILBOX_AP_3_IRQn         =  113 + NUM_INTERRUPTS,
  MAILBOX_BB_0_IRQn         =  114 + NUM_INTERRUPTS,
  MAILBOX_BB_1_IRQn         =  115 + NUM_INTERRUPTS,
  MAILBOX_BB_2_IRQn         =  116 + NUM_INTERRUPTS,
  MAILBOX_BB_3_IRQn         =  117 + NUM_INTERRUPTS,
  MAC0_MCGR_REQ_IRQn        =  118 + NUM_INTERRUPTS,
  MAC1_MCGR_REQ_IRQn        =  119 + NUM_INTERRUPTS,
  DMAC0_MASK_IRQ0_IRQn      =  120 + NUM_INTERRUPTS,
  DMAC0_MASK_IRQ1_IRQn      =  121 + NUM_INTERRUPTS,
  DMAC0_MASK_IRQ2_IRQn      =  122 + NUM_INTERRUPTS,
  DMAC1_MASK_IRQ0_IRQn      =  123 + NUM_INTERRUPTS,
  DMAC1_MASK_IRQ1_IRQn      =  124 + NUM_INTERRUPTS,
  DMAC1_MASK_IRQ2_IRQn      =  125 + NUM_INTERRUPTS,
  TOTAL_INTERRUPTS          =  (INTMUX_IRQ_START_NUM + NUM_INTERRUPTS + NUM_EXT_INTERRUPTS),
} IRQn_Type;
#endif /* __ASSEMBLY__ */

#include "core_cm0.h"            /* Cortex-M0 processor and core peripherals */
#include <stdint.h>

#include "rk3506.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __RK3506_VIHAL_H */
