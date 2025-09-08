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
#include "rk_utils.h"


bool THwUart_rk::Init(int adevnum)
{
	uint32_t tmp;

	devnum = adevnum;
	initialized = false;
	regs = nullptr;

	if (0 == devnum)
	{
		map_hw_addr(UART0_BASE, sizeof(struct UART_REG), (void * *)&regs);
    cru_reg_set_field(0x82C, 4, 1, 0); // sclk_uart0_src gate disable
    cru_reg_set_field(0x82C, 9, 1, 0); // pclk_uart0 gate disable
	}
	else if (1 == devnum)
  {
		map_hw_addr(UART1_BASE, sizeof(struct UART_REG), (void * *)&regs);
    cru_reg_set_field(0x82C,  5, 1, 0); // sclk_uart1_src gate disable
    cru_reg_set_field(0x82C, 10, 1, 0); // pclk_uart1 gate disable
  }
  else if (2 == devnum)
  {
		map_hw_addr(UART2_BASE, sizeof(struct UART_REG), (void * *)&regs);
    cru_reg_set_field(0x82C,  6, 1, 0); // sclk_uart2_src gate disable
    cru_reg_set_field(0x82C, 11, 1, 0); // pclk_uart2 gate disable
  }
	else if (3 == devnum)
	{
		map_hw_addr(UART3_BASE, sizeof(struct UART_REG), (void * *)&regs);
    cru_reg_set_field(0x82C,  7, 1, 0); // sclk_uart3_src gate disable
    cru_reg_set_field(0x82C, 12, 1, 0); // pclk_uart3 gate disable
	}
  else if (4 == devnum)
  {
		map_hw_addr(UART4_BASE, sizeof(struct UART_REG), (void * *)&regs);
    cru_reg_set_field(0x82C,  8, 1, 0); // sclk_uart4_src gate disable
    cru_reg_set_field(0x82C, 13, 1, 0); // pclk_uart4 gate disable
  }
	else
	{
	  return false;
	}

  periphclock = 24000000;

  // LCR
  tmp = (0
    | (3  <<  0)  // DATALEN(2): 0 = 5-bit, 1 = 6-bit, 2 = 7-bit, 3 = 8-bit
    | (0  <<  2)  // STOPLEN: 0 = 1 stop bit, 1 = 2 stop bits
    | (0  <<  3)  // PE: parity enable
    | (0  <<  4)  // EVEN_PARITY: 1 = even parity
    | (0  <<  5)  // STICK_PARITY
    | (0  <<  6)  // BREAK
    | (1  <<  7)  // DIVISOR_LATCH_ACCESS: 1 = access the divisor latches
  );
  if (parity)
  {
  	tmp |= (1 << 3);  // PE: parity enable
  	if (!oddparity)  tmp |= (1 << 4);
  }
  if (halfstopbits == 4)  tmp |= (1 << 2);
  regs->LCR = tmp;
  if (regs->LCR)  { }

  regs->MCR = 0; // disable auto-flow
  if (regs->MCR)  { }

  regs->FCR = (0
    | (1  <<  0)  // FIFO_EN
    | (1  <<  1)  // RCVR_FIFO_RST
    | (1  <<  2)  // XMIT_FIFO_RST
    | (0  <<  3)  // DMA_MODE: 0 = single DMA transfers, 1 = multi DMA transfers
    | (0  <<  4)  // TX_FIFO_TRIG(2): 0 = 8 bytes
    | (0  <<  6)  // RX_FIFO_TRIG(2): 0 = 8 bytes
  );
  if (regs->FCR) { }

  SetBaudRate(baudrate);

  regs->LCR &= ~(1 << 7); // disable divisor latch, enabling the access to the FIFO data
  if (regs->LCR)  { }

  initialized = true;
  return true;
}

bool THwUart_rk::SetBaudRate(int abaudrate)
{
  baudrate = abaudrate;

  regs->LCR |= (1 << 7); // enable divisor latch

  unsigned brdiv = (periphclock >> 4) / baudrate;

  regs->DLL = (brdiv & 0xFF);
  regs->DLH = (brdiv >> 8);

  regs->LCR &= ~(1 << 7); // disable divisor latch, enabling the access to the FIFO data

  return true;
}

bool THwUart_rk::TrySendChar(char ach)
{
  if (regs->USR & (1 << 1)) // transmitter FIFO not full?
  {
    regs->THR = ach;
    return true;
  }
  else
  {
    return false;
  }
}

bool THwUart_rk::TryRecvChar(char * ach)
{
  if (regs->LSR & (1 << 0)) // Receive FIFO not empty?
  {
    *ach = regs->RBR;
    return true;
  }
  else
  {
    return false;
  }
}

bool THwUart_rk::SendFinished()
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
