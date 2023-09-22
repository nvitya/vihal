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
 *  file:     hwdma_imxrt.cpp
 *  brief:    IMXRT DMA (EDMA) Implementation
 *  created:  2022-12-11
 *  authors:  nvitya
*/

#include "hwpins.h"
#include "hwdma.h"
#include "imxrt_utils.h"
#include "string.h"

#include "traces.h"

// for request sources see dma_request_source_t in MIMXRT105x.h

unsigned dma_unaligned_read_cnt = 0;
unsigned dma_unaligned_write_cnt = 0;

// this LLI array is used for circular mode
DMA_TCD_Type  hwdma_lli_array[HW_DMA_CHANNELS]  __attribute__((aligned(32)));

bool THwDmaChannel_imxrt::Init(int achnum, int arqsrc)
{
	unsigned tmp;

	imxrt_set_clock_gate(5, 6, 3);  // // Enable eDMA Clock

	chnum = achnum & 0x1F;
	chbit = (1 << chnum);
	regs = (HW_DMA_REGS *)&(DMA0->TCD[chnum]);

	Disable();
	DMA_TCD_Type tcd;
	memset(&tcd, 0, sizeof(tcd));
	*regs = tcd; // clear all channel registers, required for re-initialization

	DMA0->CR |= (1 << 7); // EMLM: 1 = enable minor loop mapping, activates MLOFF in the NBYTES registers

	// Set DMA request source / peripheral id
	if (arqsrc & 0x100)
	{
		perid = (arqsrc & 0x7F);
		DMAMUX->CHCFG[chnum] = (perid | (1u << 31)); // set source id + enable
	}
	else
	{
		perid = -1; // no peripheral id routing (mem2mem)
	}

	// General settings
	tmp = DMA0->CR;
	tmp &= ~(DMA_CR_ERCA_MASK | DMA_CR_ERGA_MASK); // fixed arbitration
	tmp |= DMA_CR_HOE_MASK;    // halt on errors, every DMA will be stopped on errors !
	tmp &= ~DMA_CR_CLM_MASK;   // disable Continous Link Mode
	tmp &= ~DMA_CR_EDBG_MASK;  // keep running during debug
	DMA0->CR = tmp;

	// do not change the channel priority:
	//  by default the channels prioritized by their numbers

	//unsigned char * ppreg = (unsigned char *)&(DMA0->DCHPRI3); // the first address
	//ppreg += 4 * (chnum >> 4);
	//ppreg += (3 - (chnum & 3));
	//*ppreg = priority;

	Prepare(true, nullptr, 0); // set some defaults

	initialized = true;

	return true;
}

void THwDmaChannel_imxrt::Disable()
{
	DMA0->CERQ = chnum;
}

void THwDmaChannel_imxrt::Enable()
{
	DMA0->SERQ = chnum; // enable peripheral request:
}

void THwDmaChannel_imxrt::Prepare(bool aistx, void * aperiphaddr, unsigned aflags)
{
	istx = aistx;
	periphaddr = aperiphaddr;
}

// SSIZE / DSIZE based unaligned memory handling

void THwDmaChannel_imxrt::PrepareTransfer(THwDmaTransfer * axfer)
{
	DMA_TCD_Type  dmad;

	unsigned alignmask;
	unsigned memaddr;
	int      mloff;

	unsigned bytewidth = axfer->bytewidth;

	unsigned sdsize;
	if (bytewidth == 4)
	{
		sdsize = 2;
		alignmask = 3;
	}
	else if (bytewidth == 1)
	{
		sdsize = 0;
		alignmask = 0;
	}
	else if (bytewidth == 2)
	{
		sdsize = 1;
		alignmask = 1;
	}
	else if (bytewidth == 8)
	{
		sdsize = 3;
		alignmask = 7;
	}
	else
	{
		sdsize = 0;
		alignmask = 0;
	}

	unsigned unalignment;
	unsigned uasize;
	unsigned doff, soff, attr;
	unsigned csr;

	if (axfer->flags & DMATR_MEM_TO_MEM)
	{
		dmad.SADDR = unsigned(axfer->srcaddr);
		dmad.DADDR = unsigned(axfer->dstaddr);
		if (axfer->flags & DMATR_NO_SRC_INC)
		{
			soff = 0;
		}
		if (axfer->flags & DMATR_NO_DST_INC)
		{
			doff = 0;
		}
	}
	else
	{
		// PER <-> MEM transfer

		if (istx)
		{
			memaddr = unsigned(axfer->srcaddr);
			dmad.SADDR = memaddr;
			dmad.DADDR = unsigned(periphaddr);
			doff = 0;

			if (memaddr & alignmask)
			{
				// wrong aligned
				++dma_unaligned_write_cnt;

				// we still need to transfer bytewidth amount in one minor cycle because usually only one
				// HW DMA request generated for a bytewith amount of bytes

				unalignment = (memaddr & alignmask);
				//TRACE("Unaligned DMA write: %u\r\n", unalignment);

				if (unalignment & 1) // 8 bit transfers
				{
					uasize = 0;
					soff = (axfer->flags & DMATR_NO_SRC_INC ? 0 : 1);
				}
				else if (unalignment & 2) // 16 bit transfers
				{
					soff = (axfer->flags & DMATR_NO_SRC_INC ? 0 : 2);
					uasize = 1;
				}
				else // 32 bit tranfers
				{
					soff = (axfer->flags & DMATR_NO_SRC_INC ? 0 : 4);
					uasize = 2;
				}

				attr = 0
					| (uasize << 8) // SSIZE
					| sdsize        // DSIZE
				;
			}
			else
			{
				// right aligned
				soff = (axfer->flags & DMATR_NO_SRC_INC ? 0 : bytewidth);
				attr = sdsize | (sdsize << 8);
			}
		}
		else
		{
			memaddr = unsigned(axfer->dstaddr);
			dmad.SADDR = unsigned(periphaddr);
			dmad.DADDR = memaddr;
			soff = 0;

			if (memaddr & alignmask)
			{
				++dma_unaligned_read_cnt;

				// we still need to transfer bytewidth amount in one minor cycle because usually only one
				// HW DMA request generated for a bytewith amount of bytes

				unalignment = (memaddr & alignmask);

				//TRACE("Unaligned DMA read: %u\r\n", unalignment);

				if (unalignment & 1) // 8 bit transfers
				{
					uasize = 0;
					doff = (axfer->flags & DMATR_NO_DST_INC ? 0 : 1);
				}
				else if (unalignment & 2) // 16 bit transfers
				{
					doff = (axfer->flags & DMATR_NO_DST_INC ? 0 : 2);
					uasize = 1;
				}
				else // 32 bit tranfers
				{
					doff = (axfer->flags & DMATR_NO_DST_INC ? 0 : 4);
					uasize = 2;
				}

				attr = 0
					| (sdsize << 8) // SSIZE
					| uasize        // DSIZE
				;
			}
			else
			{
				// right aligned
				doff = (axfer->flags & DMATR_NO_DST_INC ? 0 : bytewidth);
				attr = sdsize | (sdsize << 8);
			}

		}
	}

	dmad.NBYTES = bytewidth;
	dmad.SLAST = 0;
	dmad.SOFF_ATTR = (attr << 16) | soff;

	csr = 0
	  | (0 << 14)  // BWC(2): Bandwidth Control, 0 = no engine stalls, 2 = 4 cycles after each RW, 3 = 8 cycles
		| (0 <<  8)  // MAJORCHLINK(5): Major Loop Link channel number
		| (0 <<  7)  // DONE: Channel Done
		| (0 <<  6)  // ACTIVE: Channel Active
		| (0 <<  5)  // MAJORELINK: Enable channel-channel linking on major loop complete
		| (0 <<  4)  // ESG: Enable Scatter/Gather (linked list)
		| (0 <<  3)  // DREQ: 1 = clear the hw request enable on complete
		| (0 <<  2)  // INTHALF: 1 = enable half major counter interrupt
		| (0 <<  1)  // INTMAJOR: 1 = enable interrupt when the major loop completed
		| (0 <<  0)  // START: software start request
	;

	if (axfer->flags & DMATR_IRQ)       csr |= (1 << 1);  // set the IRQ at major
  if (axfer->flags & DMATR_IRQ_HALF)  csr |= (1 << 2);  // set the IRQ at half
	if (axfer->flags & DMATR_CIRCULAR)
	{
		csr |= (1 << 4); // ESG: Enable Scatter/Gather (linked list)
		dmad.DLAST_SGA = (uint32_t)(&hwdma_lli_array[chnum]);
	}
	else
	{
		csr |= (1 <<  3);  // DREQ: 1 = clear the hw request enable on complete
		dmad.DLAST_SGA = 0;  // alternatively next TCD, when CSR.ESG == 1
	}

	dmad.DOFF_CITER = (axfer->count << 16) | doff;
	dmad.CSR_BITER = (axfer->count << 16) | csr;

	if (axfer->flags & DMATR_CIRCULAR)
	{
		hwdma_lli_array[chnum] = dmad;  // copy to the LLI array too
	}

	*regs = dmad; // copy the stack allocated structure to the real DMA registers

	DMA0->CERR = chnum;  // clear the errors
	DMA0->CINT = chnum;	 // clear the interrupt request
}

