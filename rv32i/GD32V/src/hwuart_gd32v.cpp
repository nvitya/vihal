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
// file:     hwuart_gd32v.cpp
// brief:    GD32V UART driver
// created:  2021-10-21
// authors:  nvitya

#include <stdio.h>
#include <stdarg.h>

#include "hwuart.h"

#include "gd32v_utils.h"

bool THwUart_gd32v::Init(int adevnum)
{
	unsigned code;
	uint8_t busid = GD32V_BUSID_APB1;

	//bool     lpuart = false;

	devnum = adevnum;
	initialized = false;

	regs = nullptr;

	if (0 == devnum)
	{
	  regs = USART0;
	  busid = GD32V_BUSID_APB2; // this is driven by higher clock
		RCU->APB2EN |= RCU_APB2EN_USART0EN;
	}
	else if (1 == devnum)
	{
    regs = USART1;
    RCU->APB1EN |= RCU_APB1EN_USART1EN;
	}
  else if (2 == devnum)
  {
    regs = USART2;
    RCU->APB1EN |= RCU_APB1EN_USART2EN;
  }
  else if (3 == devnum)
  {
    regs = UART3;
    RCU->APB1EN |= RCU_APB1EN_UART3EN;
  }
  else if (4 == devnum)
  {
    regs = UART4;
    RCU->APB1EN |= RCU_APB1EN_UART4EN;
  }

	if (!regs)
	{
		return false;
	}

	// disable UART
	regs->CTL0 &=  ~USART_CTL0_UEN;

  uint32_t ctl0 = (0
    | (1  << 13)  // UEN: 1 = USART Enable
    | (0  << 12)  // WL: 0 = 8 data bits, 1 = 9 data bits
    | (0  << 11)  // WM: wakeup method
    | (0  << 10)  // PCEN: 1 = parity check function enable
    | (0  <<  9)  // PM: parity mode, 0 = even, 1 = odd
    | (0  <<  8)  // PERRIE: parity error interrupt
    | (0  <<  7)  // TBEIE: Transmitter buffer empty interrupt enable
    | (0  <<  6)  // TCIE: Transmission complete interrupt enable
    | (0  <<  5)  // RBNEIE: Read data buffer not empty interrupt and overrun error interrupt enable
    | (0  <<  4)  // IDLEIE: IDLE line detected interrupt enable
    | (1  <<  3)  // TEN: 1 = transmitter enable
    | (1  <<  2)  // REN: 1 = receiver enable
    | (0  <<  1)  // RWU: 0 = receiver in active mode
    | (0  <<  0)  // SBKCMD: 1 = transmit break frame
  );
  // PARITY:
  if (parity)
  {
    ctl0 |= (1 << 10); // enable parity
    if (oddparity)  ctl0 |= (1 << 9);
  }

  uint32_t ctl1 = (0
    | (0  << 14)  // LMEN: 1 = LIN mode enable
    | (0  << 12)  // STB(2): Stop bits, 0 = 1 bit, 1 = 0.5 bit, 2 = bits, 3 = 1.5 bits
    | (0  << 11)  // CKEN: 0 = CK pin disabled
    | (0  << 10)  // CPL: CK polarity
    | (0  <<  9)  // CPH: CK phase
    | (0  <<  8)  // CLEN: CK length
    | (0  <<  6)  // LBDIE: LIN break detected interrupt enable
    | (0  <<  5)  // LBLEN: LIN break frame length
    | (0  <<  0)  // ADDR(4):  Address of the USART (for fake up address matching)
  );
  // Special STOP bits:
  if      (1 == halfstopbits)  ctl1 |= (1 << 12);
  else if (3 == halfstopbits)  ctl1 |= (3 << 12);
  else if (4 == halfstopbits)  ctl1 |= (2 << 12);

  uint32_t ctl2 = (0
    | (0  << 10)  // CTSIE
    | (0  <<  9)  // CTSEN
    | (0  <<  8)  // RTSEN
    | (0  <<  7)  // DENT: 1 = Tx DMA Enable
    | (0  <<  6)  // DENR: 1 = Rx DMA Enable
    | (0  <<  5)  // SCEN: 1 = Smartcard mode enable
    | (0  <<  4)  // NKEN: 1 = NACK transmission enable in Smartcard mode
    | (0  <<  3)  // HDEN: 1 = Half-duplex enable
    | (0  <<  2)  // IRLP: 1 = IrDA low-power mode
    | (0  <<  1)  // IREN: 1 = IrDA mode enable
    | (0  <<  0)  // ERRIE: 1 = Error interrupt enable
  );

	regs->GP = 1;  // set the prescaler

	unsigned periphclock = gd32v_bus_speed(busid);
	unsigned baseclock = periphclock / 16;
	unsigned divider = ((baseclock << 4) + 8) / baudrate;

	regs->BAUD = divider;

	regs->CTL2 = ctl2;
	regs->CTL1 = ctl1;
  regs->CTL0 = ctl0;  // also enables the UART

	initialized = true;

	return true;
}

bool THwUart_gd32v::TrySendChar(char ach)
{
	if ((regs->STAT & USART_STAT_TBE) == 0)
	{
		return false;
	}

	regs->DATA = ach;

	return true;
}

bool THwUart_gd32v::TryRecvChar(char * ach)
{
	if (regs->STAT & USART_STAT_RBNE)
	{
		*ach = regs->DATA;
		return true;
	}
	else
	{
		return false;
	}
}

bool THwUart_gd32v::SendFinished()
{
	if (regs->STAT & USART_STAT_TBE) // not perfect
	{
		return true;
	}
	else
	{
		return false;
	}
}

#if HWDMA_IMPLEMENTED

void THwUart_gd32v::DmaAssign(bool istx, THwDmaChannel * admach)
{
	if (istx)
	{
		txdma = admach;
		admach->Prepare(istx, (void *)&regs->DATA, 0);
	}
	else
	{
		rxdma = admach;
  	admach->Prepare(istx, (void *)&regs->DATA, 0);
	}
}

bool THwUart_gd32v::DmaStartSend(THwDmaTransfer * axfer)
{
	if (!txdma)
	{
		return false;
	}

	regs->CTL2 |= (1 << 7); // enable the TX DMA

	txdma->StartTransfer(axfer);

	return true;
}

bool THwUart_gd32v::DmaStartRecv(THwDmaTransfer * axfer)
{
	if (!rxdma)
	{
		return false;
	}

	regs->CTL2 |= (1 << 6); // enable the RX DMA

	rxdma->StartTransfer(axfer);

	return true;
}

#endif
