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
 *  file:     hwdma_gd32v.h
 *  brief:    GD32V DMA
 *  created:  2022-06-15
 *  authors:  nvitya
*/

#ifndef HWDMA_GD32V_H_
#define HWDMA_GD32V_H_

#include "platform.h"

#define HWDMA_PRE_ONLY
#include "hwdma.h"

class THwDmaChannel_gd32v : public THwDmaChannel_pre
{
public:
	int                dmanum = 0;
	gd32v_dmach_t *    regs = nullptr;
	gd32v_dma_t *      dmaregs = nullptr;

	bool Init(int admanum, int achannel);

	void Prepare(bool aistx, void * aperiphaddr, unsigned aflags);
	void Disable();
	void Enable();

	inline bool Enabled()        { return ((regs->CTL & 1) != 0); }

  // the pure EN flag based termination check does not always work on G4
  // using the NDTR register for the termination is not reliable
  // because sometimes it might overflow due bursts
  inline bool Active()         { return ((regs->CTL & 1) && (regs->CNT)); }  // combined termination check

	inline uint16_t Remaining()  { return regs->CNT; }

	bool StartTransfer(THwDmaTransfer * axfer);
	bool StartMemToMem(THwDmaTransfer * axfer);

	void PrepareTransfer(THwDmaTransfer * axfer);
	inline void StartPreparedTransfer() { Enable(); }

	inline void ClearIrqFlag()
	{
			dmaregs->INTC = (0x0F << irqstshift);
	}

public:
  unsigned           irqstshift;

};

#define HWDMACHANNEL_IMPL  THwDmaChannel_gd32v

#endif // def HWDMA_GD32V_H_
