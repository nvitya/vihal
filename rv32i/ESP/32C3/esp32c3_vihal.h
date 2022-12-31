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
 *  file:     esp32c3_vihal.h
 *  brief:    VIHAL ESP32C3 register definitions
 *            Inlcuding some original Espressif definitions from ESP-IDF: https://github.com/espressif/esp-idf
 *            The soc defition files are here:
 *               esp-idf/components/soc/esp32c3/include/soc/
 *
 *            I made my own peripheral structs for some peripherals like GPIO
 *  version:  1.00
 *  date:     2022-02-03
 *  authors:  nvitya
*/

#pragma once

#include <cstddef>
#include "stdint.h"

//-----------------------------------------------------------------------------
// GPIO
//-----------------------------------------------------------------------------

#define ESP_GPIO_COUNT   22
#define ESP_PAD_COUNT    22

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

// pads with special names

#define PAD_XTAL_32K_P   0
#define PAD_XTAL_32K_N   1
#define PAD_MTMS         4
#define PAD_MTDI         5
#define PAD_MTCK         6
#define PAD_MTDO         7
#define PAD_VDD_SPI     11
#define PAD_SPIHD       12
#define PAD_SPIWP       13
#define PAD_SPICS0      14
#define PAD_SPICLK      15
#define PAD_SPID        16
#define PAD_SPIQ        17
#define PAD_GPIO18      18
#define PAD_GPIO19      19
#define PAD_U0RXD       20
#define PAD_U0TXD       21

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
  volatile uint32_t      PIN[22];      // 0074 configuration
  uint32_t               _pad_0CC[32];
  volatile uint32_t      STATUS_NEXT;  // 014C interrupt source register
  uint32_t               _pad_150;
  volatile uint32_t      FUNC_IN_SEL_CFG[128];  // 0154 Peripheral function n input selection
  uint32_t               _pad_354[128];
  volatile uint32_t      FUNC_OUT_SEL_CFG[22];  // 0554 Peripheral output selection for GPIOn
  uint32_t               _pad_5AC[32];
  volatile uint32_t      CLOCK_GATE;   // 062C GPIO clock gate register
  uint32_t               _pad_630[51];
  volatile uint32_t      DATE;         // 06FC GPIO version register
//
} esp_gpio_t;

#if 1
  // definition check:

  static_assert(offsetof(esp_gpio_t, ENABLE)    == 0x020, "enable offset error");
  static_assert(offsetof(esp_gpio_t, STRAP)    == 0x038, "strap offset error");
  static_assert(offsetof(esp_gpio_t, PCPU_INT) == 0x05C, "pcpu offset error");
  static_assert(offsetof(esp_gpio_t, PIN)      == 0x074, "pin offset error");
  static_assert(offsetof(esp_gpio_t, FUNC_IN_SEL_CFG) == 0x154, "func_in_sel_cfg offset error");
  static_assert(offsetof(esp_gpio_t, FUNC_OUT_SEL_CFG) == 0x554, "func_out_sel_cfg offset error");
#endif

typedef struct
{
  volatile uint32_t   PIN_CTRL;  // Clock output configuration
  volatile uint32_t   CFG[22];   // Configuration register, the order here is same as the GPIO numbering (not like by xtensa)
//
} esp_io_mux_t;

#define GPIO         ((esp_gpio_t *)(0x60004000))
#define IO_MUX     ((esp_io_mux_t *)(0x60009000))

//-----------------------------------------------------------------------------
// SYSTEM
//-----------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t      CPU_PERI_CLK_EN;    // 0000
  volatile uint32_t      CPU_PERI_RST_EN;    // 0004
  volatile uint32_t      CPU_PER_CONF;       // 0008
  uint32_t               _pad_000C;
  volatile uint32_t      PERIP_CLK_EN0;      // 0010
  volatile uint32_t      PERIP_CLK_EN1;      // 0014
  volatile uint32_t      PERIP_RST_EN0;      // 0018
  volatile uint32_t      PERIP_RST_EN1;      // 001C
  uint32_t               _pad_0020;
  volatile uint32_t      BT_LPCK_DIV_FRAC;   // 0024
  volatile uint32_t      CPU_INTR_FROM_CPU_0;// 0028
  volatile uint32_t      CPU_INTR_FROM_CPU_1;// 002C
  volatile uint32_t      CPU_INTR_FROM_CPU_2;// 0030
  volatile uint32_t      CPU_INTR_FROM_CPU_3;// 0034
  volatile uint32_t      RSA_PD_CTRL;        // 0038
  uint32_t               _pad_003C;
  volatile uint32_t      CACHE_CONTROL;      // 0040
  volatile uint32_t      EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROLLER;      // 0044
  volatile uint32_t      RTC_FASTMEM_CONFIG; // 0048
  volatile uint32_t      RTC_FASTMEM_CRC;    // 004C
  uint32_t               _pad_0050;
  volatile uint32_t      CLOCK_GATE;         // 0054
  volatile uint32_t      SYSCLK_CONF;        // 0058
//
} esp_sysreg_t;

#if 1
  // definition check:

  static_assert(offsetof(esp_sysreg_t, BT_LPCK_DIV_FRAC)    == 0x024, "offset error");
  static_assert(offsetof(esp_sysreg_t, CLOCK_GATE)          == 0x054, "offset error");
#endif


typedef struct
{
  uint32_t               _pad_0000[41];
  volatile uint32_t      CLKGATE_FORCE_ON;   // 00A4
  volatile uint32_t      MEM_POWER_DOWN;     // 00A8
  volatile uint32_t      MEM_POWER_UP;       // 00AC
//
} esp_apb_ctrl_t;


#define SYSTEM     ((esp_sysreg_t *)(0x600C0000))
#define APB_CTRL ((esp_apb_ctrl_t *)(0x60026000))

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

#define SYS_TIMER ((esp_systimer_t *)(0x60023000))

//-----------------------------------------------------------------------------
// UART
//-----------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t  FIFO;            // 0000
  volatile uint32_t  INT_RAW;         // 0004
  volatile uint32_t  INT_ST;          // 0008
  volatile uint32_t  INT_ENA;         // 000C
  volatile uint32_t  INT_CLR;         // 0010
  volatile uint32_t  CLKDIV;          // 0014
  volatile uint32_t  RX_FILT;         // 0018
  volatile uint32_t  STATUS;          // 001C
  volatile uint32_t  CONF0;           // 0020
  volatile uint32_t  CONF1;           // 0024
  volatile uint32_t  LOWPULSE;        // 0028
  volatile uint32_t  HIGHPULSE;       // 002C
  volatile uint32_t  RXD_CNT;         // 0030
  volatile uint32_t  FLOW_CONF;       // 0034
  volatile uint32_t  SLEEP_CONF;      // 0038
  volatile uint32_t  SWFC_CONF0;      // 003C
  volatile uint32_t  SWFC_CONF1;      // 0040
  volatile uint32_t  TXBRK_CONF;      // 0044
  volatile uint32_t  IDLE_CONF;       // 0048
  volatile uint32_t  RS485_CONF;      // 004C
  volatile uint32_t  AT_CMD_PRECNT;   // 0050
  volatile uint32_t  AT_CMD_POSTCNT;  // 0054
  volatile uint32_t  AT_CMD_GAPTOUT;  // 0058
  volatile uint32_t  AT_CMD_CHAR;     // 005C
  volatile uint32_t  MEM_CONF;        // 0060
  volatile uint32_t  MEM_TX_STATUS;   // 0064
  volatile uint32_t  MEM_RX_STATUS;   // 0068
  volatile uint32_t  FSM_STATUS;      // 006C
  volatile uint32_t  POSPULSE;        // 0070
  volatile uint32_t  NEGPULSE;        // 0074
  volatile uint32_t  CLK_CONF;        // 0078
  volatile uint32_t  DATE;            // 007C
  volatile uint32_t  ID;              // 0080
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

#define UART0     ((esp_uart_t *)(0x60000000))
#define UART1     ((esp_uart_t *)(0x60010000))

#define UHCI0     ((esp_uhci_t *)(0x60014000))

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

#define SPI2     ((esp_spi_t *)(0x60024000))

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

#define SPIMEM0   ((esp_spi_mem_t *)(0x60003000))
#define SPIMEM1   ((esp_spi_mem_t *)(0x60002000))

//-----------------------------------------------------------------------------
// Espressif definitions
//-----------------------------------------------------------------------------

#include "soc.h"
#include "soc_caps.h"

#include "soc/apb_ctrl_reg.h"
#include "soc/cache_memory.h"
#include "soc/efuse_reg.h"
#include "soc/extmem_reg.h"
#include "soc/fe_reg.h"
#include "soc/gdma_channel.h"
#include "soc/gdma_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/hwcrypto_reg.h"
#include "soc/i2c_reg.h"
#include "soc/i2s_reg.h"
#include "soc/interrupt_core0_reg.h"
#include "soc/ledc_reg.h"
#include "soc/nrx_reg.h"
#include "soc/periph_defs.h"
#include "soc/rmt_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/spi_mem_reg.h"
#include "soc/spi_pins.h"
#include "soc/spi_reg.h"
#include "soc/syscon_reg.h"
#include "soc/system_reg.h"
#include "soc/systimer_reg.h"
#include "soc/timer_group_reg.h"
#include "soc/uart_channel.h"
#include "soc/uart_reg.h"
#include "soc/uhci_reg.h"

//-----------------------------------------------------------------------------
// Miscellenious
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


