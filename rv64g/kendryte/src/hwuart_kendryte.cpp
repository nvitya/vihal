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
// file:     hwuart_kendryte.cpp
// brief:    Kendryte UART driver
// created:  2022-07-23
// authors:  nvitya

#include <stdio.h>
#include <stdarg.h>

#include "hwuart.h"

#include "kendryte_utils.h"

bool THwUart_kendryte::Init(int adevnum)
{
	unsigned code;
	unsigned resetmask;

	devnum = adevnum;
	initialized = false;

	regs = nullptr;

  SYSCTL->CLK_EN_CENT |= SYSCTL_CLK_EN_CENT_APB0;

	if (1 == devnum)
	{
    regs = UART1;
    SYSCTL->CLK_EN_PERI |= SYSCTL_CLK_EN_PERI_UART1;
    resetmask = SYSCTL_PERI_RESET_UART1;
	}
  else if (2 == devnum)
  {
    regs = UART2;
    SYSCTL->CLK_EN_PERI |= SYSCTL_CLK_EN_PERI_UART2;
    resetmask = SYSCTL_PERI_RESET_UART2;
  }
  else if (3 == devnum)
  {
    regs = UART3;
    SYSCTL->CLK_EN_PERI |= SYSCTL_CLK_EN_PERI_UART3;
    resetmask = SYSCTL_PERI_RESET_UART3;
  }

	if (!regs)
	{
		return false;
	}

	SYSCTL->PERI_RESET |= resetmask;
	// some delay
	for (unsigned n = 0; n < 100; ++n)
	{
	  if (SYSCTL->PERI_RESET) { }
	}
	SYSCTL->PERI_RESET &= ~resetmask;

  uint32_t stopbit_val = (halfstopbits > 2 ? 1 : 0);
  uint32_t parity_val = 0;
  if (parity)
  {
    parity_val = (oddparity ? 1 : 3);
  }

  uint32_t freq = kendryte_bus_speed(0);
  uint32_t divisor = freq / baudrate;
  uint8_t dlh = divisor >> 12;
  uint8_t dll = (divisor - (dlh << 12)) / 16;
  uint8_t dlf = divisor - (dlh << 12) - dll * 16;

  /* Set UART registers */

  regs->LCR |= (1 << 7);  // this should be some kind of config_en
  regs->DLH = dlh;
  regs->DLL = dll;
  regs->DLF = dlf;

  regs->LCR = 0;
  regs->LCR = (databits - 5) | (stopbit_val << 2) | (parity_val << 3);
  regs->LCR &= ~(1 << 7);  // remove config en

  regs->IER = 0;  // disable interrupts
  regs->FCR = (0
    | (0 << 6)  // Receive FIFO threshold
    | (0 << 4)  // Send FIFO threshold
    | (1 << 3)
    | (1 << 0)
  );

	initialized = true;

	return true;
}

bool THwUart_kendryte::TrySendChar(char ach)
{
	if (0 == (regs->LSR & (1 << 5)))
	{
		return false;
	}

	regs->THR = ach;

	return true;
}

bool THwUart_kendryte::TryRecvChar(char * ach)
{
	if (regs->LSR & 1)
	{
		*ach = (regs->RBR & 0xFF);
		return true;
	}
	else
	{
		return false;
	}
}

bool THwUart_kendryte::SendFinished()
{
	if (regs->LSR & (1 << 5)) // not perfect
	{
		return true;
	}
	else
	{
		return false;
	}
}

#if HWDMA_IMPLEMENTED

void THwUart_kendryte::DmaAssign(bool istx, THwDmaChannel * admach)
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

bool THwUart_kendryte::DmaStartSend(THwDmaTransfer * axfer)
{
	if (!txdma)
	{
		return false;
	}

	regs->CR3 |= (1 << 7); // enable the TX DMA

	txdma->StartTransfer(axfer);

	return true;
}

bool THwUart_kendryte::DmaStartRecv(THwDmaTransfer * axfer)
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
