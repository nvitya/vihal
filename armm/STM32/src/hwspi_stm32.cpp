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
 *  file:     hwspi_stm32.cpp
 *  brief:    STM32 SPI (master only)
 *  version:  1.00
 *  created:  2021-10-24
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwspi.h"

#include "stm32_utils.h"

#ifdef RCC_APB1ENR1_SPI2EN
  #define RCC_APB1ENR_SPI2EN   RCC_APB1ENR1_SPI2EN
  #define RCC_APB1ENR_SPI3EN   RCC_APB1ENR1_SPI3EN
#elif defined(RCC_APBENR1_SPI2EN)
  #define RCC_APB1ENR_SPI2EN   RCC_APBENR1_SPI2EN
  #define RCC_APB2ENR_SPI1EN   RCC_APBENR2_SPI1EN
#elif defined(RCC_APB1LENR_SPI2EN)
  #define RCC_APB1ENR_SPI2EN   RCC_APB1LENR_SPI2EN
  #define RCC_APB1ENR_SPI3EN   RCC_APB1LENR_SPI3EN
#endif


bool THwSpi_stm32::Init(int adevnum)
{
	uint8_t busid = STM32_BUSID_APB2;

	devnum = adevnum;

	initialized = false;

	regs = nullptr;
	if (false)
	{

	}
#if defined(SPI1_BASE)
	else if (1 == devnum)
	{
		regs = (SPI_TypeDef *)SPI1_BASE;
		APB2ENR_REGISTER |= RCC_APB2ENR_SPI1EN;
	}
#endif
#if defined(SPI2_BASE)
	else if (2 == devnum)
	{
		regs = (SPI_TypeDef *)SPI2_BASE;
		APB1ENR_REGISTER |= RCC_APB1ENR_SPI2EN;
		busid = STM32_BUSID_APB1;
	}
#endif
#if defined(SPI3_BASE)
	else if (3 == devnum)
	{
		regs = (SPI_TypeDef *)SPI3_BASE;
	  APB1ENR_REGISTER |= RCC_APB1ENR_SPI3EN;
		busid = STM32_BUSID_APB1;
	}
#endif
#if defined(SPI4_BASE)
	else if (4 == devnum)
	{
		regs = (SPI_TypeDef *)SPI4_BASE;
		RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;
	}
#endif
#if defined(SPI5_BASE)
	else if (5 == devnum)
	{
		regs = (SPI_TypeDef *)SPI5_BASE;
		RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
	}
#endif
#if defined(SPI6_BASE)
	else if (6 == devnum)
	{
		regs = (SPI_TypeDef *)SPI6_BASE;
#if defined(RCC_APB4ENR_SPI6EN)
		RCC->APB4ENR |= RCC_APB4ENR_SPI6EN;
#else
		RCC->APB2ENR |= RCC_APB2ENR_SPI6EN;
#endif
	}
#endif

	if (!regs)
	{
		return false;
	}

	basespeed = stm32_bus_speed(busid);

	regs->CR1 &= ~(1 << 6);  // SPI Disable

	unsigned n;

	n = (7 << 3)   // BR(3): will be changed later at SetSpeed()
		| (1 << 2)   // Master Mode
		| (1 << 9)   // 1 = software NSS management
		| (1 << 8)   // SSI bit
	;

	if (lsb_first)  n |= (1 << 7);
	if (idleclk_high) n |= (1 << 1);
	if (datasample_late) n |= (1 << 0);

	regs->CR1 = n;

	SetSpeed(speed);

	n =	(((databits-1) & 0xF) << 8)
		| (0 <<  4)  // Motorola Frame mode
		| (1 << 12)  // 8 bit fifo settings
	;

	if (inter_frame_pulse)  n |= (1 << 3);  // generate NSS pulse
	n |= (1 << 2);  // 1 = SS output enable

	regs->CR2 = n;

	regs->CR1 |= (1 << 6);  // SPI Enable

	initialized = true;

	return true;
}

void THwSpi_stm32::SetSpeed(unsigned aspeed)
{
  // speed: the base speed will be divided with the powers of 2
  unsigned dcode = 0;
  while ((basespeed / (2 << dcode) > aspeed) && (dcode < 7))
  {
    ++dcode;
  }

  speed = basespeed / (2 << dcode);  // shows the actual speed

  uint32_t c1reg = regs->CR1;
  c1reg &= ~(7 << 3);
  c1reg |= dcode << 3;
  regs->CR1 = c1reg;
}

bool THwSpi_stm32::DmaStartSend(THwDmaTransfer * axfer)
{
	if (!txdma)
	{
		return false;
	}

	//regs->CR2 &= ~(1 << 1); // disable the TX DMA
	regs->CR2 |= (1 << 1); // enable the TX DMA

	txdma->StartTransfer(axfer);

	return true;
}

bool THwSpi_stm32::DmaStartRecv(THwDmaTransfer * axfer)
{
	if (!rxdma)
	{
		return false;
	}

	//regs->CR2 &= ~(1 << 0); // disable the RX DMA
	regs->CR2 |= (1 << 0); // enable the RX DMA

	rxdma->StartTransfer(axfer);

	return true;
}

bool THwSpi_stm32::DmaSendCompleted()
{
	if (txdma && txdma->Active())
	{
		// Send DMA is still active
		return false;
	}

	return SendFinished();
}

bool THwSpi_stm32::DmaRecvCompleted()
{
	if (rxdma && rxdma->Active())
	{
		// Send DMA is still active
		return false;
	}

	return true;
}

#if defined(MCUSF_H7)  // SPI v2

bool THwSpi_stm32::TrySendData(uint8_t adata)
{
	if (regs->SR & SPI_SR_TXP)
	{
		*(__IO uint8_t *)(&(regs->TXDR)) = adata;
		return true;
	}
	else
	{
		return false;
	}
}

bool THwSpi_stm32::TryRecvData(uint8_t * dstptr)
{
	if (regs->SR & SPI_SR_RXP)
	{
		*dstptr = *(__IO uint8_t *)(&regs->RXDR);
		return true;
	}
	else
	{
		return false;
	}
}

void THwSpi_stm32::DmaAssign(bool istx, THwDmaChannel * admach)
{
	if (istx)
	{
		txdma = admach;
	}
	else
	{
		rxdma = admach;
	}

	admach->Prepare(istx, (void *)&regs->RXDR, 0);
}

bool THwSpi_stm32::SendFinished()
{
	if (regs->SR & SPI_SR_TXC)
	{
		return false;
	}
	else
	{
		return true;
	}
}

#else

bool THwSpi_stm32::TrySendData(uint8_t adata)
{
	if (regs->SR & SPI_SR_TXE)
	{
		*(__IO uint8_t *)(&(regs->DR)) = adata;
		return true;
	}
	else
	{
		return false;
	}
}

bool THwSpi_stm32::TryRecvData(uint8_t * dstptr)
{
	if (regs->SR & SPI_SR_RXNE)
	{
		*dstptr = *(__IO uint8_t *)(&regs->DR);
		return true;
	}
	else
	{
		return false;
	}
}

void THwSpi_stm32::DmaAssign(bool istx, THwDmaChannel * admach)
{
	if (istx)
	{
		txdma = admach;
	}
	else
	{
		rxdma = admach;
	}

	admach->Prepare(istx, (void *)&regs->DR, 0);
}

bool THwSpi_stm32::SendFinished()
{
	if (regs->SR & SPI_SR_BSY)
	{
		return false;
	}
	else
	{
		return true;
	}
}

#endif

void THwSpi_stm32::SetCs(unsigned value)
{
  if (manualcspin)
  {
    manualcspin->SetTo(value);
  }
  else
  {
    uint32_t tmp = regs->CR1;
    if (value)
    {
      tmp |= SPI_CR1_SSI;
    }
    else
    {
      tmp &= ~SPI_CR1_SSI;
    }
    regs->CR1 = tmp;
  }
}

