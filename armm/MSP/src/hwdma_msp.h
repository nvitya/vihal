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
*/

#ifndef HWDMA_MSP_H_
#define HWDMA_MSP_H_

#include "platform.h"

#define HWDMA_PRE_ONLY
#include "hwdma.h"

class THwDmaChannel_msp : public THwDmaChannel_pre
{
public:
	int                   dmanum = 0;
	uint8_t               trigger = 0;

	DMA_DMACHAN_Regs *    regs = nullptr;
	DMA_Regs *            dmaregs = nullptr;
	__IO uint32_t *       tctlreg = nullptr;
	DMA_INT_EVENT_Regs *  intregs = nullptr;
	uint32_t              irqmask = 0;

	bool Init(int achannel, uint8_t atrigger);  // only the channels 0-2 are advanced DMAs !

	void Prepare(bool aistx, void * aperiphaddr, unsigned aflags);
	void Disable();
	void Enable();

	inline bool Enabled()        { return ((regs->DMACTL & DMA_DMACTL_DMAEN_MASK) != 0); }
  inline bool Active()         { return ((regs->DMACTL & DMA_DMACTL_DMAEN_MASK) != 0); }

	inline uint16_t Remaining()  { return regs->DMASZ; }

	bool StartTransfer(THwDmaTransfer * axfer);
	bool StartMemToMem(THwDmaTransfer * axfer);

	void PrepareTransfer(THwDmaTransfer * axfer);
	inline void StartPreparedTransfer() { Enable(); }

	inline void ClearIrqFlag()
	{
		intregs->ICLR = irqmask;
	}

};

#define HWDMACHANNEL_IMPL  THwDmaChannel_msp

#endif // def HWDMA_MSP_H_
