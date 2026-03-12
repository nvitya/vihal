
#include "platform.h"
//#include "hwclkctrl.h"
#include "hwspi.h"
//#include "lpc_utils.h"

bool THwSpi_lpc::Init(int adevnum)
{
	unsigned tmp;

	devnum = adevnum;

	initialized = false;

	regs = 0;
	if (false)
	{

	}
	else if (0 == devnum)
	{
		regs = (HW_SPI_REGS *)LPC_SSP0;
		//LPC_SC->PCONP |= PCONP_PCSSP0_ENABLED;  // because errata it must be set up before
	}
	else if (1 == devnum)
	{
		regs = (HW_SPI_REGS *)LPC_SSP1;
		//LPC_SC->PCONP |= PCONP_PCSSP1_ENABLED;
	}

	if (!regs)
	{
		return false;
	}

	regs->CR1 &= ~(1 << 1); // disable the controller

	// Disable DMA for now
	regs->DMACR = 0;

	basespeed = SystemCoreClock;

	unsigned prescaler = 2;  // fix
  regs->CPSR = prescaler;

	// CR0

	unsigned scr = (basespeed / prescaler) / speed;
	if (scr < 1)  scr = 1;
	if (basespeed / (prescaler * scr) > speed)  ++scr;

	tmp = 0
		| ((databits - 1) << 0)  // DSS(4)
		| (0 << 4)  // FRF(2): 0 = SPI
		| (0 << 6)  // CPOL: 0 = low on idle
		| (0 << 7)  // CPHA: 0 = early sample
		| ((scr - 1) << 8)  // SCR(8): Serial Clock Rate
	;

	if (idleclk_high)  tmp |= (1 << 6);
	if (datasample_late)  tmp |= (1 << 7);

	regs->CR0 = tmp;

	// CR1
	tmp = 0
		| (0 << 0)  // LBM: 0 = loop back mode disabled
		| (1 << 1)  // SE: 1 = SSP Enable
		| (0 << 2)  // MS: 0 = Master Mode
		| (0 << 3)  // SOD (not relevant in master mode)
	;
	regs->CR1 = tmp; // unit enabled as well

	initialized = true;

	return true;
}

bool THwSpi_lpc::TrySendData(uint8_t adata)
{
	if (regs->SR & SSP_SR_TNF_MASK)
	{
		regs->DR = adata;
		return true;
	}
	else
	{
		return false;
	}
}

bool THwSpi_lpc::TryRecvData(uint8_t * dstptr)
{
	if (regs->SR & SSP_SR_RNE_MASK)
	{
		*dstptr = regs->DR;
		return true;
	}
	else
	{
		return false;
	}
}

void THwSpi_lpc::DmaAssign(bool istx, THwDmaChannel * admach)
{
	if (istx)
	{
		txdma = admach;
		regs->DMACR |= (1 << 1); // enable the TX DMA
	}
	else
	{
		rxdma = admach;
		regs->DMACR |= (1 << 0); // enable the RX DMA
	}

	admach->Prepare(istx, (void *)&regs->DR, 0);
}

bool THwSpi_lpc::DmaStartSend(THwDmaTransfer * axfer)
{
	if (!txdma)
	{
		return false;
	}

	txdma->StartTransfer(axfer);

	return true;
}

bool THwSpi_lpc::DmaStartRecv(THwDmaTransfer * axfer)
{
	if (!rxdma)
	{
		return false;
	}

	rxdma->StartTransfer(axfer);

	return true;
}

bool THwSpi_lpc::SendFinished()
{
	if (regs->SR & SSP_SR_BSY_MASK)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool THwSpi_lpc::DmaSendCompleted()
{
	if (txdma && txdma->Enabled())
	{
		// Send DMA is still active
		return false;
	}

	return SendFinished();
}

bool THwSpi_lpc::DmaRecvCompleted()
{
	if (rxdma && rxdma->Enabled())
	{
		// Send DMA is still active
		return false;
	}

	return true;
}

void THwSpi_lpc::SetCs(unsigned value)
{
  if (manualcspin)
  {
    manualcspin->SetTo(value);
  }
  else
  {
    //#warning "implement SPI CS control"
  }
}
