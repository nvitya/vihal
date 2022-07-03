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
 *  file:     hwuart_esp.cpp
 *  brief:    ESP32 UART
 *  date:     2022-06-29
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>

#include "hwuart.h"
#include "esp_utils.h"

bool THwUart_esp::Init(int adevnum)
{
	devnum = adevnum;
	initialized = false;

	//uhci = UHCI0;
	regs = nullptr;

	uint32_t sys_rst_mask;

  DPORT->PERIP_CLK_EN |= DPORT_UART_MEM_CLK_EN;

	if (0 == devnum)
	{
    regs = UART0;
    DPORT->PERIP_CLK_EN |= DPORT_UART_CLK_EN;
    sys_rst_mask = DPORT_UART_RST;
	}
	else if (1 == devnum)
	{
    regs = UART1;
    DPORT->PERIP_CLK_EN |= DPORT_UART1_CLK_EN;
    sys_rst_mask = DPORT_UART1_RST;
	}
  else if (2 == devnum)
  {
    regs = UART2;
    DPORT->PERIP_CLK_EN |= DPORT_UART2_CLK_EN;
    sys_rst_mask = DPORT_UART2_RST;
  }

	if (!regs)
	{
		return false;
	}

  // following the Espressif recommended reset sequence:

  DPORT->PERIP_RST_EN &= ~sys_rst_mask;  // clear system UART reset
  DPORT->PERIP_RST_EN |= sys_rst_mask;   // set system UART reset
  if (DPORT->PERIP_RST_EN) {} // some sync
  DPORT->PERIP_RST_EN &= ~sys_rst_mask;  // remove system UART reset

  unsigned periphclock = esp_apb_speed();

  // * 16 for the fractional divider
  uint32_t brdiv_m16 = (periphclock << 4) / baudrate;  // *16 = 1

  regs->CLKDIV = (0
      | ((brdiv_m16 >> 4)  <<  0)  // CLKDIV(12)
      | ((brdiv_m16 & 15)  << 20)  // CLKDIV_FRAG(4)
  );

  uint32_t conf0 = (0
    | (0  <<  0)  // PARITY
    | (0  <<  1)  // PARITY_EN
    | (0  <<  2)  // BIT_NUM(2): 0 = 5 bit, 3 = 8 bit
    | (0  <<  4)  // STOP_BIT_NUM(2): 0 = 1 stop bit, 1 = 1.5, 2 = 2, 3 = 3
    | (0  <<  6)  // SW_RTS
    | (0  <<  7)  // SW_DTR
    | (0  <<  8)  // TXD_BRK
    | (0  <<  9)  // IRDA_DPLX
    | (0  << 10)  // IRDA_TX_EN
    | (0  << 11)  // IRDA_WCTL
    | (0  << 12)  // IRDA_TX_INV
    | (0  << 13)  // IRDA_RX_INV
    | (0  << 14)  // LOOPBACK
    | (0  << 15)  // TX_FLOW_EN:
    | (0  << 16)  // IRDA_EN
    | (0  << 17)  // RXFIFO_RST
    | (0  << 18)  // TXFIFO_RST
    | (0  << 19)  // RXD_INV
    | (0  << 20)  // CTS_INV
    | (0  << 21)  // DSR_INV
    | (0  << 22)  // TXD_INV
    | (0  << 23)  // RTS_INV
    | (0  << 24)  // DTR_INV
    | (1  << 27)  // TICK_DEF_ALWAYS_ON
  );
  // data bits
  conf0 |= ((databits - 5) << 2);
  // stop bits
  if (4 == halfstopbits)  conf0 |= (2 << 4);  // set two stop bits
  // parity
  if (parity)
  {
    conf0 |= (1 << 1); // enable parity
    if (!oddparity)  conf0 |= (1 << 0);
  }
  regs->CONF0 = conf0;

  uint32_t conf1 = (0
    | (0x60 <<  0)  // RXFIFO_FULL_THRHD(7)
    | (0x60 <<  8)  // TXFIFO_EMPTY_THRHD(7)
    | (0    << 16)  // RX_FLOW_THRD(7)
    | (0    << 23)  // RX_FLOW_EN
    | (0    << 24)  // RX_TOUT_THRD(7)
    | (0    << 31)  // RX_TOUT_EN
  );
  regs->CONF1 = conf1;

	initialized = true;

	return true;
}

bool THwUart_esp::TrySendChar(char ach)
{
  uint32_t sr = regs->STATUS;
	if (((sr >> 16) & 0xFF) >= tx_fifo_size) // TX FIFO Full ?
	{
		return false;
	}

	regs->FIFO = ach;
	return true;
}

bool THwUart_esp::TryRecvChar(char * ach)
{
  uint32_t sr = regs->STATUS;
	if (0 == (sr & 0xFF)) // RX FIFO Empty?
	{
	  return false;
	}

	*ach = regs->FIFO;
	return true;
}

bool THwUart_esp::SendFinished()
{
  uint32_t sr = regs->STATUS;
  if (0 == (sr & 0xFF)) // RX FIFO Empty?
  {
    return true;
  }
	else
	{
		return false;
	}
}

#if HWDMA_IMPLEMENTED

void THwUart_esp::DmaAssign(bool istx, THwDmaChannel * admach)
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

bool THwUart_esp::DmaStartSend(THwDmaTransfer * axfer)
{
	if (!txdma)
	{
		return false;
	}

	regs->CR3 |= (1 << 7); // enable the TX DMA

	txdma->StartTransfer(axfer);

	return true;
}

bool THwUart_esp::DmaStartRecv(THwDmaTransfer * axfer)
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
