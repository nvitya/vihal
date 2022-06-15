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
 *  file:     hwdma_gd32v.cpp
 *  brief:    GD32V DMA
 *  created:  2022-06-15
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>

#include "platform.h"

//#include "hwdma.h"
#include "hwdma_gd32v.h"
#include "gd32v_utils.h"

bool THwDmaChannel_gd32v::Init(int admanum, int achannel)  // admanum = 0..1, achannel = 0..6
{
	initialized = false;

	if ((achannel < 0) || (achannel > 6)) // check if the channel number is 1 based
	{
		return false;
	}

	dmanum = (admanum  & 0x03);
  chnum  = (achannel & 0x07);

	if (0 == dmanum)
	{
	  RCU->AHBEN |= RCU_AHBEN_DMA0EN;
		dmaregs = (gd32v_dma_t * )DMA0;
	}
	else if (1 == dmanum)
	{
    RCU->AHBEN |= RCU_AHBEN_DMA1EN;
    dmaregs = (gd32v_dma_t * )DMA1;
	}
	else
	{
		return false;
	}

	regs = &dmaregs->CH[chnum];

	irqstshift = 4 * chnum;

	Prepare(true, nullptr, 0); // set some defaults

	initialized = true;

	return true;
}

void THwDmaChannel_gd32v::Prepare(bool aistx, void * aperiphaddr, unsigned aflags)
{
	istx = aistx;
	periphaddr = aperiphaddr;
}

void THwDmaChannel_gd32v::Disable()
{
	regs->CTL &= ~1;

	// wait until it is disabled
	while (regs->CTL & 1)
	{
		// wait
	}

	regs->CNT = 0; // this is required (at least on the F303) to properly stop the channel after broken transaction
}

void THwDmaChannel_gd32v::Enable()
{
	// start the channel
	regs->CTL |= 1;
}

void THwDmaChannel_gd32v::PrepareTransfer(THwDmaTransfer * axfer)
{
  Disable();  // this is important here
	ClearIrqFlag();

	// this register layout is fully STM32F1 compatible
	uint32_t ctl = (0
    | (0  << 14)  // MEM2MEM: 1 = memory to memory mode
    | (0  << 12)  // PL(2): priority level
    | (0  << 10)  // MSIZE(2): Memory data size, 0 = 8-bit, 1 = 16-bit
    | (0  <<  8)  // PSIZE(2): Periph data size, 0 = 8-bit, 1 = 16-bit
    | (0  <<  7)  // MINC: Memory increment mode, 1 = increment memory address
    | (0  <<  6)  // PINC: Peripheral increment mode, 1 = increment peripheral address
    | (0  <<  5)  // CIRC: 1 = Circular mode
    | (0  <<  4)  // DIR: Data transfer direction, 0 = RX (per -> mem), 1 = TX (mem -> per)
    | (0  <<  3)  // TEIE: Transfer error interrupt enable
    | (0  <<  2)  // HTIE: Half transfer interrupt enable
    | (0  <<  1)  // TCIE: TCIE: Transfer complete interrupt enable
    | (0  <<  0)  // EN: keep not enabled yet
  );

  if (axfer->flags & DMATR_MEM_TO_MEM)
  {
    ctl |= (1 << 14); // MEM2MEM
  }
  else if (istx)
  {
    ctl |= (1 << 4); // DIR: 1 = TX
  }

  if      (axfer->bytewidth == 2)  { ctl |= ((1 << 10) | (1 << 8)); }
  else if (axfer->bytewidth == 4)  { ctl |= ((2 << 10) | (2 << 8)); }

  if (0 == (axfer->flags & DMATR_NO_ADDR_INC))
  {
    ctl |= (1 << 7);
  }

  if (axfer->flags & DMATR_CIRCULAR)
  {
    ctl |= (1 << 5);
  }

  if (axfer->flags & DMATR_IRQ)
  {
    ctl |= (1 << 1);
  }

  regs->CTL = ctl;

	if (axfer->flags & DMATR_MEM_TO_MEM)
	{
		regs->PADDR = (uint32_t)axfer->srcaddr;
		regs->MADDR = (uint32_t)axfer->dstaddr;
	}
	else if (istx)
	{
		regs->PADDR = (uint32_t)periphaddr;
		regs->MADDR = (uint32_t)axfer->srcaddr;
	}
	else
	{
		regs->PADDR = (uint32_t)periphaddr;
		regs->MADDR = (uint32_t)axfer->dstaddr;
	}

	regs->CNT = (uint32_t)axfer->count;
}



