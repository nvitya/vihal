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
 *  file:     hwsdmmc_atsam.cpp
 *  brief:    ATSAM SDCARD driver
 *  created:  2018-06-07
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwsdmmc.h"
#include "atsam_utils.h"
#include "clockcnt.h"

#include "traces.h"

#if defined(HSMCI)

bool THwSdmmc_atsam::Init()
{
	unsigned code;
	unsigned perid;
	unsigned periphclock = atsam_peripheral_clock();

	regs = HSMCI;
	perid = ID_HSMCI;

	atsam_enable_peripheral(perid);

  // DMA channel is an user setting
  dma.Init(dma_channel, 0); // 0 = HSMCI DMA Peripheral Id (Transmit and Receive)

	// Set the Data Timeout Register to 2 Mega Cycles
	regs->HSMCI_DTOR = HSMCI_DTOR_DTOMUL_1048576 | HSMCI_DTOR_DTOCYC(2);
	// Set Completion Signal Timeout to 2 Mega Cycles
	regs->HSMCI_CSTOR = HSMCI_CSTOR_CSTOMUL_1048576 | HSMCI_CSTOR_CSTOCYC(2);
	// Set Configuration Register
	regs->HSMCI_CFG = HSMCI_CFG_FIFOMODE | HSMCI_CFG_FERRCTRL; // | HSMCI_CFG_LSYNC;

	// Set power saving to maximum value
	regs->HSMCI_MR = HSMCI_MR_PWSDIV_Msk;

  SetSpeed(400000);
  SetBusWidth(1);

	// Enable the HSMCI and the Power Saving
	regs->HSMCI_CR = HSMCI_CR_MCIEN | HSMCI_CR_PWSEN;

#ifdef HSMCI_DMA_DMAEN
	regs->HSMCI_DMA = 0;
#endif

  // wait 74 SD clycles

  delay_us(200);

	return true;
}

void THwSdmmc_atsam::SetSpeed(uint32_t speed)
{
	uint32_t mck = atsam_peripheral_clock();

  if (speed > 25000000)
  {
    HSMCI->HSMCI_CFG |= HSMCI_CFG_HSMODE;
  }
  else
  {
    HSMCI->HSMCI_CFG &= ~HSMCI_CFG_HSMODE;
  }

#if defined(HSMCI_MR_CLKODD)
	uint32_t clkdiv = 0;
	uint32_t clkodd = 0;
	// clock divider, represent (((clkdiv << 1) + clkodd) + 2)
	uint32_t div = 0;

	// Speed = MCK clock / (((clkdiv << 1) + clkodd) + 2)
	if ((speed * 2) < mck)
	{
		div = (mck / speed) - 2;
		if (mck % speed)
		{
			// Ensure that the card speed not be higher than expected.
			div++;
		}
		clkdiv = div >> 1;
		// clkodd is the last significant bit of the clock divider (div).
		clkodd = div % 2;
	}
	else
	{
		clkdiv = 0;
		clkodd = 0;
	}

	regs->HSMCI_MR &= ~HSMCI_MR_CLKDIV_Msk;
	regs->HSMCI_MR |= HSMCI_MR_CLKDIV(clkdiv);
	if (clkodd)
	{
		regs->HSMCI_MR |= HSMCI_MR_CLKODD;
	}
	else
	{
		regs->HSMCI_MR &= ~HSMCI_MR_CLKODD;
	}
#else
	uint32_t clkdiv = 0;
	uint32_t rest = 0;

	// Speed = MCK clock / (2 * (CLKDIV + 1))
	if ((speed * 2) < mck)
	{
		clkdiv = mck / (2 * speed);
		rest = mck % (2 * speed);
		if (rest > 0)
		{
			// Ensure that the card speed not be higher than expected.
			clkdiv++;
		}
		if (clkdiv > 0)
		{
			clkdiv -= 1;
		}
	}
	else
	{
		clkdiv = 0;
	}
	regs->HSMCI_MR &= ~HSMCI_MR_CLKDIV_Msk;
	regs->HSMCI_MR |= HSMCI_MR_CLKDIV(clkdiv);
#endif

}

void THwSdmmc_atsam::SetBusWidth(uint8_t abuswidth)
{
	uint32_t hsmci_slot = HSMCI_SDCR_SDCSEL_SLOTA;
	uint32_t hsmci_bus_width;

	switch (abuswidth)
	{
	case 4:
		hsmci_bus_width = HSMCI_SDCR_SDCBUS_4;
		break;

	case 8:
		hsmci_bus_width = HSMCI_SDCR_SDCBUS_8;
		break;

	default:
	case 1:
		hsmci_bus_width = HSMCI_SDCR_SDCBUS_1;
		break;
	}
	HSMCI->HSMCI_SDCR = hsmci_slot | hsmci_bus_width;
}

void THwSdmmc_atsam::SendSpecialCmd(uint32_t aspecialcmd)
{
	uint32_t cmdr = HSMCI_CMDR_RSPTYP_NORESP | HSMCI_CMDR_OPDCMD_OPENDRAIN;

	if (SD_SPECIAL_CMD_INIT == aspecialcmd)
	{
		cmdr |= HSMCI_CMDR_SPCMD_INIT;
	}
	else
	{
		cmderror = true;
		return;
	}

	cmderror = false;

	// Configure command
	regs->HSMCI_MR &= ~(HSMCI_MR_WRPROOF | HSMCI_MR_RDPROOF | HSMCI_MR_FBYTE);
#ifdef HSMCI_DMA_DMAEN
	regs->HSMCI_DMA = 0;
#endif
	// Write argument
	regs->HSMCI_ARGR = 0;
	regs->HSMCI_CMDR = cmdr; // start the execution

	cmdrunning = true;
	lastcmdtime = CLOCKCNT;
}

void THwSdmmc_atsam::SendCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags)
{
	regs->HSMCI_MR &= ~(HSMCI_MR_WRPROOF | HSMCI_MR_RDPROOF | HSMCI_MR_FBYTE);
#ifdef HSMCI_DMA_DMAEN
	regs->HSMCI_DMA = 0;
#endif
	regs->HSMCI_BLKR = 0;

	uint32_t cmdr = (acmd & 0x3F);
	uint8_t restype = (cmdflags & SDCMD_RES_MASK);
	if (restype)
	{
		// response present
		cmdr |= HSMCI_CMDR_MAXLAT; // increase latency for commands with response
		if (restype == SDCMD_RES_48BIT)  cmdr |= HSMCI_CMDR_RSPTYP_48_BIT;
		else if (restype == SDCMD_RES_136BIT)  cmdr |= HSMCI_CMDR_RSPTYP_136_BIT;
		else if (restype == SDCMD_RES_R1B)  cmdr |= HSMCI_CMDR_RSPTYP_R1B;
	}
	if (cmdflags & SDCMD_OPENDRAIN)
	{
		cmdr |= HSMCI_CMDR_OPDCMD_OPENDRAIN;
	}

	cmderror = false;

	regs->HSMCI_ARGR = cmdarg;
	regs->HSMCI_CMDR = cmdr; // start the execution

	lastcmdtime = CLOCKCNT;
	cmdrunning = true;
}

bool THwSdmmc_atsam::CmdFinished()
{
	uint32_t sr = regs->HSMCI_SR;

	if (sr & (HSMCI_SR_CSTOE | HSMCI_SR_RTOE | HSMCI_SR_RENDE	| HSMCI_SR_RDIRE | HSMCI_SR_RINDE))
	{
		cmderror = true;
		return true;
	}

	if (sr & HSMCI_SR_CMDRDY)
	{
		return true;
	}

	return false;
}

bool THwSdmmc_atsam::TransferFinished()
{
  if (dma.initialized && dma.Active())
  {
    return false;
  }

  if (0 == (regs->HSMCI_SR & HSMCI_SR_XFRDONE))
  {
    return false;
  }

  return true;
}

void THwSdmmc_atsam::CloseTransfer()
{
#if 0
#ifndef MCUSF_H7
  while (regs->STA & SDMMC_STA_RXDAVL)
  {
    if (regs->FIFO) { }  // read the fifo
  }

  //regs->DCTRL &= ~(1 | (1 << 3)); // disable data, disable DMA
  regs->DCTRL = 0;
#endif

  regs->ICR = 0xFFFFFFFF;
#endif
}

void THwSdmmc_atsam::GetCmdResult128(void * adataptr)
{
	uint32_t * dst = (uint32_t *)adataptr;
	dst += 3;
	for (int n = 0; n < 4; ++n)
	{
		*dst = regs->HSMCI_RSPR[0];
		--dst;
	}
}

uint32_t THwSdmmc_atsam::GetCmdResult32()
{
	return regs->HSMCI_RSPR[0];
}

bool THwSdmmc_atsam::CmdResult32Ok()
{
  uint32_t cmdres = GetCmdResult32();
  if ((cmdres & SDMMC_OCR_ERRORBITS) != 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}

void THwSdmmc_atsam::StartDataReadCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags, void * dataptr, uint32_t datalen)
{
	// Enabling Read/Write Proof allows to stop the HSMCI Clock during
	// read/write  access if the internal FIFO is full.
	// This will guarantee data integrity, not bandwidth.
	regs->HSMCI_MR |= HSMCI_MR_WRPROOF | HSMCI_MR_RDPROOF;

	if (datalen & 0x3)
	{
		regs->HSMCI_MR |= HSMCI_MR_FBYTE;
	}
	else
	{
		regs->HSMCI_MR &= ~HSMCI_MR_FBYTE;
	}

#ifdef HSMCI_DMA_DMAEN
	regs->HSMCI_DMA = HSMCI_DMA_DMAEN;
#else
	regs->HSMCI_MR |= HSMCI_MR_PDCMODE;
#endif

	uint32_t cmdr = (acmd & 0x3F);
	uint8_t restype = (cmdflags & SDCMD_RES_MASK);
	if (restype)
	{
		// response present
		cmdr |= HSMCI_CMDR_MAXLAT; // increase latency for commands with response
		if (restype == SDCMD_RES_48BIT)  cmdr |= HSMCI_CMDR_RSPTYP_48_BIT;
		else if (restype == SDCMD_RES_136BIT)  cmdr |= HSMCI_CMDR_RSPTYP_136_BIT;
		else if (restype == SDCMD_RES_R1B)  cmdr |= HSMCI_CMDR_RSPTYP_R1B;
	}
	if (cmdflags & SDCMD_OPENDRAIN)
	{
		cmdr |= HSMCI_CMDR_OPDCMD_OPENDRAIN;
	}

	// add data transfer flags
	cmdr |= HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRDIR_READ;
	if (datalen <= 512)
	{
		cmdr |= HSMCI_CMDR_TRTYP_SINGLE;
		regs->HSMCI_BLKR = (datalen << 16) | (1 << 0);
	}
	else
	{
		cmdr |= HSMCI_CMDR_TRTYP_MULTIPLE;
		regs->HSMCI_BLKR = (512 << 16) | (datalen >> 9);  // fix 512 byte blocks
	}

	cmderror = false;

	// start the DMA channel
	dma.Prepare(false, (void *)&regs->HSMCI_FIFO[0], 0);
	dmaxfer.flags = 0; // use defaults
	dmaxfer.bytewidth = 4;  // destination must be aligned !!!
	dmaxfer.count = (datalen >> 2);
	dmaxfer.dstaddr = dataptr;
	dma.StartTransfer(&dmaxfer);

	regs->HSMCI_ARGR = cmdarg;
	regs->HSMCI_CMDR = cmdr; // start the execution

	lastcmdtime = CLOCKCNT;
	cmdrunning = true;
}

void THwSdmmc_atsam::StartDataWriteCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags, void * dataptr, uint32_t datalen)
{
#if 0 // STM32 !!
  // clear data control register
  regs->DCTRL = 0;

  // 1. Issue the command first

  curcmd = acmd;
  curcmdarg = cmdarg;
  curcmdflags = cmdflags;

  uint32_t waitresp = 0;
  uint8_t restype = (cmdflags & SDCMD_RES_MASK);
  if (restype)
  {
    // response present
    if (restype == SDCMD_RES_48BIT)        waitresp = 1; // short response
    else if (restype == SDCMD_RES_136BIT)  waitresp = 3; // long response
    else if (restype == SDCMD_RES_R1B)     waitresp = 1; // short response
  }

  uint32_t cmdr = (0
    | SDMMC_CMD_CPSMEN
    | (waitresp << SDMMC_CMD_WAITRESP_Pos)
    | (acmd <<  SDMMC_CMD_CMDINDEX_Pos)
  );

  regs->ICR = 0xFFFFFFFF; // clear all flags

  regs->ARG = cmdarg;
  regs->CMD = cmdr; // start the execution

  curcmdreg = cmdr;
  cmderror = false;
  cmdrunning = true;
  lastcmdtime = CLOCKCNT;
#endif
}

void THwSdmmc_atsam::StartDataWriteTransmit(void * dataptr, uint32_t datalen)
{
#if 0 // STM32
  regs->ICR = SDMMC_STATIC_CMD_FLAGS;

  // 2. setup data path

  uint32_t bsizecode = 9; // 512 byte
  if (datalen <= 512)
  {
    bsizecode = 31 - __CLZ(datalen);
  }

  #ifdef MCUSF_H7

    // setup data control register
    uint32_t dctrl = (0
      | (1  << 13)  // FIFORST
      | (0  << 12)  // BOOTACKEN
      | (0  << 11)  // SDIOEN
      | (0  << 10)  // RWMOD
      | (0  <<  9)  // RWSTOP
      | (0  <<  8)  // RWSTART
      | (bsizecode <<  4)  // DBLOCKSIZE(4): 0 = 1 byte, 9 = 512
      | (0  <<  2)  // DTMODE(2): 0 = single block, 3 = multiple blocks  ??? only single block works
      | (0  <<  1)  // DTDIR: 0 = host to card, 1 = card to host
      | (1  <<  0)  // DTEN: 1 = start data without CPSM transfer command
    );

    // the DMA must be started before DCTRL (DTEN)

    regs->IDMABASE0 = (uint32_t)dataptr;
    regs->IDMABSIZE = datalen; // not really necessary for single buffer mode ?
    regs->IDMACTRL = 1; // enable the internal DMA
  #else

    regs->DCTRL |= (1 << 3); // enable DMA

    uint32_t dctrl = (regs->DCTRL & 0xFFFF0008);  // keep the bits above
    dctrl |= (0
      | (0  << 11)  // SDIOEN
      | (0  << 10)  // RWMOD
      | (0  <<  9)  // RWSTOP
      | (0  <<  8)  // RWSTART
      | (bsizecode <<  4)  // DBLOCKSIZE(4): 0 = 1 byte, 9 = 512
      | (1  <<  3)  // DMAEN: 1 = enable DMA
      | (0  <<  2)  // DTMODE: 0 = block mode, 1 = stream / multibyte
      | (0  <<  1)  // DTDIR: 0 = host to card, 1 = card to host
      | (1  <<  0)  // DTEN: 1 = data enable
    );

    // start the DMA channel

    dma.Prepare(true, (void *)&regs->FIFO, 0); // setup the DMA for transmit

    dmaxfer.flags = 0; // use defaults
    dmaxfer.bytewidth = 4;  // destination must be aligned !!!
    dmaxfer.count = (datalen >> 2);
    dmaxfer.srcaddr = dataptr;
    dma.StartTransfer(&dmaxfer);

  #endif

  regs->DTIMER = 0xFFFFFF; // todo: adjust data timeout
  regs->DLEN = datalen;
  regs->DCTRL = dctrl;

#ifdef MCUSF_H7
  // this flag was set in the ST hal drivers, but seems to work without it
  //regs->CMD |= SDMMC_CMD_CMDTRANS;
#endif
#endif
}

#if 0
void THwSdmmc_atsam::StartBlockReadCmd()
{
	// Enabling Read/Write Proof allows to stop the HSMCI Clock during
	// read/write  access if the internal FIFO is full.
	// This will guarantee data integrity, not bandwidth.
	regs->HSMCI_MR |= HSMCI_MR_WRPROOF | HSMCI_MR_RDPROOF;
	regs->HSMCI_MR &= ~HSMCI_MR_FBYTE; // fix 512 bytes

#ifdef HSMCI_DMA_DMAEN
	regs->HSMCI_DMA = HSMCI_DMA_DMAEN;
#else
	regs->HSMCI_MR |= HSMCI_MR_PDCMODE;
#endif

	uint32_t cmdr = 0
	  | HSMCI_CMDR_RSPTYP_48_BIT
	  | HSMCI_CMDR_MAXLAT   // increase latency for commands with response
	;

	uint32_t cmdarg = startblock;
	if (!high_capacity)  cmdarg <<= 9; // byte addressing for low capacity cards

	// add data transfer flags
	cmdr |= HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRDIR_READ;

	if (blockcount <= 1)
	{
		cmdr |= 17; // single block read command (a little faster finish)
		cmdr |= HSMCI_CMDR_TRTYP_SINGLE;
		regs->HSMCI_BLKR = (512 << 16) | (1 << 0);
	}
	else
	{
		// always use multiple block read
		cmdr |= 18; // multi block read command
		cmdr |= HSMCI_CMDR_TRTYP_MULTIPLE;
		regs->HSMCI_BLKR = (512 << 16) | (blockcount);  // fix 512 byte blocks, block count not really used
	}

	regs->HSMCI_ARGR = cmdarg;
	regs->HSMCI_CMDR = cmdr; // start the execution

	// start the DMA channel

	dma.Prepare(false, (void *)&regs->HSMCI_FIFO[0], 0); // setup the DMA for receive
	dmaxfer.flags = 0; // use defaults
	dmaxfer.bytewidth = 4;  // destination must be aligned !!!
	dmaxfer.count = (blockcount << 7);  // 512 * n / 4 == 128 * n
	dmaxfer.dstaddr = dataptr;
	dma.StartTransfer(&dmaxfer);

	cmderror = false;
	lastcmdtime = CLOCKCNT;
	cmdrunning = true;
}
#endif

#if 0

void THwSdmmc_atsam::RunTransfer()
{
	if (cmdrunning && !CmdFinished())
	{
		return;
	}

	cmdrunning = false;

	switch (trstate)
	{
	case 0: // idle
		break;

	case 1: // start read blocks
		StartBlockReadCmd();
		trstate = 101;
		break;

	case 101: // wait until the block transfer finishes

		if (cmderror)
		{
			// transfer error
			errorcode = 1;
			completed = true;
			trstate = 0; // transfer finished.
		}
		else
		{
			if (dma.Active())
			{
				return; // wait until DMA finishes.
			}

			if (blockcount > 1)
			{
				// send the stop transmission command
				SendCmd(12, 0, SDCMD_RES_R1B);
				trstate = 106;
			}
			else
			{
				trstate = 106; RunTransfer(); return; // jump directly to the last phase
			}
		}
		break;

	case 106: // wait until transfer done flag set

		if (regs->HSMCI_SR & HSMCI_SR_XFRDONE)
		{
			errorcode = 0;
			completed = true;
			trstate = 0; // transfer finished.
		}
		break;
	}
}

#endif

#endif
