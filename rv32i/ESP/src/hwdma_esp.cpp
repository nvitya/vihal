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
 *  file:     hwdma_esp.cpp
 *  brief:    ESP32-C3 DMA
 *  created:  2023-03-25
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>

#include "platform.h"

#include "hwdma.h"
#include "hwdma_esp.h"
#include "esp_utils.h"

#define GDMA_LINK_AUTORET (1 << 20)
#define GDMA_LINK_STOP    (1 << 21)
#define GDMA_LINK_START   (1 << 22)
#define GDMA_LINK_RESTART (1 << 23)

bool THwDmaChannel_esp::Init(int achannel, int aperiph)
{
	initialized = false;

	if ((achannel < 0) || (achannel >= GDMA_MAX_CHANNELS)) // check if the channel number is 1 based
	{
		return false;
	}

  chnum  = achannel;
  periph = (aperiph & 0xF);

  dmaregs = GDMA;
  SYSTEM->PERIP_CLK_EN1 |= SYSTEM_DMA_CLK_EN;
  SYSTEM->PERIP_RST_EN1 &= ~SYSTEM_DMA_RST;
  irqreg = &dmaregs->INTR[chnum].RAW;
  irqclr = &dmaregs->INTR[chnum].CLR;

  regs = &dmaregs->CH[chnum].IN;  // set some defaults, this will be changed by Prepare()

  desc.next_desc = 0;

  // more initializations happen at the real Prepare call

	initialized = true;

	return true;
}

void THwDmaChannel_esp::Prepare(bool aistx, void * aperiphaddr, unsigned aflags)
{
	istx = aistx;
	if (istx)
	{
	  regs = &dmaregs->CH[chnum].OUT;
	  done_flag = (1 << 3);
	  irq_mask = 0b1100101011000; // out
	}
	else
	{
	  regs = &dmaregs->CH[chnum].IN;
    done_flag = (1 << 0);
    irq_mask = 0b0011010100111; // in
	}

	// reset the channel
	regs->CONF0 |=  1;
  regs->CONF0 &= ~1;

  if (istx)
  {
    regs->CONF0 |= (1 << 2); // AUTO_WRBACK
  }

  regs->PERI_SEL = periph;

	periphaddr = aperiphaddr;
}

void THwDmaChannel_esp::Disable()
{
  regs->LINK |= GDMA_LINK_STOP;

  mactive = false;
}

void THwDmaChannel_esp::Enable()
{
	// start the channel
	regs->LINK |= GDMA_LINK_START;

	mactive = true;
}

uint16_t THwDmaChannel_esp::Remaining()
{
  unsigned len  = ((desc.size_len_flags >> 12) & 0xFFF);
  unsigned size = ((desc.size_len_flags >>  0) & 0xFFF);
  return (size - len);
}

void THwDmaChannel_esp::PrepareTransfer(THwDmaTransfer * axfer)
{
  Disable();  // this is important here
	ClearIrqFlag();

	uint32_t slf = 0;  // | DMADESC_FLAG_SUC_EOF;

	if (istx)
	{
	  slf |= DMADESC_FLAG_OWN | DMADESC_FLAG_SUC_EOF;
	  slf |= ((axfer->count << 12) | (axfer->count << 0));
	  desc.bufaddr = (uint32_t)axfer->srcaddr;
	}
	else
	{
	  slf |= DMADESC_FLAG_OWN;
	  slf |= ((0 << 12) | (axfer->count << 0));
    desc.bufaddr = (uint32_t)axfer->dstaddr;
	}

	desc.size_len_flags = slf;
	desc.next_desc = 0; //unsigned(&desc);

  regs->LINK = (0
    //| GDMA_LINK_AUTORET
    | (unsigned(&desc) & 0xFFFFF)
  );

	// todo: handle circular, irq
#if 0
  if (axfer->flags & DMATR_CIRCULAR)
  {
    ctl |= (1 << 5);
  }

  if (axfer->flags & DMATR_IRQ)
  {
    ctl |= (1 << 1);
  }
#endif

}



