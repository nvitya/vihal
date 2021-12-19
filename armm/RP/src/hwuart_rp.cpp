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
 *  file:     hwuart_rp.cpp
 *  brief:    RP UART
 *  version:  1.00
 *  date:     2021-12-19
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>

#include "hwuart.h"
#include "rp_utils.h"

bool THwUart_rp::Init(int adevnum)
{
	unsigned code;

	devnum = adevnum;
	initialized = false;

	uint32_t reset_mask;

	regs = nullptr;

	if (0 == devnum)  // PUART1
	{
    regs = uart0_hw;
    reset_mask = RESETS_RESET_UART0_BITS;
	}
	else if (1 == devnum)
	{
    regs = uart1_hw;
    reset_mask = RESETS_RESET_UART1_BITS;
	}

	if (!regs)
	{
		return false;
	}

  rp_reset_control(reset_mask, true);  // force reset
  rp_reset_control(reset_mask, false); // remove reset

  // main control register

  regs->cr = (0
    | (0  << 15)  // CTSEN
    | (0  << 14)  // RTSEN
    | (0  << 13)  // OUT2
    | (0  << 12)  // OUT1
    | (0  << 11)  // RTS
    | (0  << 10)  // DTR
    | (1  <<  9)  // RXE: 1 = enable rx
    | (1  <<  8)  // TXE: 1 = enable tx
    | (0  <<  7)  // LBE: 1 = loopback enable
    | (0  <<  2)  // SIRLP
    | (0  <<  1)  // SIREN
    | (1  <<  0)  // UARTEN: 1 = enable uart
  );

  regs->imsc = 0; // clear all interrupt masks

  // set baudrate
  //   fix /16 divisor for oversampling + 6-bit fractional divider

  unsigned periphclock = SystemCoreClock;

  uint32_t brdiv_m64 = (periphclock << 2) / baudrate;  // /16 *64 = *4 = << 2
  regs->ibrd = (brdiv_m64 >> 6);    // the integer part
  regs->fbrd = (brdiv_m64 & 0x3F);  // the fractional part

  // set line format

  uint32_t lcrh = (0
    | (0  <<  7)  // SPS
    | (0  <<  5)  // WLEN(2): 0 = 5 bits, 1 = 6 bits, 2 = 7 bits, 3 = 8 bits
    | (1  <<  4)  // FEN: 1 = enable fifos
    | (0  <<  3)  // STP2
    | (0  <<  2)  // EPS
    | (0  <<  1)  // PEN
    | (0  <<  0)  // BRK
  );

  // data length
  lcrh |= ((databits - 5) << 5);

	// stop bits
	if (4 == halfstopbits)  lcrh |= (1 << 3);  // set two stop bits

  // parity
	if (parity)
	{
	  lcrh |= (1 << 1); // enable parity
	  if (!oddparity)  lcrh |= (1 << 2);
	}

	regs->lcr_h = lcrh;

  regs->icr = 0x7FF; // clear all interrupt flags

	initialized = true;

	return true;
}

bool THwUart_rp::TrySendChar(char ach)
{
	if (regs->fr & UART_UARTFR_TXFF_BITS) // TX FIFO Full ?
	{
		return false;
	}

	regs->dr = ach;

	return true;
}

bool THwUart_rp::TryRecvChar(char * ach)
{
	if (regs->fr & UART_UARTFR_RXFE_BITS) // RX FIFO Empty?
	{
	  return false;
	}

	*ach = regs->dr;
	return true;
}

bool THwUart_rp::SendFinished()
{
	if (regs->fr & UART_UARTFR_BUSY_BITS)
	{
		return false;
	}
	else
	{
		return true;
	}
}

#if HWDMA_IMPLEMENTED

void THwUart_rp::DmaAssign(bool istx, THwDmaChannel * admach)
{
	if (istx)
	{
		txdma = admach;
		#ifdef USART_TDR_TDR
			admach->Prepare(istx, (void *)&regs->TDR, 0);
		#else
			admach->Prepare(istx, (void *)&regs->DR, 0);
		#endif
	}
	else
	{
		rxdma = admach;
		#ifdef USART_RDR_RDR
			admach->Prepare(istx, (void *)&regs->RDR, 0);
		#else
			admach->Prepare(istx, (void *)&regs->DR, 0);
		#endif
	}
}

bool THwUart_rp::DmaStartSend(THwDmaTransfer * axfer)
{
	if (!txdma)
	{
		return false;
	}

	regs->CR3 |= (1 << 7); // enable the TX DMA

	txdma->StartTransfer(axfer);

	return true;
}

bool THwUart_rp::DmaStartRecv(THwDmaTransfer * axfer)
{
	if (!rxdma)
	{
		return false;
	}

	regs->CR3 |= (1 << 6); // enable the RX DMA

	rxdma->StartTransfer(axfer);

	return true;
}

#endif
