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

uint32_t                   rp_dma_used_channels = 0;

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

	rp_dma_used_channels |= chbit;

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
  regs->al1_ctrl = crreg;
  if (crreg & DMA_CTRL_BUSY)
  {
    gregs->abort = chbit;
    while (gregs->abort & chbit)
    {
      // wait until the abort completes
    }
  }

  if (helper_regs)
  {
    helper_regs->al1_ctrl &= ~DMA_CTRL_EN;  // this pauses only the channel
    gregs->abort = (1 << helper_chnum);
  }
}

unsigned THwDmaChannel_rp::Remaining()
{
  if (helper_regs)
  {
    if (helper_regs->read_addr != uint32_t(&circ_data.data[0]))
    {
      // the helper channel has been run trough
      // reset to the beginning
      UpdateHelperChannel();
    }
  }

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
    | (0     << 11)  // CHAIN_TO(4): Disable the chaining by setting self-number here
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

  regs->transfer_count = axfer->count;

  // Circular buffer support using helper channel
  if (axfer->flags & DMATR_CIRCULAR)
  {
    // allocate (or re-use the already allocated) helper DMA channel
    if (AllocateHelper())
    {
      // helper channel allocate was successful, go on with circuar setup
      if (istx)
      {
        // update TRANSFER COUNT and READ ADDR (alias 3)
        circ_data.data[0] = axfer->count;
        circ_data.data[1] = ((uint32_t)axfer->srcaddr);
        circ_data.target_addr = (uint32_t *)&(regs->al3_transfer_count);
      }
      else
      {
        // update WRIE ADDR and TRANSFER COUNT (alias 1)
        circ_data.data[0] = ((uint32_t)axfer->dstaddr);
        circ_data.data[1] = axfer->count;
        circ_data.target_addr = (uint32_t *)&(regs->al1_write_addr);
      }

      UpdateHelperChannel();

      crreg |= (helper_chnum << 11);  // CHAIN_TO(4): trigger the helper channel on completition
    }
    else
    {
      crreg |= (chnum << 11);  // CHAIN_TO(4): Disable the chaining by setting self-number here
    }
  }
  else
  {
    crreg |= (chnum << 11);  // CHAIN_TO(4): Disable the chaining by setting self-number here
  }


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

	regs->al1_ctrl = crreg;  // the EN bit is not set so the transfer does not start yet
}

bool THwDmaChannel_rp::AllocateHelper()
{
  int hch;
  if ((helper_chnum >= 0) && (helper_chnum < MAX_DMA_CHANNELS))
  {
    hch = helper_chnum;
  }
  else
  {
    // search
    hch = MAX_DMA_CHANNELS - 1;
    while ((hch >= 0) && (rp_dma_used_channels & (1 << hch)))
    {
      --hch;
    }
  }

  helper_chnum = hch;

  if (hch >= 0)
  {
    rp_dma_used_channels |= (1 << hch);
    helper_regs = &gregs->ch[hch];
    return true;
  }
  else
  {
    helper_regs = nullptr;
    return false;
  }
}

void THwDmaChannel_rp::UpdateHelperChannel()
{
  helper_regs->read_addr      = uint32_t(&circ_data.data[0]);
  helper_regs->write_addr     = uint32_t(circ_data.target_addr);
  helper_regs->transfer_count = 2;
  helper_regs->al1_ctrl = (0
    | (0     << 24)  // BUSY: (read only)
    | (0     << 23)  // SNIFF_EN:
    | (0     << 22)  // BSWAP: 1 = byte swap
    | (0x3F  << 15)  // TREQ_SEL(6): Transfer Request Signal, 0x3F = permanent request
    | (helper_chnum << 11)  // CHAIN_TO(4): Disable the chaining by setting self-number here
    | (0     << 10)  // RING_SEL: not used because of the strict alignment requirement
    | (0     <<  9)  // RING_SIZE(4):
    | (1     <<  5)  // INCR_WRITE:
    | (1     <<  4)  // INCR_READ:
    | (2     <<  2)  // DATA_SIZE(2): 0 = 1 byte, 1 = 2 byte, 2 = 4 byte
    | (0     <<  1)  // HIGH_PRIORIY
    | (1     <<  0)  // EN
  );
}
