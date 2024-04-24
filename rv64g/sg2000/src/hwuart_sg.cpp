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
// file:     hwuart_sg.cpp
// brief:    SG2000 UART driver
// created:  2024-04-21
// authors:  nvitya

#include <stdio.h>
#include <stdarg.h>

//#include "hwuart.h"
#include "hwuart_sg.h"

#include "sg_utils.h"

bool THwUart_sg::Init(int adevnum)
{
	initialized = false;

	if (0 == adevnum)
	{
	  regs = UART0;
	}
	else if (1 == adevnum)
	{
	  regs = UART1;
	}
  else if (2 == adevnum)
  {
    regs = UART2;
  }
  else if (3 == adevnum)
  {
    regs = UART3;
  }
  else if (4 == adevnum)
  {
    regs = UART4;
  }
  else if (5 == adevnum)  // = RTCSYS_UART
  {
    regs = UART5;
  }
	else
	{
	  regs = nullptr;
	  return false;
	}

	regs->MCR = 0; // disable auto-flow
	regs->LCR = (0
	  | (3  <<  0)  // DATALEN(2): 0 = 5-bit, 1 = 6-bit, 2 = 7-bit, 3 = 8-bit
	  | (0  <<  2)  // STOPLEN: 0 = 1 stop bit, 1 = 2 stop bits
	  | (0  <<  3)  // PE: parity enable
	  | (0  <<  4)  // EVEN_PARITY: 1 = even parity
	  | (0  <<  5)  // STICK_PARITY
	  | (0  <<  6)  // BREAK
	  | (1  <<  7)  // DIVISOR_LATCH_ACCESS: 1 = access the divisor latches
	);

	unsigned basespeed = (25000000 >> 4);
	unsigned brdiv = basespeed / baudrate;

	regs->RBR_THR_DLL = (brdiv & 0xFF);
	regs->IER_DLH     = (brdiv >> 8);
	regs->FCR_IIR = (0
	  | (1  <<  0)  // FIFO_EN
	  | (1  <<  1)  // RCVR_FIFO_RST
	  | (1  <<  2)  // XMIT_FIFO_RST
	  | (0  <<  3)  // DMA_MODE: 0 = single DMA transfers, 1 = multi DMA transfers
	  | (0  <<  4)  // TX_EMPTY(2): 0 = FIFO empty, 1 = 2 chars, 2 = 1/4, 3 = 1/2
	  | (0  <<  6)  // RCVR_TRIGGER(2): 0 = 1 char, 1 = 1/4, 2 = 1/2, 3 = 2 char less full
	);

	regs->LCR &= ~(1 << 7); // disable divisor latch, enabling the access to the FIFO data

  initialized = true;
	return true;
}

bool THwUart_sg::TrySendChar(char ach)
{
  if (regs->LSR & (1 << 5)) // transmitter holding register empty ? (tx fifo not full)
  {
    regs->RBR_THR_DLL = ach;
    return true;
  }
  else
  {
    return false;
  }
}

bool THwUart_sg::TryRecvChar(char * ach)
{
  if (regs->LSR & (1 << 0)) // FIFO not empty?
  {
    *ach = regs->RBR_THR_DLL;
    return true;
  }
  else
  {
    return false;
  }
}

bool THwUart_sg::SendFinished()
{
  if (regs->LSR & (1 << 6)) // transmitter empty?
  {
    return true;
  }
  else
  {
    return false;
  }
}

#if 0

void THwUart_sg::DmaAssign(bool istx, THwDmaChannel * admach)
{
	if (istx)
	{
		txdma = admach;
  	admach->Prepare(istx, (void *)&regs->DR, 0);
	}
	else
	{
		rxdma = admach;
		admach->Prepare(istx, (void *)&regs->DR, 0);
	}
}

bool THwUart_sg::DmaStartSend(THwDmaTransfer * axfer)
{
	if (!txdma)
	{
		return false;
	}

	regs->CR3 |= (1 << 7); // enable the TX DMA

	txdma->StartTransfer(axfer);

	return true;
}

bool THwUart_sg::DmaStartRecv(THwDmaTransfer * axfer)
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
