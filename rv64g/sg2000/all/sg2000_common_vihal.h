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
 *  file:     sg2000_vihal.h
 *  brief:    own CV1800/SG200x definitions, some definition copied from the Sophgo SDK
 *  created:  2024-04-21
 *  authors:  nvitya
*/

// Sophgo SDK Copyright:
/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
*/

#ifndef _SG2000_COMMON_VIHAL_H
#define _SG2000_COMMON_VIHAL_H

#include <cstddef>
#include "stdint.h"
#include "stdint.h"

/* Register base address */

/* Under Coreplex */
#define CLINT_BASE_ADDR     (0x02000000)
#define PLIC_BASE_ADDR      (0x0C000000)

#define UARTHS_BASE_ADDR    (0x38000000)

#define GPIO0_BASE_ADDR     (0x03020000)
#define GPIO1_BASE_ADDR     (0x03021000)
#define GPIO2_BASE_ADDR     (0x03022000)
#define GPIO3_BASE_ADDR     (0x03023000)
#define GPIOPWR_BASE_ADDR      (0x05021000)
#define RTCSYS_GPIO_BASE_ADDR  (0x05021000)

//------------------------------------------------------------------------------
// GPIO
//------------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t     SWPORTA_DR;    // 0x00  Port A data register
  volatile uint32_t     SWPORTA_DDR;   // 0x04  Port A data direction register
  uint32_t              _pad_08[(0x30-0x08)/4];
  volatile uint32_t     INTEN;         // 0x30  Interrupt Enable
  volatile uint32_t     INTMASK;       // 0x34
  volatile uint32_t     INTTYPE_LEVEL; // 0x38
  volatile uint32_t     INT_POLARITY;  // 0x3C
  volatile uint32_t     INTSTATUS;     // 0x40
  volatile uint32_t     RAW_INTSTATUS; // 0x44
  volatile uint32_t     DEBOUNCE;      // 0x48
  volatile uint32_t     PORTA_EOI;     // 0x4C
  volatile uint32_t     EXT_PORTA;     // 0x50  External Port register (data input)
  uint32_t              _pad_54[(0x60-0x54)/4];
  volatile uint32_t     LS_SYNC;       // 0x60  External Port register (data input)
//
} gpio_regs_t;




//------------------------------------------------------------------------------
// SYSCTL
//------------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t     GIT_ID;        // 0x00  Git short commit id
  volatile uint32_t     CLK_FREQ;      // 0x04  System clock base frequency
  volatile uint32_t     PLL0;          // 0x08  PLL0 controller
  volatile uint32_t     PLL1;          // 0x0c  PLL1 controller
  volatile uint32_t     PLL2;          // 0x10  PLL2 controller
  volatile uint32_t     RESV5;         // 0x14  Reserved
  volatile uint32_t     PLL_LOCK;      // 0x18  PLL lock tester
  volatile uint32_t     ROM_ERROR;     // 0x1c  AXI ROM detector
  volatile uint32_t     CLK_SEL0;      // 0x20  Clock select controller0
  volatile uint32_t     CLK_SEL1;      // 0x24  Clock select controller1
  volatile uint32_t     CLK_EN_CENT;   // 0x28  Central clock enable
  volatile uint32_t     CLK_EN_PERI;   // 0x2c  Peripheral clock enable
  volatile uint32_t     SOFT_RESET;    // 0x30  Soft reset ctrl
  volatile uint32_t     PERI_RESET;    // 0x34  Peripheral reset controller
  volatile uint32_t     CLK_TH0;       // 0x38  Clock threshold controller 0
  volatile uint32_t     CLK_TH1;       // 0x3c  Clock threshold controller 1
  volatile uint32_t     CLK_TH2;       // 0x40  Clock threshold controller 2
  volatile uint32_t     CLK_TH3;       // 0x44  Clock threshold controller 3
  volatile uint32_t     CLK_TH4;       // 0x48  Clock threshold controller 4
  volatile uint32_t     CLK_TH5;       // 0x4c  Clock threshold controller 5
  volatile uint32_t     CLK_TH6;       // 0x50  Clock threshold controller 6
  volatile uint32_t     MISC;          // 0x54  Miscellaneous controller
  volatile uint32_t     PERI;          // 0x58  Peripheral controller
  volatile uint32_t     SPI_SLEEP;     // 0x5c  SPI sleep controller
  volatile uint32_t     RESET_STATUS;  // 0x60  Reset source status
  volatile uint32_t     DMA_SEL0;      // 0x64  DMA handshake selector
  volatile uint32_t     DMA_SEL1;      // 0x68  DMA handshake selector
  volatile uint32_t     POWER_SEL;     // 0x6c  IO Power Mode Select controller
  volatile uint32_t     RESV28;        // 0x70  Reserved
  volatile uint32_t     RESV29;        // 0x74  Reserved
  volatile uint32_t     RESV30;        // 0x78  Reserved
  volatile uint32_t     RESV31;        // 0x7c  Reserved
//
} sysctl_regs_t;


#define SYSCTL_CLK_EN_CENT_CPU     (1 << 0)
#define SYSCTL_CLK_EN_CENT_SRAM0   (1 << 1)
#define SYSCTL_CLK_EN_CENT_SRAM1   (1 << 2)
#define SYSCTL_CLK_EN_CENT_APB0    (1 << 3)
#define SYSCTL_CLK_EN_CENT_APB1    (1 << 4)
#define SYSCTL_CLK_EN_CENT_APB2    (1 << 5)

#define SYSCTL_CLK_EN_PERI_ROM     (1 <<  0)
#define SYSCTL_CLK_EN_PERI_DMA     (1 <<  1)
#define SYSCTL_CLK_EN_PERI_AI      (1 <<  2)
#define SYSCTL_CLK_EN_PERI_DVP     (1 <<  3)
#define SYSCTL_CLK_EN_PERI_FFT     (1 <<  4)
#define SYSCTL_CLK_EN_PERI_GPIO    (1 <<  5)
#define SYSCTL_CLK_EN_PERI_SPI0    (1 <<  6)
#define SYSCTL_CLK_EN_PERI_SPI1    (1 <<  7)
#define SYSCTL_CLK_EN_PERI_SPI2    (1 <<  8)
#define SYSCTL_CLK_EN_PERI_SPI3    (1 <<  9)
#define SYSCTL_CLK_EN_PERI_I2S0    (1 << 10)
#define SYSCTL_CLK_EN_PERI_I2S1    (1 << 11)
#define SYSCTL_CLK_EN_PERI_I2S2    (1 << 12)
#define SYSCTL_CLK_EN_PERI_I2C0    (1 << 13)
#define SYSCTL_CLK_EN_PERI_I2C1    (1 << 14)
#define SYSCTL_CLK_EN_PERI_I2C2    (1 << 15)
#define SYSCTL_CLK_EN_PERI_UART1   (1 << 16)
#define SYSCTL_CLK_EN_PERI_UART2   (1 << 17)
#define SYSCTL_CLK_EN_PERI_UART3   (1 << 18)
#define SYSCTL_CLK_EN_PERI_AES     (1 << 19)
#define SYSCTL_CLK_EN_PERI_FPIOA   (1 << 20)
#define SYSCTL_CLK_EN_PERI_TIMER0  (1 << 21)
#define SYSCTL_CLK_EN_PERI_TIMER1  (1 << 22)
#define SYSCTL_CLK_EN_PERI_TIMER2  (1 << 23)
#define SYSCTL_CLK_EN_PERI_WDT0    (1 << 24)
#define SYSCTL_CLK_EN_PERI_WDT1    (1 << 25)
#define SYSCTL_CLK_EN_PERI_SHA     (1 << 26)
#define SYSCTL_CLK_EN_PERI_OTP     (1 << 27)
// 28 reserved
#define SYSCTL_CLK_EN_PERI_RTC     (1 << 29)


#define SYSCTL_PERI_RESET_ROM      (1 <<  0)
#define SYSCTL_PERI_RESET_DMA      (1 <<  1)
#define SYSCTL_PERI_RESET_AI       (1 <<  2)
#define SYSCTL_PERI_RESET_DVP      (1 <<  3)
#define SYSCTL_PERI_RESET_FFT      (1 <<  4)
#define SYSCTL_PERI_RESET_GPIO     (1 <<  5)
#define SYSCTL_PERI_RESET_SPI0     (1 <<  6)
#define SYSCTL_PERI_RESET_SPI1     (1 <<  7)
#define SYSCTL_PERI_RESET_SPI2     (1 <<  8)
#define SYSCTL_PERI_RESET_SPI3     (1 <<  9)
#define SYSCTL_PERI_RESET_I2S0     (1 << 10)
#define SYSCTL_PERI_RESET_I2S1     (1 << 11)
#define SYSCTL_PERI_RESET_I2S2     (1 << 12)
#define SYSCTL_PERI_RESET_I2C0     (1 << 13)
#define SYSCTL_PERI_RESET_I2C1     (1 << 14)
#define SYSCTL_PERI_RESET_I2C2     (1 << 15)
#define SYSCTL_PERI_RESET_UART1    (1 << 16)
#define SYSCTL_PERI_RESET_UART2    (1 << 17)
#define SYSCTL_PERI_RESET_UART3    (1 << 18)
#define SYSCTL_PERI_RESET_AES      (1 << 19)
#define SYSCTL_PERI_RESET_FPIOA    (1 << 20)
#define SYSCTL_PERI_RESET_TIMER0   (1 << 21)
#define SYSCTL_PERI_RESET_TIMER1   (1 << 22)
#define SYSCTL_PERI_RESET_TIMER2   (1 << 23)
#define SYSCTL_PERI_RESET_WDT0     (1 << 24)
#define SYSCTL_PERI_RESET_WDT1     (1 << 25)
#define SYSCTL_PERI_RESET_SHA      (1 << 26)
#define SYSCTL_PERI_RESET_RTC      (1 << 29)

//------------------------------------------------------------------------------
// FPIOA
//------------------------------------------------------------------------------

#define FPIOA_NUM_IO    (48)

/*
 * Any IO of FPIOA have 256 functions, it is a IO-function matrix.
 * All IO have default reset function, after reset, re-configure
 * IO function is required.
 */

typedef enum
{
    FUNC_JTAG_TCLK        = 0,  /*!< JTAG Test Clock */
    FUNC_JTAG_TDI         = 1,  /*!< JTAG Test Data In */
    FUNC_JTAG_TMS         = 2,  /*!< JTAG Test Mode Select */
    FUNC_JTAG_TDO         = 3,  /*!< JTAG Test Data Out */
    FUNC_SPI0_D0          = 4,  /*!< SPI0 Data 0 */
    FUNC_SPI0_D1          = 5,  /*!< SPI0 Data 1 */
    FUNC_SPI0_D2          = 6,  /*!< SPI0 Data 2 */
    FUNC_SPI0_D3          = 7,  /*!< SPI0 Data 3 */
    FUNC_SPI0_D4          = 8,  /*!< SPI0 Data 4 */
    FUNC_SPI0_D5          = 9,  /*!< SPI0 Data 5 */
    FUNC_SPI0_D6          = 10, /*!< SPI0 Data 6 */
    FUNC_SPI0_D7          = 11, /*!< SPI0 Data 7 */
    FUNC_SPI0_SS0         = 12, /*!< SPI0 Chip Select 0 */
    FUNC_SPI0_SS1         = 13, /*!< SPI0 Chip Select 1 */
    FUNC_SPI0_SS2         = 14, /*!< SPI0 Chip Select 2 */
    FUNC_SPI0_SS3         = 15, /*!< SPI0 Chip Select 3 */
    FUNC_SPI0_ARB         = 16, /*!< SPI0 Arbitration */
    FUNC_SPI0_SCLK        = 17, /*!< SPI0 Serial Clock */
    FUNC_UARTHS_RX        = 18, /*!< UART High speed Receiver */
    FUNC_UARTHS_TX        = 19, /*!< UART High speed Transmitter */
    FUNC_RESV6            = 20, /*!< Reserved function */
    FUNC_RESV7            = 21, /*!< Reserved function */
    FUNC_CLK_SPI1         = 22, /*!< Clock SPI1 */
    FUNC_CLK_I2C1         = 23, /*!< Clock I2C1 */
    FUNC_GPIOHS0          = 24, /*!< GPIO High speed 0 */
    FUNC_GPIOHS1          = 25, /*!< GPIO High speed 1 */
    FUNC_GPIOHS2          = 26, /*!< GPIO High speed 2 */
    FUNC_GPIOHS3          = 27, /*!< GPIO High speed 3 */
    FUNC_GPIOHS4          = 28, /*!< GPIO High speed 4 */
    FUNC_GPIOHS5          = 29, /*!< GPIO High speed 5 */
    FUNC_GPIOHS6          = 30, /*!< GPIO High speed 6 */
    FUNC_GPIOHS7          = 31, /*!< GPIO High speed 7 */
    FUNC_GPIOHS8          = 32, /*!< GPIO High speed 8 */
    FUNC_GPIOHS9          = 33, /*!< GPIO High speed 9 */
    FUNC_GPIOHS10         = 34, /*!< GPIO High speed 10 */
    FUNC_GPIOHS11         = 35, /*!< GPIO High speed 11 */
    FUNC_GPIOHS12         = 36, /*!< GPIO High speed 12 */
    FUNC_GPIOHS13         = 37, /*!< GPIO High speed 13 */
    FUNC_GPIOHS14         = 38, /*!< GPIO High speed 14 */
    FUNC_GPIOHS15         = 39, /*!< GPIO High speed 15 */
    FUNC_GPIOHS16         = 40, /*!< GPIO High speed 16 */
    FUNC_GPIOHS17         = 41, /*!< GPIO High speed 17 */
    FUNC_GPIOHS18         = 42, /*!< GPIO High speed 18 */
    FUNC_GPIOHS19         = 43, /*!< GPIO High speed 19 */
    FUNC_GPIOHS20         = 44, /*!< GPIO High speed 20 */
    FUNC_GPIOHS21         = 45, /*!< GPIO High speed 21 */
    FUNC_GPIOHS22         = 46, /*!< GPIO High speed 22 */
    FUNC_GPIOHS23         = 47, /*!< GPIO High speed 23 */
    FUNC_GPIOHS24         = 48, /*!< GPIO High speed 24 */
    FUNC_GPIOHS25         = 49, /*!< GPIO High speed 25 */
    FUNC_GPIOHS26         = 50, /*!< GPIO High speed 26 */
    FUNC_GPIOHS27         = 51, /*!< GPIO High speed 27 */
    FUNC_GPIOHS28         = 52, /*!< GPIO High speed 28 */
    FUNC_GPIOHS29         = 53, /*!< GPIO High speed 29 */
    FUNC_GPIOHS30         = 54, /*!< GPIO High speed 30 */
    FUNC_GPIOHS31         = 55, /*!< GPIO High speed 31 */
    FUNC_GPIO0            = 56, /*!< GPIO pin 0 */
    FUNC_GPIO1            = 57, /*!< GPIO pin 1 */
    FUNC_GPIO2            = 58, /*!< GPIO pin 2 */
    FUNC_GPIO3            = 59, /*!< GPIO pin 3 */
    FUNC_GPIO4            = 60, /*!< GPIO pin 4 */
    FUNC_GPIO5            = 61, /*!< GPIO pin 5 */
    FUNC_GPIO6            = 62, /*!< GPIO pin 6 */
    FUNC_GPIO7            = 63, /*!< GPIO pin 7 */
    FUNC_UART1_RX         = 64, /*!< UART1 Receiver */
    FUNC_UART1_TX         = 65, /*!< UART1 Transmitter */
    FUNC_UART2_RX         = 66, /*!< UART2 Receiver */
    FUNC_UART2_TX         = 67, /*!< UART2 Transmitter */
    FUNC_UART3_RX         = 68, /*!< UART3 Receiver */
    FUNC_UART3_TX         = 69, /*!< UART3 Transmitter */
    FUNC_SPI1_D0          = 70, /*!< SPI1 Data 0 */
    FUNC_SPI1_D1          = 71, /*!< SPI1 Data 1 */
    FUNC_SPI1_D2          = 72, /*!< SPI1 Data 2 */
    FUNC_SPI1_D3          = 73, /*!< SPI1 Data 3 */
    FUNC_SPI1_D4          = 74, /*!< SPI1 Data 4 */
    FUNC_SPI1_D5          = 75, /*!< SPI1 Data 5 */
    FUNC_SPI1_D6          = 76, /*!< SPI1 Data 6 */
    FUNC_SPI1_D7          = 77, /*!< SPI1 Data 7 */
    FUNC_SPI1_SS0         = 78, /*!< SPI1 Chip Select 0 */
    FUNC_SPI1_SS1         = 79, /*!< SPI1 Chip Select 1 */
    FUNC_SPI1_SS2         = 80, /*!< SPI1 Chip Select 2 */
    FUNC_SPI1_SS3         = 81, /*!< SPI1 Chip Select 3 */
    FUNC_SPI1_ARB         = 82, /*!< SPI1 Arbitration */
    FUNC_SPI1_SCLK        = 83, /*!< SPI1 Serial Clock */
    FUNC_SPI_SLAVE_D0     = 84, /*!< SPI Slave Data 0 */
    FUNC_SPI_SLAVE_SS     = 85, /*!< SPI Slave Select */
    FUNC_SPI_SLAVE_SCLK   = 86, /*!< SPI Slave Serial Clock */
    FUNC_I2S0_MCLK        = 87, /*!< I2S0 Master Clock */
    FUNC_I2S0_SCLK        = 88, /*!< I2S0 Serial Clock(BCLK) */
    FUNC_I2S0_WS          = 89, /*!< I2S0 Word Select(LRCLK) */
    FUNC_I2S0_IN_D0       = 90, /*!< I2S0 Serial Data Input 0 */
    FUNC_I2S0_IN_D1       = 91, /*!< I2S0 Serial Data Input 1 */
    FUNC_I2S0_IN_D2       = 92, /*!< I2S0 Serial Data Input 2 */
    FUNC_I2S0_IN_D3       = 93, /*!< I2S0 Serial Data Input 3 */
    FUNC_I2S0_OUT_D0      = 94, /*!< I2S0 Serial Data Output 0 */
    FUNC_I2S0_OUT_D1      = 95, /*!< I2S0 Serial Data Output 1 */
    FUNC_I2S0_OUT_D2      = 96, /*!< I2S0 Serial Data Output 2 */
    FUNC_I2S0_OUT_D3      = 97, /*!< I2S0 Serial Data Output 3 */
    FUNC_I2S1_MCLK        = 98, /*!< I2S1 Master Clock */
    FUNC_I2S1_SCLK        = 99, /*!< I2S1 Serial Clock(BCLK) */
    FUNC_I2S1_WS          = 100,    /*!< I2S1 Word Select(LRCLK) */
    FUNC_I2S1_IN_D0       = 101,    /*!< I2S1 Serial Data Input 0 */
    FUNC_I2S1_IN_D1       = 102,    /*!< I2S1 Serial Data Input 1 */
    FUNC_I2S1_IN_D2       = 103,    /*!< I2S1 Serial Data Input 2 */
    FUNC_I2S1_IN_D3       = 104,    /*!< I2S1 Serial Data Input 3 */
    FUNC_I2S1_OUT_D0      = 105,    /*!< I2S1 Serial Data Output 0 */
    FUNC_I2S1_OUT_D1      = 106,    /*!< I2S1 Serial Data Output 1 */
    FUNC_I2S1_OUT_D2      = 107,    /*!< I2S1 Serial Data Output 2 */
    FUNC_I2S1_OUT_D3      = 108,    /*!< I2S1 Serial Data Output 3 */
    FUNC_I2S2_MCLK        = 109,    /*!< I2S2 Master Clock */
    FUNC_I2S2_SCLK        = 110,    /*!< I2S2 Serial Clock(BCLK) */
    FUNC_I2S2_WS          = 111,    /*!< I2S2 Word Select(LRCLK) */
    FUNC_I2S2_IN_D0       = 112,    /*!< I2S2 Serial Data Input 0 */
    FUNC_I2S2_IN_D1       = 113,    /*!< I2S2 Serial Data Input 1 */
    FUNC_I2S2_IN_D2       = 114,    /*!< I2S2 Serial Data Input 2 */
    FUNC_I2S2_IN_D3       = 115,    /*!< I2S2 Serial Data Input 3 */
    FUNC_I2S2_OUT_D0      = 116,    /*!< I2S2 Serial Data Output 0 */
    FUNC_I2S2_OUT_D1      = 117,    /*!< I2S2 Serial Data Output 1 */
    FUNC_I2S2_OUT_D2      = 118,    /*!< I2S2 Serial Data Output 2 */
    FUNC_I2S2_OUT_D3      = 119,    /*!< I2S2 Serial Data Output 3 */
    FUNC_RESV0            = 120,    /*!< Reserved function */
    FUNC_RESV1            = 121,    /*!< Reserved function */
    FUNC_RESV2            = 122,    /*!< Reserved function */
    FUNC_RESV3            = 123,    /*!< Reserved function */
    FUNC_RESV4            = 124,    /*!< Reserved function */
    FUNC_RESV5            = 125,    /*!< Reserved function */
    FUNC_I2C0_SCLK        = 126,    /*!< I2C0 Serial Clock */
    FUNC_I2C0_SDA         = 127,    /*!< I2C0 Serial Data */
    FUNC_I2C1_SCLK        = 128,    /*!< I2C1 Serial Clock */
    FUNC_I2C1_SDA         = 129,    /*!< I2C1 Serial Data */
    FUNC_I2C2_SCLK        = 130,    /*!< I2C2 Serial Clock */
    FUNC_I2C2_SDA         = 131,    /*!< I2C2 Serial Data */
    FUNC_CMOS_XCLK        = 132,    /*!< DVP System Clock */
    FUNC_CMOS_RST         = 133,    /*!< DVP System Reset */
    FUNC_CMOS_PWDN        = 134,    /*!< DVP Power Down Mode */
    FUNC_CMOS_VSYNC       = 135,    /*!< DVP Vertical Sync */
    FUNC_CMOS_HREF        = 136,    /*!< DVP Horizontal Reference output */
    FUNC_CMOS_PCLK        = 137,    /*!< Pixel Clock */
    FUNC_CMOS_D0          = 138,    /*!< Data Bit 0 */
    FUNC_CMOS_D1          = 139,    /*!< Data Bit 1 */
    FUNC_CMOS_D2          = 140,    /*!< Data Bit 2 */
    FUNC_CMOS_D3          = 141,    /*!< Data Bit 3 */
    FUNC_CMOS_D4          = 142,    /*!< Data Bit 4 */
    FUNC_CMOS_D5          = 143,    /*!< Data Bit 5 */
    FUNC_CMOS_D6          = 144,    /*!< Data Bit 6 */
    FUNC_CMOS_D7          = 145,    /*!< Data Bit 7 */
    FUNC_SCCB_SCLK        = 146,    /*!< SCCB Serial Clock */
    FUNC_SCCB_SDA         = 147,    /*!< SCCB Serial Data */
    FUNC_UART1_CTS        = 148,    /*!< UART1 Clear To Send */
    FUNC_UART1_DSR        = 149,    /*!< UART1 Data Set Ready */
    FUNC_UART1_DCD        = 150,    /*!< UART1 Data Carrier Detect */
    FUNC_UART1_RI         = 151,    /*!< UART1 Ring Indicator */
    FUNC_UART1_SIR_IN     = 152,    /*!< UART1 Serial Infrared Input */
    FUNC_UART1_DTR        = 153,    /*!< UART1 Data Terminal Ready */
    FUNC_UART1_RTS        = 154,    /*!< UART1 Request To Send */
    FUNC_UART1_OUT2       = 155,    /*!< UART1 User-designated Output 2 */
    FUNC_UART1_OUT1       = 156,    /*!< UART1 User-designated Output 1 */
    FUNC_UART1_SIR_OUT    = 157,    /*!< UART1 Serial Infrared Output */
    FUNC_UART1_BAUD       = 158,    /*!< UART1 Transmit Clock Output */
    FUNC_UART1_RE         = 159,    /*!< UART1 Receiver Output Enable */
    FUNC_UART1_DE         = 160,    /*!< UART1 Driver Output Enable */
    FUNC_UART1_RS485_EN   = 161,    /*!< UART1 RS485 Enable */
    FUNC_UART2_CTS        = 162,    /*!< UART2 Clear To Send */
    FUNC_UART2_DSR        = 163,    /*!< UART2 Data Set Ready */
    FUNC_UART2_DCD        = 164,    /*!< UART2 Data Carrier Detect */
    FUNC_UART2_RI         = 165,    /*!< UART2 Ring Indicator */
    FUNC_UART2_SIR_IN     = 166,    /*!< UART2 Serial Infrared Input */
    FUNC_UART2_DTR        = 167,    /*!< UART2 Data Terminal Ready */
    FUNC_UART2_RTS        = 168,    /*!< UART2 Request To Send */
    FUNC_UART2_OUT2       = 169,    /*!< UART2 User-designated Output 2 */
    FUNC_UART2_OUT1       = 170,    /*!< UART2 User-designated Output 1 */
    FUNC_UART2_SIR_OUT    = 171,    /*!< UART2 Serial Infrared Output */
    FUNC_UART2_BAUD       = 172,    /*!< UART2 Transmit Clock Output */
    FUNC_UART2_RE         = 173,    /*!< UART2 Receiver Output Enable */
    FUNC_UART2_DE         = 174,    /*!< UART2 Driver Output Enable */
    FUNC_UART2_RS485_EN   = 175,    /*!< UART2 RS485 Enable */
    FUNC_UART3_CTS        = 176,    /*!< UART3 Clear To Send */
    FUNC_UART3_DSR        = 177,    /*!< UART3 Data Set Ready */
    FUNC_UART3_DCD        = 178,    /*!< UART3 Data Carrier Detect */
    FUNC_UART3_RI         = 179,    /*!< UART3 Ring Indicator */
    FUNC_UART3_SIR_IN     = 180,    /*!< UART3 Serial Infrared Input */
    FUNC_UART3_DTR        = 181,    /*!< UART3 Data Terminal Ready */
    FUNC_UART3_RTS        = 182,    /*!< UART3 Request To Send */
    FUNC_UART3_OUT2       = 183,    /*!< UART3 User-designated Output 2 */
    FUNC_UART3_OUT1       = 184,    /*!< UART3 User-designated Output 1 */
    FUNC_UART3_SIR_OUT    = 185,    /*!< UART3 Serial Infrared Output */
    FUNC_UART3_BAUD       = 186,    /*!< UART3 Transmit Clock Output */
    FUNC_UART3_RE         = 187,    /*!< UART3 Receiver Output Enable */
    FUNC_UART3_DE         = 188,    /*!< UART3 Driver Output Enable */
    FUNC_UART3_RS485_EN   = 189,    /*!< UART3 RS485 Enable */
    FUNC_TIMER0_TOGGLE1   = 190,    /*!< TIMER0 Toggle Output 1 */
    FUNC_TIMER0_TOGGLE2   = 191,    /*!< TIMER0 Toggle Output 2 */
    FUNC_TIMER0_TOGGLE3   = 192,    /*!< TIMER0 Toggle Output 3 */
    FUNC_TIMER0_TOGGLE4   = 193,    /*!< TIMER0 Toggle Output 4 */
    FUNC_TIMER1_TOGGLE1   = 194,    /*!< TIMER1 Toggle Output 1 */
    FUNC_TIMER1_TOGGLE2   = 195,    /*!< TIMER1 Toggle Output 2 */
    FUNC_TIMER1_TOGGLE3   = 196,    /*!< TIMER1 Toggle Output 3 */
    FUNC_TIMER1_TOGGLE4   = 197,    /*!< TIMER1 Toggle Output 4 */
    FUNC_TIMER2_TOGGLE1   = 198,    /*!< TIMER2 Toggle Output 1 */
    FUNC_TIMER2_TOGGLE2   = 199,    /*!< TIMER2 Toggle Output 2 */
    FUNC_TIMER2_TOGGLE3   = 200,    /*!< TIMER2 Toggle Output 3 */
    FUNC_TIMER2_TOGGLE4   = 201,    /*!< TIMER2 Toggle Output 4 */
    FUNC_CLK_SPI2         = 202,    /*!< Clock SPI2 */
    FUNC_CLK_I2C2         = 203,    /*!< Clock I2C2 */
    FUNC_INTERNAL0        = 204,    /*!< Internal function signal 0 */
    FUNC_INTERNAL1        = 205,    /*!< Internal function signal 1 */
    FUNC_INTERNAL2        = 206,    /*!< Internal function signal 2 */
    FUNC_INTERNAL3        = 207,    /*!< Internal function signal 3 */
    FUNC_INTERNAL4        = 208,    /*!< Internal function signal 4 */
    FUNC_INTERNAL5        = 209,    /*!< Internal function signal 5 */
    FUNC_INTERNAL6        = 210,    /*!< Internal function signal 6 */
    FUNC_INTERNAL7        = 211,    /*!< Internal function signal 7 */
    FUNC_INTERNAL8        = 212,    /*!< Internal function signal 8 */
    FUNC_INTERNAL9        = 213,    /*!< Internal function signal 9 */
    FUNC_INTERNAL10       = 214,    /*!< Internal function signal 10 */
    FUNC_INTERNAL11       = 215,    /*!< Internal function signal 11 */
    FUNC_INTERNAL12       = 216,    /*!< Internal function signal 12 */
    FUNC_INTERNAL13       = 217,    /*!< Internal function signal 13 */
    FUNC_INTERNAL14       = 218,    /*!< Internal function signal 14 */
    FUNC_INTERNAL15       = 219,    /*!< Internal function signal 15 */
    FUNC_INTERNAL16       = 220,    /*!< Internal function signal 16 */
    FUNC_INTERNAL17       = 221,    /*!< Internal function signal 17 */
    FUNC_CONSTANT         = 222,    /*!< Constant function */
    FUNC_INTERNAL18       = 223,    /*!< Internal function signal 18 */
    FUNC_DEBUG0           = 224,    /*!< Debug function 0 */
    FUNC_DEBUG1           = 225,    /*!< Debug function 1 */
    FUNC_DEBUG2           = 226,    /*!< Debug function 2 */
    FUNC_DEBUG3           = 227,    /*!< Debug function 3 */
    FUNC_DEBUG4           = 228,    /*!< Debug function 4 */
    FUNC_DEBUG5           = 229,    /*!< Debug function 5 */
    FUNC_DEBUG6           = 230,    /*!< Debug function 6 */
    FUNC_DEBUG7           = 231,    /*!< Debug function 7 */
    FUNC_DEBUG8           = 232,    /*!< Debug function 8 */
    FUNC_DEBUG9           = 233,    /*!< Debug function 9 */
    FUNC_DEBUG10          = 234,    /*!< Debug function 10 */
    FUNC_DEBUG11          = 235,    /*!< Debug function 11 */
    FUNC_DEBUG12          = 236,    /*!< Debug function 12 */
    FUNC_DEBUG13          = 237,    /*!< Debug function 13 */
    FUNC_DEBUG14          = 238,    /*!< Debug function 14 */
    FUNC_DEBUG15          = 239,    /*!< Debug function 15 */
    FUNC_DEBUG16          = 240,    /*!< Debug function 16 */
    FUNC_DEBUG17          = 241,    /*!< Debug function 17 */
    FUNC_DEBUG18          = 242,    /*!< Debug function 18 */
    FUNC_DEBUG19          = 243,    /*!< Debug function 19 */
    FUNC_DEBUG20          = 244,    /*!< Debug function 20 */
    FUNC_DEBUG21          = 245,    /*!< Debug function 21 */
    FUNC_DEBUG22          = 246,    /*!< Debug function 22 */
    FUNC_DEBUG23          = 247,    /*!< Debug function 23 */
    FUNC_DEBUG24          = 248,    /*!< Debug function 24 */
    FUNC_DEBUG25          = 249,    /*!< Debug function 25 */
    FUNC_DEBUG26          = 250,    /*!< Debug function 26 */
    FUNC_DEBUG27          = 251,    /*!< Debug function 27 */
    FUNC_DEBUG28          = 252,    /*!< Debug function 28 */
    FUNC_DEBUG29          = 253,    /*!< Debug function 29 */
    FUNC_DEBUG30          = 254,    /*!< Debug function 30 */
    FUNC_DEBUG31          = 255,    /*!< Debug function 31 */
    FUNC_MAX              = 256,    /*!< Function numbers */
} fpioa_function_t;

/**
 * FPIOA IO's register bits Layout
 *
 * | Bits      | Name     |Description                                        |
 * |-----------|----------|---------------------------------------------------|
 * | 31        | PAD_DI   | Read current IO's data input.                     |
 * | 30:24     | NA       | Reserved bits.                                    |
 * | 23        | ST       | Schmitt trigger.                                  |
 * | 22        | DI_INV   | Invert Data input.                                |
 * | 21        | IE_INV   | Invert the input enable signal.                   |
 * | 20        | IE_EN    | Input enable. It can disable or enable IO input.  |
 * | 19        | SL       | Slew rate control enable.                         |
 * | 18        | SPU      | Strong pull up.                                   |
 * | 17        | PD       | Pull select: 0 for pull down, 1 for pull up.      |
 * | 16        | PU       | Pull enable.                                      |
 * | 15        | DO_INV   | Invert the result of data output select (DO_SEL). |
 * | 14        | DO_SEL   | Data output select: 0 for DO, 1 for OE.           |
 * | 13        | OE_INV   | Invert the output enable signal.                  |
 * | 12        | OE_EN    | Output enable.It can disable or enable IO output. |
 * | 11:8      | DS       | Driving selector.                                 |
 * | 7:0       | CH_SEL   | Channel select from 256 input.                    |
 *
 */

typedef struct
{
  volatile uint32_t   IOCFG[FPIOA_NUM_IO];

  volatile uint32_t   TIE_EN[8];
  volatile uint32_t   TIE_VAL[8];
//
} fpioa_regs_t;

//------------------------------------------------------------------------------
// GPIOHS
//------------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t   INPUT_VAL;  // offset 0x00
  volatile uint32_t   INPUT_EN;   // offset 0x04
  volatile uint32_t   OUTPUT_EN;  // offset 0x08
  volatile uint32_t   OUTPUT_VAL; // offset 0x0c
  volatile uint32_t   PULLUP_EN;  // offset 0x10
  volatile uint32_t   DRIVE;      // offset 0x14
  volatile uint32_t   RISE_IE;    // offset 0x18
  volatile uint32_t   RISE_IP;    // offset 0x1c
  volatile uint32_t   FALL_IE;    // offset 0x20
  volatile uint32_t   FALL_IP;    // offset 0x24
  volatile uint32_t   HIGH_IE;    // offset 0x28
  volatile uint32_t   HIGH_IP;    // offset 0x2c
  volatile uint32_t   LOW_IE;     // offset 0x30
  volatile uint32_t   LOW_IP;     // offset 0x34
  volatile uint32_t   IOF_EN;     // offset 0x38
  volatile uint32_t   IOF_SEL;    // offset 0x3c
  volatile uint32_t   OUTPUT_XOR; // offset 0x40
//
} gpiohs_regs_t;

//------------------------------------------------------------------------------
// UART
//------------------------------------------------------------------------------

// the original Kendryte register definition was here suprisingly ok:

typedef struct
{
    union
    {
        volatile uint32_t  RBR;
        volatile uint32_t  DLL;
        volatile uint32_t  THR;
    };

    union
    {
        volatile uint32_t  DLH;
        volatile uint32_t  IER;
    };

    union
    {
        volatile uint32_t  FCR;
        volatile uint32_t  IIR;
    };

    volatile uint32_t  LCR;
    volatile uint32_t  MCR;
    volatile uint32_t  LSR;
    volatile uint32_t  MSR;

    volatile uint32_t  SCR;
    volatile uint32_t  LPDLL;
    volatile uint32_t  LPDLH;

    volatile uint32_t  reserved1[2];

    union
    {
        volatile uint32_t  SRBR[16];
        volatile uint32_t  STHR[16];
    };

    volatile uint32_t  FAR;
    volatile uint32_t  TFR;
    volatile uint32_t  RFW;
    volatile uint32_t  USR;
    volatile uint32_t  TFL;
    volatile uint32_t  RFL;
    volatile uint32_t  SRR;
    volatile uint32_t  SRTS;
    volatile uint32_t  SBCR;
    volatile uint32_t  SDMAM;
    volatile uint32_t  SFE;
    volatile uint32_t  SRT;
    volatile uint32_t  STET;
    volatile uint32_t  HTX;
    volatile uint32_t  DMASA;
    volatile uint32_t  TCR;
    volatile uint32_t  DE_EN;
    volatile uint32_t  RE_EN;
    volatile uint32_t  DET;
    volatile uint32_t  TAT;
    volatile uint32_t  DLF;
    volatile uint32_t  RAR;
    volatile uint32_t  TAR;
    volatile uint32_t  LCR_EXT;
    volatile uint32_t  reserved2[9];
    volatile uint32_t  CPR;
    volatile uint32_t  UCV;
    volatile uint32_t  CTR;
//
} uart_regs_t;

//------------------------------------------------------------------------------
// SPI / QSPI
//------------------------------------------------------------------------------

typedef struct
{

    volatile uint32_t  CTRLR0;           // SPI Control Register 0                                    (0x00)
    volatile uint32_t  CTRLR1;           // SPI Control Register 1                                    (0x04)
    volatile uint32_t  SSIENR;           // SPI Enable Register                                       (0x08)
    volatile uint32_t  MWCR;             // SPI Microwire Control Register                            (0x0c)
    volatile uint32_t  SER;              // SPI Slave Enable Register                                 (0x10)
    volatile uint32_t  BAUDR;            // SPI Baud Rate Select                                      (0x14)
    volatile uint32_t  TXFTLR;           // SPI Baud Rate Select                                      (0x14)
    volatile uint32_t  RXFTLR;           // SPI Receive FIFO Threshold Level                          (0x1c)
    volatile uint32_t  TXFLR;            // SPI Transmit FIFO Level Register                          (0x20)
    volatile uint32_t  RXFLR;            // SPI Receive FIFO Level Register                           (0x24)
    volatile uint32_t  SR;               // SPI Status Register                                       (0x28)
    volatile uint32_t  IMR;              // SPI Interrupt Mask Register                               (0x2c)
    volatile uint32_t  ISR;              // SPI Interrupt Status Register                             (0x30)
    volatile uint32_t  RISR;             // SPI Raw Interrupt Status Register                         (0x34)
    volatile uint32_t  TXOICR;           // SPI Transmit FIFO Overflow Interrupt Clear Register       (0x38)
    volatile uint32_t  RXOICR;           // SPI Receive FIFO Overflow Interrupt Clear Register        (0x3c)
    volatile uint32_t  RXUICR;           // SPI Receive FIFO Underflow Interrupt Clear Register       (0x40)
    volatile uint32_t  MSTICR;           // SPI Multi-Master Interrupt Clear Register                 (0x44)
    volatile uint32_t  ICR;              // SPI Interrupt Clear Register                              (0x48)
    volatile uint32_t  DMACR;            // SPI DMA Control Register                                  (0x4c)
    volatile uint32_t  DMATDLR;          // SPI DMA Transmit Data Level                               (0x50)
    volatile uint32_t  DMARDLR;          // SPI DMA Receive Data Level                                (0x54)
    volatile uint32_t  IDR;              // SPI Identification Register                               (0x58)
    volatile uint32_t  SSIC_VERSION_ID;  // SPI DWC_ssi component version                             (0x5c)
    volatile uint32_t  DR[36];           // SPI Data Register 0-36                                    (0x60 -- 0xec)
    volatile uint32_t  RX_SAMPLE_DELAY;  // SPI RX Sample Delay Register                              (0xf0)
    volatile uint32_t  SPI_CTRLR0;       // SPI SPI Control Register                                  (0xf4)
    volatile uint32_t  RESV;             // reserved                                                  (0xf8)
    volatile uint32_t  XIP_MODE_BITS;    // SPI XIP Mode bits                                         (0xfc)
    volatile uint32_t  XIP_INCR_INST;    // SPI XIP INCR transfer opcode                              (0x100)
    volatile uint32_t  XIP_WRAP_INST;    // SPI XIP WRAP transfer opcode                              (0x104)
    volatile uint32_t  XIP_CTRL;         // SPI XIP Control Register                                  (0x108)
    volatile uint32_t  XIP_SER;          // SPI XIP Slave Enable Register                             (0x10c)
    volatile uint32_t  XRXOICR;          // SPI XIP Receive FIFO Overflow Interrupt Clear Register    (0x110)
    volatile uint32_t  XIP_CNT_TIME_OUT; // SPI XIP time out register for continuous transfers        (0x114)
    volatile uint32_t  ENDIAN;
//
} spi_regs_t;


//------------------------------------------------------------------------------
// Instance definitions
//------------------------------------------------------------------------------

#define FPIOA  ((fpioa_regs_t *)FPIOA_BASE_ADDR)
#define SYSCTL ((sysctl_regs_t *)SYSCTL_BASE_ADDR)
#define GPIOHS ((gpiohs_regs_t *)GPIOHS_BASE_ADDR)
#define UART1  ((uart_regs_t *)UART1_BASE_ADDR)
#define UART2  ((uart_regs_t *)UART2_BASE_ADDR)
#define UART3  ((uart_regs_t *)UART3_BASE_ADDR)

#define SPI0   ((spi_regs_t *)SPI0_BASE_ADDR)
#define SPI1   ((spi_regs_t *)SPI1_BASE_ADDR)
#define SPI3   ((spi_regs_t *)SPI3_BASE_ADDR)


#endif
