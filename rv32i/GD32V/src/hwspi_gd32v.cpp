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
 *  file:     hwspi_gd32v.cpp
 *  brief:    STM32 SPI (master only)
 *  created:  2022-07-04
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwspi_gd32v.h"

#include "gd32v_utils.h"

bool THwSpi_gd32v::Init(int adevnum)
{
	uint8_t busid = GD32V_BUSID_APB2;

	devnum = adevnum;

	initialized = false;

  RCU->APB2EN |= RCU_APB2EN_AFEN;

	regs = nullptr;
	if (false)
	{

	}
	else if (0 == devnum)
	{
		regs = SPI0;
		RCU->APB2EN |= RCU_APB2EN_SPI0EN;
	}
	else if (1 == devnum)
	{
    regs = SPI1;
    RCU->APB1EN |= RCU_APB1EN_SPI1EN;
		busid = GD32V_BUSID_APB1;
	}
	else if (2 == devnum)
	{
    regs = SPI2;
    RCU->APB1EN |= RCU_APB1EN_SPI2EN;
    busid = GD32V_BUSID_APB1;
	}

	if (!regs)
	{
		return false;
	}

	basespeed = gd32v_bus_speed(busid);

	// speed: the base speed will be divided with the powers of 2
	unsigned psc = 0;
	while ((basespeed / (2 << psc) > speed) && (psc < 7))
	{
		++psc;
	}

	regs->CTL0 &= ~(1 << 6);  // SPI Disable

	unsigned n;

	n = (psc << 3)
		| (1 << 2)   // Master Mode
		| (1 << 9)   // 1 = software NSS management
		| (1 << 8)   // SSI bit
	;

	if (lsb_first)  n |= (1 << 7);
	if (idleclk_high) n |= (1 << 1);
	if (datasample_late) n |= (1 << 0);
	if (16 == databits)  n |= (1 << 11);

	regs->CTL0 = n;

	n =	(0
		| (0 <<  4)  // Motorola Frame mode
	);

	if (inter_frame_pulse)  n |= (1 << 3);  // generate NSS pulse
	n |= (1 << 2);  // 1 = SS output enable

	regs->CTL1 = n;

	regs->I2SCTL = 0;

	regs->CTL0 |= (1 << 6);  // SPI Enable

	initialized = true;

	return true;
}

bool THwSpi_gd32v::DmaStartSend(THwDmaTransfer * axfer)
{
	if (!txdma)
	{
		return false;
	}

	//regs->CR2 &= ~(1 << 1); // disable the TX DMA
	regs->CTL1 |= (1 << 1); // enable the TX DMA

	txdma->StartTransfer(axfer);

	return true;
}

bool THwSpi_gd32v::DmaStartRecv(THwDmaTransfer * axfer)
{
	if (!rxdma)
	{
		return false;
	}

	//regs->CR2 &= ~(1 << 0); // disable the RX DMA
	regs->CTL1 |= (1 << 0); // enable the RX DMA

	rxdma->StartTransfer(axfer);

	return true;
}

bool THwSpi_gd32v::DmaSendCompleted()
{
	if (txdma && txdma->Active())
	{
		// Send DMA is still active
		return false;
	}

	return SendFinished();
}

bool THwSpi_gd32v::DmaRecvCompleted()
{
	if (rxdma && rxdma->Active())
	{
		// Send DMA is still active
		return false;
	}

	return true;
}

bool THwSpi_gd32v::TrySendData(uint8_t adata)
{
	if (regs->STAT & SPI_STAT_TBE)
	{
		*(__IO uint8_t *)(&(regs->DATA)) = adata;
		return true;
	}
	else
	{
		return false;
	}
}

bool THwSpi_gd32v::TryRecvData(uint8_t * dstptr)
{
	if (regs->STAT & SPI_STAT_RBNE)
	{
		*dstptr = *(__IO uint8_t *)(&regs->DATA);
		return true;
	}
	else
	{
		return false;
	}
}

void THwSpi_gd32v::DmaAssign(bool istx, THwDmaChannel * admach)
{
	if (istx)
	{
		txdma = admach;
	}
	else
	{
		rxdma = admach;
	}

	admach->Prepare(istx, (void *)&regs->DATA, 0);
}

bool THwSpi_gd32v::SendFinished()
{
	if (regs->STAT & SPI_STAT_TRANS)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void THwSpi_gd32v::SetCs(unsigned value)
{
  if (manualcspin)
  {
    manualcspin->SetTo(value);
  }
  else
  {
    uint32_t tmp = regs->CTL0;
    if (value)
    {
      tmp |= SPI_CTL0_SWNSS;
    }
    else
    {
      tmp &= ~SPI_CTL0_SWNSS;
    }
    regs->CTL0 = tmp;
  }
}

