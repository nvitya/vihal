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
 *  file:     esp32c6_vihal.h
 *  brief:    VIHAL ESP32C6 register definitions
 *            Inlcuding some original Espressif definitions from ESP-IDF: https://github.com/espressif/esp-idf
 *            The soc defition files are here:
 *               esp-idf/components/soc/esp32c3/include/soc/
 *
 *            I made my own peripheral structs for some peripherals like GPIO
 *  date:     2023-10-02
 *  authors:  nvitya
*/

#ifndef _ESP32C6_VIHAL_H
#define _ESP32C6_VIHAL_H

#include <cstddef>
#include "stdint.h"

//-----------------------------------------------------------------------------
// Espressif definitions
//-----------------------------------------------------------------------------

#include "soc/soc.h"
#include "soc/soc_caps.h"

#include "soc/pcr_reg.h"

//#include "soc/cache_memory.h"
//#include "soc/efuse_reg.h"
//#include "soc/extmem_reg.h"
//#include "soc/fe_reg.h"
#include "soc/gdma_channel.h"
#include "soc/gdma_reg.h"
#include "soc/gpio_sig_map.h"
//#include "soc/hwcrypto_reg.h"
#include "soc/i2c_reg.h"
//#include "soc/i2s_reg.h"
//#include "soc/interrupt_core0_reg.h"
#include "soc/ledc_reg.h"
//#include "soc/nrx_reg.h"
//#include "soc/periph_defs.h"
//#include "soc/rmt_reg.h"
//#include "soc/rtc_cntl_reg.h"
#include "soc/spi_mem_reg.h"
#include "soc/spi_pins.h"
#include "soc/spi_reg.h"
//#include "soc/syscon_reg.h"
//#include "soc/system_reg.h"
#include "soc/systimer_reg.h"
#include "soc/timer_group_reg.h"
#include "soc/uart_channel.h"
#include "soc/uart_reg.h"
#include "soc/uhci_reg.h"
//#include "soc/usb_serial_jtag_reg.h"

//-----------------------------------------------------------------------------
// GPIO
//-----------------------------------------------------------------------------

#define ESP_GPIO_COUNT   31
#define ESP_PAD_COUNT    31

// The ESP32 "pad"-s are referred mostly by their GPIO number, which is different from the package pin number
// And additionally some pads have officially special names, although they also can be used as GPIO

#define PAD_GPIO0        0
#define PAD_GPIO1        1
#define PAD_GPIO2        2
#define PAD_GPIO3        3
#define PAD_GPIO4        4
#define PAD_GPIO5        5
#define PAD_GPIO6        6
#define PAD_GPIO7        7
#define PAD_GPIO8        8
#define PAD_GPIO9        9
#define PAD_GPIO10      10
#define PAD_GPIO11      11
#define PAD_GPIO12      12
#define PAD_GPIO13      13
#define PAD_GPIO14      14
#define PAD_GPIO15      15
#define PAD_GPIO16      16
#define PAD_GPIO17      17
#define PAD_GPIO18      18
#define PAD_GPIO19      19
#define PAD_GPIO20      20
#define PAD_GPIO21      21
#define PAD_GPIO22      22
#define PAD_GPIO23      23
#define PAD_GPIO24      24
#define PAD_GPIO25      25
#define PAD_GPIO26      26
#define PAD_GPIO27      27
#define PAD_GPIO28      28
#define PAD_GPIO29      29
#define PAD_GPIO30      30

// pads with special names

#define PAD_XTAL_32K_P   0
#define PAD_XTAL_32K_N   1
#define PAD_MTMS         4
#define PAD_MTDI         5
#define PAD_MTCK         6
#define PAD_MTDO         7
#define PAD_U0TXD       16
#define PAD_U0RXD       17
#define PAD_SDIO_CMD    18
#define PAD_SDIO_CLK    19
#define PAD_SDIO_DATA0  20
#define PAD_SDIO_DATA1  21
#define PAD_SDIO_DATA2  22
#define PAD_SDIO_DATA3  23
#define PAD_SPICS0      24
#define PAD_SPIQ        25
#define PAD_SPIWP       26
#define PAD_VDD_SPI     27
#define PAD_SPIHD       28
#define PAD_SPICLK      29
#define PAD_SPID        30


typedef struct
{
  volatile uint32_t   REG;
  volatile uint32_t   SET;
  volatile uint32_t   CLR;
//
} esp_gpio_out_t;

typedef struct
{
  volatile uint32_t   REG;
  volatile uint32_t   SET;
  volatile uint32_t   CLR;
//
} esp_gpio_enable_t;

typedef struct
{
  volatile uint32_t   REG;
  volatile uint32_t   SET;
  volatile uint32_t   CLR;
//
} esp_gpio_irqstat_t;

typedef struct
{
  volatile uint32_t      BT_SELECT;    // 0000  BIT SELECT REGISTER
  esp_gpio_out_t         OUT;          // 0004  output (data, set, clear)
  uint32_t               _pad_010[4];
  esp_gpio_enable_t      ENABLE;       // 0020  output enable
  uint32_t               _pad_02C[3];
  volatile uint32_t      STRAP;        // 0038  pin strapping register
  volatile uint32_t      IN[1];        // 003C  input
  uint32_t               _pad_040;
  esp_gpio_irqstat_t     STATUS;       // 0044  interrupt status register
  uint32_t               _pad_050[3];
  volatile uint32_t      PCPU_INT;     // 005C  PRO_CPU interrupt status
  volatile uint32_t      PCPU_NMI_INT; // 0060  PRO_CPU NMI interrupt status
  uint32_t               _pad_064[4];
  volatile uint32_t      PIN[31];      // 0074 configuration
  uint32_t               _pad_0CC[23];
  volatile uint32_t      STATUS_NEXT;  // 014C interrupt source register
  uint32_t               _pad_150;
  volatile uint32_t      FUNC_IN_SEL_CFG[128];  // 0154 Peripheral function n input selection
  uint32_t               _pad_354[128];
  volatile uint32_t      FUNC_OUT_SEL_CFG[31];  // 0554 Peripheral output selection for GPIOn
  uint32_t               _pad_5AC[23];
  volatile uint32_t      CLOCK_GATE;   // 062C GPIO clock gate register
  uint32_t               _pad_630[51];
  volatile uint32_t      DATE;         // 06FC GPIO version register
//
} esp_gpio_t;

#if 1
  // definition check:

  static_assert(offsetof(esp_gpio_t, ENABLE)   == 0x020, "enable offset error");
  static_assert(offsetof(esp_gpio_t, STRAP)    == 0x038, "strap offset error");
  static_assert(offsetof(esp_gpio_t, PCPU_INT) == 0x05C, "pcpu offset error");
  static_assert(offsetof(esp_gpio_t, PIN)      == 0x074, "pin offset error");
  static_assert(offsetof(esp_gpio_t, FUNC_IN_SEL_CFG) == 0x154, "func_in_sel_cfg offset error");
  static_assert(offsetof(esp_gpio_t, FUNC_OUT_SEL_CFG) == 0x554, "func_out_sel_cfg offset error");
#endif

typedef struct
{
  volatile uint32_t   PIN_CTRL;  // Clock output configuration
  volatile uint32_t   CFG[31];   // Configuration register, the order here is same as the GPIO numbering (not like by xtensa)
//
} esp_io_mux_t;

#define IO_MUX     ((esp_io_mux_t *)(DR_REG_IO_MUX_BASE))
#define GPIO         ((esp_gpio_t *)(DR_REG_GPIO_BASE))

//-----------------------------------------------------------------------------
// SYSTEM
//-----------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t      UART0_CONF;                    // 000
  volatile uint32_t      UART0_SCLK_CONF;               // 004
  volatile uint32_t      UART0_PD_CTRL;                 // 008
  volatile uint32_t      UART1_CONF;                    // 00c
  volatile uint32_t      UART1_SCLK_CONF;               // 010
  volatile uint32_t      UART1_PD_CTRL;                 // 014
  volatile uint32_t      MSPI_CONF;                     // 018
  volatile uint32_t      MSPI_CLK_CONF;                 // 01c
  volatile uint32_t      I2C_CONF;                      // 020
  volatile uint32_t      I2C_SCLK_CONF;                 // 024
  volatile uint32_t      UHCI_CONF;                     // 028
  volatile uint32_t      RMT_CONF;                      // 02c
  volatile uint32_t      RMT_SCLK_CONF;                 // 030
  volatile uint32_t      LEDC_CONF;                     // 034
  volatile uint32_t      LEDC_SCLK_CONF;                // 038
  volatile uint32_t      TIMERGROUP0_CONF;              // 03c
  volatile uint32_t      TIMERGROUP0_TIMER_CLK_CONF;    // 040
  volatile uint32_t      TIMERGROUP0_WDT_CLK_CONF;      // 044
  volatile uint32_t      TIMERGROUP1_CONF;              // 048
  volatile uint32_t      TIMERGROUP1_TIMER_CLK_CONF;    // 04c
  volatile uint32_t      TIMERGROUP1_WDT_CLK_CONF;      // 050
  volatile uint32_t      SYSTIMER_CONF;                 // 054
  volatile uint32_t      SYSTIMER_FUNC_CLK_CONF;        // 058
  volatile uint32_t      TWAI0_CONF;                    // 05c
  volatile uint32_t      TWAI0_FUNC_CLK_CONF;           // 060
  volatile uint32_t      TWAI1_CONF;                    // 064
  volatile uint32_t      TWAI1_FUNC_CLK_CONF;           // 068
  volatile uint32_t      I2S_CONF;                      // 06c
  volatile uint32_t      I2S_TX_CLKM_CONF;              // 070
  volatile uint32_t      I2S_TX_CLKM_DIV_CONF;          // 074
  volatile uint32_t      I2S_RX_CLKM_CONF;              // 078
  volatile uint32_t      I2S_RX_CLKM_DIV_CONF;          // 07c
  volatile uint32_t      SARADC_CONF;                   // 080
  volatile uint32_t      SARADC_CLKM_CONF;              // 084
  volatile uint32_t      TSENS_CLK_CONF;                // 088
  volatile uint32_t      USB_DEVICE_CONF;               // 08c
  volatile uint32_t      INTMTX_CONF;                   // 090
  volatile uint32_t      PCNT_CONF;                     // 094
  volatile uint32_t      ETM_CONF;                      // 098
  volatile uint32_t      PWM_CONF;                      // 09c
  volatile uint32_t      PWM_CLK_CONF;                  // 0a0
  volatile uint32_t      PARL_IO_CONF;                  // 0a4
  volatile uint32_t      PARL_CLK_RX_CONF;              // 0a8
  volatile uint32_t      PARL_CLK_TX_CONF;              // 0ac
  volatile uint32_t      SDIO_SLAVE_CONF;               // 0b0
  volatile uint32_t      PVT_MONITOR_CONF;              // 0b4
  volatile uint32_t      PVT_MONITOR_FUNC_CLK_CONF;     // 0b8
  volatile uint32_t      GDMA_CONF;                     // 0bc
  volatile uint32_t      SPI2_CONF;                     // 0c0
  volatile uint32_t      SPI2_CLKM_CONF;                // 0c4
  volatile uint32_t      AES_CONF;                      // 0c8
  volatile uint32_t      SHA_CONF;                      // 0cc
  volatile uint32_t      RSA_CONF;                      // 0d0
  volatile uint32_t      RSA_PD_CTRL;                   // 0d4
  volatile uint32_t      ECC_CONF;                      // 0d8
  volatile uint32_t      ECC_PD_CTRL;                   // 0dc
  volatile uint32_t      DS_CONF;                       // 0e0
  volatile uint32_t      HMAC_CONF;                     // 0e4
  volatile uint32_t      IOMUX_CONF;                    // 0e8
  volatile uint32_t      IOMUX_CLK_CONF;                // 0ec
  volatile uint32_t      MEM_MONITOR_CONF;              // 0f0
  volatile uint32_t      REGDMA_CONF;                   // 0f4
  volatile uint32_t      RETENTION_CONF;                // 0f8
  volatile uint32_t      TRACE_CONF;                    // 0fc
  volatile uint32_t      ASSIST_CONF;                   // 100
  volatile uint32_t      CACHE_CONF;                    // 104
  volatile uint32_t      MODEM_APB_CONF;                // 108
  volatile uint32_t      TIMEOUT_CONF;                  // 10c
  volatile uint32_t      SYSCLK_CONF;                   // 110
  volatile uint32_t      CPU_WAITI_CONF;                // 114
  volatile uint32_t      CPU_FREQ_CONF;                 // 118
  volatile uint32_t      AHB_FREQ_CONF;                 // 11c
  volatile uint32_t      APB_FREQ_CONF;                 // 120
  volatile uint32_t      SYSCLK_FREQ_QUERY_0;           // 124
  volatile uint32_t      PLL_DIV_CLK_EN;                // 128
  volatile uint32_t      CTRL_CLK_OUT_EN;               // 12c
  volatile uint32_t      CTRL_TICK_CONF;                // 130
  volatile uint32_t      CTRL_32K_CONF;                 // 134
  volatile uint32_t      SRAM_POWER_CONF;               // 138
           uint32_t      _pad_13C[(0xFF0-0x138) / 4];
  volatile uint32_t      RESET_EVENT_BYPASS;            // ff0
  volatile uint32_t      FPGA_DEBUG;                    // ff4
  volatile uint32_t      CLOCK_GATE;                    // ff8
  volatile uint32_t      DATE;                          // ffc
//
} esp_pcr_t;

#define PCR      ((esp_pcr_t *)(DR_REG_PCR_BASE))

//-----------------------------------------------------------------------------
// SYSTIMER
//-----------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t  HI;
  volatile uint32_t  LO;
//
} esp_systimer_reg64_t;

typedef struct
{
  volatile uint32_t     CONF;           // 000
  volatile uint32_t     OP[2];          // 004 - 008
  esp_systimer_reg64_t  LOAD_VAL[2];    // 00C - 018
  esp_systimer_reg64_t  TARGET_VAL[3];  //
  volatile uint32_t     TARGET_CONF[3]; //
  esp_systimer_reg64_t  UNIT_VALUE[2];  //

  volatile uint32_t     COMP_LOAD[3];
  volatile uint32_t     UNIT_LOAD[2];
  volatile uint32_t     INT_ENA;
  volatile uint32_t     INT_RAW;
  volatile uint32_t     INT_CLR;
  volatile uint32_t     INT_ST;         // 070

  uint32_t              _pad_074[34];
  volatile uint32_t     DATE;           // 0FC
//
} esp_systimer_t;

#define SYS_TIMER ((esp_systimer_t *)(DR_REG_SYSTIMER_BASE))

//-----------------------------------------------------------------------------
// Timer Group (TIMG0-1)
//-----------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t     T0CONFIG;        // 0x0000
  volatile uint32_t     T0LO;            // 0x0004
  volatile uint32_t     T0HI;            // 0x0008
  volatile uint32_t     T0UPDATE;        // 0x000c
  volatile uint32_t     T0ALARMLO;       // 0x0010
  volatile uint32_t     T0ALARMHI;       // 0x0014
  volatile uint32_t     T0LOADLO;        // 0x0018
  volatile uint32_t     T0LOADHI;        // 0x001c
  volatile uint32_t     T0LOAD;          // 0x0020
  uint32_t              _pad_0024[9];    //     24
  volatile uint32_t     WDTCONFIG0;      // 0x0048
  volatile uint32_t     WDTCONFIG1;      // 0x004c
  volatile uint32_t     WDTCONFIG2;      // 0x0050
  volatile uint32_t     WDTCONFIG3;      // 0x0054
  volatile uint32_t     WDTCONFIG4;      // 0x0058
  volatile uint32_t     WDTCONFIG5;      // 0x005c
  volatile uint32_t     WDTFEED;         // 0x0060
  volatile uint32_t     WDTWPROTECT;     // 0x0064
  volatile uint32_t     RTCCALICFG;      // 0x0068
  volatile uint32_t     RTCCALICFG1;     // 0x006c
  volatile uint32_t     INT_ENA_TIMERS;  // 0x0070
  volatile uint32_t     INT_RAW_TIMERS;  // 0x0074
  volatile uint32_t     INT_ST_TIMERS;   // 0x0078
  volatile uint32_t     INT_CLR_TIMERS;  // 0x007c
  volatile uint32_t     RTCCALICFG2;     // 0x0080
  uint32_t              _pad_0084[29];   //     84
  volatile uint32_t     NTIMERS_DATE;    // 0x00f8
  volatile uint32_t     CLK;             // 0x00fc
//
} esp_timg_t;

#define TIMG0  ((esp_timg_t *)(DR_REG_TIMERGROUP0_BASE))
#define TIMG1  ((esp_timg_t *)(DR_REG_TIMERGROUP1_BASE))

//-----------------------------------------------------------------------------
// UART
//-----------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t  FIFO;                  // 00
  volatile uint32_t  INT_RAW;               // 04
  volatile uint32_t  INT_ST;                // 08
  volatile uint32_t  INT_ENA;               // 0c
  volatile uint32_t  INT_CLR;               // 10
  volatile uint32_t  CLKDIV_SYNC;           // 14
  volatile uint32_t  RX_FILT;               // 18
  volatile uint32_t  STATUS;                // 1c
  volatile uint32_t  CONF0_SYNC;            // 20
  volatile uint32_t  CONF1;                 // 24
           uint32_t  _pad_28;
  volatile uint32_t  HWFC_CONF_SYNC;        // 2c
  volatile uint32_t  SLEEP_CONF0;           // 30
  volatile uint32_t  SLEEP_CONF1;           // 34
  volatile uint32_t  SLEEP_CONF2;           // 38
  volatile uint32_t  SWFC_CONF0_SYNC;       // 3c
  volatile uint32_t  SWFC_CONF1;            // 40
  volatile uint32_t  TXBRK_CONF_SYNC;       // 44
  volatile uint32_t  IDLE_CONF_SYNC;        // 48
  volatile uint32_t  RS485_CONF_SYNC;       // 4c
  volatile uint32_t  AT_CMD_PRECNT_SYNC;    // 50
  volatile uint32_t  AT_CMD_POSTCNT_SYNC;   // 54
  volatile uint32_t  AT_CMD_GAPTOUT_SYNC;   // 58
  volatile uint32_t  AT_CMD_CHAR_SYNC;      // 5c
  volatile uint32_t  MEM_CONF;              // 60
  volatile uint32_t  TOUT_CONF_SYNC;        // 64
  volatile uint32_t  MEM_TX_STATUS;         // 68
  volatile uint32_t  MEM_RX_STATUS;         // 6c
  volatile uint32_t  FSM_STATUS;            // 70
  volatile uint32_t  POSPULSE;              // 74
  volatile uint32_t  NEGPULSE;              // 78
  volatile uint32_t  LOWPULSE;              // 7c
  volatile uint32_t  HIGHPULSE;             // 80
  volatile uint32_t  RXD_CNT;               // 84
  volatile uint32_t  CLK_CONF;              // 88
  volatile uint32_t  DATE;                  // 8c
  volatile uint32_t  AFIFO_STATUS;          // 90
           uint32_t  _pad_94;
  volatile uint32_t  REG_UPDATE;            // 98
  volatile uint32_t  ID;                    // 9c
//
} esp_uart_t;

typedef struct
{
  volatile uint32_t  WORD[2];
//
} esp_uhci_reg_q_t;

typedef struct
{
  volatile uint32_t  CONF0;         // 0000
  volatile uint32_t  INT_RAW;       // 0004
  volatile uint32_t  INT_ST;        // 0008
  volatile uint32_t  INT_ENA;       // 000C
  volatile uint32_t  INT_CLR;       // 0010
  volatile uint32_t  CONF1;         // 0014
  volatile uint32_t  STATE0;        // 0018
  volatile uint32_t  STATE1;        // 001C
  volatile uint32_t  ESCAPE_CONF;   // 0020
  volatile uint32_t  HUNG_CONF;     // 0024
  volatile uint32_t  ACK_NUM;       // 0028
  volatile uint32_t  RX_HEAD;       // 002C
  volatile uint32_t  QUICK_SENT;    // 0030
  esp_uhci_reg_q_t   REG_Q[6];      // 0034
  volatile uint32_t  ESC_CONF[4];   // 006C
  volatile uint32_t  PKT_THRES;     // 007C
  volatile uint32_t  DATE;          // 0080
//
} esp_uhci_t;

#define UART0     ((esp_uart_t *)(DR_REG_UART_BASE))
#define UART1     ((esp_uart_t *)(DR_REG_UART1_BASE))

#define UHCI0     ((esp_uhci_t *)(DR_REG_UHCI0_BASE))

//-----------------------------------------------------------------------------
// SPI, applies only to SPI2
//-----------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t CMD;          // 0x00
  volatile uint32_t ADDR;         // 0x04
  volatile uint32_t CTRL;         // 0x08
  volatile uint32_t CLOCK;        // 0x0C
  volatile uint32_t USER;         // 0x10
  volatile uint32_t USER1;        // 0x14
  volatile uint32_t USER2;        // 0x18
  volatile uint32_t MS_DLEN;      // 0x1C
  volatile uint32_t MISC;         // 0x20
  volatile uint32_t DIN_MODE;     // 0x24
  volatile uint32_t DIN_NUM;      // 0x28
  volatile uint32_t DOUT_MODE;    // 0x2C
  volatile uint32_t DMA_CONF;     // 0x30
  volatile uint32_t DMA_INT_ENA;  // 0x34
  volatile uint32_t DMA_INT_CLR;  // 0x38
  volatile uint32_t DMA_INT_RAW;  // 0x3C
  volatile uint32_t DMA_INT_ST;   // 0x40
           uint32_t _pad_44[(0x98-0x44) / 4];
  volatile uint32_t W[16];        // 0x98 .. 0xD4
           uint32_t _pad_D8;
           uint32_t _pad_DC;
  volatile uint32_t SLAVE;        // 0xE0
  volatile uint32_t SLAVE1;       // 0xE4
  volatile uint32_t CLK_GATE;     // 0xE8
           uint32_t _pad_EC;
  volatile uint32_t DATE;         // 0xF0
//
} esp_spi_t;

#define SPI2     ((esp_spi_t *)(DR_REG_SPI2_BASE))

//-----------------------------------------------------------------------------
// SPI MEM = SPI1, non-documented, extracted from the register definitions
//-----------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t CMD;               // 0x000
  volatile uint32_t ADDR;              // 0x004
  volatile uint32_t CTRL;              // 0x008
  volatile uint32_t CTRL1;             // 0x00C
  volatile uint32_t CTRL2;             // 0x010
  volatile uint32_t CLOCK;             // 0x014
  volatile uint32_t USER;              // 0x018
  volatile uint32_t USER1;             // 0x01C
  volatile uint32_t USER2;             // 0x020
  volatile uint32_t MOSI_DLEN;         // 0x024
  volatile uint32_t MISO_DLEN;         // 0x028
  volatile uint32_t RD_STATUS;         // 0x02C
           uint32_t _pad_030;          // 0x030
  volatile uint32_t MISC;              // 0x034
  volatile uint32_t TX_CRC;            // 0x038
  volatile uint32_t CACHE_FCTRL;       // 0x03C
           uint32_t _pad_040[(0x54-0x40) / 4];
  volatile uint32_t FSM;               // 0x054
  volatile uint32_t W[16];             // 0x058 .. 0x094
  volatile uint32_t FLASH_WAITI_CTRL;  // 0x098
  volatile uint32_t FLASH_SUS_CTRL;    // 0x09C
  volatile uint32_t FLASH_SUS_CMD;     // 0x0A0
  volatile uint32_t SUS_STATUS;        // 0x0A4
  volatile uint32_t TIMING_CALI;       // 0x0A8
  volatile uint32_t DIN_MODE;          // 0x0AC
  volatile uint32_t DIN_NUM;           // 0x0B0
  volatile uint32_t DOUT_MODE;         // 0x0B4
           uint32_t _pad_0B8[2];
  volatile uint32_t INT_ENA;           // 0x0C0
  volatile uint32_t INT_CLR;           // 0x0C4
  volatile uint32_t INT_RAW;           // 0x0C8
  volatile uint32_t INT_ST;            // 0x0CC
           uint32_t _pad_0D0[3];
  volatile uint32_t CLOCK_GATE;        // 0x0DC
  volatile uint32_t CORE_CLK_SEL;      // 0x0E0
           uint32_t _pad_0E4[(0x3FC-0x0E4) / 4];
  volatile uint32_t DATE;              // 0x3FC
//
} esp_spi_mem_t;

#if 1
  // definition check:

  static_assert(offsetof(esp_spi_mem_t, CLOCK_GATE)      == 0x0DC, "offset error");
  static_assert(offsetof(esp_spi_mem_t, CORE_CLK_SEL)    == 0x0E0, "offset error");
#endif

#define SPIMEM0   ((esp_spi_mem_t *)(DR_REG_SPI0_BASE))
#define SPIMEM1   ((esp_spi_mem_t *)(DR_REG_SPI1_BASE))

//-----------------------------------------------------------------------------
// USB SERIAL JTAG
//-----------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t  EP1;         // 0x00
  volatile uint32_t  EP1_CONF;    // 0x04
  volatile uint32_t  INT_RAW;     // 0x08
  volatile uint32_t  INT_ST;      // 0x0c
  volatile uint32_t  INT_ENA;     // 0x10
  volatile uint32_t  INT_CLR;     // 0x14
  volatile uint32_t  CONF0;       // 0x18
  volatile uint32_t  TEST;        // 0x1c
  volatile uint32_t  JFIFO_ST;    // 0x20
  volatile uint32_t  FRAM_NUM;    // 0x24
  volatile uint32_t  IN_EP0_ST;   // 0x28
  volatile uint32_t  IN_EP1_ST;   // 0x2c
  volatile uint32_t  IN_EP2_ST;   // 0x30
  volatile uint32_t  IN_EP3_ST;   // 0x34
  volatile uint32_t  OUT_EP0_ST;  // 0x38
  volatile uint32_t  OUT_EP1_ST;  // 0x3c
  volatile uint32_t  OUT_EP2_ST;  // 0x40
  volatile uint32_t  MISC_CONF;   // 0x44
  volatile uint32_t  MEM_CONF;    // 0x48
           uint32_t  _pad[(0x80 - 0x4C) / 4];
  volatile uint32_t  DATE;        // 0x80
//
} esp_usb_serial_jtag_t;

#define USB_SERIAL_JTAG  ((esp_usb_serial_jtag_t *)(DR_REG_USB_SERIAL_JTAG_BASE))

//-----------------------------------------------------------------------------
// GDMA
//-----------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t  RAW;
  volatile uint32_t  ST;
  volatile uint32_t  ENA;
  volatile uint32_t  CLR;
//
} esp_dmach_intr_t;

typedef struct
{
  volatile uint32_t  CONF0;              // 0x070
  volatile uint32_t  CONF1;              // 0x074
  volatile uint32_t  FIFO_STATUS;        // 0x078
  volatile uint32_t  POP_PUSH;           // 0x07C
  volatile uint32_t  LINK;               // 0x080
  volatile uint32_t  STATE;              // 0x084
  volatile uint32_t  SUC_EOF_DES_ADDR;   // 0x088  OUT name: EOF_DES_ADDR
  volatile uint32_t  ERR_EOF_DES_ADDR;   // 0x08C  OUT name: EOF_BFR_DES_ADDR
  volatile uint32_t  DSCR;               // 0x090
  volatile uint32_t  DSCR_BF0;           // 0x094
  volatile uint32_t  DSCR_BF1;           // 0x098
  volatile uint32_t  PRI;                // 0x09C
  volatile uint32_t  PERI_SEL;           // 0x0A0
           uint32_t  _pad[11];
//
} esp_dmainout_t;

typedef struct
{
  esp_dmainout_t     IN;
  esp_dmainout_t     OUT;
//
} esp_dmach_t;

typedef struct
{
  esp_dmach_intr_t      INTR[3];          // 0x000 - 0x030

  uint32_t  _pad_030[(0x40 - 0x30) / 4];  // 0x030

  volatile uint32_t     AHB_TEST;         // 0x040
  volatile uint32_t     MISC_CONF;        // 0x044
  volatile uint32_t     DATE;             // 0x048

  uint32_t  _pad_04C[(0x70 - 0x4C) / 4];  // 0x04C

  esp_dmach_t           CH[3];            // 0x070
//
} esp_gdma_t;

#define GDMA  ((esp_gdma_t *)(DR_REG_GDMA_BASE))

#define GDMA_PERI_SEL_SPI2     0
#define GDMA_PERI_SEL_UHCI0    2
#define GDMA_PERI_SEL_I2S      3
#define GDMA_PERI_SEL_AES      6
#define GDMA_PERI_SEL_SHA      7
#define GDMA_PERI_SEL_ADC      8
#define GDMA_PERI_SEL_PARLIO0  9

#define GDMA_MAX_CHANNELS    3

typedef struct
{
  uint32_t   size_len_flags;
  uint32_t   bufaddr;
  uint32_t   next_desc;
//
} esp_dmadesc_t;

#define DMADESC_FLAG_OWN      (1u << 31)
#define DMADESC_FLAG_SUC_EOF  (1u << 30)
#define DMADESC_FLAG_ERR_EOF  (1u << 28)
#define DMADESC_SIZE_POS       0
#define DMADESC_LEN_POS       12

//-----------------------------------------------------------------------------
// I2C
//-----------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t  SCL_LOW_PERIOD;          // 0x00
  volatile uint32_t  CTR;                     // 0x04
  volatile uint32_t  SR;                      // 0x08
  volatile uint32_t  TO;                      // 0x0C
  volatile uint32_t  SLAVE_ADDR;              // 0x10
  volatile uint32_t  FIFO_ST;                 // 0x14
  volatile uint32_t  FIFO_CONF;               // 0x18
  volatile uint32_t  DATA;                    // 0x1C
  volatile uint32_t  INT_RAW;                 // 0x20
  volatile uint32_t  INT_CLR;                 // 0x24
  volatile uint32_t  INT_ENA;                 // 0x28
  volatile uint32_t  INT_STATUS;              // 0x2C
  volatile uint32_t  SDA_HOLD;                // 0x30
  volatile uint32_t  SDA_SAMPLE;              // 0x34
  volatile uint32_t  SCL_HIGH_PERIOD;         // 0x38
           uint32_t  _pad_3C;                 // 0x3C
  volatile uint32_t  SCL_START_HOLD;          // 0x40
  volatile uint32_t  SCL_RSTART_SETUP;        // 0x44
  volatile uint32_t  SCL_STOP_HOLD;           // 0x48
  volatile uint32_t  SCL_STOP_SETUP;          // 0x4C
  volatile uint32_t  FILTER_CFG;              // 0x50
  volatile uint32_t  CLK_CONF;                // 0x54
  volatile uint32_t  COMD[8];                 // 0x58 - 0x78
  volatile uint32_t  SCL_ST_TIME_OUT;         // 0x78
  volatile uint32_t  SCL_MAIN_ST_TIME_OUT;    // 0x7C
  volatile uint32_t  SCL_SP_CONF;             // 0x80
  volatile uint32_t  SCL_STRETCH_CONF;        // 0x84
           uint32_t  _pad_88[(0xF8-0x88)/4];  // 0x88
  volatile uint32_t  DATE;                    // 0xF8
           uint32_t  _pad_FC;                 // 0xFC
  volatile uint32_t  TXFIFO_START_ADDR[32];   // 0x100
  volatile uint32_t  RXFIFO_START_ADDR[32];   // 0x180
//
} esp_i2c_regs_t;

#if 1
  // definition check:
  static_assert(offsetof(esp_i2c_regs_t, TXFIFO_START_ADDR)  == 0x100, "offset error");
  static_assert(offsetof(esp_i2c_regs_t, RXFIFO_START_ADDR)  == 0x180, "offset error");
#endif

#define I2C0  ((esp_i2c_regs_t *)(DR_REG_I2C_EXT_BASE))

//-----------------------------------------------------------------------------
// TWAI = CAN
//-----------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t  MODE;                 // 00
  volatile uint32_t  CMD;                  // 04
  volatile uint32_t  STATUS;               // 08
  volatile uint32_t  INT_RAW;              // 0C
  volatile uint32_t  INT_ENA;              // 10
  volatile uint32_t  _pad_14;              // 14
  volatile uint32_t  BUS_TIMING_0;         // 18
  volatile uint32_t  BUS_TIMING_1;         // 1C
  uint32_t  _pad_20[(0x2C-0x20)/4];        // 20
  volatile uint32_t  ARB_LOST_CAP;         // 2C
  volatile uint32_t  ERR_CODE_CAP;         // 30
  volatile uint32_t  ERR_WARNING_LIMIT;    // 34
  volatile uint32_t  RX_ERR_CNT;           // 38
  volatile uint32_t  TX_ERR_CNT;           // 3C

  volatile uint32_t  DATA[13];             // 40 - 74

  volatile uint32_t  RX_MESSAGE_CNT;       // 74
           uint32_t  _pad_78;              // 78
  volatile uint32_t  CLOCK_DIVIDER;        // 7C
//
} esp_twai_regs_t;

#if 1
  // definition check:
  static_assert(offsetof(esp_twai_regs_t, CLOCK_DIVIDER)  == 0x7C, "offset error");
#endif

#define TWAI0  ((esp_twai_regs_t *)(DR_REG_TWAI0_BASE))
#define TWAI1  ((esp_twai_regs_t *)(DR_REG_TWAI1_BASE))

//-----------------------------------------------------------------------------
// SARADC
//-----------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t  CTRL;                 // 0x000
  volatile uint32_t  CTRL2;                // 0x004
  volatile uint32_t  FILTER_CTRL1;         // 0x008
  volatile uint32_t  FSM_WAIT;             // 0x00C
  volatile uint32_t  SAR1_STATUS;          // 0x010
  volatile uint32_t  SAR2_STATUS;          // 0x014
  volatile uint32_t  SAR_PATT_TAB1;        // 0x018
  volatile uint32_t  SAR_PATT_TAB2;        // 0x01C
  volatile uint32_t  ONETIME_SAMPLE;       // 0x020
  volatile uint32_t  APB_ADC_ARB_CTRL;     // 0x024
  volatile uint32_t  FILTER_CTRL0;         // 0x028
  volatile uint32_t  DATA_STATUS_1;        // 0x02C  (APB_SARADC_1_DATA_STATUS_REG)
  volatile uint32_t  DATA_STATUS_2;        // 0x030  (APB_SARADC_2_DATA_STATUS_REG)
  volatile uint32_t  THRES0_CTRL;          // 0x034
  volatile uint32_t  THRES1_CTRL;          // 0x038
  volatile uint32_t  THRES_CTRL;           // 0x03C
  volatile uint32_t  INT_ENA;              // 0x040
  volatile uint32_t  INT_RAW;              // 0x044
  volatile uint32_t  INT_ST;               // 0x048
  volatile uint32_t  INT_CLR;              // 0x04C
  volatile uint32_t  DMA_CONF;             // 0x050
  volatile uint32_t  APB_ADC_CLKM_CONF;    // 0x054
  volatile uint32_t  APB_TSENS_CTRL;       // 0x058
  volatile uint32_t  APB_TSENS_CTRL2;      // 0x05C
  volatile uint32_t  CALI;                 // 0x060
  uint32_t  _pad_064[(0x3FC-0x64)/4];      // 0x064
  volatile uint32_t  APB_CTRL_DATE;        // 0x3FC
//
} esp_saradc_regs_t;

#define SARADC  ((esp_saradc_regs_t *)(DR_REG_APB_SARADC_BASE))

//-----------------------------------------------------------------------------
// LEDC / LED PWM
//-----------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t  LSCH0_CONF0;      // 0x00
  volatile uint32_t  LSCH0_HPOINT;     // 0x04
  volatile uint32_t  LSCH0_DUTY;       // 0x08
  volatile uint32_t  LSCH0_CONF1;      // 0x0C
  volatile uint32_t  LSCH0_DUTY_R;     // 0x10
  volatile uint32_t  LSCH1_CONF0;      // 0x14
  volatile uint32_t  LSCH1_HPOINT;     // 0x18
  volatile uint32_t  LSCH1_DUTY;       // 0x1C
  volatile uint32_t  LSCH1_CONF1;      // 0x20
  volatile uint32_t  LSCH1_DUTY_R;     // 0x24
  volatile uint32_t  LSCH2_CONF0;      // 0x28
  volatile uint32_t  LSCH2_HPOINT;     // 0x2C
  volatile uint32_t  LSCH2_DUTY;       // 0x30
  volatile uint32_t  LSCH2_CONF1;      // 0x34
  volatile uint32_t  LSCH2_DUTY_R;     // 0x38
  volatile uint32_t  LSCH3_CONF0;      // 0x3C
  volatile uint32_t  LSCH3_HPOINT;     // 0x40
  volatile uint32_t  LSCH3_DUTY;       // 0x44
  volatile uint32_t  LSCH3_CONF1;      // 0x48
  volatile uint32_t  LSCH3_DUTY_R;     // 0x4C
  volatile uint32_t  LSCH4_CONF0;      // 0x50
  volatile uint32_t  LSCH4_HPOINT;     // 0x54
  volatile uint32_t  LSCH4_DUTY;       // 0x58
  volatile uint32_t  LSCH4_CONF1;      // 0x5C
  volatile uint32_t  LSCH4_DUTY_R;     // 0x60
  volatile uint32_t  LSCH5_CONF0;      // 0x64
  volatile uint32_t  LSCH5_HPOINT;     // 0x68
  volatile uint32_t  LSCH5_DUTY;       // 0x6C
  volatile uint32_t  LSCH5_CONF1;      // 0x70
  volatile uint32_t  LSCH5_DUTY_R;     // 0x74
  uint32_t  _pad_78[(0xA0-0x78)/4];    // 0x78
  volatile uint32_t  LSTIMER0_CONF;    // 0xA0
  volatile uint32_t  LSTIMER0_VALUE;   // 0xA4
  volatile uint32_t  LSTIMER1_CONF;    // 0xA8
  volatile uint32_t  LSTIMER1_VALUE;   // 0xAC
  volatile uint32_t  LSTIMER2_CONF;    // 0xB0
  volatile uint32_t  LSTIMER2_VALUE;   // 0xB4
  volatile uint32_t  LSTIMER3_CONF;    // 0xB8
  volatile uint32_t  LSTIMER3_VALUE;   // 0xBC
  volatile uint32_t  INT_RAW;          // 0xC0
  volatile uint32_t  INT_ST;           // 0xC4
  volatile uint32_t  INT_ENA;          // 0xC8
  volatile uint32_t  INT_CLR;          // 0xCC
  volatile uint32_t  CONF;             // 0xD0
  uint32_t  _pad_D4[(0xFC-0xD4)/4];    // 0xD4
  volatile uint32_t  DATE;             // 0xFC
//
} esp_ledc_regs_t;

#define LEDC  ((esp_ledc_regs_t *)(DR_REG_LEDC_BASE))





//-----------------------------------------------------------------------------
// Miscellaneous
//-----------------------------------------------------------------------------

typedef struct
{
    uint8_t   magic;             // Magic word ESP_IMAGE_HEADER_MAGIC = 0xE9
    uint8_t   segment_count;     // Count of memory segments
    uint8_t   spi_mode;          // flash read mode, 2 = DIO
    uint8_t   spi_speed_size;    // low 4 bits: speed, 0 = 40 MHz;  high 4 bits: size, 2 = 4MB
    uint32_t  entry_addr;        // Entry address
    uint8_t   wp_pin;            // 0xEE = disabled
    uint8_t   spi_pin_drv[3];    // Drive settings for the SPI flash pins (read by ROM bootloader)
    uint16_t  chip_id;           // Chip identification number, 0x0005 = ESP32c3
    uint8_t   min_chip_rev;      // Minimum chip revision supported by image
    uint8_t   reserved[8];       // Reserved bytes in additional header space, currently unused
    uint8_t   hash_appended;     // If 1, a SHA256 digest "simple hash" (of the entire image) is appended after the checksum.
    // first segment
    uint32_t  load_addr;         // load address of the first segment
    uint32_t  data_len;          // size of the first segment
//
} vihal_esp_image_header_t;

// a valid (second stage bootloader) example at flash address 0:
//   E9 03 02 20 00 E0 3C 40 EE 00 00 00 05 00 00 00
//   00 00 00 00 00 00 00 00 00 61 CD 3F 8C 03 00 00

#endif // ifndef _ESP32C6_VIHAL_H
