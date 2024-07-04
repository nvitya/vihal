/*
 * hwspi_am.cpp
 *
 *  Created on: Jul 3, 2024
 *      Author: vitya
 */

#include "hwspi_am.h"
#include "hw_utils.h"
#include "am_utils.h"

bool THwSpi_am::Init(int adevnum)
{
	initialized = false;
	mregs = nullptr;
	if (0 == adevnum)
	{
		mregs = (THwSpiRegs *)hw_memmap(HW_MCSPI0_BASE, sizeof(THwSpiRegs));
		am_enable_per(CM_PER_REG_OFFS_MCSPI0, true);
	}
	else if (1 == adevnum)
	{
		mregs = (THwSpiRegs *)hw_memmap(HW_MCSPI1_BASE, sizeof(THwSpiRegs));
		am_enable_per(CM_PER_REG_OFFS_MCSPI1, true);
	}
	else
	{
		mregs = nullptr;
	}

	if (!mregs)
	{
		return false;
	}

	devnum = adevnum;
	regs = &mregs->CH[0];

	// reset module
	mregs->SYSCONFIG |= (1 << 1);
	while (0 == (mregs->SYSSTATUS & 1))
	{
		// wait until the reset is finished
	}

	mregs->SYSCONFIG = (0
	  | (3  <<  8)  // CLOCKACTIVITY(2): 3 = do not stop module clocks
	  | (0  <<  3)  // SIDLEMODE(2): 0 = accept idle requests
	  | (0  <<  1)  // SOFTRESET: 1 = reset (auto-clear)
	  | (0  <<  0)  // AUTOIDLE: 0 = OCP clock is free running
	);

	mregs->IRQENABLE = 0; // disable all IRQs

	mregs->SYST = (0
    | (0  << 11)  // SSB
    | (0  << 10)  // SPIENDIR
    | (1  <<  9)  // SPIDATDIR1
    | (0  <<  8)  // SPIDATDIR0
    | (0  <<  6)  // SPICLK
    | (0  <<  5)  // SPIDAT_1
    | (0  <<  4)  // SPIDAT_0
    | (0  <<  3)  // SPIEN_3
    | (0  <<  2)  // SPIEN_2
    | (0  <<  1)  // SPIEN_1
    | (0  <<  0)  // SPIEN_0
	);

	mregs->MODULCTRL = (0
    | (0  <<  8)  // FDDA: 0 = fifo data managed by TX and RX
    | (0  <<  7)  // MOA: 0 = disable multiple word access
    | (0  <<  4)  // INITDLY(3): 0 = no delay
    | (0  <<  3)  // SYSTEM_TEST: 1 = system test mode
    | (0  <<  2)  // MS: 0 = master mode
    | (1  <<  1)  // PIN34: 1 = SPIEN is not used, no inter-byte gaps
    | (0  <<  0)  // SINGLE: 1 = only one channel will be used in master mode
	);

	regs->CTRL = 0; // disable the channel

  uint32_t conf = (0
    | (1  << 29)  // CLKG: 0 = 4-bit power of two clock division, 1 = 4 + 8 bit clock divisor
    | (1  << 28)  // FFER: 1 = buffer is used to receive data
    | (1  << 27)  // FFEW: 1 = buffer is used to transmit data
    | (0  << 25)  // TCS(2): chip select time control
    | (0  << 24)  // SBPOL: Start bit polarity
    | (0  << 23)  // SBE: start bit enable
    | (0  << 21)  // SPIENSLV(2): spi slave SPIEN selection
    | (0  << 20)  // FORCE
    | (1  << 19)  // TURBO: 1 = enable turbo mode (smaller inter-word gaps)
    | (1  << 18)  // IS: input select, 0 = D0 used for RX, 1 = D1 used for RX
    | (1  << 17)  // DPE1: 0 = enable TX for D1, 1 = disable TX for D1
    | (0  << 16)  // DPE0: 0 = enable TX for D0, 1 = disable TX for D0
    | (0  << 15)  // DMAR: 1 = enable RX DMA
    | (0  << 14)  // DMAW: 1 = enable TX DMA
    | (0  << 12)  // TRM(2): 0 = TX+RX, 1 = RX only, 2 = TX only
    | ((databits - 1) <<  7)  // WL(5): SPI word length, 7 = 8-bit
    | (1  <<  6)  // EPOL: 1 = SPIEN is low active
    | (0  <<  2)  // CLKD(4): clock divisor, 0 = do not divide
    | (0  <<  1)  // POL: SPI clock polarity
    | (0  <<  0)  // PHA: SPI clock phase
  );
  if (idleclk_high)     conf |= (1 << 1);
  if (datasample_late)  conf |= (1 << 0);
  if (tx_only)          conf |= (2 << 12);
  regs->CONF = conf;

  SetSpeed(speed);

  regs->CTRL |= 1; // enable the channel

	initialized = true;
	return true;
}

void THwSpi_am::SetSpeed(uint32_t aspeed)
{
	uint32_t tmp;

	speed = aspeed;
	uint32_t baseclock = 48000000;

	uint32_t clkdiv = baseclock / speed;
	if (clkdiv * speed < baseclock) ++clkdiv;

	if (clkdiv < 1) clkdiv = 1;
	--clkdiv;

  tmp = regs->CONF;
  tmp &= ~(0xF << 2);
  tmp |= ((clkdiv & 0xF) << 2);
  regs->CONF = tmp;

  tmp = regs->CTRL;
  tmp &= ~(0xFF << 8);
  tmp |= ((clkdiv & 0xFF0) << 4);
  regs->CTRL = tmp;
}

bool THwSpi_am::TrySendData(uint8_t adata)
{
  if (regs->STAT & (1 << 4))  // TXFFF: 1 = transmit FIFO full
  {
  	return false;
  }
  else
  {
    regs->TX = adata;
    return true;
  }
}

bool THwSpi_am::TryRecvData(uint8_t * dstptr)
{
  if (regs->STAT & (1 << 5)) // RXFFE: 1 = RX FIFO empty
  {
  	return false;
  }
  else
  {
    *dstptr = regs->RX;
    return true;
  }
}

bool THwSpi_am::SendFinished()
{
	// bit1: TXS: 1 = transmit register is empty
	// bit3: TXFFE: 1 = transmit fifo empty
  if ((regs->STAT & ((1 << 1) | (1 << 3) | (1 << 2))) == ((1 << 1) | (1 << 3) | (1 << 2)))
  {
    return true;
  }
  else
  {
    return false;
  }
}

void THwSpi_am::SetCs(unsigned value)
{
  if (manualcspin)
  {
    manualcspin->SetTo(value);
  }
  else
  {
  	// todo: implement
  }
}

void THwSpi_am::DmaAssign(bool istx, THwDmaChannel * admach)
{
	if (istx)
	{
		txdma = admach;
		admach->Prepare(istx, (void *)&regs->TX, 0);
	}
	else
	{
		rxdma = admach;
		admach->Prepare(istx, (void *)&regs->RX, 0);
	}
}

bool THwSpi_am::DmaStartSend(THwDmaTransfer * axfer)
{
	if (!txdma)
	{
		return false;
	}

  regs->CONF |= (1 << 14); // DMAW: 1 = enable TX DMA

	txdma->StartTransfer(axfer);
	return true;
}

bool THwSpi_am::DmaStartRecv(THwDmaTransfer * axfer)
{
	if (!rxdma)
	{
		return false;
	}

  regs->CONF |= (1 << 15); // DMAR: 1 = enable RX DMA

	rxdma->StartTransfer(axfer);
	return true;
}

bool THwSpi_am::DmaSendCompleted()
{
	if (txdma && txdma->Active())
	{
		// Send DMA is still active
		return false;
	}

	return SendFinished();
}

bool THwSpi_am::DmaRecvCompleted()
{
	if (rxdma && rxdma->Active())
	{
		// Send DMA is still active
		return false;
	}

	return true;
}

