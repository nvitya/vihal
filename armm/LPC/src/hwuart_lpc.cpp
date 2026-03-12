/* -----------------------------------------------------------------------------
 * This file is a part of the NVCM project: https://github.com/nvitya/nvcm
 * Copyright (c) 2018 Viktor Nagy, nvitya
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
 *  file:     hwuart_lpc.cpp
 *  brief:    Simple LPC UART
 *  date:     2018-02-10
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>

#include "hwuart.h"

#include "lpc_utils.h"

bool THwUart_lpc::Init(int adevnum)
{
	unsigned tmp;

	devnum = adevnum;
	initialized = false;

	unsigned pclock;
	unsigned bclock;

	regs = nullptr;

	if (false)
	{

	}
	else if (0 == devnum)
	{
		#ifdef MCUSF_17XX
			regs = (LPC_UART_TypeDef *)LPC_UART0;
			LPC_SC->PCONP |= PCONP_PCUART0_ENABLED;
		#else
			regs = LPC_USART0;
			lpc_enable_clock(450, 1); // base clock
			lpc_enable_clock(129, 7); // uart clock
		#endif
	}
	else if (1 == devnum)
	{
		#ifdef MCUSF_17XX
			regs = (LPC_UART_TypeDef *)LPC_UART1;
			LPC_SC->PCONP |= PCONP_PCUART1_ENABLED;
		#else
			regs = LPC_USART1;
		  lpc_enable_clock(418, 1); // base clock
		  lpc_enable_clock(130, 7); // uart clock
    #endif
	}
	else if (2 == devnum)
	{
		regs = (LPC_UART_TypeDef *)LPC_UART2;
    #ifdef MCUSF_17XX
		  LPC_SC->PCONP |= PCONP_PCUART2_ENABLED;
    #else
			regs = LPC_USART2;
		  lpc_enable_clock(386, 1); // base clock
		  lpc_enable_clock(161, 7); // uart clock
    #endif
	}
	else if (3 == devnum)
	{
		#ifdef MCUSF_17XX
  		regs = (LPC_UART_TypeDef *)LPC_UART3;
			LPC_SC->PCONP |= PCONP_PCUART3_ENABLED;
		#else
			regs = LPC_USART3;
			lpc_enable_clock(354, 1); // base clock
			lpc_enable_clock(162, 7); // uart clock
		#endif
	}

	if (!regs)
	{
		return false;
	}

  // Disable Tx
  regs->TER = 0;

	// Enable FIFOs by default, reset them
	regs->FCR = (0
	  | (1  <<  0)  // FIFO_EN: Enable UART FIFOs
	  | (1  <<  1)  // RX_FIFO_RESET
	  | (1  <<  2)  // TX_FIFO_RESET
	  | (1  <<  3)  // DMA_MODE
	  | (1  <<  6)  // RX_TRIGGER_LEVEL(2): 0 = 1 char
	);

  // Disable interrupts
	regs->IER = 0;
	// Set LCR to default state
	regs->LCR = 0;
	// Set ACR to default state
	regs->ACR = 0;

	// LCR (LINE control)
	tmp = (0
	  | ((databits - 5)  <<  0)  // WORD_LEN(2)
	  | (0               <<  2)  // STOP_BITS: 0 = 1 bit, 1 = 2 bits
	  | (0               <<  3)  // PARITY_EN
	  | (0               <<  4)  // PARITY_SEL(2): 0 = odd, 1 = even, 2 = forced 1, 3 = forced 0
	  | (0               <<  6)  // BREAK_CTRL
	  | (0               <<  7)  // DLAB_ACCESS
	);
	if (parity)
	{
		tmp |= (1 << 3);
		if (!oddparity)  tmp |= (1 << 4);
	}
	if (halfstopbits >= 2)  tmp |= (1 << 2);
	regs->LCR = tmp;

	// Disable fractional divider
	regs->FDR = 0x10;

	unsigned baseclock = SystemCoreClock;

	unsigned div = baseclock / (baudrate * 16);

	// High and low halves of the divider
	unsigned divh = div / 256;
	unsigned divl = div - (divh * 256);

	regs->LCR |= 0x80;  // enable access of the divisor latches
	regs->DLL = divl;
	regs->DLM = divh;
	regs->LCR &= 0x7F;  // back to the normal mode

	// Enable:
  regs->TER = UART_TER_TXEN;

	initialized = true;

	return true;
}

bool THwUart_lpc::TrySendChar(char ach)
{
	if (regs->LSR & UART_LSR_THRE)
	{
		regs->THR = ach;
		return true;
	}

	return false;
}

bool THwUart_lpc::TryRecvChar(char * ach)
{
	if (regs->LSR & 1)
	{
		*ach = regs->RBR;
		return true;
	}

	return false;
}
