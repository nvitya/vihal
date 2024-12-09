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
 *  file:     hwuart_stm32.cpp
 *  brief:    STM32 UART
 *  version:  1.00
 *  date:     2018-02-10
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>

#include "hwuart.h"

#include "stm32_utils.h"

#ifdef RCC_APB1ENR1_USART2EN
  #define RCC_APB1ENR_USART2EN   RCC_APB1ENR1_USART2EN
  #define RCC_APB1ENR_USART3EN   RCC_APB1ENR1_USART3EN
  #define RCC_APB1ENR_UART4EN    RCC_APB1ENR1_UART4EN
  #define RCC_APB1ENR_UART5EN    RCC_APB1ENR1_UART5EN
#elif defined(RCC_APBENR1_USART2EN)
  #define RCC_APB2ENR_USART1EN   RCC_APBENR2_USART1EN
  #define RCC_APB1ENR_USART2EN   RCC_APBENR1_USART2EN
  #define RCC_APB1ENR_USART3EN   RCC_APBENR1_USART3EN
  #define RCC_APB1ENR_USART5EN   RCC_APBENR1_USART5EN
  #define RCC_APB1ENR_USART6EN   RCC_APBENR1_USART6EN
  #define RCC_APB1ENR_LPUART1EN  RCC_APBENR1_LPUART1EN
#elif defined(RCC_APB1LENR_USART2EN)
  #define RCC_APB1ENR_USART2EN   RCC_APB1LENR_USART2EN
  #define RCC_APB1ENR_USART3EN   RCC_APB1LENR_USART3EN
  #define RCC_APB1ENR_UART4EN    RCC_APB1LENR_UART4EN
  #define RCC_APB1ENR_UART5EN    RCC_APB1LENR_UART5EN
#endif

bool THwUart_stm32::Init(int adevnum)
{
	unsigned code;
	uint8_t busid = STM32_BUSID_APB1;

	//bool     lpuart = false;

	devnum = adevnum;
	initialized = false;

	regs = nullptr;

	if ( (0x101 == devnum)  // fix LPUART1
#if !defined(USART1_BASE)
			 || (1 == devnum)
#endif
		 )
	{
		#if defined(LPUART1_BASE)
			regs = (USART_TypeDef *)LPUART1_BASE;
			#ifdef RCC_APB1ENR2_LPUART1EN
				RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;
			#else
        #if defined(RCC_APB4ENR_LPUART1EN)
				  RCC->APB4ENR |= RCC_APB4ENR_LPUART1EN;
        #else
				  APB1ENR_REGISTER |= RCC_APB1ENR_LPUART1EN;
        #endif
			#endif
			//lpuart = true;
		#endif
	}
#if defined(USART1_BASE)
	else if (1 == devnum)
	{
			regs = (USART_TypeDef *)USART1_BASE;
			//RCC->APB1ENR |= RCC_APB1ENR_USART1EN;
			APB2ENR_REGISTER |= RCC_APB2ENR_USART1EN;
			busid = STM32_BUSID_APB2;
	}
#endif
#if defined(USART2_BASE)
	else if (2 == devnum)
	{
		regs = (USART_TypeDef *)USART2_BASE;
		APB1ENR_REGISTER |= RCC_APB1ENR_USART2EN;
	}
#endif
#if defined(USART3_BASE)
	else if (3 == devnum)
	{
		regs = (USART_TypeDef *)USART3_BASE;
		APB1ENR_REGISTER |= RCC_APB1ENR_USART3EN;
	}
#endif
#if defined(UART4_BASE)
	else if (4 == devnum)
	{
		regs = (USART_TypeDef *)UART4_BASE;
		APB1ENR_REGISTER |= RCC_APB1ENR_UART4EN;
	}
#endif
#if defined(UART5_BASE)
	else if (5 == devnum)
	{
		regs = (USART_TypeDef *)UART5_BASE;
		APB1ENR_REGISTER |= RCC_APB1ENR_UART5EN;
	}
#endif
#if defined(USART5_BASE)
	else if (5 == devnum)
	{
		regs = (USART_TypeDef *)USART5_BASE;
		APB1ENR_REGISTER |= RCC_APB1ENR_USART5EN;
	}
#endif
#if defined(USART6_BASE)
	else if (6 == devnum)
	{
		regs = (USART_TypeDef *)USART6_BASE;
    #if defined(RCC_APBENR1_USART6EN)
		  APB1ENR_REGISTER |= RCC_APBENR1_USART6EN;
		#else
			RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
			busid = STM32_BUSID_APB2;
		#endif
	}
#endif

	if (!regs)
	{
		return false;
	}

	uint32_t cr1 = regs->CR1;
	uint32_t cr2 = regs->CR2;

	// disable UART
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
#if defined(USART_CR2_LINEN)
	cr2 &= ~(USART_CR2_LINEN | USART_CR2_CLKEN);
	// disable hadware flow control and others:
	regs->CR3 &= ~(USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN);
#else // e.g. F030
	cr2 &= ~(USART_CR2_CLKEN);
	// disable hadware flow control and others:
	regs->CR3 &= ~(USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_HDSEL);
#endif

#if defined(USART_PRESC_PRESCALER)
	regs->PRESC = 0; // do not divide the input clock
#endif

	regs->CR2 = cr2;
	regs->CR1 = cr1;  // the uart is still not enabled yet !

	periphclock = stm32_bus_speed(busid);
	SetBaudRate();

	// Enable:
	cr1 |= USART_CR1_UE;
	regs->CR1 = cr1;

	initialized = true;

	return true;
}

bool THwUart_stm32::SetPeriphClock(unsigned aclk)
{
  periphclock = aclk;
  return SetBaudRate();
}

bool THwUart_stm32::SetBaudRate(int abaudrate)
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

bool THwUart_stm32::TrySendChar(char ach)
{
#if defined(USART_ISR_TXE)
	if ((regs->ISR & USART_ISR_TXE) == 0)
	{
		return false;
	}

	regs->TDR = ach;
#elif defined(USART_ISR_TXE_TXFNF)
	if ((regs->ISR & USART_ISR_TXE_TXFNF) == 0)
	{
		return false;
	}

	regs->TDR = ach;
#else
	if (((regs->SR & USART_SR_TC) == 0) && ((regs->SR & USART_SR_TXE) == 0))
	{
		return false;
	}

	regs->DR = ach;
#endif

	return true;
}

bool THwUart_stm32::TryRecvChar(char * ach)
{
#if defined(USART_ISR_RXNE)
	if (regs->ISR & USART_ISR_RXNE)
	{
		*ach = regs->RDR;
		return true;
	}
	else
	{
		return false;
	}
#elif defined(USART_ISR_RXNE_RXFNE)
	if (regs->ISR & USART_ISR_RXNE_RXFNE)
	{
		*ach = regs->RDR;
		return true;
	}
	else
	{
		return false;
	}
#else
	if (regs->SR & USART_SR_RXNE)
	{
		*ach = regs->DR;
		return true;
	}
	else
	{
		return false;
	}
#endif
}

bool THwUart_stm32::SendFinished()
{
#if defined(USART_ISR_TC)
  // clear transfer complete flag, the flag is immediately set again if uart is idle
  regs->ICR = USART_ICR_CTSCF;

  // __DSB grants that all explicit memory accesses before this instruction are completed
  __DSB();


	if (regs->ISR & USART_ISR_TC)
#else
	if (regs->SR & USART_SR_IDLE)
#endif
	{
		return true;
	}
	else
	{
		return false;
	}
}

#if HWDMA_IMPLEMENTED

void THwUart_stm32::DmaAssign(bool istx, THwDmaChannel * admach)
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

bool THwUart_stm32::DmaStartSend(THwDmaTransfer * axfer)
{
	if (!txdma)
	{
		return false;
	}

	regs->CR3 |= (1 << 7); // enable the TX DMA

	txdma->StartTransfer(axfer);

	return true;
}

bool THwUart_stm32::DmaStartRecv(THwDmaTransfer * axfer)
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
