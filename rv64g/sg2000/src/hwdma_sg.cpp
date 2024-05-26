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
 *  file:     hwdma_sg.cpp
 *  brief:    SG200x/CV1800 (Sophgo) DMA
 *  created:  2024-05-25
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>

#include "platform.h"

//#include "hwdma.h"
#include "hwdma_sg.h"
#include "sg_utils.h"

bool THwDmaChannel_sg::Init(int achnum, int aperid)
{
  uint32_t tmp;

	initialized = false;

	if ((achnum < 0) || (achnum > 7)) // check if the channel number is 1 based
	{
		return false;
	}

	// enable the DMA clock (not documented in the reference manual 2024-02-26)
	DMA_CLK_EN_REG |= DMA_CLK_EN_BIT_ENABLE;

  chnum = (achnum & 0x07);
  chbit = (1 << chnum);
  perid = aperid;
  dmaregs = SDMA;
	regs = &dmaregs->CHAN[chnum];

	// remove the DMA channel interrupt from the other CPUS
	// bits 0-8: CPU0 (arm)
	// bits 10:18: CPU1 (C906 Linux)
	// bits 20:28: CPU2 (C906 @ 700 MHz, this one)
	tmp = DMA_INT_MUX_REG;
	tmp &= ~((chbit <<  0) | (chbit << 10));
  tmp |=  ((chbit << 20));
  DMA_INT_MUX_REG = tmp;

  // set the peripheral mapping
  uint32_t reg_idx   = (chnum >> 2);
  uint32_t reg_shift = (chnum & 3) << 3;
  tmp = DMA_CH_REMAP->CH_REMAP[reg_idx];
  tmp &= ~(0x3F << reg_shift);
  tmp |= (perid << reg_shift);
  DMA_CH_REMAP->CH_REMAP[reg_idx] = tmp;

  // global DMA init

  dmaregs->CFG = (0
    | (1 << 0) // DMAC_EN: 1 = enable DMA
    | (1 << 1) // INT_EN:  1 = enable interrupts
  );

  Disable();

	Prepare(true, nullptr, 0); // set some defaults

	initialized = true;

	return true;
}

void THwDmaChannel_sg::Prepare(bool aistx, void * aperiphaddr, unsigned aflags)
{
	istx = aistx;
	periphaddr = aperiphaddr;
}

void THwDmaChannel_sg::Disable()
{
  // WARNING: not IRQ and multi-core safe !!!
  dmaregs->CH_EN &= ~chbit;
}

void THwDmaChannel_sg::Enable()
{
  // WARNING: not IRQ and multi-core safe !!!
  dmaregs->CH_EN |= chbit;
}

void THwDmaChannel_sg::PrepareTransfer(THwDmaTransfer * axfer)
{
  Disable();  // this is important here
	ClearIrqFlag();

#if 0

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
#endif
}



