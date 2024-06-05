/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2021 Viktor Nagy, nvitya
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 * --------------------------------------------------------------------------- */
/*
 *  file:     sg2002_vihal.h
 *  brief:    own SG2002 definitions, some definition copied from the Sophgo SDK
 *  created:  2024-04-21
 *  authors:  nvitya
*/

// Sophgo SDK Copyright:
/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
*/

#ifndef _SG2002_VIHAL_H
#define _SG2002_VIHAL_H

#include <cstddef>
#include "stdint.h"
#include "stdint.h"

//------------------------------------------------------------------------------
// IRQ MAP
//------------------------------------------------------------------------------
#define IRQNUM_SDMA      25
#define IRQNUM_I2S0      26
#define IRQNUM_I2S1      27
#define IRQNUM_I2S2      28
#define IRQNUM_I2S3      29
#define IRQNUM_UART0     30
#define IRQNUM_UART1     31
#define IRQNUM_I2C0      32
#define IRQNUM_I2C1      33
#define IRQNUM_I2C2      34
#define IRQNUM_I2C3      35
#define IRQNUM_I2C4      36
#define IRQNUM_SPI1      37
#define IRQNUM_SPI2      38
#define IRQNUM_WDG2      39
#define IRQNUM_KEYSCAN   40
#define IRQNUM_GPIO0     41
#define IRQNUM_GPIO1     42
#define IRQNUM_GPIO2     43
#define IRQNUM_GPIO3     44
#define IRQNUM_WIEGAND0  45
#define IRQNUM_PWR_MBOX  46
#define IRQNUM_PWR_IRRX  47
#define IRQNUM_PWR_GPIO  48
#define IRQNUM_PWR_UART  49
#define IRQNUM_PWR_I2C   50
#define IRQNUM_PWR_WDG   51
#define IRQNUM_TDMA      52
#define IRQNUM_TIMER4    55
#define IRQNUM_TIMER5    56
#define IRQNUM_TIMER6    57
#define IRQNUM_TIMER7    58
#define IRQNUM_MBOX      61

//------------------------------------------------------------------------------
// DMA REQUEST MAP
//------------------------------------------------------------------------------

//                       RQ
#define DMARQ_I2S0_RX     0
#define DMARQ_I2S0_TX     1
#define DMARQ_I2S1_RX     2
#define DMARQ_I2S1_TX     3
#define DMARQ_I2S2_RX     4
#define DMARQ_I2S2_TX     5
#define DMARQ_I2S3_RX     6
#define DMARQ_I2S3_TX     7
#define DMARQ_UART0_TX    8
#define DMARQ_UART0_RX    9
#define DMARQ_UART1_TX   10
#define DMARQ_UART1_RX   11
#define DMARQ_UART2_TX   12
#define DMARQ_UART2_RX   13
#define DMARQ_UART3_TX   14
#define DMARQ_UART3_RX   15
#define DMARQ_SPI0_RX    16
#define DMARQ_SPI0_TX    17
#define DMARQ_SPI1_RX    18
#define DMARQ_SPI1_TX    19
#define DMARQ_SPI2_RX    20
#define DMARQ_SPI2_TX    21
#define DMARQ_SPI3_RX    22
#define DMARQ_SPI3_TX    23
#define DMARQ_I2C0_RX    24
#define DMARQ_I2C0_TX    25
#define DMARQ_I2C1_RX    26
#define DMARQ_I2C1_TX    27
#define DMARQ_I2C2_RX    28
#define DMARQ_I2C2_TX    29
#define DMARQ_I2C3_RX    30
#define DMARQ_I2C3_TX    31
#define DMARQ_I2C4_RX    32
#define DMARQ_I2C4_TX    33
#define DMARQ_TDM0_RX    34
#define DMARQ_TDM0_TX    35
#define DMARQ_TDM1_RX    36
#define DMARQ_AUDSRC     37
#define DMARQ_SPI_NAND   38
#define DMARQ_SPI_NOR    39
#define DMARQ_UART4_TX   40
#define DMARQ_UART4_RX   41
#define DMARQ_SPI_NOR1   42

//-----------------------------------------------------------------------------
// PAD Definitions
//-----------------------------------------------------------------------------

//                          FMUX      IOBLK     GPIO           QFN-88
#define PAD_AUD_AINL_MIC   0x1BC,  0,           (0x200 + 23)   //  2
#define PAD_AUD_AOUTR      0x1C8,  0,           (0x200 + 24)   //  4
#define PAD_SD0_CLK        0x01C,  0x03001A00,  (0x000 +  7)   //  6
#define PAD_SD0_CMD        0x020,  0x03001A04,  (0x000 +  8)   //  7
#define PAD_SD0_D0         0x024,  0x03001A08,  (0x000 +  9)   //  8
#define PAD_SD0_D1         0x028,  0x03001A0C,  (0x000 + 10)   // 10
#define PAD_SD0_D2         0x02C,  0x03001A10,  (0x000 + 11)   // 11
#define PAD_SD0_D3         0x030,  0x03001A14,  (0x000 + 12)   // 12
#define PAD_SD0_CD         0x034,  0x03001900,  (0x000 + 13)   // 14
#define PAD_SD0_PWR_EN     0x038,  0x03001904,  (0x000 + 14)   // 15
#define PAD_SPK_EN         0x03C,  0x03001908,  (0x000 + 15)   // 17
#define PAD_UART0_TX       0x040,  0x0300190C,  (0x000 + 16)   // 18
#define PAD_UART0_RX       0x044,  0x03001910,  (0x000 + 17)   // 19
#define PAD_EMMC_DAT2      0x04C,  0x03001918,  (0x000 + 26)   // 20
#define PAD_EMMC_CLK       0x050,  0x0300191C,  (0x000 + 22)   // 21
#define PAD_EMMC_DAT0      0x054,  0x03001920,  (0x000 + 25)   // 22
#define PAD_EMMC_DAT3      0x058,  0x03001924,  (0x000 + 27)   // 23
#define PAD_EMMC_CMD       0x05C,  0x03001928,  (0x000 + 23)   // 24
#define PAD_EMMC_DAT1      0x060,  0x0300192C,  (0x000 + 24)   // 25
#define PAD_JTAG_CPU_TMS   0x064,  0x03001930,  (0x000 + 19)   // 26
#define PAD_JTAG_CPU_TCK   0x068,  0x03001934,  (0x000 + 18)   // 27
#define PAD_IIC0_SCL       0x070,  0x0300193C,  (0x000 + 28)   // 28
#define PAD_IIC0_SDA       0x074,  0x03001940,  (0x000 + 29)   // 29
#define PAD_JTAG_CPU_TDI   PAD_IIC0_SCL
#define PAD_JTAG_CPU_TDO   PAD_IIC0_SDA
#define PAD_AUX0           0x078,  0x03001944,  (0x000 + 30)   // 30
#define PAD_GPIO_ZQ        0x1D0,  0x050270E0,  (0x400 + 24)   // 35
#define PAD_PWR_VBAT_DET   0x07C,  0x05027000,  0              // 38
#define PAD_PWR_RSTN       0x080,  0x05027004,  0              // 39
#define PAD_PWR_SEQ1       0x084,  0x05027008,  (0x400 +  3)   // 40
#define PAD_PWR_SEQ2       0x088,  0x05027010,  (0x400 +  4)   // 41
#define PAD_PWR_WAKEUP0    0x090,  0x05027018,  (0x400 +  6)   // 43
#define PAD_PWR_BUTTON1    0x098,  0x05027020,  (0x400 +  8)   // 44
#define PAD_XTAL_XIN       0x0A0,  0x05027020,  0              // 45
#define PAD_PWR_GPIO0      0x0A4,  0x0502702C,  (0x400 +  0)   // 47
#define PAD_PWR_GPIO1      0x0A8,  0x05027030,  (0x400 +  1)   // 48
#define PAD_PWR_GPIO2      0x0AC,  0x05027034,  (0x400 +  2)   // 49
#define PAD_SD1_D3         0x0D0,  0x05027058,  (0x400 + 18)   // 51
#define PAD_SD1_D2         0x0D4,  0x0502705C,  (0x400 + 19)   // 52
#define PAD_SD1_D1         0x0D8,  0x05027060,  (0x400 + 20)   // 53
#define PAD_SD1_D0         0x0DC,  0x05027064,  (0x400 + 21)   // 54
#define PAD_SD1_CMD        0x0E0,  0x05027068,  (0x400 + 22)   // 55
#define PAD_SD1_CLK        0x0E4,  0x0502706C,  (0x400 + 23)   // 56
#define PAD_PWM0_BUCK      0x0EC,  0x03001804,  (0x100 +  0)   // 58
#define PAD_ADC1           0x0F8,  0x03001804,  (0x100 +  3)   // 59
#define PAD_USB_VBUS_DET   0x108,  0x03001808,  (0x100 +  6)   // 60
#define PAD_ETH_RXN        0x124,  0,           0              // 62
#define PAD_ETH_RXP        0x128,  0,           0              // 63
#define PAD_ETH_TXN        0x12C,  0,           0              // 64
#define PAD_ETH_TXP        0x130,  0,           0              // 65
#define PAD_GPIO_RTX       0x1CC,  0x03001C8C,  (0x100 + 23)   // 67
#define PAD_MIPIRX4N       0x16C,  0x03001C38,  (0x200 +  2)   // 72
#define PAD_MIPIRX4P       0x170,  0x03001C3C,  (0x200 +  3)   // 73
#define PAD_MIPIRX3N       0x174,  0x03001C40,  (0x200 +  4)   // 74
#define PAD_MIPIRX3P       0x178,  0x03001C44,  (0x200 +  5)   // 75
#define PAD_MIPIRX2N       0x17C,  0x03001C48,  (0x200 +  6)   // 76
#define PAD_MIPIRX2P       0x180,  0x03001C4C,  (0x200 +  7)   // 77
#define PAD_MIPIRX1N       0x184,  0x03001C50,  (0x200 +  8)   // 78
#define PAD_MIPIRX1P       0x188,  0x03001C54,  (0x200 +  9)   // 79
#define PAD_MIPIRX0N       0x18C,  0x03001C58,  (0x200 + 10)   // 80
#define PAD_MIPIRX0P       0x190,  0x03001C5C,  (0x200 + 11)   // 81
#define PAD_MIPI_TXM2      0x1A4,  0x03001C70,  (0x200 + 16)   // 83
#define PAD_MIPI_TXP2      0x1A8,  0x03001C74,  (0x200 + 17)   // 84
#define PAD_MIPI_TXM1      0x1AC,  0x03001C78,  (0x200 + 14)   // 85
#define PAD_MIPI_TXP1      0x1B0,  0x03001C7C,  (0x200 + 15)   // 86
#define PAD_MIPI_TXM0      0x1B4,  0x03001C80,  (0x200 + 13)   // 87
#define PAD_MIPI_TXP0      0x1B8,  0x03001C84,  (0x200 + 12)   // 88

// PAD table is required for pad addressing by GPIO numbering
#define PAD_TABLE           \
  { PAD_AUD_AINL_MIC   },   \
  { PAD_AUD_AOUTR      },   \
  { PAD_SD0_CLK        },   \
  { PAD_SD0_CMD        },   \
  { PAD_SD0_D0         },   \
  { PAD_SD0_D1         },   \
  { PAD_SD0_D2         },   \
  { PAD_SD0_D3         },   \
  { PAD_SD0_CD         },   \
  { PAD_SD0_PWR_EN     },   \
  { PAD_SPK_EN         },   \
  { PAD_UART0_TX       },   \
  { PAD_UART0_RX       },   \
  { PAD_EMMC_DAT2      },   \
  { PAD_EMMC_CLK       },   \
  { PAD_EMMC_DAT0      },   \
  { PAD_EMMC_DAT3      },   \
  { PAD_EMMC_CMD       },   \
  { PAD_EMMC_DAT1      },   \
  { PAD_JTAG_CPU_TMS   },   \
  { PAD_JTAG_CPU_TCK   },   \
  { PAD_IIC0_SCL       },   \
  { PAD_IIC0_SDA       },   \
  { PAD_AUX0           },   \
  { PAD_GPIO_ZQ        },   \
  { PAD_PWR_SEQ1       },   \
  { PAD_PWR_SEQ2       },   \
  { PAD_PWR_WAKEUP0    },   \
  { PAD_PWR_BUTTON1    },   \
  { PAD_PWR_GPIO0      },   \
  { PAD_PWR_GPIO1      },   \
  { PAD_PWR_GPIO2      },   \
  { PAD_SD1_D3         },   \
  { PAD_SD1_D2         },   \
  { PAD_SD1_D1         },   \
  { PAD_SD1_D0         },   \
  { PAD_SD1_CMD        },   \
  { PAD_SD1_CLK        },   \
  { PAD_PWM0_BUCK      },   \
  { PAD_ADC1           },   \
  { PAD_USB_VBUS_DET   },   \
  { PAD_GPIO_RTX       },   \
  { PAD_MIPIRX4N       },   \
  { PAD_MIPIRX4P       },   \
  { PAD_MIPIRX3N       },   \
  { PAD_MIPIRX3P       },   \
  { PAD_MIPIRX2N       },   \
  { PAD_MIPIRX2P       },   \
  { PAD_MIPIRX1N       },   \
  { PAD_MIPIRX1P       },   \
  { PAD_MIPIRX0N       },   \
  { PAD_MIPIRX0P       },   \
  { PAD_MIPI_TXM2      },   \
  { PAD_MIPI_TXP2      },   \
  { PAD_MIPI_TXM1      },   \
  { PAD_MIPI_TXP1      },   \
  { PAD_MIPI_TXM0      },   \
  { PAD_MIPI_TXP0      }



//-----------------------------------------------------------------------------
// all pinmuxes
//-----------------------------------------------------------------------------

#define FMUX_SD0_CLK__SDIO0_CLK     0
#define FMUX_SD0_CLK__IIC1_SDA      1
#define FMUX_SD0_CLK__SPI0_SCK      2
#define FMUX_SD0_CLK__XGPIOA_7      3
#define FMUX_SD0_CLK__PWM_15        5
#define FMUX_SD0_CLK__EPHY_LNK_LED  6
#define FMUX_SD0_CLK__DBG_0         7

#define FMUX_SD0_CMD__SDIO0_CMD     0
#define FMUX_SD0_CMD__IIC1_SCL      1
#define FMUX_SD0_CMD__SPI0_SDO      2
#define FMUX_SD0_CMD__XGPIOA_8      3
#define FMUX_SD0_CMD__PWM_14        5
#define FMUX_SD0_CMD__EPHY_SPD_LED  6
#define FMUX_SD0_CMD__DBG_1         7

#define FMUX_SD0_D0__SDIO0_D_0      0
#define FMUX_SD0_D0__CAM_MCLK1      1
#define FMUX_SD0_D0__SPI0_SDI       2
#define FMUX_SD0_D0__XGPIOA_9       3
#define FMUX_SD0_D0__UART3_TX       4
#define FMUX_SD0_D0__PWM_13         5
#define FMUX_SD0_D0__WG0_D0         6
#define FMUX_SD0_D0__DBG_2          7

#define FMUX_SD0_D1__SDIO0_D_1  0
#define FMUX_SD0_D1__IIC1_SDA   1
#define FMUX_SD0_D1__AUX0       2
#define FMUX_SD0_D1__XGPIOA_10  3
#define FMUX_SD0_D1__UART1_TX   4
#define FMUX_SD0_D1__PWM_12     5
#define FMUX_SD0_D1__WG0_D1     6
#define FMUX_SD0_D1__DBG_3      7

#define FMUX_SD0_D2__SDIO0_D_2  0
#define FMUX_SD0_D2__IIC1_SCL   1
#define FMUX_SD0_D2__AUX1       2
#define FMUX_SD0_D2__XGPIOA_11  3
#define FMUX_SD0_D2__UART1_RX   4
#define FMUX_SD0_D2__PWM_11     5
#define FMUX_SD0_D2__WG1_D0     6
#define FMUX_SD0_D2__DBG_4      7

#define FMUX_SD0_D3__SDIO0_D_3  0
#define FMUX_SD0_D3__CAM_MCLK0  1
#define FMUX_SD0_D3__SPI0_CS_X  2
#define FMUX_SD0_D3__XGPIOA_12  3
#define FMUX_SD0_D3__UART3_RX   4
#define FMUX_SD0_D3__PWM_10     5
#define FMUX_SD0_D3__WG1_D1     6
#define FMUX_SD0_D3__DBG_5      7

#define FMUX_SD0_CD__SDIO0_CD          0
#define FMUX_SD0_CD__XGPIOA_13         3
#define FMUX_SD0_PWR_EN__SDIO0_PWR_EN  0
#define FMUX_SD0_PWR_EN__XGPIOA_14     3

#define FMUX_SPK_EN__XGPIOA_15         3

#define FMUX_UART0_TX__UART0_TX        0
#define FMUX_UART0_TX__CAM_MCLK1       1
#define FMUX_UART0_TX__PWM_4           2
#define FMUX_UART0_TX__XGPIOA_16       3
#define FMUX_UART0_TX__UART1_TX        4
#define FMUX_UART0_TX__DBG_6           7

#define FMUX_UART0_RX__UART0_RX        0
#define FMUX_UART0_RX__CAM_MCLK0       1
#define FMUX_UART0_RX__PWM_5           2
#define FMUX_UART0_RX__XGPIOA_17       3
#define FMUX_UART0_RX__UART1_RX        4
#define FMUX_UART0_RX__AUX0            5
#define FMUX_UART0_RX__DBG_7           7

#define FMUX_EMMC_DAT2__EMMC_DAT_2     0
#define FMUX_EMMC_DAT2__SPINOR_HOLD_X  1
#define FMUX_EMMC_DAT2__SPINAND_HOLD   2
#define FMUX_EMMC_DAT2__XGPIOA_26      3
#define FMUX_EMMC_CLK__EMMC_CLK        0
#define FMUX_EMMC_CLK__SPINOR_SCK      1
#define FMUX_EMMC_CLK__SPINAND_CLK     2
#define FMUX_EMMC_CLK__XGPIOA_22       3
#define FMUX_EMMC_DAT0__EMMC_DAT_0     0
#define FMUX_EMMC_DAT0__SPINOR_MOSI    1
#define FMUX_EMMC_DAT0__SPINAND_MOSI   2
#define FMUX_EMMC_DAT0__XGPIOA_25      3
#define FMUX_EMMC_DAT3__EMMC_DAT_3     0
#define FMUX_EMMC_DAT3__SPINOR_WP_X    1
#define FMUX_EMMC_DAT3__SPINAND_WP     2
#define FMUX_EMMC_DAT3__XGPIOA_27      3
#define FMUX_EMMC_CMD__EMMC_CMD        0
#define FMUX_EMMC_CMD__SPINOR_MISO     1
#define FMUX_EMMC_CMD__SPINAND_MISO    2
#define FMUX_EMMC_CMD__XGPIOA_23       3
#define FMUX_EMMC_DAT1__EMMC_DAT_1     0
#define FMUX_EMMC_DAT1__SPINOR_CS_X    1
#define FMUX_EMMC_DAT1__SPINAND_CS     2
#define FMUX_EMMC_DAT1__XGPIOA_24      3

#define FMUX_JTAG_CPU_TMS__JTAG_TMS    0
#define FMUX_JTAG_CPU_TMS__CAM_MCLK0   1
#define FMUX_JTAG_CPU_TMS__PWM_7       2
#define FMUX_JTAG_CPU_TMS__XGPIOA_19   3
#define FMUX_JTAG_CPU_TMS__UART1_RTS   4
#define FMUX_JTAG_CPU_TMS__AUX0        5
#define FMUX_JTAG_CPU_TMS__UART1_TX    6
#define FMUX_JTAG_CPU_TMS__VO_D_28     7

#define FMUX_JTAG_CPU_TCK__JTAG_TCK    0
#define FMUX_JTAG_CPU_TCK__CAM_MCLK1   1
#define FMUX_JTAG_CPU_TCK__PWM_6       2
#define FMUX_JTAG_CPU_TCK__XGPIOA_18   3
#define FMUX_JTAG_CPU_TCK__UART1_CTS   4
#define FMUX_JTAG_CPU_TCK__AUX1        5
#define FMUX_JTAG_CPU_TCK__UART1_RX    6
#define FMUX_JTAG_CPU_TCK__VO_D_29     7

#define FMUX_IIC0_SCL__JTAG_TDI    0
#define FMUX_IIC0_SCL__UART1_TX    1
#define FMUX_IIC0_SCL__UART2_TX    2
#define FMUX_IIC0_SCL__XGPIOA_28   3
#define FMUX_IIC0_SCL__IIC0_SCL    4
#define FMUX_IIC0_SCL__WG0_D0      5
#define FMUX_IIC0_SCL__DBG_10      7
#define FMUX_IIC0_SDA__JTAG_TDO    0
#define FMUX_IIC0_SDA__UART1_RX    1
#define FMUX_IIC0_SDA__UART2_RX    2
#define FMUX_IIC0_SDA__XGPIOA_29   3
#define FMUX_IIC0_SDA__IIC0_SDA    4
#define FMUX_IIC0_SDA__WG0_D1      5
#define FMUX_IIC0_SDA__WG1_D0      6
#define FMUX_IIC0_SDA__DBG_11      7

// veryify these:

#define FMUX_AUX0__AUX0                  0
#define FMUX_AUX0__XGPIOA_30             3
#define FMUX_AUX0__IIS1_MCLK             4
#define FMUX_AUX0__WG1_D1                6
#define FMUX_AUX0__DBG_12                7
#define FMUX_GPIO_ZQ__PWR_GPIO_24        3
#define FMUX_GPIO_ZQ__PWM_2              4
#define FMUX_PWR_VBAT_DET__PWR_VBAT_DET  0
#define FMUX_PWR_RSTN__PWR_RSTN          0
#define FMUX_PWR_SEQ1__PWR_SEQ1          0
#define FMUX_PWR_SEQ1__PWR_GPIO_3        3
#define FMUX_PWR_SEQ2__PWR_SEQ2          0
#define FMUX_PWR_SEQ2__PWR_GPIO_4        3
#define FMUX_PWR_WAKEUP0__PWR_WAKEUP0    0
#define FMUX_PWR_WAKEUP0__PWR_IR0        1
#define FMUX_PWR_WAKEUP0__PWR_UART0_TX   2
#define FMUX_PWR_WAKEUP0__PWR_GPIO_6     3
#define FMUX_PWR_WAKEUP0__UART1_TX       4
#define FMUX_PWR_WAKEUP0__IIC4_SCL       5
#define FMUX_PWR_WAKEUP0__EPHY_LNK_LED   6
#define FMUX_PWR_WAKEUP0__WG2_D0         7
#define FMUX_PWR_BUTTON1__PWR_BUTTON1    0
#define FMUX_PWR_BUTTON1__PWR_GPIO_8     3
#define FMUX_PWR_BUTTON1__UART1_RX       4
#define FMUX_PWR_BUTTON1__IIC4_SDA       5
#define FMUX_PWR_BUTTON1__EPHY_SPD_LED   6
#define FMUX_PWR_BUTTON1__WG2_D1         7
#define FMUX_XTAL_XIN__PWR_XTAL_CLKIN    0
#define FMUX_PWR_GPIO0__PWR_GPIO_0       0
#define FMUX_PWR_GPIO0__UART2_TX         1
#define FMUX_PWR_GPIO0__PWR_UART0_RX     2
#define FMUX_PWR_GPIO0__PWM_8            4
#define FMUX_PWR_GPIO1__PWR_GPIO_1       0
#define FMUX_PWR_GPIO1__UART2_RX         1
#define FMUX_PWR_GPIO1__EPHY_LNK_LED     3
#define FMUX_PWR_GPIO1__PWM_9            4
#define FMUX_PWR_GPIO1__PWR_IIC_SCL      5
#define FMUX_PWR_GPIO1__IIC2_SCL         6
#define FMUX_PWR_GPIO1__IIC0_SDA         7
#define FMUX_PWR_GPIO2__PWR_GPIO_2       0
#define FMUX_PWR_GPIO2__PWR_SECTICK      2
#define FMUX_PWR_GPIO2__EPHY_SPD_LED     3
#define FMUX_PWR_GPIO2__PWM_10           4
#define FMUX_PWR_GPIO2__PWR_IIC_SDA      5
#define FMUX_PWR_GPIO2__IIC2_SDA         6
#define FMUX_PWR_GPIO2__IIC0_SCL         7
#define FMUX_SD1_GPIO1__UART4_TX         1
#define FMUX_SD1_GPIO1__PWR_GPIO_26      3
#define FMUX_SD1_GPIO1__PWM_10           7
#define FMUX_SD1_GPIO0__UART4_RX         1
#define FMUX_SD1_GPIO0__PWR_GPIO_25      3
#define FMUX_SD1_GPIO0__PWM_11           7

#define FMUX_SD1_D3__PWR_SD1_D3          0
#define FMUX_SD1_D3__SPI2_CS_X           1
#define FMUX_SD1_D3__IIC1_SCL            2
#define FMUX_SD1_D3__PWR_GPIO_18         3
#define FMUX_SD1_D3__CAM_MCLK0           4
#define FMUX_SD1_D3__UART3_CTS           5
#define FMUX_SD1_D3__PWR_SPINOR1_CS_X    6
#define FMUX_SD1_D3__PWM_4               7
#define FMUX_SD1_D2__PWR_SD1_D2          0
#define FMUX_SD1_D2__IIC1_SCL            1
#define FMUX_SD1_D2__UART2_TX            2
#define FMUX_SD1_D2__PWR_GPIO_19         3
#define FMUX_SD1_D2__CAM_MCLK0           4
#define FMUX_SD1_D2__UART3_TX            5
#define FMUX_SD1_D2__PWR_SPINOR1_HOLD_X  6
#define FMUX_SD1_D2__PWM_5               7
#define FMUX_SD1_D1__PWR_SD1_D1          0
#define FMUX_SD1_D1__IIC1_SDA            1
#define FMUX_SD1_D1__UART2_RX            2
#define FMUX_SD1_D1__PWR_GPIO_20         3
#define FMUX_SD1_D1__CAM_MCLK1           4
#define FMUX_SD1_D1__UART3_RX            5
#define FMUX_SD1_D1__PWR_SPINOR1_WP_X    6
#define FMUX_SD1_D1__PWM_6               7
#define FMUX_SD1_D0__PWR_SD1_D0          0
#define FMUX_SD1_D0__SPI2_SDI            1
#define FMUX_SD1_D0__IIC1_SDA            2
#define FMUX_SD1_D0__PWR_GPIO_21         3
#define FMUX_SD1_D0__CAM_MCLK1           4
#define FMUX_SD1_D0__UART3_RTS           5
#define FMUX_SD1_D0__PWR_SPINOR1_MISO    6
#define FMUX_SD1_D0__PWM_7               7
#define FMUX_SD1_CMD__PWR_SD1_CMD        0
#define FMUX_SD1_CMD__SPI2_SDO           1
#define FMUX_SD1_CMD__IIC3_SCL           2
#define FMUX_SD1_CMD__PWR_GPIO_22        3
#define FMUX_SD1_CMD__CAM_VS0            4
#define FMUX_SD1_CMD__EPHY_LNK_LED       5
#define FMUX_SD1_CMD__PWR_SPINOR1_MOSI   6
#define FMUX_SD1_CMD__PWM_8              7
#define FMUX_SD1_CLK__PWR_SD1_CLK        0
#define FMUX_SD1_CLK__SPI2_SCK           1
#define FMUX_SD1_CLK__IIC3_SDA           2
#define FMUX_SD1_CLK__PWR_GPIO_23        3
#define FMUX_SD1_CLK__CAM_HS0            4
#define FMUX_SD1_CLK__EPHY_SPD_LED       5
#define FMUX_SD1_CLK__PWR_SPINOR1_SCK    6
#define FMUX_SD1_CLK__PWM_9              7

#define FMUX_PWM0_BUCK__PWM_0            0
#define FMUX_PWM0_BUCK__XGPIOB_0         3
#define FMUX_ADC1__XGPIOB_3              3
#define FMUX_ADC1__KEY_COL2              4
#define FMUX_ADC1__PWM_3                 6
#define FMUX_USB_VBUS_DET__USB_VBUS_DET  0
#define FMUX_USB_VBUS_DET__XGPIOB_6      3
#define FMUX_USB_VBUS_DET__CAM_MCLK0     4
#define FMUX_USB_VBUS_DET__CAM_MCLK1     5
#define FMUX_USB_VBUS_DET__PWM_4         6

#define MUX_SPI1_MISO__UART3_RTS  1
#define MUX_SPI1_MISO__IIC1_SDA   2
#define MUX_SPI1_MISO__XGPIOB_8   3
#define MUX_SPI1_MISO__PWM_9      4
#define MUX_SPI1_MISO__KEY_COL1   5
#define MUX_SPI1_MISO__SPI1_SDI   6
#define MUX_SPI1_MISO__DBG_14     7
#define MUX_SPI1_MOSI__UART3_RX   1
#define MUX_SPI1_MOSI__IIC1_SCL   2
#define MUX_SPI1_MOSI__XGPIOB_7   3
#define MUX_SPI1_MOSI__PWM_8      4
#define MUX_SPI1_MOSI__KEY_COL0   5
#define MUX_SPI1_MOSI__SPI1_SDO   6
#define MUX_SPI1_MOSI__DBG_13     7
#define MUX_SPI1_CS__UART3_CTS    1
#define MUX_SPI1_CS__CAM_MCLK0    2
#define MUX_SPI1_CS__XGPIOB_10    3
#define MUX_SPI1_CS__PWM_11       4
#define MUX_SPI1_CS__KEY_ROW3     5
#define MUX_SPI1_CS__SPI1_CS_X    6
#define MUX_SPI1_CS__DBG_16       7
#define MUX_SPI1_SCK__UART3_TX    1
#define MUX_SPI1_SCK__CAM_MCLK1   2
#define MUX_SPI1_SCK__XGPIOB_9    3
#define MUX_SPI1_SCK__PWM_10      4
#define MUX_SPI1_SCK__KEY_ROW2    5
#define MUX_SPI1_SCK__SPI1_SCK    6
#define MUX_SPI1_SCK__DBG_15      7

#define FMUX_ETH_TXP__UART3_RX    1
#define FMUX_ETH_TXP__IIC1_SCL    2
#define FMUX_ETH_TXP__XGPIOB_25   3
#define FMUX_ETH_TXP__PWM_13      4
#define FMUX_ETH_TXP__CAM_MCLK0   5
#define FMUX_ETH_TXP__SPI1_SDO    6
#define FMUX_ETH_TXP__IIS2_LRCK   7
#define FMUX_ETH_TXM__UART3_RTS   1
#define FMUX_ETH_TXM__IIC1_SDA    2
#define FMUX_ETH_TXM__XGPIOB_24   3
#define FMUX_ETH_TXM__PWM_12      4
#define FMUX_ETH_TXM__CAM_MCLK1   5
#define FMUX_ETH_TXM__SPI1_SDI    6
#define FMUX_ETH_TXM__IIS2_BCLK   7
#define FMUX_ETH_RXP__UART3_TX    1
#define FMUX_ETH_RXP__CAM_MCLK1   2
#define FMUX_ETH_RXP__XGPIOB_27   3
#define FMUX_ETH_RXP__PWM_15      4
#define FMUX_ETH_RXP__CAM_HS0     5
#define FMUX_ETH_RXP__SPI1_SCK    6
#define FMUX_ETH_RXP__IIS2_DO     7
#define FMUX_ETH_RXM__UART3_CTS   1
#define FMUX_ETH_RXM__CAM_MCLK0   2
#define FMUX_ETH_RXM__XGPIOB_26   3
#define FMUX_ETH_RXM__PWM_14      4
#define FMUX_ETH_RXM__CAM_VS0     5
#define FMUX_ETH_RXM__SPI1_CS_X   6
#define FMUX_ETH_RXM__IIS2_DI     7

#define FMUX_GPIO_RTX__VI0_D_15       1
#define FMUX_GPIO_RTX__XGPIOB_23      3
#define FMUX_GPIO_RTX__PWM_1          4
#define FMUX_GPIO_RTX__CAM_MCLK0      5
#define FMUX_GPIO_RTX__IIS2_MCLK      7

#define FMUX_MIPIRX4N__VI0_CLK        1
#define FMUX_MIPIRX4N__IIC0_SCL       2
#define FMUX_MIPIRX4N__XGPIOC_2       3
#define FMUX_MIPIRX4N__IIC1_SDA       4
#define FMUX_MIPIRX4N__CAM_MCLK0      5
#define FMUX_MIPIRX4N__KEY_ROW0       6
#define FMUX_MIPIRX4N__MUX_SPI1_SCK   7
#define FMUX_MIPIRX4P__VI0_D_0        1
#define FMUX_MIPIRX4P__IIC0_SDA       2
#define FMUX_MIPIRX4P__XGPIOC_3       3
#define FMUX_MIPIRX4P__IIC1_SCL       4
#define FMUX_MIPIRX4P__CAM_MCLK1      5
#define FMUX_MIPIRX4P__KEY_ROW1       6
#define FMUX_MIPIRX4P__MUX_SPI1_CS    7
#define FMUX_MIPIRX3N__VI0_D_1        1
#define FMUX_MIPIRX3N__XGPIOC_4       3
#define FMUX_MIPIRX3N__CAM_MCLK0      4
#define FMUX_MIPIRX3N__MUX_SPI1_MISO  7
#define FMUX_MIPIRX3P__VI0_D_2        1
#define FMUX_MIPIRX3P__XGPIOC_5       3
#define FMUX_MIPIRX3P__MUX_SPI1_MOSI  7
#define FMUX_MIPIRX2N__VI0_D_3        1
#define FMUX_MIPIRX2N__XGPIOC_6       3
#define FMUX_MIPIRX2N__IIC4_SCL       5
#define FMUX_MIPIRX2N__DBG_6          7
#define FMUX_MIPIRX2P__VI0_D_4        1
#define FMUX_MIPIRX2P__XGPIOC_7       3
#define FMUX_MIPIRX2P__IIC4_SDA       5
#define FMUX_MIPIRX2P__DBG_7          7
#define FMUX_MIPIRX1N__VI0_D_5        1
#define FMUX_MIPIRX1N__XGPIOC_8       3
#define FMUX_MIPIRX1N__KEY_ROW3       6
#define FMUX_MIPIRX1N__DBG_8          7
#define FMUX_MIPIRX1P__VI0_D_6        1
#define FMUX_MIPIRX1P__XGPIOC_9       3
#define FMUX_MIPIRX1P__IIC1_SDA       4
#define FMUX_MIPIRX1P__KEY_ROW2       6
#define FMUX_MIPIRX1P__DBG_9          7
#define FMUX_MIPIRX0N__VI0_D_7        1
#define FMUX_MIPIRX0N__XGPIOC_10      3
#define FMUX_MIPIRX0N__IIC1_SCL       4
#define FMUX_MIPIRX0N__CAM_MCLK1      5
#define FMUX_MIPIRX0N__DBG_10         7
#define FMUX_MIPIRX0P__VI0_D_8        1
#define FMUX_MIPIRX0P__XGPIOC_11      3
#define FMUX_MIPIRX0P__CAM_MCLK0      4
#define FMUX_MIPIRX0P__DBG_11         7

#define FMUX_AUD_AINL_MIC__XGPIOC_23  3
#define FMUX_AUD_AINL_MIC__IIS1_BCLK  4
#define FMUX_AUD_AINL_MIC__IIS2_BCLK  5
#define FMUX_AUD_AOUTR__XGPIOC_24     3
#define FMUX_AUD_AOUTR__IIS1_DI       4
#define FMUX_AUD_AOUTR__IIS2_DO       5
#define FMUX_AUD_AOUTR__IIS1_DO       6

#define FMUX_GPIO         3
#define FMUX_INVALID   0xFF

#include "sg2000_common_vihal.h"

#endif // _SG2002_VIHAL_H
