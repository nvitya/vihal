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
 *  brief:    VIHAL ESP32C3 include and own definitions
 *            the original Espressif peripheral definitions are not so usable
 *            so I made my own peripheral structs like the most ARM vendors provide
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

//-----------------------------------------------------------------------------
// Instances
//-----------------------------------------------------------------------------

#define GPIO         ((esp_gpio_t *)(0x60004000))
#define IO_MUX     ((esp_io_mux_t *)(0x60009000))

#define UART0              ((void *)(0x60000000))
#define UART1              ((void *)(0x60010000))
