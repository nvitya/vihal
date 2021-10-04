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
// file:     hwspi_vrv100.cpp
// brief:    VexRiscV / SpinalHDL SPI Master for VRV100
// created:  2021-10-04
// authors:  nvitya

#include "platform.h"
#include "hwspi.h"

/*---------------------------------------------------------------------------------------
VexRiscV / SpinalHDL SPI Master register datasheet from the "SpiMasterCtrl.scala" source:
-----------------------------------------------------------------------------------------
In short, it has one command fifo (write/read/chip-select commands) and one read fifo.
data -> 0x00 :
  rxData -> R[7:0]
  rxOccupancy -> R[30:16] rx fifo occupancy (include the rxData in the amount)
  rxValid -> R[31] Inform that the readed rxData is valid
  When you read this register it pop an byte of the rx fifo and provide its value (via rxData)
  When you write this register, it push a command into the fifo. There is the commands that you can use :
    0x000000xx =>  Send byte xx
    0x010000xx =>  Send byte xx and also push the read data into the FIFO
    0x1100000X =>  Enable the SS line X
    0x1000000X =>  Disable the SS line X

status -> 0x04
  cmdIntEnable -> RW[0] Command fifo empty interrupt enable
  rspIntEnable -> RW[1] Read fifo not empty interrupt enable
  cmdInt -> RW[8] Command fifo empty interrupt pending
  rspInt -> RW[9] Read fifo not empty interrupt pending
  txAvailability -> R[31:16] Command fifo space availability (SS commands + send byte commands)

config -> 0x08
  cpol -> W[0]
  cpha -> W[1]
  ssActiveHigh -> W[31..4] For each ss, the corresponding bit specify if that's a active high one.

clockDivider -> W 0x0C SPI frequency = FCLK / (2 * clockDivider)
ssSetup -> W 0x10 time between chip select enable and the next byte
ssHold -> W 0x14 time between the last byte transmission and the chip select disable
ssDisable -> W 0x18 time between chip select disable and chip select enable
-----------------------------------------------------------------------------------------*/


bool THwSpi_vrv100::Init(int adevnum)
{
	devnum = adevnum;
	initialized = false;

	regs = nullptr;

	if (1 == devnum)
	{
		regs = (vexriscv_spim_t *)SPIM1_BASE;
	}
	else if (2 == devnum)
	{
		regs = (vexriscv_spim_t *)SPIM2_BASE;
	}
#if defined(SPIM3_BASE)
	else if (3 == devnum)
	{
		regs = (vexriscv_spim_t *)SPIM3_BASE;
	}
#endif
#if defined(SPIM4_BASE)
	else if (4 == devnum)
	{
		regs = (vexriscv_spim_t *)SPIM4_BASE;
	}
#endif

	if (!regs)
	{
		return false;
	}

	basespeed = SystemCoreClock / 2;
	uint32_t clkdiv = basespeed / speed;
	if (clkdiv * speed < basespeed)  ++clkdiv;
	regs->CLOCK_DIVIDER = clkdiv;

	regs->STATUS = 0; // clear interrupts and pending

	uint32_t cfg = (0
		| (0  <<  0)  // CPOL: 0 = clock low by default, 1 = clock high by default
		| (0  <<  1)  // CPHA: 0 = sampling by clock rising edge, 1 = sampling by clock falling edge
		| (0  <<  4)  // SSACTIVEHIGH(28): bitn 1 = SS is active high, 0 = SS active low;
	);
	if (idleclk_high)     cfg |= (1 << 0);
	if (datasample_late)  cfg |= (1 << 1);
	regs->CONFIG = cfg;

	regs->SSSETUP    =  1;  // time between chip select enable and the next byte
	regs->SSHOLD     =  1;  // time between the last byte transmission and the chip select disable
	regs->SSDISABLE  =  1;  // time between chip select disable and chip select enable

	initialized = true;

	return true;
}

bool THwSpi_vrv100::TrySendData(unsigned short adata)
{
#if 0
	if (regs->SR & SPI_SR_TXP)
	{
		*(__IO uint8_t *)(&(regs->TXDR)) = (adata & 0xff);
		return true;
	}
	else
	{
		return false;
	}
#endif
	return false;
}

bool THwSpi_vrv100::TryRecvData(unsigned short * dstptr)
{
#if 0
	if (regs->SR & SPI_SR_RXP)
	{
		*dstptr = *(__IO uint8_t *)(&regs->RXDR);
		return true;
	}
	else
	{
		return false;
	}
#endif
	return false;
}

bool THwSpi_vrv100::SendFinished()
{
	if (SPIM_TXFIFO_DEPTH == (regs->STATUS >> 16))
	{
		return true;
	}
	else
	{
		return false;
	}
}

