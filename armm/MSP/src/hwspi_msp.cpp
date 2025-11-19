/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     hwspi_msp.cpp
 *  brief:    Vihal MSPM0G SPI Master Driver
 *  created:  2023-12-14
 *  authors:  nvitya
*/

#include "platform.h"
#include "msp_utils.h"
#include "hwspi.h"
#include "hwspi_msp.h"

bool THwSpi_msp::Init(int adevnum)
{
	unsigned tmp;
	devnum = adevnum;

	regs = nullptr;
	if (0 == devnum)
	{
		regs = SPI0;
	}
	else if (1 == devnum)
	{
		regs = SPI1;
	}

	if (!regs)
	{
		return false;
	}

#if 0
  // note from the reference manual:
  //  "Please note that a SPI Software reset (See section 15.3.4) is required when switching SPI protocol format."
  regs->GPRCM.RSTCTL = (SPI_RSTCTL_KEY_UNLOCK_W | SPI_RSTCTL_RESETASSERT_MASK);
  if (regs->GPRCM.RSTCTL) { } // do some syncing
#endif
  regs->GPRCM.RSTCTL = (SPI_RSTCTL_KEY_UNLOCK_W | SPI_RSTCTL_RESETSTKYCLR_MASK);
  if (regs->GPRCM.RSTCTL) { } // do some syncing

  regs->GPRCM.PWREN = (SPI_PWREN_KEY_UNLOCK_W | SPI_PWREN_ENABLE_MASK);
  if (regs->GPRCM.PWREN) { } // do some syncing

  regs->CLKSEL = (1 << 3);  // select SYSCLK = BUS CLOCK as clock source

  regs->CTL1 &= ~(SPI_CTL1_ENABLE_MASK);  // disable first

  // SPH bit is set to 1 because of continuous sending mode

  uint32_t ctl0 = (0
  	| (0  << 14)  // CSCLR
  	| (0  << 12)  // CSSEL(2): 0 = CS0
  	| (1  <<  9)  // SPH: 0 = capture at first clock edge, 1 = capture at second edge
  	| (0  <<  8)  // SPO: 0 = idle clock level low, 1 = idle clock level high
  	| (0  <<  7)  // PACKEN: 0 = disable packing
  	| (1  <<  5)  // FRF(2): 0 = 3-wire Motorola, 1 = 4-wire Motorola, 2 = TI sync, 3 = Microwire
  	| ((databits - 1) <<  0)  // DSS(5): data size select, 0 = 4-bit, 7 = 8-bits, 15 = 16-bits
  );
	if (idleclk_high)     ctl0 |= (1 << 8);
	if (datasample_late)  ctl0 |= (1 << 9); // this has no effect
  regs->CTL0 = ctl0;

  uint32_t ctl1 = (0
    | (0  << 24)  // RXTIMEOUT(6)
    | (0  << 16)  // REPEATTX(8)
    | (0  << 12)  // CDMODE(4): 0 = manual mode, 1-15=n: C/D (CS3) goes from high to low after n bytes
    | (0  << 11)  // CDENABLE: 1 = CS3 is used as C/D signal
    | (0  <<  8)  // PTEN: parity transmit enable, 1 = tx parity enabled
    | (0  <<  6)  // PES: 1 = even parity
    | (0  <<  5)  // PREN: parity receive enable, 1 = rx parity enabled
    | (0  <<  4)  // MSB: 1 = MSB first
    | (0  <<  3)  // POD: peripheral data output disabled, 1 = MISO drive disabled
    | (1  <<  2)  // CP: 1 = controller (master) mode, 0 = SPI slave (peripheral)
    | (0  <<  1)  // LBM: 1 = loopback mode
    | (0  <<  0)  // ENABLE: 1 = enable
  );
	if (!lsb_first)  ctl1 |= (1 << 4);
	regs->CTL1 = ctl1;

	regs->IFLS = (0
	  | (7  <<  3)  // RXIFLSEL(3): 7 = RX trigger if at least one frame
	  | (7  <<  0)  // TXIFLSEL(3): 7 = TX trigger if at least one free fifo entry
	);

  regs->DMA_TRIG_RX.IMASK = (1 << 3); // enable the RX DMA trigger
  regs->DMA_TRIG_TX.IMASK = (1 << 4); // enable the TX DMA trigger

	SetSpeed(speed);

	regs->CTL1 |= SPI_CTL1_ENABLE_MASK;

	return true;
}

void THwSpi_msp::SetSpeed(unsigned aspeed)
{
	speed = aspeed;

  unsigned baseclock = (msp_bus_speed(1) >> 1);  // a fix clock division is internally added by the formula

	// the minimal clock divisor is 2 !
	unsigned sckdiv = baseclock / speed;
	if (sckdiv * speed != baseclock)
	{
		++sckdiv;
	}

	if (sckdiv < 1)  sckdiv = 1;
	if (sckdiv > 0x3FF)  sckdiv = 0x3FF;

	regs->CLKCTL = (0
	  | (0  << 28)  // DSAMPLE(4): delayed input sample
	  | ((sckdiv - 1) <<  0)  // SCR(10): clock divider
	);
}

bool THwSpi_msp::TrySendData(uint16_t adata)
{
	if (regs->STAT & SPI_STAT_TNF_MASK)
	{
		regs->TXDATA = adata;
		return true;
	}
	else
	{
		return false;
	}
}

bool THwSpi_msp::TrySendData(uint8_t adata)
{
	if (regs->STAT & SPI_STAT_TNF_MASK)
	{
		regs->TXDATA = adata;
		return true;
	}
	else
	{
		return false;
	}
}

bool THwSpi_msp::TryRecvData(uint16_t * dstptr)
{
	if (regs->STAT & SPI_STAT_RFE_MASK) // FIFO Empty?
	{
		return false;
	}

	*dstptr = regs->RXDATA;
	return true;
}

bool THwSpi_msp::TryRecvData(uint8_t * dstptr)
{
	if (regs->STAT & SPI_STAT_RFE_MASK) // FIFO Empty?
	{
		return false;
	}

	*dstptr = regs->RXDATA;
	return true;
}

bool THwSpi_msp::SendFinished()
{
	if (regs->STAT & SPI_STAT_BUSY_MASK)
	{
		return false;
	}

	return true;
}

void THwSpi_msp::DmaAssign(bool istx, THwDmaChannel * admach)
{
	if (istx)
	{
		txdma = admach;
		admach->Prepare(true, (void *)&regs->TXDATA, 0);
	}
	else
	{
		rxdma = admach;
		admach->Prepare(false, (void *)&regs->RXDATA, 0);
	}
}

bool THwSpi_msp::DmaStartSend(THwDmaTransfer * axfer)
{
	if (!txdma)
	{
		return false;
	}

	txdma->StartTransfer(axfer);
	return true;
}

bool THwSpi_msp::DmaStartRecv(THwDmaTransfer * axfer)
{
	if (!rxdma)
	{
		return false;
	}

	rxdma->StartTransfer(axfer);
	return true;
}

bool THwSpi_msp::DmaSendCompleted()
{
	if (txdma && txdma->Active())
	{
		return false;  // Send DMA is still active
	}
	return SendFinished();
}

bool THwSpi_msp::DmaRecvCompleted()
{
	if (rxdma && rxdma->Active())
	{
		return false;  // Send DMA is still active
	}
	return true;
}
