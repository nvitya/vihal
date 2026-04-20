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
 *  file:     hwuart_py32.cpp
 *  brief:    PY32 UART
 *  date:     2026-04-20
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>

#include "hwuart.h"

#include "py32_utils.h"

bool THwUart_py32::Init(int adevnum)
{
	unsigned code;
	uint8_t busid = PY32_BUSID_APB1;

	//bool     lpuart = false;

	devnum = adevnum;
	initialized = false;

	regs = nullptr;

  if (1 == devnum)
	{
		regs = (USART_TypeDef *)USART1_BASE;
		RCC->APBENR2 |= RCC_APBENR2_USART1EN;
		busid = PY32_BUSID_APB2;
	}
	else if (2 == devnum)
	{
		regs = (USART_TypeDef *)USART2_BASE;
		RCC->APBENR1 |= RCC_APBENR1_USART2EN;
	}
	else if (3 == devnum)
	{
		regs = (USART_TypeDef *)USART3_BASE;
		RCC->APBENR1 |= RCC_APBENR1_USART3EN;
	}

	if (!regs)
	{
		return false;
	}

	uint32_t cr1 = regs->CR1;
	uint32_t cr2 = regs->CR2;

	// disable UART in order to the CR registers
	cr1 &=  ~USART_CR1_UE;
	regs->CR1 = cr1;

	// STOP BITS:
	code = 0; // = 1 stopbit
	if      (1 == halfstopbits)  code = 1;
	else if (3 == halfstopbits)  code = 3;
	else if (4 == halfstopbits)  code = 2;
	cr2 &= ~(3 << 12);
	cr2 |= (code << 12);

	// DATA BITS = 8:
	uint32_t effdatabits = databits;

	// PARITY:
	code = 0; // parity off
	if (parity)
	{
		code |= 2;
		if (oddparity)  code |= 1;
		++effdatabits;  // parity increases the effective data bits !
	}
	cr1 &= ~(3 << 9);
	cr1 |= (code << 9);

	code = 0; // = 8 databits
	if      (7 == effdatabits)  code = 2;
	if      (9 == effdatabits)  code = 1;
	cr1 &= ~((1 << 28) | (1 << 12));
	cr1 |= ((code & 1) << 12) | (((code >> 1) & 1) << 28);


#if defined(USART_CR1_FIFOEN)
	cr1 |= USART_CR1_FIFOEN;
#endif

	// Enable RX, TX:
	cr1 |= USART_CR1_TE | USART_CR1_RE;

	// disable LIN and CLK out
	cr2 &= ~(USART_CR2_CLKEN);
	// disable hadware flow control and others:
	regs->CR3 &= ~(USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_HDSEL);

	regs->CR2 = cr2;
	regs->CR1 = cr1;  // the uart is still not enabled yet !

	periphclock = py32_bus_speed(busid);
	SetBaudRate();  // might change the CR1 !

	// Finally enable the the UART (locks the CR registers)
	regs->CR1 |= USART_CR1_UE;

	initialized = true;

	return true;
}

bool THwUart_py32::SetPeriphClock(unsigned aclk)
{
  periphclock = aclk;
  return SetBaudRate();
}

bool THwUart_py32::SetBaudRate(int abaudrate)
{
  if(abaudrate > 0)
  {
    baudrate = abaudrate;
  }

  unsigned baseclock = periphclock / 16;
  unsigned divider = ((baseclock << 4) + 8) / baudrate;

  uint32_t tmp = regs->CR1;
  regs->CR1 = tmp & ~USART_CR1_UE;

  #ifdef USART_CR1_OVER8
    if (divider < 16)
    {
      baseclock = periphclock / 8;
      divider = ((baseclock << 4) + 8) / baudrate;
      tmp |= USART_CR1_OVER8;
      divider = ((divider & 0xFFF0) | ((divider & 0xE) >> 1));
    }
    else
    {
      tmp &= ~USART_CR1_OVER8;
    }
    regs->CR1 = tmp & ~USART_CR1_UE;
  #endif

  regs->BRR = divider;

  regs->CR1 = tmp;

  return true;
}

bool THwUart_py32::TrySendChar(char ach)
{
	if (((regs->SR & USART_SR_TC) == 0) && ((regs->SR & USART_SR_TXE) == 0))
	{
		return false;
	}

	regs->DR = ach;
	return true;
}

bool THwUart_py32::TryRecvChar(char * ach)
{
	if (regs->SR & USART_SR_RXNE)
	{
		*ach = regs->DR;
		return true;
	}
	else
	{
		return false;
	}
}

bool THwUart_py32::SendFinished()
{
	if (regs->SR & USART_SR_IDLE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

