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

//------------------------------------------------------------------------------
// CLINT
//------------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t     MSIP0;
  volatile uint32_t     MSIP1;
  volatile uint32_t     MSIP2;
  volatile uint32_t     MSIP3;
           uint32_t     RESERVED0[(0x4004000 - 0x400000C) / 4 - 1];
  volatile uint32_t     MTIMECMPL0;
  volatile uint32_t     MTIMECMPH0;
  volatile uint32_t     MTIMECMPL1;
  volatile uint32_t     MTIMECMPH1;
  volatile uint32_t     MTIMECMPL2;
  volatile uint32_t     MTIMECMPH2;
  volatile uint32_t     MTIMECMPL3;
  volatile uint32_t     MTIMECMPH3;
           uint32_t     RESERVED1[(0x400C000 - 0x400401C) / 4 - 1];
  volatile uint32_t     SSIP0;
  volatile uint32_t     SSIP1;
  volatile uint32_t     SSIP2;
  volatile uint32_t     SSIP3;
           uint32_t     RESERVED2[(0x400D000 - 0x400C00C) / 4 - 1];
  volatile uint32_t     STIMECMPL0;
  volatile uint32_t     STIMECMPH0;
  volatile uint32_t     STIMECMPL1;
  volatile uint32_t     STIMECMPH1;
  volatile uint32_t     STIMECMPL2;
  volatile uint32_t     STIMECMPH2;
  volatile uint32_t     STIMECMPL3;
  volatile uint32_t     STIMECMPH3;
//
} clint_regs_t;

#define PLIC_BASE_ADDR      0x70000000
#define CLINT_BASE_ADDR     0x74000000
#define CLINT               ((clint_regs_t *)CLINT_BASE_ADDR)

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

#define GPIO0_BASE_ADDR     (0x03020000)
#define GPIO1_BASE_ADDR     (0x03021000)
#define GPIO2_BASE_ADDR     (0x03022000)
#define GPIO3_BASE_ADDR     (0x03023000)
#define GPIOPWR_BASE_ADDR      (0x05021000)
#define RTCSYS_GPIO_BASE_ADDR  (0x05021000)

#define GPIO0  ((gpio_regs_t *)GPIO0_BASE_ADDR)
#define GPIO1  ((gpio_regs_t *)GPIO1_BASE_ADDR)
#define GPIO2  ((gpio_regs_t *)GPIO2_BASE_ADDR)
#define GPIO3  ((gpio_regs_t *)GPIO3_BASE_ADDR)

#define GPIO4       ((gpio_regs_t *)GPIOPWR_BASE_ADDR)
#define GPIOPWR     ((gpio_regs_t *)GPIOPWR_BASE_ADDR)
#define GPIORTCSYS  ((gpio_regs_t *)GPIOPWR_BASE_ADDR)

//------------------------------------------------------------------------------
// UART
//------------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t     RBR_THR_DLL;   // 0x00  Receive Buffer,Transmit Holding or Divisor Latch Low byte Register
  volatile uint32_t     IER_DLH;       // 0x04  Interrupt Enable or Divisor Latch high byte Register
  volatile uint32_t     FCR_IIR;       // 0x08  FIFO Control or Interrupt Identification Register
  volatile uint32_t     LCR;           // 0x0C  Line Control Register
  volatile uint32_t     MCR;           // 0x10
  volatile uint32_t     LSR;           // 0x14
  volatile uint32_t     MSR;           // 0x18
  volatile uint32_t     _pad_1C;       // 0x1C
  volatile uint32_t     LPDLL;         // 0x20
  volatile uint32_t     LPDLH;         // 0x24
  volatile uint32_t     _pad_28;       // 0x28
  volatile uint32_t     _pad_2C;       // 0x2C
  volatile uint32_t     SRBR_STHR;     // 0x30
  uint32_t              _pad_34[(0x70-0x34)/4];
  volatile uint32_t     FAR;           // 0x70
  volatile uint32_t     TFR;           // 0x74
  volatile uint32_t     RFW;           // 0x78
  volatile uint32_t     USR;           // 0x7C
  volatile uint32_t     TFL;           // 0x80
  volatile uint32_t     RFL;           // 0x84
  volatile uint32_t     SRR;           // 0x88
  volatile uint32_t     SRTS;          // 0x8C
  volatile uint32_t     SBCR;          // 0x90
  volatile uint32_t     SDMAM;         // 0x94
  volatile uint32_t     SFE;           // 0x98
  volatile uint32_t     SRT;           // 0x9C
  volatile uint32_t     STET;          // 0xA0
  volatile uint32_t     HTX;           // 0xA4
  volatile uint32_t     DMASA;         // 0xA8
//
} uart_regs_t;

#define UART0_BASE_ADDR     (0x04140000)
#define UART1_BASE_ADDR     (0x04150000)
#define UART2_BASE_ADDR     (0x04160000)
#define UART3_BASE_ADDR     (0x04170000)
#define UART4_BASE_ADDR     (0x041C0000)
#define RTCSYS_UART_BASE_ADDR  (0x05022000)

#define UART0  ((uart_regs_t *)UART0_BASE_ADDR)
#define UART1  ((uart_regs_t *)UART1_BASE_ADDR)
#define UART2  ((uart_regs_t *)UART2_BASE_ADDR)
#define UART3  ((uart_regs_t *)UART3_BASE_ADDR)
#define UART4  ((uart_regs_t *)UART4_BASE_ADDR)

#define UART5        ((uart_regs_t *)RTCSYS_UART_BASE_ADDR)
#define RTCSYS_UART  ((uart_regs_t *)RTCSYS_UART_BASE_ADDR)


#endif
