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
 *  file:     esp32_vihal.h
 *  brief:    VIHAL ESP32 include and own definitions
 *            the original Espressif peripheral definitions are not so usable
 *            so I made my own peripheral structs like the most ARM vendors provide
 *  created:  2022-01-29
 *  authors:  nvitya
*/

#pragma once

#include <cstddef>
#include "stdint.h"

// add some basic xtensa includes

//#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
//#include <xtensa/config/system.h>

//-----------------------------------------------------------------------------
// GPIO
//-----------------------------------------------------------------------------

#define ESP_GPIO_COUNT   40
#define ESP_PAD_COUNT    40

// The ESP32 "pad"-s are referred mostly by their GPIO number, which is different from the package pin number
// And additionally some pads have officially special names, although they also can be used as GPIO

#define PAD_GPIO0    0
#define PAD_GPIO1    1
#define PAD_GPIO2    2
#define PAD_GPIO3    3
#define PAD_GPIO4    4
#define PAD_GPIO5    5
#define PAD_GPIO6    6
#define PAD_GPIO7    7
#define PAD_GPIO8    8
#define PAD_GPIO9    9
#define PAD_GPIO10  10
#define PAD_GPIO11  11
#define PAD_GPIO12  12
#define PAD_GPIO13  13
#define PAD_GPIO14  14
#define PAD_GPIO15  15
#define PAD_GPIO16  16
#define PAD_GPIO17  17
#define PAD_GPIO18  18
#define PAD_GPIO19  19
#define PAD_GPIO20  20
#define PAD_GPIO21  21
#define PAD_GPIO22  22
#define PAD_GPIO23  23
#define PAD_GPIO24  24
#define PAD_GPIO25  25
#define PAD_GPIO26  26
#define PAD_GPIO27  27
#define PAD_GPIO28  28
#define PAD_GPIO29  29
#define PAD_GPIO30  30
#define PAD_GPIO31  31
#define PAD_GPIO32  32
#define PAD_GPIO33  33
#define PAD_GPIO34  34
#define PAD_GPIO35  35
#define PAD_GPIO36  36
#define PAD_GPIO37  37
#define PAD_GPIO38  38
#define PAD_GPIO39  39

// pads with special names

#define PAD_U0TXD          1
#define PAD_U0RXD          3
#define PAD_SD_CLK         6
#define PAD_SD_DATA_0      7
#define PAD_SD_DATA_1      8
#define PAD_SD_DATA_2      9
#define PAD_SD_DATA_3     10
#define PAD_SD_CMD        11
#define PAD_MTDI          12
#define PAD_MTCK          13
#define PAD_MTMS          14
#define PAD_MTDO          15
#define PAD_32K_XP        32
#define PAD_32K_XN        33
#define PAD_VDET_1        34
#define PAD_VDET_2        35
#define PAD_SENSOR_VP     36
#define PAD_SENSOR_CAPP   37
#define PAD_SENSOR_CAPN   37
#define PAD_SENSOR_VN     39


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
  uint32_t               _pad_000;
  esp_gpio_out_t         OUT[2];     // output (data, set, clear)
  uint32_t               _pad_01C;
  esp_gpio_enable_t      ENABLE[2];  // output enable
  volatile uint32_t      STRAP;      // bootstrap pin value
  volatile uint32_t      IN[2];      // input
  esp_gpio_irqstat_t     STATUS[2];  // interrupt status register
  uint32_t               _pad_05C;
  volatile uint32_t      ACPU_INT;
  volatile uint32_t      ACPU_NMI_INT;
  volatile uint32_t      PCPU_INT;
  volatile uint32_t      PCPU_NMI_INT;
  uint32_t               _pad_070;
  volatile uint32_t      ACPU_INT1;
  volatile uint32_t      ACPU_NMI_INT1;
  volatile uint32_t      PCPU_INT1;
  volatile uint32_t      PCPU_NMI_INT1;
  uint32_t               _pad_084;
  volatile uint32_t      PIN[40];    // pin configuration
  uint32_t               _pad_128;
  uint32_t               _pad_12C;
  volatile uint32_t      FUNC_IN_SEL_CFG[256];  // Peripheral function n input selection
  volatile uint32_t      FUNC_OUT_SEL_CFG[40];  // Peripheral output selection for GPIOn
//
} esp_gpio_t;

#if 0
  // definition check:

  static_assert(offsetof(esp_gpio_t, STRAP)    == 0x038, "strap offset error");
  static_assert(offsetof(esp_gpio_t, PCPU_INT) == 0x068, "pcpu offset error");
  static_assert(offsetof(esp_gpio_t, PIN)      == 0x088, "pin offset error");
  static_assert(offsetof(esp_gpio_t, FUNC_IN_SEL_CFG) == 0x130, "func_in_sel_cfg offset error");
  static_assert(offsetof(esp_gpio_t, FUNC_OUT_SEL_CFG) == 0x530, "func_out_sel_cfg offset error");
#endif

#define IO_MUX_PAD_GPIO36     0
#define IO_MUX_PAD_GPIO37     1
#define IO_MUX_PAD_GPIO38     2
#define IO_MUX_PAD_GPIO39     3
#define IO_MUX_PAD_GPIO34     4
#define IO_MUX_PAD_GPIO35     5
#define IO_MUX_PAD_GPIO32     6
#define IO_MUX_PAD_GPIO33     7
#define IO_MUX_PAD_GPIO25     8
#define IO_MUX_PAD_GPIO26     9
#define IO_MUX_PAD_GPIO27    10
#define IO_MUX_PAD_MTMS      11  // GPIO14
#define IO_MUX_PAD_MTDI      12  // GPIO12
#define IO_MUX_PAD_MTCK      13  // GPIO13
#define IO_MUX_PAD_MTDO      14  // GPIO15
#define IO_MUX_PAD_GPIO2     15
#define IO_MUX_PAD_GPIO0     16
#define IO_MUX_PAD_GPIO4     17
#define IO_MUX_PAD_GPIO16    18
#define IO_MUX_PAD_GPIO17    19
#define IO_MUX_PAD_SD_DATA2  20  // GPIO9
#define IO_MUX_PAD_SD_DATA3  21  // GPIO10
#define IO_MUX_PAD_SD_CMD    22  // GPIO11
#define IO_MUX_PAD_SD_CLK    23  // GPIO6
#define IO_MUX_PAD_SD_DATA0  24  // GPIO7
#define IO_MUX_PAD_SD_DATA1  25  // GPIO8
#define IO_MUX_PAD_GPIO5     26
#define IO_MUX_PAD_GPIO18    27
#define IO_MUX_PAD_GPIO19    28
#define IO_MUX_PAD_GPIO20    29
#define IO_MUX_PAD_GPIO21    30
#define IO_MUX_PAD_GPIO22    31
#define IO_MUX_PAD_U0RXD     32  // GPIO3
#define IO_MUX_PAD_U0TXD     33  // GPIO1
#define IO_MUX_PAD_GPIO23    34
#define IO_MUX_PAD_GPIO24    35

#define IO_MUX_PAD_CFG_COUNT 36

typedef struct
{
  volatile uint32_t   PIN_CTRL;  // Clock output configuration
  volatile uint32_t   CFG[36];   // Configuration register, the order here is different from GPIO numbering !
//
} esp_io_mux_t;

//-----------------------------------------------------------------------------
// Instances
//-----------------------------------------------------------------------------

#define GPIO         ((esp_gpio_t *)(0x3FF44000))
#define IO_MUX     ((esp_io_mux_t *)(0x3FF49000))
