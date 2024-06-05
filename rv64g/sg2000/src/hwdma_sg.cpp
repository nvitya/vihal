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

#include "string.h"
#include <stdio.h>
#include <stdarg.h>

#include "platform.h"

#include "hwdma.h"
#include "hwdma_sg.h"
#include "sg_utils.h"

dma_lli_t  g_dma_lli[8]  __attribute__((aligned(64)));

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
  dmaregs->CFG = (0
    | (1 << 0) // DMAC_EN: 1 = enable DMA
    | (1 << 1) // INT_EN:  1 = enable interrupts
  );

	regs = &dmaregs->CHAN[chnum];
	lli = &g_dma_lli[chnum];

	memset(lli, 0, sizeof(dma_lli_t));
  lli->LLP = (intptr_t)lli; // link it back
	regs->LLP = (intptr_t)lli;

  dbg_dma_en = dmaregs->CH_EN;
  dbg_dma_remap[0] = DMA_CH_REMAP->CH_REMAP[0];
  dbg_dma_remap[1] = DMA_CH_REMAP->CH_REMAP[1];

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
  if (DMA_CH_REMAP->CH_REMAP[reg_idx])  { }
  tmp |= (1 << 31); // set the update bit
  DMA_CH_REMAP->CH_REMAP[reg_idx] = tmp;
  if (DMA_CH_REMAP->CH_REMAP[reg_idx])  { }

  Abort();

  ClearIrqFlag();

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
  dmaregs->CH_EN = (0x00 | (chbit << 8));
}

void THwDmaChannel_sg::Enable()
{
  dmaregs->CH_EN = (0xFF | (chbit << 8));
}

void THwDmaChannel_sg::Abort()
{
  while (dmaregs->CH_EN & chbit)
  {
    dmaregs->CH_EN = (0
      | (0x00            <<  0)
      | (chbit           <<  8)  // disable en
      | (uint64_t(0xFF)  << 32)  // abort
      | (uint64_t(chbit) << 40)  // abort en
    );
  }
}

void THwDmaChannel_sg::PrepareTransfer(THwDmaTransfer * axfer)
{
  Disable();  // should not be running
	ClearIrqFlag();

  uint64_t tr_width;
  if      (1  == axfer->bytewidth)  tr_width = 0;  // this is here too because it is the most probable
  else if (4  == axfer->bytewidth)  tr_width = 2;
  else if (2  == axfer->bytewidth)  tr_width = 1;
	else if (8  == axfer->bytewidth)  tr_width = 3;
	else if (16 == axfer->bytewidth)  tr_width = 4;
	else if (32 == axfer->bytewidth)  tr_width = 5;
	else                              tr_width = 0;

  uint64_t msize = 0; // burst size: 0 = 1x, 1 = 4x, 2 = 8x, 3 = 16x, 4 = 32x .. 7 = 256x

	uint64_t ctl = (0
	  | (1         <<  0)  // SMS: src master select: 0 = AXI master 1, 1 = AXI master 2
	  | (1         <<  2)  // DMS: dst master select: 0 = AXI master 1, 1 = AXI master 2
	  | (0         <<  4)  // SINC: ! 0 = Increment src address, 1 = fix !
	  | (0         <<  6)  // DINC: ! 0 = Increment dst address, 1 = fix !
	  | (tr_width  <<  8)  // SRC_TR_WIDTH(3): src transfer width: 0 = 1 byte, 1 = 2 byte, 2 = 4 byte, 3 = 8 byte, 4 = 16 byte ?
	  | (tr_width  << 11)  // DST_TR_WIDTH(3): dst transfer width: 0 = 1 byte, 1 = 2 byte, 2 = 4 byte, 3 = 8 byte, 4 = 16 byte ?
	  | (msize     << 14)  // SRC_MSIZE(4): src burst transaction length: 0 = 1x, 1 = 4x, 2 = 8x, 3 = 16x, 4 = 32x .. 7 = 256x
	  | (msize     << 18)  // DST_MSIZE(4): dst burst transaction length: 0 = 1x, 1 = 4x, 2 = 8x, 3 = 16x, 4 = 32x .. 7 = 256x
	  | (0         << 22)  // AR_CACHE(4)
	  | (0         << 26)  // AW_CACHE(4)
	  | (0         << 30)  // NonPosted_LastWrite_En
	  | (0ull      << 32)  // AR_PROT(3)
	  | (0ull      << 35)  // AW_PROT(3)
	  | (0ull      << 38)  // ARLEN_EN
	  | (0ull      << 39)  // ARLEN(8)
	  | (0ull      << 47)  // AWLEN_EN
	  | (0ull      << 48)  // ARLEN(8)
	  | (0ull      << 56)  // SRC_STAT_EN
	  | (0ull      << 57)  // DST_STAT_EN
	  | (0ull      << 58)  // IOC_BlkTfr
	  | (0ull      << 62)  // SHADOWREG_OR_LLI_LAST
	  | (1ull      << 63)  // SHADOWREG_OR_LLI_VALID
	);
  if (0 == (axfer->flags & DMATR_CIRCULAR))
  {
    ctl |= (1ull << 62);  // SHADOWREG_OR_LLI_LAST
  }

  uint64_t cfg = (0
    | (3     <<  0)  // SRC_MULTBLK_TYPE(2): 0 = keep address, 1 = reload address, 2 = shadow reg, 3 = linked list
    | (3     <<  2)  // DST_MULTBLK_TYPE(2): 0 = keep address, 1 = reload address, 2 = shadow reg, 3 = linked list
    | (0ull  << 32)  // TT_FC(3): transfer type + flow control: 0 = mem2mem, 4 = per->mem srcfc, 6 = mem->per dstfc
    | (0ull  << 35)  // HS_SEL_SRC: 1 = SW handshaking
    | (0ull  << 36)  // HS_SEL_DST: 1 = SW handshaking
    | (0ull  << 37)  // SRC_HWHS_POL
    | (0ull  << 38)  // DST_HWHS_POL
    | (0ull  << 39)  // SRC_PER: hw handshaking interface
    | (0ull  << 44)  // DST_PER: hw handshaking interface
    | (0ull  << 49)  // CH_PRIOR(3)
    | (0ull  << 52)  // LOCK_CH
    | (0ull  << 53)  // LOCK_CH_L(2)
    | (0ull  << 55)  // SRC_OSR_LMT(4)
    | (0ull  << 59)  // DST_OSR_LMT(4)
  );

  if (axfer->flags & DMATR_MEM_TO_MEM)
  {
    lli->DAR = (intptr_t)axfer->dstaddr;
    lli->SAR = (intptr_t)axfer->srcaddr;
  }
  else if (istx)  // MEM -> PER
  {
    ctl |= (1 << 6); // DINC: 1 = do not incrmement the DST address
    if (axfer->flags & DMATR_NO_ADDR_INC)
    {
      ctl |= (1 << 4); // SINC: 1 = do not incrmement the SRC address
    }
    cfg |= (0
      | (1ull << 32)  // TT_FC(3): 1 = MEM_TO_PER_DMAC: Transfer Type is memory to peripheral and Flow Controller is DMA
      | (uint64_t(chnum) << 44)  // DST_PER: hw handshaking interface = chnum ?
    );
    lli->DAR = (intptr_t)periphaddr;
    lli->SAR = (intptr_t)axfer->srcaddr;
  }
  else // PER -> MEM
  {
    ctl |= (1 << 4); // SINC: 1 = do not incrmement the SRC address
    if (axfer->flags & DMATR_NO_ADDR_INC)
    {
      ctl |= (1 << 6); // DINC: 1 = do not incrmement the DST address
    }
    cfg |= (0
      | (2ull << 32)  // TT_FC(3): 2 = PER_TO_MEM_DMAC Transfer Type is peripheral to Memory and Flow Controller is DMA
      | (uint64_t(chnum) << 39)  // SRC_PER: hw handshaking interface = chnum ?
    );
    lli->SAR = (intptr_t)periphaddr;
    lli->DAR = (intptr_t)axfer->dstaddr;
  }

  if (axfer->flags & DMATR_IRQ)
  {
    ctl |= (1ull << 58);  // IOC_BlkTfr
  }

  lli->CTL = ctl;
  lli->BLOCK_TS = axfer->count - 1;

  regs->CFG = cfg;
  //regs->LLP = (intptr_t)lli;  // already set at the channel initialization

  cpu_dcache_clear(lli);  // flush cache, to make the updates available to the DMA
}



