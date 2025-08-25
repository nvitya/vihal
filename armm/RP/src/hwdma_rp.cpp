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
 2. HELPER (allocated internally automatically using the highest numbered unused DMA channel)

The RP2040 DMA restores the original transfer count when finished.
For the circular buffer implementation only the read or write address must be resetted.
The HELPER channel is set up with the following properties:
  - no address increment
  - single 32-bit transfer
  - write the original value to the WORK channel address register
  - linked back to the WORK channel (triggers the WORK channel on completition)

The WORK channel triggers the HELPER channel on completition (links to the HELPER channel)

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

void THwDmaChannel_rp::PrepareTransfer(THwDmaTransfer * axfer)
{
  Disable();

  #if defined(MCUSF_20)
    uint32_t crreg = (0
      | (0     << 24)  // BUSY: (read only)
      | (0     << 23)  // SNIFF_EN:
      | (0     << 22)  // BSWAP: 1 = byte swap
      | (perid << 15)  // TREQ_SEL(6): Transfer Request Signal
      | (0     << 11)  // CHAIN_TO(4): will be set later, disable the chaining by setting self-number here
      | (0     << 10)  // RING_SEL: 1 = ring applies to write address, 0 = ring applies to read address
      | (0     <<  6)  // RING_SIZE(4): use 16 byte wrapping here !!!
      | (0     <<  5)  // INCR_WRITE: do not increment !
      | (0     <<  4)  // INCR_READ: do not increment !
      | (0     <<  2)  // DATA_SIZE(2): 0 = 1 byte, 1 = 2 byte, 2 = 4 byte
      | (0     <<  1)  // HIGH_PRIORIY
      | (0     <<  0)  // EN
    );
  #elif defined(MCUSF_23)
    uint32_t crreg = (0
      | (0     << 31)  // AHB_ERROR
      | (0     << 30)  // READ_ERROR
      | (0     << 29)  // WRITE_ERROR
      | (0     << 26)  // BUSY: (read only)
      | (0     << 25)  // SNIFF_EN:
      | (0     << 24)  // BSWAP: 1 = byte swap
      | (0     << 23)  // IRQ_QUIET
      | (perid << 17)  // TREQ_SEL(6): Transfer Request Signal
      | (0     << 13)  // CHAIN_TO(4): will be set later, disable the chaining by setting self-number here
      | (0     << 12)  // RING_SEL: 1 = ring applies to write address, 0 = ring applies to read address
      | (0     <<  8)  // RING_SIZE(4): use 16 byte wrapping here !!!
      | (0     <<  7)  // INCR_WRITE_REV:
      | (0     <<  6)  // INCR_WRITE: do not increment !
      | (0     <<  5)  // INCR_RED_REV:
      | (0     <<  4)  // INCR_READ: do not increment !
      | (0     <<  2)  // DATA_SIZE(2): 0 = 1 byte, 1 = 2 byte, 2 = 4 byte
      | (0     <<  1)  // HIGH_PRIORIY
      | (0     <<  0)  // EN
    );
  #endif

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
      #if defined(MCUSF_20)
        crreg |= (1 << 5);
      #elif defined(MCUSF_23)
        crreg |= (1 << 6);
      #endif
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
        #if defined(MCUSF_20)
          crreg |= (1 << 5);
        #elif defined(MCUSF_23)
          crreg |= (1 << 6);
        #endif
      }

	    regs->read_addr  = (uint32_t)periphaddr;
	    regs->write_addr = ((uint32_t)axfer->dstaddr);
		}
	}

  regs->transfer_count = axfer->count;

  if (axfer->flags & DMATR_BYTESWAP)
  {
    // the QSPI unit uses this
    #if defined(MCUSF_20)
      crreg |= (1 << 22);
    #elif defined(MCUSF_23)
      crreg |= (1 << 24);
    #endif
  }

  // Circular buffer support using helper channel
  if (axfer->flags & DMATR_CIRCULAR)
  {
    if (ConfigureCircular(axfer, crreg))
    {
      // CHAIN_TO(4): trigger the helper channel on completition
      #if defined(MCUSF_20)
        crreg |= (helper_chnum << 11);
      #elif defined(MCUSF_23)
        crreg |= (helper_chnum << 13);
      #endif
    }
    else
    {
      // CHAIN_TO(4): Disable the chaining by setting self-number here
      #if defined(MCUSF_20)
        crreg |= (chnum << 11);
      #elif defined(MCUSF_23)
        crreg |= (chnum << 13);
      #endif
    }
  }
  else
  {
    // CHAIN_TO(4): Disable the chaining by setting self-number here
    #if defined(MCUSF_20)
      crreg |= (chnum << 11);
    #elif defined(MCUSF_23)
      crreg |= (chnum << 13);
    #endif
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

  helper_regs->al1_ctrl &= ~DMA_CTRL_EN;  // this pauses only the channel
  gregs->abort = (1 << helper_chnum);

  if (istx)
  {
    circ_data.original_address = ((uint32_t)axfer->srcaddr);

    helper_regs->read_addr      = uint32_t(&circ_data.original_address);
    helper_regs->write_addr     = (uint32_t)(&regs->read_addr);
  }
  else
  {
    circ_data.original_address = ((uint32_t)axfer->dstaddr);

    helper_regs->read_addr      = uint32_t(&circ_data.original_address);
    helper_regs->write_addr     = (uint32_t)(&regs->write_addr);
  }

  helper_regs->transfer_count = 1;  // will be re-setted
  #if defined(MCUSF_20)
    helper_regs->al1_ctrl = (0
      | (0     << 24)  // BUSY: (read only)
      | (0     << 23)  // SNIFF_EN:
      | (0     << 22)  // BSWAP: 1 = byte swap
      | (0x3F  << 15)  // TREQ_SEL(6): Transfer Request Signal
      | (chnum << 11)  // CHAIN_TO(4): will be set later, disable the chaining by setting self-number here
      | (0     << 10)  // RING_SEL: 1 = ring applies to write address, 0 = ring applies to read address
      | (0     <<  6)  // RING_SIZE(4): use 16 byte wrapping here !!!
      | (0     <<  5)  // INCR_WRITE: do not increment !
      | (0     <<  4)  // INCR_READ: do not increment !
      | (2     <<  2)  // DATA_SIZE(2): 0 = 1 byte, 1 = 2 byte, 2 = 4 byte
      | (1     <<  1)  // HIGH_PRIORIY
      | (1     <<  0)  // EN
    );
  #elif defined(MCUSF_23)
    helper_regs->al1_ctrl = (0
      | (0     << 31)  // AHB_ERROR
      | (0     << 30)  // READ_ERROR
      | (0     << 29)  // WRITE_ERROR
      | (0     << 26)  // BUSY: (read only)
      | (0     << 25)  // SNIFF_EN:
      | (0     << 24)  // BSWAP: 1 = byte swap
      | (0     << 23)  // IRQ_QUIET
      | (0x3F  << 17)  // TREQ_SEL(6): Transfer Request Signal
      | (chnum << 13)  // CHAIN_TO(4): will be set later, disable the chaining by setting self-number here
      | (0     << 12)  // RING_SEL: 1 = ring applies to write address, 0 = ring applies to read address
      | (0     <<  8)  // RING_SIZE(4): use 16 byte wrapping here !!!
      | (0     <<  7)  // INCR_WRITE_REV:
      | (0     <<  6)  // INCR_WRITE: do not increment !
      | (0     <<  5)  // INCR_RED_REV:
      | (0     <<  4)  // INCR_READ: do not increment !
      | (2     <<  2)  // DATA_SIZE(2): 0 = 1 byte, 1 = 2 byte, 2 = 4 byte
      | (1     <<  1)  // HIGH_PRIORIY
      | (1     <<  0)  // EN
    );
  #endif

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

