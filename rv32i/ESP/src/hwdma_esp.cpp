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

#define GDMA_INLINK_AUTORET (1 << 20)
#define GDMA_INLINK_STOP    (1 << 21)
#define GDMA_INLINK_START   (1 << 22)
#define GDMA_INLINK_RESTART (1 << 23)

#define GDMA_OUTLINK_STOP    (1 << 20)
#define GDMA_OUTLINK_START   (1 << 21)
#define GDMA_OUTLINK_RESTART (1 << 22)


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
	  link_start_bit = GDMA_OUTLINK_START;
	  link_stop_bit  = GDMA_OUTLINK_STOP;
	  irq_mask = 0b1100101011000; // out
	}
	else
	{
	  regs = &dmaregs->CH[chnum].IN;
    done_flag = (1 << 0);
    link_start_bit = GDMA_INLINK_START;
    link_stop_bit  = GDMA_INLINK_STOP;
    irq_mask = 0b0011010100111; // in
	}

	// reset the channel
	regs->CONF0 |=  1;
  regs->CONF0 &= ~1;

  if (istx)
  {
    //regs->CONF0 |= (1 << 2); // AUTO_WRBACK
  }

  regs->PERI_SEL = periph;

	periphaddr = aperiphaddr;
}

void THwDmaChannel_esp::Disable()
{
  regs->LINK |= link_stop_bit;

  mactive = false;
}

void THwDmaChannel_esp::Enable()
{
	// start the channel
	regs->LINK |= link_start_bit;

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

	if (axfer->flags & DMATR_MEM_TO_MEM)
	{
	  // special case
	  esp_dmainout_t *  outregs = &dmaregs->CH[chnum].OUT;
	  esp_dmainout_t *  inregs  = &dmaregs->CH[chnum].IN;

	  inregs->CONF0 = (1 << 4); // enable mem2mem
	  inregs->CONF1 = 0;

	  outregs->CONF0 = (1 << 3); // EOF mode
	  outregs->CONF1 = 0;

	  outregs->PERI_SEL = 0x00;  // this peripheral must have privilege to access the internal ram ! (0 = SPI2)
	  inregs->PERI_SEL  = 0x00;  // this peripheral must have privilege to access the internal ram !

	  // descriptor for read from memory, OUT
	  desc.bufaddr  = unsigned(axfer->srcaddr);
	  desc.next_desc = 0;
	  desc.size_len_flags = ( DMADESC_FLAG_OWN | DMADESC_FLAG_ERR_EOF | (axfer->count << 12) | (axfer->count << 12) );

	  // descriptor for write to memory, IN
	  desc2.bufaddr = unsigned(axfer->dstaddr);
    desc2.next_desc = 0;
    desc2.size_len_flags = ( DMADESC_FLAG_OWN | DMADESC_FLAG_ERR_EOF | (0 << 12) | axfer->count );

    inregs->LINK = (0
        | GDMA_INLINK_AUTORET
        | (unsigned(&desc2) & 0xFFFFF)
    );

    outregs->LINK = (0
        | (unsigned(&desc) & 0xFFFFF)
    );

    inregs->LINK  |= GDMA_INLINK_START;
    outregs->LINK |= GDMA_OUTLINK_START;
	}
	else
	{
	  uint32_t slf = DMADESC_FLAG_OWN | DMADESC_FLAG_SUC_EOF;

    if (istx)
    {
      slf |= (DMADESC_FLAG_ERR_EOF);
      slf |= ((axfer->count << 12) | (axfer->count << 0));
      desc.bufaddr = (uint32_t)axfer->srcaddr;
    }
    else
    {
      slf |= (DMADESC_FLAG_ERR_EOF);
      slf |= ((0 << 12) | (axfer->count << 0));
      desc.bufaddr = (uint32_t)axfer->dstaddr;
    }

    // todo: handle circular, irq
    if (axfer->flags & DMATR_CIRCULAR)
    {
      //ctl |= (1 << 5);
    }

    if (axfer->flags & DMATR_IRQ)
    {
      //ctl |= (1 << 1);
    }

    desc.size_len_flags = slf;
    desc.next_desc = 0;

    regs->LINK = (0
      //| GDMA_LINK_AUTORET
      | (unsigned(&desc) & 0xFFFFF)
    );
	}
}



