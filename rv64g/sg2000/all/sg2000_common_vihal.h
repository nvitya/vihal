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
// PLIC
//------------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t     PRIO[1024];  // 000000: The priority configuration registers
  volatile uint32_t     IP[32];      // 001000: The interrupt pending registers
  uint32_t _pad_001080[(0x2000-0x1080)/4];
  volatile uint32_t     H0_MIE[32];  // 002000: The M-mode interrupt enable registers
  volatile uint32_t     H0_SIE[32];  // 002080: The S-mode interrupt enable registers
  uint32_t _pad_002100[(0x1FFFFC-0x2100)/4];
  volatile uint32_t     PER;         // 1FFFFC: Permission control register
  volatile uint32_t     H0_MTH;      // 200000: M-mode interrupt threshold register
  volatile uint32_t     H0_MCLAIM;   // 200004: M-mode interrupt claim/complete register
  uint32_t _pad_200008[(0x201000-0x200008)/4];
  volatile uint32_t     H0_STH;      // 201000: S-mode interrupt threshold register
  volatile uint32_t     H0_SCLAIM;   // 201004  S-mode interrupt claim/complete register
//
} plic_regs_t;

#define PLIC_BASE_ADDR      0x70000000
#define PLIC                ((plic_regs_t *)PLIC_BASE_ADDR)

inline void mcu_irq_priority_set(unsigned intnum, unsigned priority)
{
  PLIC->PRIO[intnum] = priority;
}

inline bool mcu_irq_pending(unsigned intnum)
{
  return 0 != (PLIC->IP[intnum >> 5] & (1 << (intnum & 0x1F)));
}

inline void mcu_irq_pending_clear(unsigned intnum)
{
  PLIC->IP[intnum >> 5] &= ~(1 << (intnum & 0x1F));
}

inline void mcu_irq_pending_set(unsigned intnum)
{
  PLIC->IP[intnum >> 5] |= (1 << (intnum & 0x1F));
}

inline void mcu_irq_enable(unsigned intnum)
{
  PLIC->H0_MIE[intnum >> 5] |= (1 << (intnum & 0x1F));
}

inline void mcu_irq_disable(unsigned intnum)
{
  PLIC->H0_MIE[intnum >> 5] &= ~(1 << (intnum & 0x1F));
}

inline bool mcu_irq_enabled(unsigned intnum)
{
  return 0 != (PLIC->H0_MIE[intnum >> 5] & (1 << (intnum & 0x1F)));
}

inline bool mcu_irq_ack(unsigned intnum)
{
  return PLIC->H0_MCLAIM = intnum;
}

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

#define CLINT_BASE_ADDR     0x74000000
#define CLINT               ((clint_regs_t *)CLINT_BASE_ADDR)

//------------------------------------------------------------------------------
// DMA
//------------------------------------------------------------------------------


typedef struct
{
  volatile uint64_t  SAR;         /* Source Address Register */
  volatile uint64_t  DAR;         /* Destination Address Register */
  volatile uint64_t  BLOCK_TS;    /* Block transfer size */
  volatile uint64_t  CTL;         /* Contorl dma transer */
  volatile uint64_t  CFG;         /* Configure dma transer */
  volatile uint64_t  LLP;         /* Linked List Pointer */
  volatile uint64_t  STATUS;      /* Status of dma transer */
  volatile uint64_t  SWHSSRCREG;  /* SW handshake source register */
  volatile uint64_t  SWHSDSTREG;  /* SW handshake Destination register */
  volatile uint64_t  BLK_TFR_RESUMEREQREG;    /* Block transfer resume request */
  volatile uint64_t  AXI_IDREG;   /* AXI ID register */
  volatile uint64_t  AXI_QOSREG;  /* AXI QoS register */
  volatile uint64_t  SSTAT;
  volatile uint64_t  DSTAT;
  volatile uint64_t  SSTATAR;
  volatile uint64_t  DSTATAR;
  volatile uint64_t  INTSTATUS_ENABLEREG;
  volatile uint64_t  INTSTATUS;
  volatile uint64_t  INTSIGNAL_ENABLEREG;
  volatile uint64_t  INTCLEARREG;

  uint64_t __reserved1[12];
//
} sdma_chregs_t;

#define HWDMA_MAX_CHANNELS  8

typedef struct
{
  volatile uint64_t  ID;
  volatile uint64_t  COMPVER;
  volatile uint64_t  CFG;
  volatile uint64_t  CH_EN;
  uint64_t _pad_020[2];
  volatile uint64_t  INTSTATUS;          // 030
  volatile uint64_t  COMM_INTCLEAR;      // 038
  volatile uint64_t  COMM_INTSTATUS_EN;  // 040
  volatile uint64_t  COMM_INTSIGNAL_EN;  // 048
  volatile uint64_t  COMM_INTSTATUS;     // 050
  volatile uint64_t  RESET;              // 058
  uint64_t _pad_060[20];                 // 060
  sdma_chregs_t      CHAN[HWDMA_MAX_CHANNELS];  // 100
//
} sdma_regs_t;

typedef struct
{
  volatile uint32_t  CH_REMAP[2];
//
} dma_ch_remap_regs_t;

#define SDMA_BASE_ADDR        0x04330000
#define SDMA                  ((sdma_regs_t *)SDMA_BASE_ADDR)

#define DMA_CH_REMAP_ADDR     0x03000154
#define DMA_CH_REMAP          ((dma_ch_remap_regs_t *)DMA_CH_REMAP_ADDR)

#define DMA_INT_MUX_REG_ADDR  0x03000298
#define DMA_INT_MUX_REG       (*(volatile uint32_t *)DMA_INT_MUX_REG_ADDR)

// this is not documented in the reference manual 2024-02-26:
#define DMA_CLK_EN_REG_ADDR    0x03002004
#define DMA_CLK_EN_REG         (*(volatile uint32_t *)DMA_CLK_EN_REG_ADDR)
#define DMA_CLK_EN_BIT_ENABLE  (1 << 0)


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

//------------------------------------------------------------------------------
// SPI
//------------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t CTRLR0;    // 0x000 (R/W)  Control register 0
  volatile uint32_t CTRLR1;    // 0x004 (R/W)  Control register 1
  volatile uint32_t SPIENR;    // 0x008 (R/W)  SPI enable regiseter
  volatile uint32_t MWCR;      // 0x00C (R/W)  Microwire Control Register
  volatile uint32_t SER;       // 0x010 (R/W)  Slave enable register
  volatile uint32_t BAUDR;     // 0x014 (R/W)  Baud rate select
  volatile uint32_t TXFTLR;    // 0x018 (R/W)  Transmit FIFO Threshold Level
  volatile uint32_t RXFTLR;    // 0x01c (R/W)  Receive FIFO Threshold Level
  volatile uint32_t TXFLR;     // 0x020 (R/W)  Transmit FIFO Level register
  volatile uint32_t RXFLR;     // 0x024 (R/W)  Receive FIFO Level Register
  volatile uint32_t SR;        // 0x028 (R/W)  status register
  volatile uint32_t IMR;       // 0x02C (R/W)  Interrupt Mask Register
  volatile uint32_t ISR;       // 0x030 (R/W)  interrupt status register
  volatile uint32_t RISR;      // 0x034 (R/W)  Raw Interrupt Status Register
  volatile uint32_t TXOICR;    // 0x038 (R/W)  Transmit FIFO Overflow Interrupt Clear Register
  volatile uint32_t RXOICR;    // 0x03C (R/W)  Receive FIFO Overflow Interrupt Clear Register
  volatile uint32_t RXUICR;    // 0x040 (R/W)  Receive FIFO Underflow Interrupt Clear Register
  volatile uint32_t MSTICR;    // 0x044 (R/W)  Multi-Master Interrupt Clear Register
  volatile uint32_t ICR;       // 0x048 (R/W)  Interrupt Clear Register
  volatile uint32_t DMACR;     // 0x04C (R/W)  DMA Control Register
  volatile uint32_t DMATDLR;   // 0x050 (R/W)  DMA Transmoit Data Level
  volatile uint32_t DMARDLR;   // 0x054 (R/W)  DMA Receive Data Level
  volatile uint32_t IDR;       // 0x058 (R/W)  identification register
  volatile uint32_t SSI_COMP_VERSION;
  volatile uint32_t DR;        // 0x060 (R/W)  Data Register
  uint32_t          _pad_64[15];
  volatile uint32_t SPIMSSEL;  // 0x0A0 (R/W)  SPI is Master or Slave Select Register
//
} spi_regs_t;

#define SPI0_BASE_ADDR     (0x04180000)
#define SPI1_BASE_ADDR     (0x04190000)
#define SPI2_BASE_ADDR     (0x041A0000)
#define SPI3_BASE_ADDR     (0x041B0000)

#define SPI0  ((spi_regs_t *)SPI0_BASE_ADDR)
#define SPI1  ((spi_regs_t *)SPI1_BASE_ADDR)
#define SPI2  ((spi_regs_t *)SPI2_BASE_ADDR)
#define SPI3  ((spi_regs_t *)SPI3_BASE_ADDR)

//------------------------------------------------------------------------------
// PWM
//------------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t  HLPERIOD0;        // 00
  volatile uint32_t  PERIOD0;          // 04
  volatile uint32_t  HLPERIOD1;        // 08
  volatile uint32_t  PERIOD1;          // 0C
  volatile uint32_t  HLPERIOD2;        // 10
  volatile uint32_t  PERIOD2;          // 14
  volatile uint32_t  HLPERIOD3;        // 18
  volatile uint32_t  PERIOD3;          // 1C
  volatile uint32_t  CAP_FREQNUM;      // 20
  volatile uint32_t  CAP_FREQDATA;     // 24
  uint32_t _pad_28[(0x40-0x28)/4];
  volatile uint32_t  POLARITY;         // 40
  volatile uint32_t  PWMSTART;         // 44
  volatile uint32_t  PWMDONE;          // 48
  volatile uint32_t  PWMUPDATE;        // 4C
  volatile uint32_t  PCOUNT0;          // 50
  volatile uint32_t  PCOUNT1;          // 54
  volatile uint32_t  PCOUNT2;          // 58
  volatile uint32_t  PCOUNT3;          // 5C
  volatile uint32_t  PULSECOUNT0;      // 60
  volatile uint32_t  PULSECOUNT1;      // 64
  volatile uint32_t  PULSECOUNT2;      // 68
  volatile uint32_t  PULSECOUNT3;      // 6C
  uint32_t _pad_70[(0x80-0x70)/4];
  volatile uint32_t  SHIFTCOUNT0;      // 80
  volatile uint32_t  SHIFTCOUNT1;      // 84
  volatile uint32_t  SHIFTCOUNT2;      // 88
  volatile uint32_t  SHIFTCOUNT3;      // 8C
  volatile uint32_t  SHIFTSTART;       // 90
  uint32_t _pad_94[(0x9C-0x94)/4];
  volatile uint32_t  CAP_FREQEN;       // 9C
  uint32_t _pad_A0[(0xC0-0xA0)/4];
  volatile uint32_t  CAP_FREQDONE_NUM; // C0
  uint32_t _pad_C4[(0xD0-0xC4)/4];
  volatile uint32_t  PWM_OE;           // D0
  //
} pwm_regs_t;

#define PWM0_BASE_ADDR     (0x03060000)
#define PWM1_BASE_ADDR     (0x03061000)
#define PWM2_BASE_ADDR     (0x03062000)
#define PWM3_BASE_ADDR     (0x03063000)

#define PWM0  ((pwm_regs_t *)PWM0_BASE_ADDR)
#define PWM1  ((pwm_regs_t *)PWM1_BASE_ADDR)
#define PWM2  ((pwm_regs_t *)PWM2_BASE_ADDR)
#define PWM3  ((pwm_regs_t *)PWM3_BASE_ADDR)


#endif
