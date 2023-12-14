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
#include "msp_utils.h"


bool THwUart_msp::Init(int adevnum)  // devnum = 0 - 3 (UART0 .. UART3)
{
	unsigned code;
	unsigned busid = 0;

	devnum = adevnum;
	initialized = false;
	regs = nullptr;

	if (0 == devnum)
	{
	  regs = UART0;
	}
	else if (1 == devnum)
  {
    regs = UART1;
  }
  else if (2 == devnum)
  {
    regs = UART2;
  }
	else if (3 == devnum)
	{
	  regs = UART3;
	  busid = 1; // high speed bus
	}
	else
	{
	  return false;
	}

  periphclock = msp_bus_speed(busid);

  regs->GPRCM.PWREN = (UART_PWREN_KEY_UNLOCK_W | UART_PWREN_ENABLE_MASK);
  if (regs->GPRCM.PWREN) { } // do some syncing
  regs->CTL0 &= ~(UART_CTL0_ENABLE_ENABLE);  // disable first

  regs->CLKSEL = UART_CLKSEL_BUSCLK_SEL_ENABLE;
  regs->CLKDIV = 0; // do not divide the bus clock

  regs->INT_EVENT1.IMASK = (1 << 10); // enable the RX DMA trigger
  regs->INT_EVENT2.IMASK = (1 << 11); // enable the TX DMA trigger

  SetBaudRate(baudrate);

  unsigned lcrh = (0
    | (0  << 21)  // EXTDIR_HOLD(5)
    | (0  << 16)  // EXTDIR_SETUP(5)
    | (0  <<  7)  // SENDIDLE: 0 = no idle pattern sending
    | (0  <<  6)  // SPS: 0 = disable stick parity
    | (0  <<  4)  // WLEN(2): 0 = 5 bit, 3 = 8 bits
    | (0  <<  3)  // STP2: 0 = 1x stop bit, 1 = 2 stop bits
    | (0  <<  2)  // EPS: 0 = odd parity, 1 = even parity
    | (0  <<  1)  // PEN: 1 = enable parity
    | (0  <<  0)  // BRK: 1 = send break
  );
  if (parity)
  {
    if (!oddparity)
    {
      lcrh |= (1 << 2);
    }
    lcrh |= (1 << 1);
  }
  lcrh |= ((databits - 5) << 4);
  if (halfstopbits > 2)
  {
    lcrh |= (1 << 3);
  }
  regs->LCRH = lcrh;

  regs->IFLS = (0
    | (0  <<  8)  // RXTOSEL(4): RX timeout select
    | (7  <<  4)  // RXIFLSEL(3): RX interrupt / DMA level, 7 = at least 1 entry is filled
    | (7  <<  0)  // TXIFLSEL(3): TX interrupt / DMA level, 7 = at least 1 entry is free
  );

  unsigned ctl = (0
    | (0  << 19)  // MSBFIRST: 0 = LSB first (standard), 1 = MSB first
    | (1  << 18)  // MAJVOTE: 1 = majority voting of 3 samples, 0 = takes center value
    | (1  << 17)  // FEN: fifo enable, 1 = enable transmit and receive FIFOs
    | (0  << 15)  // HSE(2): 0 = 16x oversampling, 1 = 8x oversampling, 2 = 3x oversampling
    | (0  << 14)  // CTSEN: 1 = enable CTS HW flow control
    | (0  << 13)  // RTSEN: 1 = enable RTS HW flow control
    | (0  << 12)  // RTS: 1 = signal RTS
    | (0  <<  8)  // MODE(3): 0 = normal operation, 1 = RS485, 2 = IDLE, 3 = 9-bit address, 4 = Smart Card, 5 = DALI
    | (0  <<  7)  // MENC: 1 = enable manchester encoding
    | (0  <<  6)  // TXD_OUT: manual TXD value
    | (0  <<  5)  // TXD_OUT_EN: 1 = enable manual TXD control
    | (1  <<  4)  // TXE: 1 = enable transmit
    | (1  <<  3)  // RXE: 1 = enable receive
    | (0  <<  2)  // LBE: 1 = loopback enable
    | (1  <<  0)  // ENABLE: 1 = enable the uart
  );
  regs->CTL0 = ctl;


	initialized = true;

	return true;

}

bool THwUart_msp::SetBaudRate(int abaudrate)
{
  baudrate = abaudrate;

  unsigned ctl = regs->CTL0;
  ctl &= ~(3 << 15); // clear the oversampling field (0 = 16x)
  unsigned baseclock = (periphclock >> 4);  // div 16
  if (baseclock < baudrate)
  {
    // switch to 8x oversampling
    baseclock = (periphclock >> 3);  // div 8
    ctl |= (1 << 15);
  }
  regs->CTL0 = ctl;

  unsigned divider = (baseclock << 6) / baudrate;
  regs->IBRD = (divider >> 6);
  regs->FBRD = (divider & 0x3F);

  // Reference manual:
  // "any changes to the baud-rate divisor must be followed by a write to the LCRH register for the changes to take effect"
  volatile unsigned lcrh = regs->LCRH;
  regs->LCRH = lcrh;

  return true;
}

bool THwUart_msp::TrySendChar(char ach)
{
	if (regs->STAT & UART_STAT_TXFF_MASK)
	{
	  return false;
	}

	regs->TXDATA = ach;
	return true;
}

bool THwUart_msp::TryRecvChar(char * ach)
{
	if (regs->STAT & UART_STAT_RXFE_MASK)
	{
	  return false;
	}

	*ach = regs->RXDATA;
	return true;
}

void THwUart_msp::DmaAssign(bool istx, THwDmaChannel * admach)
{
	if (istx)
	{
		txdma = admach;
		admach->Prepare(istx, (void *)&regs->TXDATA, 0);
	}
	else
	{
		rxdma = admach;
		admach->Prepare(istx, (void *)&regs->RXDATA, 0);
	}
}

bool THwUart_msp::DmaStartSend(THwDmaTransfer * axfer)
{
	if (!txdma)
	{
		return false;
	}

	txdma->StartTransfer(axfer);
	return true;
}

bool THwUart_msp::DmaStartRecv(THwDmaTransfer * axfer)
{
	if (!rxdma)
	{
		return false;
	}

	rxdma->StartTransfer(axfer);
	return true;
}
