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
 *  file:     hwspi_rp.cpp
 *  brief:    RP2040 SPI (master only)
 *  version:  1.00
 *  date:     2022-05-18
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwspi.h"

#include "rp_utils.h"

bool THwSpi_rp::Init(int adevnum)
{
  uint32_t reset_mask;

	devnum = adevnum;
	initialized = false;

	regs = nullptr;
	if (0 == devnum)
	{
		regs = spi0_hw;
    reset_mask = RESETS_RESET_SPI0_BITS;
	}
	else if (1 == devnum)
	{
    regs = spi1_hw;
    reset_mask = RESETS_RESET_SPI1_BITS;
	}

	if (!regs)
	{
		return false;
	}

  rp_reset_control(reset_mask, true);  // force reset
  rp_reset_control(reset_mask, false); // remove reset

  regs->cpsr = 2; // fix /2 prescaler

  basespeed = (SystemCoreClock >> 1);
  unsigned scr = basespeed / speed;
  if (scr > 0)  --scr; // one less

	unsigned cr0 = (0
	  | (scr <<  8)  // SCR(8)
	  | (0  <<  7)  // SPH: clock phase
	  | (0  <<  6)  // SPO: clock polarity
	  | (1  <<  4)  // FRF(2): 0 = Motorola, 1 = TI, 2 = Microwire
	  | (7  <<  0)  // DSS(4): 3 = 4-bit ... 7 = 8-bit ... 15 = 16-bit
	);

  if (idleclk_high)     cr0 |= (1 << 6);
  if (datasample_late)  cr0 |= (1 << 7);

  unsigned cr1 = (0
    | (0  <<  3)  // SOD: slave-mode output disable (slave mode only)
    | (0  <<  2)  // MS: 0 = master mode, 1 = slave mode
    | (1  <<  1)  // SSE: 1 = enable SSP
    | (0  <<  0)  // LBM: 1 = loopback mode
  );

  regs->cr1 = cr1;
  regs->cr0 = cr0;

	initialized = true;

	return true;
}

bool THwSpi_rp::TrySendData(uint8_t adata)
{
  if (regs->sr & SPI_SSPSR_TNF_BITS)
  {
    regs->dr = adata;
    return true;
  }
  else
  {
    return false;
  }
}

bool THwSpi_rp::TryRecvData(uint8_t * dstptr)
{
  if (regs->sr & SPI_SSPSR_RNE_BITS)
  {
    *dstptr = regs->dr;
    return true;
  }
  else
  {
    return false;
  }
}

bool THwSpi_rp::SendFinished()
{
  if (regs->sr & SPI_SSPSR_BSY_BITS)
  {
    return false;
  }
  else
  {
    return true;
  }
}

void THwSpi_rp::DmaAssign(bool istx, THwDmaChannel * admach)
{
  if (istx)
  {
    txdma = admach;
  }
  else
  {
    rxdma = admach;
  }

  admach->Prepare(istx, (void *)&regs->dr, 0);
}

bool THwSpi_rp::DmaStartSend(THwDmaTransfer * axfer)
{
	if (!txdma)
	{
		return false;
	}

  regs->dmacr |= (1 << 1); // enable the TX DMA

	txdma->StartTransfer(axfer);

	return true;
}

bool THwSpi_rp::DmaStartRecv(THwDmaTransfer * axfer)
{
	if (!rxdma)
	{
		return false;
	}

  regs->dmacr |= (1 << 0); // enable the RX DMA

	rxdma->StartTransfer(axfer);

	return true;
}

bool THwSpi_rp::DmaSendCompleted()
{
	if (txdma && txdma->Active())
	{
		// Send DMA is still active
		return false;
	}

	return SendFinished();
}

bool THwSpi_rp::DmaRecvCompleted()
{
	if (rxdma && rxdma->Active())
	{
		// Send DMA is still active
		return false;
	}

	return true;
}

void THwSpi_rp::SetCs(unsigned value)
{
  if (manualcspin)
  {
    manualcspin->SetTo(value);
  }
  else
  {
#if 0
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
#endif
  }
}

