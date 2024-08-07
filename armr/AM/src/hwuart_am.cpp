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
 *  file:     hwuart_msp.cpp
 *  brief:    TI MSP UART
 *  date:     2023-06-28
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>

#include "hwuart.h"
#include "am_utils.h"


bool THwUart_am::Init(int adevnum)
{
	unsigned code;
	unsigned busid = 0;

	devnum = adevnum;
	initialized = false;
	regs = nullptr;

	if (0 == devnum)
	{
	  regs = (uart_regs_t *)SDL_UART0_BASE;
	}
	else if (1 == devnum)
  {
    regs = (uart_regs_t *)SDL_UART1_BASE;
  }
  else if (2 == devnum)
  {
    regs = (uart_regs_t *)SDL_UART2_BASE;
  }
	else if (3 == devnum)
	{
    regs = (uart_regs_t *)SDL_UART3_BASE;
	}
  else if (4 == devnum)
  {
    regs = (uart_regs_t *)SDL_UART4_BASE;
  }
  else if (5 == devnum)
  {
    regs = (uart_regs_t *)SDL_UART5_BASE;
  }
  else if (6 == devnum)
  {
    regs = (uart_regs_t *)SDL_UART6_BASE;
  }
	else
	{
	  return false;
	}

  periphclock = 48000000;

  // note: writing 0xBF to LCR activates the Configuration B, see the chapter "12.1.5.4.7.1.1 Operational Mode and Configuration Modes"

  regs->LCR = (0
    | (3  <<  0)  // DATALEN(2): 0 = 5-bit, 1 = 6-bit, 2 = 7-bit, 3 = 8-bit
    | (0  <<  2)  // STOPLEN: 0 = 1 stop bit, 1 = 2 stop bits
    | (0  <<  3)  // PE: parity enable
    | (0  <<  4)  // EVEN_PARITY: 1 = even parity
    | (0  <<  5)  // STICK_PARITY
    | (0  <<  6)  // BREAK
    | (1  <<  7)  // DIVISOR_LATCH_ACCESS: 1 = access the divisor latches
  );

  regs->MCR = 0; // disable auto-flow

  regs->FCR_IIR_EFR = (0
    | (1  <<  0)  // FIFO_EN
    | (1  <<  1)  // RCVR_FIFO_RST
    | (1  <<  2)  // XMIT_FIFO_RST
    | (0  <<  3)  // DMA_MODE: 0 = single DMA transfers, 1 = multi DMA transfers
    | (0  <<  4)  // TX_FIFO_TRIG(2): 0 = 8 bytes
    | (0  <<  6)  // RX_FIFO_TRIG(2): 0 = 8 bytes
  );

  SetBaudRate(baudrate);

  regs->LCR &= ~(1 << 7); // disable divisor latch, enabling the access to the FIFO data

  initialized = true;
  return true;
}

bool THwUart_am::SetBaudRate(int abaudrate)
{
  baudrate = abaudrate;

  regs->LCR |= (1 << 7); // enable divisor latch

  unsigned brdiv = (periphclock >> 4) / baudrate;

  regs->RBR_THR_DLL = (brdiv & 0xFF);
  regs->IER_DLH     = (brdiv >> 8);

  regs->LCR &= ~(1 << 7); // disable divisor latch, enabling the access to the FIFO data

  return true;
}

bool THwUart_am::TrySendChar(char ach)
{
  if (regs->SSR) {}  // at higher optimization level, the fifo full flag seems to be delayed
                     // this extra read causes enough delay

  if (regs->SSR & (1 << 0)) // transmitter FIFO full?
  {
    return false;
  }
  else
  {
    regs->RBR_THR_DLL = ach;
    return true;
  }
}

bool THwUart_am::TryRecvChar(char * ach)
{
  if (regs->LSR & (1 << 0)) // Receive FIFO not empty?
  {
    *ach = regs->RBR_THR_DLL;
    return true;
  }
  else
  {
    return false;
  }
}

bool THwUart_am::SendFinished()
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

void THwUart_am::DmaAssign(bool istx, THwDmaChannel * admach)
{
	if (istx)
	{
		txdma = admach;
		admach->Prepare(istx, (void *)&regs->RBR_THR_DLL, 0);
	}
	else
	{
		rxdma = admach;
		admach->Prepare(istx, (void *)&regs->RBR_THR_DLL, 0);
	}
}

bool THwUart_am::DmaStartSend(THwDmaTransfer * axfer)
{
	if (!txdma)
	{
		return false;
	}

	txdma->StartTransfer(axfer);
	return true;
}

bool THwUart_am::DmaStartRecv(THwDmaTransfer * axfer)
{
	if (!rxdma)
	{
		return false;
	}

	rxdma->StartTransfer(axfer);
	return true;
}
