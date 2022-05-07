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
 *  file:     hwdma_rp.coo
 *  brief:    RP2040  DMA
 *  version:  1.00
 *  date:     2022-05-06
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>

#include "hwdma.h"
#include "rp_utils.h"

bool THwDmaChannel_rp::Init(int achnum, int aperid)  // perid = peripheral request id
{
	initialized = false;

  gregs = dma_hw;

  // channel init

	chnum = achnum;
	perid = (aperid & 0x3F);

	if ((chnum < 0) || (chnum >= MAX_DMA_CHANNELS))
	{
		chnum = -1;
		return false;
	}

  rp_reset_control(RESETS_RESET_DMA_BITS, false); // remove reset

	regs = &gregs->ch[chnum];
  irq_regs = (dma_irq_regs_t *)&gregs->inte0;

	chbit = (1 << chnum);

	// no register preparation is required

	initialized = true;

	return true;
}

void THwDmaChannel_rp::Prepare(bool aistx, void * aperiphaddr, unsigned aflags)
{
	istx = aistx;
	periphaddr = aperiphaddr;
}

void THwDmaChannel_rp::Enable()
{
  regs->ctrl_trig |= DMA_CTRL_EN;  // also triggers the transfer, the BUSY bit should go high
}

void THwDmaChannel_rp::Disable()
{
  uint32_t crreg = regs->ctrl_trig;
  crreg &= ~DMA_CTRL_EN;  // this pauses only the channel
  regs->ctrl_trig = crreg;
  if (crreg & DMA_CTRL_BUSY)
  {
    gregs->abort = chbit;
    while (gregs->abort & chbit)
    {
      // wait until the abort completes
    }
  }
}

unsigned THwDmaChannel_rp::Remaining()
{
  // TODO: for the circular buffer support a helping channel required
  //       and on overflow it must be re-activated

	return regs->transfer_count;
}

void THwDmaChannel_rp::PrepareTransfer(THwDmaTransfer * axfer)
{
  Disable();

  uint32_t crreg = (0
    | (0     << 24)  // BUSY: (read only)
    | (0     << 23)  // SNIFF_EN:
    | (0     << 22)  // BSWAP: 1 = byte swap
    | (perid << 15)  // TREQ_SEL(6): Transfer Request Signal
    | (chnum << 11)  // CHAIN_TO(4): Disable the chaining by setting self-number here
    | (0     << 10)  // RING_SEL: not used because of the strict alignment requirement
    | (0     <<  9)  // RING_SIZE(4):
    | (0     <<  5)  // INCR_WRITE:
    | (0     <<  4)  // INCR_READ:
    | (0     <<  2)  // DATA_SIZE(2): 0 = 1 byte, 1 = 2 byte, 2 = 4 byte
    | (0     <<  1)  // HIGH_PRIORIY
    | (0     <<  0)  // EN
  );

	if      (axfer->bytewidth == 4)  crreg |= (2 << 2);
	else if (axfer->bytewidth == 2)  crreg |= (1 << 2);

	if (axfer->flags & DMATR_MEM_TO_MEM)
	{
		if ((axfer->flags & DMATR_NO_SRC_INC) == 0)
		{
			crreg |= (1 << 4);
		}

		if ((axfer->flags & DMATR_NO_DST_INC) == 0)
		{
			crreg |= (1 << 5);
		}

		regs->read_addr  = ((uint32_t)axfer->srcaddr);
		regs->write_addr = ((uint32_t)axfer->dstaddr);
	}
	else
	{
		// PER <-> MEM transfer

		if (istx)
		{
	    if ((axfer->flags & DMATR_NO_SRC_INC) == 0)
	    {
	      crreg |= (1 << 4);
	    }

	    regs->read_addr  = ((uint32_t)axfer->srcaddr);
	    regs->write_addr = (uint32_t)periphaddr;
		}
		else
		{
	    if ((axfer->flags & DMATR_NO_DST_INC) == 0)
	    {
	      crreg |= (1 << 5);
	    }

	    regs->read_addr  = (uint32_t)periphaddr;
	    regs->write_addr = ((uint32_t)axfer->dstaddr);
		}
	}

  // TODO: implement circular buffer support
#if 0

  if (axfer->flags & DMATR_CIRCULAR)
  {
    regs->DESCADDR = (uint32_t)llregs;
    *llregs = *regs;  // copy to the LL
  }
  else
  {
    regs->DESCADDR = 0;
  }

#endif

  gregs->ints0 = chbit; // clear current interrupt bit (both lines)
  gregs->ints1 = chbit;

  if (irq_line)
  {
    irq_regs = (dma_irq_regs_t *)&gregs->inte1;
  }
  else
  {
    irq_regs = (dma_irq_regs_t *)&gregs->inte0;
  }

  if (axfer->flags & DMATR_IRQ)
  {
    irq_regs->inte |= chbit;
  }
  else
  {
    irq_regs->inte &= ~chbit;
  }

	regs->transfer_count = axfer->count;
	regs->ctrl_trig = crreg;  // the EN bit is not set so the transfer does not starts yet
}
