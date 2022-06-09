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
 *  date:     2022-05-06
 *  authors:  nvitya
*/

/* CIRCULAR Buffer Implementation

Requires two channels:
 1. WORK
 2. HELPER

Working flow

Control blocks
  WORKEND:
    - executed by the HELPER
    - Loads the HRESET block into channel WORK and starts it

  HRESET:
    - executed by the WORKER
    - Loads the WRESET block into channel HELPER and starts it
    - this block is followed by the WORKEND block in the memory

  WRESET:
    - executed by the HELPER
    - Resets the worker to the normal transfer pointing to the beginning of the buffer
    - the HELPER will execute the WORKEND when it is triggered again (by the end of work)

All of these control block are full 16 byte / 4 register blocks for Alias 0.

 1. WORK channel:    When the Work (W) finishes, it is chained to the HELPER, which is set up to execute the
                     WORKEND block.
 2. HELPER channel:  Loads the WORKEND block into the WORK channel and starts the WORK channel
 3. WORK channel:    Loads the HRESET block into the HELPER channel and starts the HELPER channel
 4. HELPER channel:  Loads the WRESET block into the WORK channel and starts the WORK channel in normal mode
                     The helper channel is configured to execute the the WORKEND block on the next trigger

NOTE2:
  Somehow this works only with the AL1 aliases only!

*/


#include <stdio.h>
#include <stdarg.h>

#include "hwdma.h"
#include "rp_utils.h"

bool                       rp_dma_resetted = false;
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

	if (!rp_dma_resetted)
	{
	  rp_reset_control(RESETS_RESET_DMA_BITS, true); // issue
	  rp_dma_resetted = true;
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
  // the circular buffer rewind takes more time and
  // the work channel will be used to reset the helper so this situation must be detected here
  if (helper_regs)
  {
    if (helper_regs->read_addr != uint32_t(&circ_data.workend))
    {
      // the circular update is running at this moment
      return 0;
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
    | (0     << 10)  // RING_SEL: 1 = ring applies to write address, 0 = ring applies to read address
    | (0     <<  6)  // RING_SIZE(4):
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

  if (axfer->flags & DMATR_BYTESWAP)
  {
    crreg |= (1 << 22); // the QSPI unit uses this
  }

  // Circular buffer support using helper channel
  if (axfer->flags & DMATR_CIRCULAR)
  {
    if (ConfigureCircular(axfer, crreg))
    {
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

bool THwDmaChannel_rp::ConfigureCircular(THwDmaTransfer * axfer, uint32_t crreg)
{
  // allocate (or re-use the already allocated) helper DMA channel
  if (!AllocateHelper())
  {
    return false;
  }

  // 1. WRESET: the normal worker reset

  if (istx)
  {
    circ_data.wreset.read_addr = ((uint32_t)axfer->srcaddr);
    circ_data.wreset.write_addr = (uint32_t)periphaddr;
  }
  else
  {
    circ_data.wreset.read_addr  = (uint32_t)periphaddr;
    circ_data.wreset.write_addr = ((uint32_t)axfer->dstaddr);
  }
  circ_data.wreset.trans_count = axfer->count;
  circ_data.wreset.ctrl = (crreg | (helper_chnum << 11) | DMA_CTRL_EN);

  // 2. WORKEND, executed by the HELPER, starts the worker channel

  circ_data.workend.read_addr = (uint32_t)(&circ_data.hreset);
  circ_data.workend.write_addr = (uint32_t)(&regs->al1_ctrl);
  circ_data.workend.trans_count = 4;
  circ_data.workend.ctrl = (0
    | (0     << 24)  // BUSY: (read only)
    | (0     << 23)  // SNIFF_EN:
    | (0     << 22)  // BSWAP: 1 = byte swap
    | (0x3F  << 15)  // TREQ_SEL(6): Transfer Request Signal, 0x3F = always
    | (chnum << 11)  // CHAIN_TO(4): chain to WORK
    | (1     << 10)  // RING_SEL: 1 = ring applies to write address, 0 = ring applies to read address
    | (4     <<  6)  // RING_SIZE(4):
    | (1     <<  5)  // INCR_WRITE:
    | (1     <<  4)  // INCR_READ:
    | (2     <<  2)  // DATA_SIZE(2): 0 = 1 byte, 1 = 2 byte, 2 = 4 byte
    | (0     <<  1)  // HIGH_PRIORIY
    | (1     <<  0)  // EN
  );

  // 3. HRESET, executed by the WORK channel, starts the HELPER channel

  circ_data.hreset.read_addr = (uint32_t)(&circ_data.hreset2);
  circ_data.hreset.write_addr = (uint32_t)(&helper_regs->al1_ctrl);
  circ_data.hreset.trans_count = 4;
  circ_data.hreset.ctrl = (0
    | (0     << 24)  // BUSY: (read only)
    | (0     << 23)  // SNIFF_EN:
    | (0     << 22)  // BSWAP: 1 = byte swap
    | (0x3F  << 15)  // TREQ_SEL(6): Transfer Request Signal, 0x3F = always
    | (helper_chnum << 11)  // CHAIN_TO(4): chain to HELPER
    | (1     << 10)  // RING_SEL: 1 = ring applies to write address, 0 = ring applies to read address
    | (4     <<  6)  // RING_SIZE(4):
    | (1     <<  5)  // INCR_WRITE:
    | (1     <<  4)  // INCR_READ:
    | (2     <<  2)  // DATA_SIZE(2): 0 = 1 byte, 1 = 2 byte, 2 = 4 byte
    | (0     <<  1)  // HIGH_PRIORIY
    | (1     <<  0)  // EN
  );

  // 4. HRESET2, executed by the HELPER channel, resets the WORK channel and starts it

  circ_data.hreset2.read_addr = (uint32_t)(&circ_data.wreset);
  circ_data.hreset2.write_addr = (uint32_t)(&regs->al1_ctrl);
  circ_data.hreset2.trans_count = 4;
  circ_data.hreset2.ctrl = (0
    | (0     << 24)  // BUSY: (read only)
    | (0     << 23)  // SNIFF_EN:
    | (0     << 22)  // BSWAP: 1 = byte swap
    | (0x3F  << 15)  // TREQ_SEL(6): Transfer Request Signal, 0x3F = always
    | (chnum << 11)  // CHAIN_TO(4): chain to WORK
    | (1     << 10)  // RING_SEL: 1 = ring applies to write address, 0 = ring applies to read address
    | (4     <<  6)  // RING_SIZE(4): use 16 byte wrapping here !!!
    | (1     <<  5)  // INCR_WRITE:
    | (1     <<  4)  // INCR_READ:
    | (2     <<  2)  // DATA_SIZE(2): 0 = 1 byte, 1 = 2 byte, 2 = 4 byte
    | (0     <<  1)  // HIGH_PRIORIY
    | (1     <<  0)  // EN
  );

  // finally setup the HELPER channel for the first run

  helper_regs->al1_ctrl &= ~DMA_CTRL_EN;  // this pauses only the channel
  gregs->abort = (1 << helper_chnum);

  helper_regs->read_addr      = uint32_t(&circ_data.workend);
  helper_regs->write_addr     = circ_data.workend.write_addr;
  helper_regs->transfer_count = circ_data.workend.trans_count;
  helper_regs->al1_ctrl       = circ_data.workend.ctrl;  // do not trigger now !

  return true;
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

    uint32_t crreg = helper_regs->ctrl_trig;
    crreg &= ~DMA_CTRL_EN;  // this pauses only the channel
    helper_regs->al1_ctrl = crreg;
    if (crreg & DMA_CTRL_BUSY)
    {
      gregs->abort = (1 << helper_chnum);
      while (gregs->abort & (1 << helper_chnum))
      {
        // wait until the abort completes
      }
    }

    return true;
  }
  else
  {
    helper_regs = nullptr;
    return false;
  }
}

