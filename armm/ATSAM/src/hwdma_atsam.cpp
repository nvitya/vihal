/* -----------------------------------------------------------------------------
 * This file is a part of the NVCM project: https://github.com/nvitya/nvcm
 * Copyright (c) 2018 Viktor Nagy, nvitya
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
 *  file:     hwdma_atsam.cpp
 *  brief:    ATSAM DMA
 *  version:  1.00
 *  date:     2018-02-10
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>
#include "atsam_utils.h"

#include "hwdma.h"

void THwDmaChannel_atsam::Prepare(bool aistx, void * aperiphaddr, unsigned aflags)
{
  istx = aistx;
  periphaddr = aperiphaddr;
}

#ifdef XDMAC  // single central DMA (E70)

#define MAX_DMA_CHANNELS  XDMACCHID_NUMBER

bool THwDmaChannel_atsam::Init(int achnum, int aperid)  // perid = peripheral request id
{
  initialized = false;

  atsam_enable_peripheral(ID_XDMAC);  // enable XDMAC

  chnum = achnum;
  perid = aperid;

  if ((chnum < 0) || (chnum >= MAX_DMA_CHANNELS))
  {
    chnum = -1;
    return false;
  }

  chbit = (1 << chnum);
  regs = (HW_DMA_REGS *)&(XDMAC->XDMAC_CHID[chnum]);

  regs->XDMAC_CNDC = 0; // disable next descriptor fetch
  regs->XDMAC_CBC = 0;
  regs->XDMAC_CDS_MSP = 0;
  regs->XDMAC_CSUS = 0;
  regs->XDMAC_CDUS = 0;

  initialized = true;

  return true;
}

void THwDmaChannel_atsam::Disable()
{
  XDMAC->XDMAC_GD = chbit;

  // wait until it is disabled
  while (XDMAC->XDMAC_GS & chbit)
  {
    // wait
  }
}

void THwDmaChannel_atsam::PrepareTransfer(THwDmaTransfer * axfer)
{
  // prepare all bits with 0
  uint32_t ccreg = 0
      | (0  <<  0)  // TYPE: 1 = Peripheral - Memory transfer, 0 = MEM2MEM
      | (0  <<  1)  // MBSIZE(2): burst size
      | (0  <<  4)  // DSYNC
      | (0  <<  6)  // SWREQ
      | (0  <<  7)  // MEMSET, 1 = zero memory
      | (0  <<  8)  // CSIZE(3): chunk size
      | (0  << 11)  // DWIDTH(2): data width, 0 = 1 byte by default
      | (0  << 13)  // SIF: 1 = APB/RAM, 0 = CCM/RAM
      | (0  << 14)  // DIF: 1 = APB/RAM, 0 = CCM/RAM
      | (0  << 16)  // SAM(2)
      | (0  << 18)  // DAM(2)
      | (perid << 24)
  ;

  if      (axfer->bytewidth == 4)  ccreg |= (2 << 11);
  else if (axfer->bytewidth == 2)  ccreg |= (1 << 11);

  if (axfer->flags & DMATR_MEM_TO_MEM)
  {
    ccreg |= (0
      | (0 <<  0)   // TYPE: 0 = Memory to Memory transfer
      | (0 <<  4)   // DSYNC = 0
      | (1 <<  6)   // SWREQ = 1 to start without HW signal
    );

    if ((axfer->flags & DMATR_NO_SRC_INC) == 0)
    {
      ccreg |= (1 << 16); // SAM = 1
    }
    if ((axfer->flags & DMATR_NO_DST_INC) == 0)
    {
      ccreg |= (1 << 18); // DAM = 1
    }

    // auto detect interface based on the address
    unsigned addr;
    addr = (uint32_t)axfer->srcaddr;
    regs->XDMAC_CSA = addr;
    if (((addr >> 20) != 0x200) && ((addr >> 20) != 0x000))  ccreg |= (1 << 13); // SIF = 1
    addr = (uint32_t)axfer->dstaddr;
    regs->XDMAC_CDA = addr;
    if (((addr >> 20) != 0x200) && ((addr >> 20) != 0x000))  ccreg |= (1 << 14); // DIF = 1
  }
  else
  {
    // PER <-> MEM transfer

    if (istx)
    {
      ccreg |= (0
        | (1  <<  0)  // TYPE: 1 = Peripheral - Memory transfer
        | (1 << 4)    // DSYNC = 1;
        | (1 << 14)   // DIF = 1, IF1 required for peripheral bus access
        | (0 << 13)   // SIF = 0
      );

      if ((axfer->flags & DMATR_NO_SRC_INC) == 0)
      {
        ccreg |= (3 << 16); // SAM == 3
      }
      regs->XDMAC_CSA = (uint32_t)axfer->srcaddr;
      regs->XDMAC_CDA = (uint32_t)periphaddr;
    }
    else
    {
      ccreg |= (0
        | (1 <<  0)  // TYPE: 1 = Peripheral - Memory transfer
        | (0 <<  4)  // DSYNC = 0;
        | (0 << 14)  // DIF = 0
        | (1 << 13)  // SIF = 1, IF1 required for peripheral bus access
      );

      if ((axfer->flags & DMATR_NO_DST_INC) == 0)
      {
        ccreg |= (3 << 18); // DAM == 3
      }
      regs->XDMAC_CSA = (uint32_t)periphaddr;
      regs->XDMAC_CDA = (uint32_t)axfer->dstaddr;
    }
  }

  //regs->XDMAC_CBC = 0; // already set to 0 at the Init()

  orig_count = axfer->count;
  regs->XDMAC_CUBC = axfer->count;
  regs->XDMAC_CC = ccreg;
}

#endif

#if defined(HW_PDMA_REGS)

bool THwDmaChannel_atsam::InitPeriphDma(bool aistx, void * aregs, void * aaltregs)  // special function for Atmel PDMA
{
	initialized = false;

	if (aregs)
	{
		pdmaregs = (HW_PDMA_REGS *)(unsigned(aregs) + 0x100);
	}
	else if (aaltregs)
	{
		pdmaregs = (HW_PDMA_REGS *)(unsigned(aaltregs) + 0x100);
	}
	else
	{
		return false;
	}

	istx = aistx;

	initialized = true;

	return true;
}

void THwDmaChannel_atsam::PerDmaDisable()
{
	if (istx)
	{
		pdmaregs->PTCR = 0x200;
	}
	else
	{
		pdmaregs->PTCR = 0x002;
	}
}

void THwDmaChannel_atsam::PerDmaEnable()
{
	// start the channel
	if (istx)
	{
		pdmaregs->PTCR = 0x100;
	}
	else
	{
		pdmaregs->PTCR = 0x001;
	}
}

bool THwDmaChannel_atsam::PerDmaEnabled()
{
	if (istx)
	{
		return (pdmaregs->TCR > 0);
		//return ((altregs->PTSR & 0x100) != 0);
	}
	else
	{
		return (pdmaregs->RCR > 0);
		//return ((altregs->PTSR & 0x001) != 0);
	}
}

void THwDmaChannel_atsam::PerDmaPrepareTransfer(THwDmaTransfer * axfer)
{
	if (istx)
	{
		pdmaregs->TPR = (uint32_t)axfer->srcaddr;
		pdmaregs->TCR = axfer->count;
		pdmaregs->TNCR = 0;
		pdmaregs->TNPR = 0;

    // This DMA does not support DMATR_NO_SRC_INC (no address increment)
    // but if DMATR_NO_DST_INC is given then random bytes beyond the axfer->srcaddr buffer will be sent
		// in case of SPI flash this is not a problem
	}
	else
	{
		pdmaregs->RPR = (uint32_t)axfer->dstaddr;
		if (axfer->flags & DMATR_NO_DST_INC)
		{
	    // This DMA does not support DMATR_NO_DST_INC (no address increment)
	    // which might lead to write beyond the allocated buffer, so in this case the RX DMA
		  // will be deactivated.
		  rxvoid = true; // this is a flag to the peripheral that unread bytes must be flushed
      pdmaregs->RCR = 0;
		}
		else
		{
		  pdmaregs->RCR = axfer->count;
		}
		pdmaregs->RNCR = 0;
		pdmaregs->RNPR = 0;
	}
}

unsigned THwDmaChannel_atsam::PerDmaRemaining()
{
  if (istx)
  {
    return pdmaregs->TCR;
  }
  else
  {
    return pdmaregs->RCR;
  }
}

#endif


#ifdef DMAC

#define MAX_DMA_CHANNELS  DMACCH_NUM_NUMBER

bool THwDmaChannel_atsam::Init(int achnum, int aperid)  // perid = peripheral request id
{
  initialized = false;

  atsam_enable_peripheral(ID_DMAC);  // enable DMAC

  DMAC->DMAC_EN = 1; // enable the DMA Controller

  chnum = achnum;
  perid = aperid;

  if ((chnum < 0) || (chnum >= MAX_DMA_CHANNELS))
  {
    chnum = -1;
    return false;
  }

  chbit = (1 << chnum);
  regs = (HW_DMA_REGS *)&(DMAC->DMAC_CH_NUM[chnum]);

  regs->DMAC_DSCR = 0; // disable next descriptor fetch

  initialized = true;

  return true;
}

void THwDmaChannel_atsam::DmacPrepareTransfer(THwDmaTransfer * axfer)
{
  Disable();

  if (DMAC->DMAC_EBCISR)  // read the status register to clear the flag
  {
    //
  }

  uint32_t twidth;
  if      (axfer->bytewidth == 4)  twidth = 2;
  else if (axfer->bytewidth == 2)  twidth = 1;
  else                             twidth = 0;

  uint32_t cfg = (0
    | (0  <<  0)  // SRC_PER(4): peripheral id
    | (0  <<  4)  // DST_PER(4): peripheral id
    | (0  <<  9)  // SRC_H2SEL: 1 = HW handshaking
    | (0  << 13)  // DST_H2SEL: 1 = HW handshaking
    | (1  << 16)  // SOD
    | (0  << 20)  // LOCK_IF
    | (0  << 21)  // LOCK_B
    | (0  << 22)  // LOCK_IF_L
    | (0  << 24)  // AHB_PROT(3): ???
    | (2  << 28)  // FIFOCFG(2): 2 = ASAP
  );

  uint32_t ctrla = (0
    | (0  <<  0)  // BTSIZE(16)
    | (0  << 16)  // SRCSIZE(3)
    | (0  << 20)  // DSTSIZE(3)
    | (twidth << 24)  // SRC_WIDTH(2)
    | (twidth << 28)  // DST_WIDTH(2)
    | (0  << 31)  // DONE
  );

  uint32_t ctrlb = (0
    | (0  << 16)  // SRC_DSCR
    | (0  << 20)  // DST_DSCR
    | (0  << 21)  // FC(3): 0 = mem->mem, 1 = mem->per, 2 = per->mem
    | (0  << 24)  // SRC_INCR(2): 0 = increment, 1 = decrement, 2 = fixed
    | (0  << 28)  // DST_INCR(2): 0 = increment, 1 = decrement, 2 = fixed
    | (0  << 30)  // IEN: 1 = enable interrupt
  );

  if (axfer->flags & DMATR_MEM_TO_MEM)
  {
    if ((axfer->flags & DMATR_NO_SRC_INC) == 0)
    {
      ctrlb |= (2 << 24);  // fixed SRC address
    }
    if ((axfer->flags & DMATR_NO_DST_INC) == 0)
    {
      ctrlb |= (2 << 28);  // fixed DST address
    }

    regs->DMAC_SADDR = (uint32_t)axfer->srcaddr;
    regs->DMAC_DADDR = (uint32_t)axfer->dstaddr;
  }
  else // peripheral transfer
  {
    if (istx)  // DMA TX = MEM -> PER
    {
      cfg |= (0
        | (perid <<  4)  // DST_PER(4)
        | (1  << 13)     // DST_H2SEL
      );
      ctrlb |= (0
        | (1 << 21)      // FC(3): 0 = mem->mem, 1 = mem->per, 2 = per->mem
        | (2 << 28)      // fixed DST address
      );
      if (axfer->flags & DMATR_NO_SRC_INC)
      {
        ctrlb |= (2 << 24);  // SRC_INCR(2): 2 = fixed SRC
      }

      regs->DMAC_SADDR = (uint32_t)axfer->srcaddr;
      regs->DMAC_DADDR = (uint32_t)periphaddr;
    }
    else  // DMA RX = PER -> MEM
    {
      cfg |= (0
        | (perid <<  0)  // SRC_PER(4): peripheral id
        | (1  <<  9)     // SRC_H2SEL: 1 = HW handshaking
      );
      ctrlb |= (0
        | (2 << 21)      // FC(3): 0 = mem->mem, 1 = mem->per, 2 = per->mem
        | (2 << 24)      // SRC_INCR(2): 2 = fixed SRC
      );
      if (axfer->flags & DMATR_NO_DST_INC)
      {
        ctrlb |= (2 << 28);  // fixed DST address
      }
      regs->DMAC_SADDR = (uint32_t)periphaddr;
      regs->DMAC_DADDR = (uint32_t)axfer->dstaddr;
    }
  }

  if (axfer->flags & DMATR_IRQ)
  {
    ctrlb = (1 << 30);
  }

  orig_count = (axfer->count & 0xFFFF);
  ctrla |= orig_count;  // BTSIZE(16)

  regs->DMAC_DSCR = 0;
  regs->DMAC_CFG = cfg;
  regs->DMAC_CTRLB = ctrlb;
  regs->DMAC_CTRLA = ctrla;
}

void THwDmaChannel_atsam::DmacEnable()
{
  DMAC->DMAC_CHER = chbit;
}

bool THwDmaChannel_atsam::DmacEnabled()
{
  return ((DMAC->DMAC_CHSR & chbit) != 0);
}

void THwDmaChannel_atsam::DmacDisable()
{
  DMAC->DMAC_CHDR = chbit;
  while (DMAC->DMAC_CHSR & chbit)
  {
    // wait until it is disabled
  }
}

unsigned THwDmaChannel_atsam::DmacRemaining()
{
  return (orig_count - (regs->DMAC_CTRLA & 0xFFFF));
}

#endif

#if defined(HW_PDMA_REGS) && !defined(DMAC) // peripheral DMA only

void THwDmaChannel_atsam::PrepareTransfer(THwDmaTransfer * axfer)
{
  PerDmaPrepareTransfer(axfer);
}

void THwDmaChannel_atsam::Enable()
{
  PerDmaEnable();
}

bool THwDmaChannel_atsam::Enabled()
{
  return PerDmaEnabled();
}

void THwDmaChannel_atsam::Disable()
{
  PerDmaDisable();
}

unsigned THwDmaChannel_atsam::Remaining()
{
  return PerDmaRemaining();
}

#elif defined(HW_PDMA_REGS) && defined(DMAC) // peripheral DMA and DMAC

void THwDmaChannel_atsam::PrepareTransfer(THwDmaTransfer * axfer)
{
  if (pdmaregs)
  {
    PerDmaPrepareTransfer(axfer);
  }
  else
  {
    DmacPrepareTransfer(axfer);
  }
}

void THwDmaChannel_atsam::Enable()
{
  if (pdmaregs)
  {
    PerDmaEnable();
  }
  else
  {
    DmacEnable();
  }
}

bool THwDmaChannel_atsam::Enabled()
{
  if (pdmaregs)
  {
    return PerDmaEnabled();
  }
  else
  {
    return DmacEnabled();
  }
}

void THwDmaChannel_atsam::Disable()
{
  if (pdmaregs)
  {
    PerDmaDisable();
  }
  else
  {
    DmacDisable();
  }
}

unsigned THwDmaChannel_atsam::Remaining()
{
  if (pdmaregs)
  {
    return PerDmaRemaining();
  }
  else
  {
    return DmacRemaining();
  }
}

#endif
