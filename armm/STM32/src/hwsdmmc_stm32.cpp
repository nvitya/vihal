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
 *  file:     hwsdmmc_stm32.h
 *  brief:    STM32 Internal SD/SDIO/MMC driver
 *  created:  2022-11-16
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwsdmmc.h"
#include "stm32_utils.h"
#include "clockcnt.h"

#include "traces.h"

#if defined(SDIO) || defined(SDMMC) || defined(SDMMC1)

#ifdef SDIO
  #define SDMMC_CLKCR_CLKEN       SDIO_CLKCR_CLKEN
  #define SDMMC_CLKCR_WIDBUS_Pos  SDIO_CLKCR_WIDBUS_Pos

  #define SDMMC_STA_CMDSENT       SDIO_STA_CMDSENT
  #define SDMMC_STA_CCRCFAIL      SDIO_STA_CCRCFAIL
  #define SDMMC_STA_DCRCFAIL      SDIO_STA_DCRCFAIL
  #define SDMMC_STA_CMDREND       SDIO_STA_CMDREND
  #define SDMMC_STA_CTIMEOUT      SDIO_STA_CTIMEOUT
  #define SDMMC_STA_DTIMEOUT      SDIO_STA_DTIMEOUT
  #define SDMMC_STA_DATAEND       SDIO_STA_DATAEND
  #define SDMMC_STA_RXDAVL        SDIO_STA_RXDAVL

  #define SDMMC_CMD_CPSMEN        SDIO_CMD_CPSMEN
  #define SDMMC_CMD_WAITRESP_Pos  SDIO_CMD_WAITRESP_Pos
  #define SDMMC_CMD_CMDINDEX_Pos  SDIO_CMD_CMDINDEX_Pos
#endif

bool THwSdmmc_stm32::Init()
{
	unsigned code;
	unsigned perid;

#ifdef SDIO
	regs = SDIO;

	#ifdef RCC_APB2ENR_SDIOEN
		RCC->APB2ENR |= RCC_APB2ENR_SDIOEN;
		if (RCC->APB2ENR) { } // some syncing

		// reset
		RCC->APB2RSTR |= RCC_APB2RSTR_SDIORST;
		if (RCC->APB2RSTR) { } // some syncing
		RCC->APB2RSTR &= ~RCC_APB2RSTR_SDIORST;
		if (RCC->APB2RSTR) { } // some syncing
	#endif

#else
	regs = SDMMC1;

	#if defined(RCC_AHB3ENR_SDMMC1EN) // H7

		RCC->AHB3ENR |= RCC_AHB3ENR_SDMMC1EN;
		if (RCC->AHB3ENR) { } // some syncing

		// reset
		RCC->AHB3RSTR |= RCC_AHB3RSTR_SDMMC1RST;
		if (RCC->AHB3RSTR) { } // some syncing
		RCC->AHB3RSTR &= ~RCC_AHB3RSTR_SDMMC1RST;
		if (RCC->AHB3RSTR) { } // some syncing

	#else
		RCC->APB2ENR |= RCC_APB2ENR_SDMMC1EN;
		if (RCC->APB2ENR) { } // some syncing

		// reset
		RCC->APB2RSTR |= RCC_APB2RSTR_SDMMC1RST;
		if (RCC->APB2RSTR) { } // some syncing
		RCC->APB2RSTR &= ~RCC_APB2RSTR_SDMMC1RST;
		if (RCC->APB2RSTR) { } // some syncing
	#endif

#endif

#if defined(SDMMC_IDMA_IDMAEN)
	// use the IDMA
#else
	dma.Init(2, dma_stream, 4); // channel 4

	// the DMA requires special parameters here otherwise it won't work properly,
	// the last word will be missing
  dma.per_flow_controller = 1;
  dma.fifo_mode = 1;
  dma.fifo_thr  = 3;  // 4 word full
  dma.per_burst = 1;  // 4x burst
  dma.mem_burst = 1;  // 4x burst
#endif

	regs->POWER = (0
		| (1  <<  4)  // DIRPOL
		| (0  <<  3)  // VSWITCHEN
		| (0  <<  2)  // VSWITCH
		| (0  <<  0)  // PWRCTRL(2): 3 = power on, 0 = disabled, 2 = power cycle
	);

	if (regs->POWER) { }

#ifdef MCUSF_H7
	regs->CLKCR = (0
		| (0  << 20)  // SELCLKRX(2): 0 = internal
		| (0  << 19)  // BUSSPEED: 0 = 25 MHz, 1 = 50 MHz
		| (0  << 18)  // DDR: 0 = single data rate
		| (0  << 17)  // HWFC_EN: 1 = hardware flow control enable
		| (0  << 16)  // NEGEDGE
		| (0  << 14)  // WIDBUS(2): 0 = 1 bit, 1 = 4 bit, 2 = 8 bit
		| (0  << 12)  // PWRSAV
		| (0  <<  0)  // CLKDIV(10)
	);

	// clear the data fifo
	regs->DCTRL = SDMMC_DCTRL_FIFORST;

#else
	regs->CLKCR = (0
		| (0  << 14)  // HWFC_EN: 1 = hardware flow control enable
		| (0  << 16)  // NEGEDGE
		| (0  << 11)  // WIDBUS(2): 0 = 1 bit, 1 = 4 bit, 2 = 8 bit
		| (0  <<  9)  // PWRSAV
		| (0  <<  8)  // CLKEN
		| (0  <<  0)  // CLKDIV(8)
	);
#endif

	SetSpeed(400000);
	SetBusWidth(1);

	// power on card
	regs->POWER |= 3; // power on

	// wait 74 SD clycles

	delay_us(200);

	return true;
}

void THwSdmmc_stm32::SetSpeed(uint32_t speed)
{
  uint32_t periphclock = 50000000; // SDMMCCK = 50 MHz

	uint32_t tmp = regs->CLKCR;

#ifdef MCUSF_H7

  uint32_t clkdiv = 1;
  while ((periphclock / (1 << (clkdiv - 1))) > speed)
  {
    clkdiv = (clkdiv << 1);
  }
	tmp &= ~(0x3FF);
	tmp |= ((clkdiv - 1) << 0);

#else

  uint32_t clkdiv = 1;
  while ((periphclock / clkdiv) > speed)
  {
    ++clkdiv;
  }

	tmp &= ~0x3FF;
	tmp |= SDMMC_CLKCR_CLKEN;
	if (clkdiv < 2)
	{
	  tmp |= SDMMC_CLKCR_BYPASS;
	}
	else
	{
    tmp |= ((clkdiv - 2) << 0);
	}
#endif

	regs->CLKCR = tmp;
}

void THwSdmmc_stm32::SetBusWidth(uint8_t abuswidth)
{
	uint32_t tmp = regs->CLKCR;
	tmp &= ~(3 << SDMMC_CLKCR_WIDBUS_Pos);

	if (4 == abuswidth)
	{
		tmp |= (1 << SDMMC_CLKCR_WIDBUS_Pos);
	}
	else
	{
		// already zero
	}

	regs->CLKCR = tmp;
}

bool THwSdmmc_stm32::CmdFinished()
{
	uint32_t sr = regs->STA;

	if (sr & (SDMMC_STA_CTIMEOUT | SDMMC_STA_CCRCFAIL | SDMMC_STA_DCRCFAIL | SDMMC_STA_CTIMEOUT | SDMMC_STA_DTIMEOUT))
	{
		if (sr & SDMMC_STA_CCRCFAIL)
		{
			return true;
		}

	  cmderror = true;
		return true;
	}

	if (SDCMD_RES_NO == (curcmdflags & SDCMD_RES_MASK))
	{
		if (sr & SDMMC_STA_CMDSENT)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

  #ifdef MCUSF_H7

    #if 0
      if ((curcmdflags & SDCMD_0ES_MASK) == SDCMD_RES_R1B)
      {
        if (sr & SDMMC_STA_BUSYD0END)
        {
          return true;
        }
      }
      else
      {
        if (sr & SDMMC_STA_CMDREND)
        {
          return true;
        }
      }
    #else
      if (sr & (SDMMC_STA_CMDREND | SDMMC_STA_BUSYD0END))
      {
        return true;
      }
    #endif

  #else
    if (sr & (SDMMC_STA_CMDREND))
    {
      return true;
    }
  #endif

	return false;
}

bool THwSdmmc_stm32::TransferFinished()
{
  if (dma.initialized && dma.Active())
  {
    return false;
  }

  if (0 == (regs->STA & SDMMC_STA_DATAEND))
  {
    return false;
  }

  return true;
}

void THwSdmmc_stm32::CloseTransfer()
{
#ifndef MCUSF_H7
  while (regs->STA & SDMMC_STA_RXDAVL)
  {
    if (regs->FIFO) { }  // read the fifo
  }

  //regs->DCTRL &= ~(1 | (1 << 3)); // disable data, disable DMA
  regs->DCTRL = 0;
#endif

  regs->ICR = 0xFFFFFFFF;
}

void THwSdmmc_stm32::GetCmdResult128(void * adataptr)
{
	uint32_t * dst = (uint32_t *)adataptr;
	*dst++ = regs->RESP4;
	*dst++ = regs->RESP3;
	*dst++ = regs->RESP2;
	*dst++ = regs->RESP1;
}

uint32_t THwSdmmc_stm32::GetCmdResult32()
{
	return regs->RESP1;
}

void THwSdmmc_stm32::SendCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags)  // send command without data transfer
{
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
		|	SDMMC_CMD_CPSMEN
		| (waitresp << SDMMC_CMD_WAITRESP_Pos)
		| (acmd <<  SDMMC_CMD_CMDINDEX_Pos)
	);


	regs->ICR = 0xFFFFFFFF; // clear all flags
	regs->DLEN = 0;

	regs->ARG = cmdarg;
	regs->CMD = cmdr; // start the execution

  curcmdreg = cmdr;
	cmderror = false;
	cmdrunning = true;
	lastcmdtime = CLOCKCNT;
}

void THwSdmmc_stm32::StartDataReadCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags, void * dataptr, uint32_t datalen)
{

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
		| (0  <<  2)  // DTMODE(2): 0 = single block, 3 = multiple blocks
		| (1  <<  1)  // DTDIR: 0 = host to card, 1 = card to host
		| (0  <<  0)  // DTEN: 1 = start data without CPSM transfer command
	);

	// the DMA must be started before DCTRL (DTEN)

	regs->IDMABASE0 = (uint32_t)dataptr;
	regs->IDMABSIZE = datalen; // not really necessary for single buffer mode ?
	regs->IDMACTRL = 1; // enable the internal DMA
#else

  // setup data control register
	regs->DCTRL = 0;

  //uint32_t dctrl = (regs->DCTRL & 0xFFFF0000);  // keep the bits above
  uint32_t dctrl = (0
		| (0  << 11)  // SDIOEN
		| (0  << 10)  // RWMOD
		| (0  <<  9)  // RWSTOP
		| (0  <<  8)  // RWSTART
		| (bsizecode <<  4)  // DBLOCKSIZE(4): 0 = 1 byte, 9 = 512
		| (1  <<  3)  // DMAEN: 1 = enable DMA
		| (0  <<  2)  // DTMODE: 0 = block mode, 1 = stream / multibyte
		| (1  <<  1)  // DTDIR: 0 = host to card, 1 = card to host
		| (1  <<  0)  // DTEN: 1 = data enable
	);

	// start the DMA channel

	dma.Prepare(false, (void *)&regs->FIFO, 0); // setup the DMA for receive

	dmaxfer.flags = 0; // use defaults
	dmaxfer.bytewidth = 4;  // destination must be aligned !!!
	dmaxfer.count = (datalen >> 2);
	dmaxfer.dstaddr = dataptr;
  dma.StartTransfer(&dmaxfer);

#endif

  regs->DTIMER = 0xFFFFFF; // todo: adjust data timeout
	regs->DLEN = datalen;
	regs->DCTRL = dctrl;

	// CMD

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
		|	SDMMC_CMD_CPSMEN
		| (waitresp << SDMMC_CMD_WAITRESP_Pos)
		| (acmd <<  SDMMC_CMD_CMDINDEX_Pos)
	);

#ifdef MCUSF_H7
	cmdr |= SDMMC_CMD_CMDTRANS;
#endif

	regs->ICR = 0xFFFFFFFF; // clear all flags

	regs->ARG = cmdarg;
	regs->CMD = cmdr; // start the execution

	curcmdreg = cmdr;
	cmderror = false;
	cmdrunning = true;
	lastcmdtime = CLOCKCNT;
}

#define SDMMC_STATIC_CMD_FLAGS  ((uint32_t)(SDMMC_STA_CCRCFAIL | SDMMC_STA_CTIMEOUT | SDMMC_STA_CMDREND | SDMMC_STA_CMDSENT))

void THwSdmmc_stm32::StartDataWriteCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags, void * dataptr, uint32_t datalen)
{
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
}

bool THwSdmmc_stm32::CmdResult32Ok()
{
  /* Check response received is of desired command */
  if (regs->RESPCMD != curcmd)
  {
    return false;
  }

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

void THwSdmmc_stm32::StartDataWriteTransmit(void * dataptr, uint32_t datalen)
{
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
}

#endif
