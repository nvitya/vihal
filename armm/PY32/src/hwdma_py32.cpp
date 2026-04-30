/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     hwdma_py32.cpp
 *  brief:    PY32 DMA
 *  date:     2026-04-30
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>

#include "platform.h"

#include "hwdma.h"

bool THwDmaChannel_py32::Init(int admanum, int achannel, int arequest)  // admanum = 1, achannel = 1..3
{
	initialized = false;

	if ((achannel < 1) || (achannel > 3)) // check if the channel number is 1 based
	{
		__BKPT(); // invalid channel number
		return false;
	}

	dmanum = (admanum  & 0x03);
	chnum  = achannel;
	rqnum  = (arequest & 0x3F);

	int chid = ((achannel-1) & 0x07);

	regs = nullptr;
	if (1 == dmanum)
	{
		RCC->AHBENR |= RCC_AHBENR_DMAEN;

		if      (0 == chid)	 regs = DMA1_Channel1;
		else if (1 == chid)	 regs = DMA1_Channel2;
		else if (2 == chid)	 regs = DMA1_Channel3;
	}

	if (!regs)
	{
		return false;
	}


	DMA_TypeDef * dmaptr = DMA1;
	irqstreg = (unsigned *)&dmaptr->ISR;
	irqstclrreg = (unsigned *)&dmaptr->IFCR;
	crreg = (unsigned *)&regs->CCR;

	irqstshift = 4 * chid;

	// Set the DMA signal routing (map) in the SYSCFG_CFGR3
	uint32_t tmp = SYSCFG->CFGR3;
	tmp &= ~(0xFF << (chid << 3));
	tmp |= (rqnum << (chid << 3));
	SYSCFG->CFGR3 = tmp;

	Prepare(true, nullptr, 0); // set some defaults

	initialized = true;

	return true;
}

void THwDmaChannel_py32::Prepare(bool aistx, void * aperiphaddr, unsigned aflags)
{
	istx = aistx;
	periphaddr = aperiphaddr;
}

void THwDmaChannel_py32::Disable()
{
	*crreg &= ~1;

	// wait until it is disabled
	while (*crreg & 1)
	{
		// wait
	}

	regs->CNDTR = 0; // this is required (at least on the F303) to properly stop the channel after broken transaction
}

void THwDmaChannel_py32::Enable()
{
	// start the channel
	*crreg |= 1;
}

void THwDmaChannel_py32::PrepareTransfer(THwDmaTransfer * axfer)
{
  Disable();  // this is important here
	ClearIrqFlag();

	unsigned sizecode;
	if      (axfer->bytewidth == 2)  sizecode = 1;
	else if (axfer->bytewidth == 4)  sizecode = 2;
	else                             sizecode = 0;

  unsigned psizecode;
	if (axfer->flags & DMATR_PER32)  psizecode = 2;
	else                             psizecode = sizecode;

	int meminc = (axfer->flags & DMATR_NO_ADDR_INC ? 0 : 1);

	uint32_t circ = (axfer->flags & DMATR_CIRCULAR ? 1 : 0);
	uint32_t inte = (axfer->flags & DMATR_IRQ ? 1 : 0);

  if (axfer->flags & DMATR_IRQ_HALF)
  {
    inte |= 2; // next to TCIE
  }

	int dircode = (istx ? 1 : 0);
	uint32_t mem2mem = (axfer->flags & DMATR_MEM_TO_MEM ? 1 : 0);

	regs->CCR = 0
		| (mem2mem << 14)   // MEM2MEM: 1 = memory to memory mode
		| ((priority & 3) << 12)  // PL(2): priority level
		| (sizecode  << 10) // MSIZE(2): Memory data size, 8 bit
		| (psizecode <<  8) // PSIZE(2): Periph data size, 8 bit
		| (meminc    <<  7) // MINC: Memory increment mode
		| (0    <<  6)      // PINC: Peripheral increment mode
		| (circ <<  5)      // CIRC: Circular mode
		| (dircode <<  4)   // DIR(2): Data transfer direction (init with 0)
		| (0    <<  3)      // TEIE: Transfer error interrupt enable
		| (0    <<  2)      // HTIE: Half transfer interrupt enable
		| (inte <<  1)      // TCIE: TCIE: Transfer complete interrupt enable
		| (0    <<  0)      // EN: keep not enabled
	;


	if (axfer->flags & DMATR_MEM_TO_MEM)
	{
		regs->CPAR = (uint32_t)axfer->srcaddr;
		regs->CMAR = (uint32_t)axfer->dstaddr;
	}
	else if (istx)
	{
		regs->CPAR = (uint32_t)periphaddr;
		regs->CMAR = (uint32_t)axfer->srcaddr;
	}
	else
	{
		regs->CPAR = (uint32_t)periphaddr;
		regs->CMAR = (uint32_t)axfer->dstaddr;
	}

	regs->CNDTR = (uint32_t)axfer->count;
}

