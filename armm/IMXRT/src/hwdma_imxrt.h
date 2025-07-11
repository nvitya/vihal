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
 *  file:     hwdma_imxrt.h
 *  brief:    IMXRT DMA (EDMA) Implementation
 *  created:  2022-12-11
 *  authors:  nvitya
*/

#ifndef HWDMA_IMXRT_H_
#define HWDMA_IMXRT_H_

#define HWDMA_PRE_ONLY
#include "hwdma.h"

typedef struct DMA_TCD_Type
{
  __IO uint32_t SADDR;
  __IO uint32_t SOFF_ATTR;
  __IO uint32_t NBYTES;
  __IO uint32_t SLAST;
  __IO uint32_t DADDR;
  __IO uint32_t DOFF_CITER;
  __IO uint32_t DLAST_SGA;
  __IO uint32_t CSR_BITER;
//
} __attribute__((packed))  DMA_TCD_Type;

#define HW_DMA_REGS      DMA_TCD_Type
#define HW_DMA_CHANNELS  32

class THwDmaChannel_imxrt : public THwDmaChannel_pre
{
public:
	unsigned           chbit = 0;
	int                perid = -1;
	HW_DMA_REGS *      regs = nullptr;

	bool Init(int achnum, int arqsrc);

	void Prepare(bool aistx, void * aperiphaddr, unsigned aflags);
	void Disable();
	void Enable();

	__attribute__((always_inline)) inline bool Enabled() { return ((DMA0->ERQ & chbit) != 0); }
	__attribute__((always_inline)) inline bool Active() { return ((regs->CSR_BITER & DMA_CSR_DONE_MASK) == 0); }
	__attribute__((always_inline)) inline unsigned Remaining() { return ((regs->DOFF_CITER >> 16) & 0x7FFF); }

	void PrepareTransfer(THwDmaTransfer * axfer);
	inline void StartPreparedTransfer() { Enable(); }

	inline void ClearIrqFlag()
	{
		DMA0->CINT = chnum;	 // clear the interrupt request
	}
};

#define HWDMACHANNEL_IMPL  THwDmaChannel_imxrt

extern unsigned dma_unaligned_read_cnt;
extern unsigned dma_unaligned_write_cnt;

#endif // def HWDMA_IMXRT_H_
