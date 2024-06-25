/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2023 Viktor Nagy, nvitya
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
 *  file:     hwuart_am.h
 *  brief:    TI AM UART
 *  date:     2024-06-17
 *  authors:  nvitya
*/


#ifndef HWUART_AM_H_
#define HWUART_AM_H_

#define HWUART_PRE_ONLY
#include "hwuart.h"

typedef struct
{
  volatile uint32_t     RBR_THR_DLL;     // 0x00  Receive Buffer,Transmit Holding or Divisor Latch Low byte Register
  volatile uint32_t     IER_DLH;         // 0x04  Interrupt Enable or Divisor Latch high byte Register
  volatile uint32_t     FCR_IIR;         // 0x08  FIFO Control or Interrupt Identification Register
  volatile uint32_t     LCR;             // 0x0C  Line Control Register
  volatile uint32_t     MCR;             // 0x10  Modem control register
  volatile uint32_t     LSR;             // 0x14  line status register
  volatile uint32_t     MSR;             // 0x18  Modem status register
  volatile uint32_t     SPR_TLR;         // 0x1C  Sratchpad / trigger level
  volatile uint32_t     MDR1;            // 0x20  Mode definition register 1
  volatile uint32_t     MDR2;            // 0x24  Mode definition register 2
  volatile uint32_t     SFLSR_TXFLL;     // 0x28  Status FIFO line status register / Transmit frame length register low
  volatile uint32_t     RESUME_TXFLH;    // 0x2C  Resume halted operation register/ Transmit frame length register high
  volatile uint32_t     SFREGL_RXFLL;    // 0x30  Status FIFO register low / Received frame length register low
  volatile uint32_t     SFREGH_RXFLH;    // 0x34  Status FIFO register high / Received frame length register high
  volatile uint32_t     BLR_UASR;        // 0x38  BOF control register / UART autobauding status register
  volatile uint32_t     ACREG;           // 0x3C  Auxiliary control register
  volatile uint32_t     SCR;             // 0x40  Supplementary control register
  volatile uint32_t     SSR;             // 0x44  Supplementary status register
  volatile uint32_t     EBLR;            // 0x48  BOF length register
  volatile uint32_t     _pad_4C;         // 0x4C
  volatile uint32_t     MVR;             // 0x50  Module version register
  volatile uint32_t     SYSC;            // 0x54  System configuration register
  volatile uint32_t     SYSS;            // 0x58  System status register
  volatile uint32_t     WER;             // 0x5C  Wake-up enable register
  volatile uint32_t     CFPS;            // 0x60  Carrier frequency prescaler register
  volatile uint32_t     RXFIFO_LVL;      // 0x64  RX FIFO level register
  volatile uint32_t     TXFIFO_LVL;      // 0x68  TX FIFO level register
  volatile uint32_t     IER2;            // 0x6C  Interrupt enable register 2
  volatile uint32_t     ISR2;            // 0x70  Interrupt status register 2
  volatile uint32_t     FREQ_SEL;        // 0x74  Sample per bit selector register
  volatile uint32_t     ABAUD_1ST_CHAR;  // 0x78
  volatile uint32_t     ABAUD_2ND_CHAR;  // 0x7C
  volatile uint32_t     MDR3;            // 0x80  Mode definition register 3
  volatile uint32_t     TX_DMA_THRESH;   // 0x84  TX DMA threshold level register
  volatile uint32_t     MDR4;            // 0x88  Mode definition register 4
  volatile uint32_t     EFR2;            // 0x8C  Enhanced features register 2
  volatile uint32_t     ECR;             // 0x90  Enhanced control register
  volatile uint32_t     TIMEGUARD;       // 0x94  Timeguard register
  volatile uint32_t     TIMEOUTL;        // 0x98  Timeout lower byte register
  volatile uint32_t     TIMEOUTH;        // 0x9C  Timeout higher byte register
  volatile uint32_t     SCCR;            // 0xA0  Smartcard mode control register
  volatile uint32_t     ERHR_ETHR;       // 0xA4  Extended receive holding / transmit holding register
  volatile uint32_t     MAR;             // 0xA8  Multidrop address register
  volatile uint32_t     MMR;             // 0xAC  Multidrop mask register
  volatile uint32_t     MBR;             // 0xB0  Multidrop broadcast address register
//
} uart_regs_t;

class THwUart_am : public THwUart_pre
{
public:
	bool Init(int adevnum);

  bool SetBaudRate(int abaudrate);

	bool TrySendChar(char ach);
	bool TryRecvChar(char * ach);
	bool SendFinished();

	void DmaAssign(bool istx, THwDmaChannel * admach);

	bool DmaStartSend(THwDmaTransfer * axfer);
	bool DmaStartRecv(THwDmaTransfer * axfer);

public:

	uart_regs_t *      regs = nullptr;
};

#define HWUART_IMPL THwUart_am

#endif // def HWUART_MSP_H_
