/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2023 Viktor Nagy, nvitya
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
 *  file:     hwdma_msp.h
 *  brief:    TI MSPM0G DMA
 *  created:  2023-12-11
 *  authors:  nvitya
 *  notes:
 *    Only the DMA channels 0-2 are advanced DMAs which are capable for circular (repeated) operation !
*/

#include <stdio.h>
#include <stdarg.h>

#include "platform.h"

#include "hwdma.h"
#include "hwdma_msp.h"
#include "msp_utils.h"

bool THwDmaChannel_msp::Init(int achannel, uint8_t atrigger)  // achannel = 0..6, atrigger = 0..FF
{
	initialized = false;

	if ((achannel < 0) || (achannel > 6))
	{
		return false;
	}

  chnum  = achannel;
  trigger = atrigger;

  //RCU->AHBEN |= RCU_AHBEN_DMA0EN;
  dmaregs = DMA;
	regs = &(dmaregs->DMACHAN[chnum]);
	tctlreg = &(dmaregs->DMATRIG[chnum].DMATCTL);
	intregs = &(dmaregs->INT_EVENT[0]);

	*tctlreg = trigger;

	Prepare(true, nullptr, 0); // set some defaults

	initialized = true;

	return true;
}

void THwDmaChannel_msp::Prepare(bool aistx, void * aperiphaddr, unsigned aflags)
{
	istx = aistx;
	periphaddr = aperiphaddr;
}

void THwDmaChannel_msp::Disable()
{
	regs->DMACTL &= ~DMA_DMACTL_DMAEN_MASK;

	// wait until it is disabled
	while (regs->DMACTL & DMA_DMACTL_DMAEN_MASK)
	{
		// wait
	}
}

void THwDmaChannel_msp::Enable()
{
	// start the channel
	regs->DMACTL |= DMA_DMACTL_DMAEN_MASK;
}

void THwDmaChannel_msp::PrepareTransfer(THwDmaTransfer * axfer)
{
  Disable();  // this is important here
	ClearIrqFlag();

	uint32_t ctl = (0
    | (0  << 28)  // TM(2): transfer mode, 0 = single, 1 = block, 2 = repeated single, 3 = repeated block
    | (0  << 24)  // EM(2): extended mode, 0 = normal, 2 = fill mode, 3 = table mode
    | (0  << 20)  // DSTINCR(4): destination addr change, 0 = no change, 2 = decrement, 3 = increment, 8-F = stride modes
    | (0  << 16)  // SRCINCR(4): source addr change, 0 = no change, 2 = decrement, 3 = increment, 8-F = stride modes
    | (0  << 12)  // DSTWDTH(2): destination width, 0 = 8-bit, 1 = 16-bit, 2 = 32-bit, 3 = 64-bit
    | (0  <<  8)  // SRCWDTH(2): source width, 0 = 8-bit, 1 = 16-bit, 2 = 32-bit, 3 = 64-bit
    | (0  <<  4)  // PREIRQ(3): early IRQ, 0 = disabled, 7 = half size, 1-6 = 1 - 64 transfers remaining
    | (0  <<  1)  // EN: 1 = enable
    | (0  <<  0)  // REQ: DMA request (software start)
  );

  if      (axfer->bytewidth > 1)
  {
		if      (axfer->bytewidth == 2)  { ctl |= ((1 << 12) | (1 << 8)); }
		else if (axfer->bytewidth == 4)  { ctl |= ((2 << 12) | (2 << 8)); }
		else if (axfer->bytewidth == 8)  { ctl |= ((3 << 12) | (3 << 8)); }
  }


  if (axfer->flags & DMATR_CIRCULAR)
  {
    ctl |= (2 << 28); // repeated single mode (supported only by the 0-2 channels !)
  }

  uint32_t imask = intregs->IMASK & (0x10001 << chnum);
	if (axfer->flags & DMATR_IRQ)
	{
		irqmask = (1 << chnum);
		if (axfer->flags & DMATR_IRQ_HALF)
		{
			ctl |= (7 << 4);  // request early IRQ too
			irqmask |= (1 << (16 + chnum));
		}
		imask |= irqmask;
	}
	else
	{
	  irqmask = 0;
	}
	intregs->IMASK = imask;

	if (istx)
	{
	  if (0 == (axfer->flags & DMATR_NO_ADDR_INC))
	  {
	    ctl |= (3 << 16);  // increment the source address
	  }

		regs->DMADA = (uint32_t)periphaddr;
		regs->DMASA = (uint32_t)axfer->srcaddr;
	}
	else
	{
	  if (0 == (axfer->flags & DMATR_NO_ADDR_INC))
	  {
	    ctl |= (3 << 20);  // increment the destination address
	  }

		regs->DMASA = (uint32_t)periphaddr;
		regs->DMADA = (uint32_t)axfer->dstaddr;
	}

	regs->DMASZ = axfer->count;
  regs->DMACTL = ctl;
}



