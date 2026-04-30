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
 *  file:     hwdma_py32.h
 *  brief:    PY32 DMA
 *  date:     2026-04-30
 *  authors:  nvitya
*/

#ifndef HWDMA_PY32_H_
#define HWDMA_PY32_H_

#include "platform.h"

#define HWDMA_PRE_ONLY
#include "hwdma.h"

class THwDmaChannel_py32 : public THwDmaChannel_pre
{
public:
	uint8_t                dmanum = 1;
	uint8_t                rqnum  = 0;
	DMA_Channel_TypeDef *  regs = nullptr;

	bool Init(int admanum, int achannel, int arequest);

	void Prepare(bool aistx, void * aperiphaddr, unsigned aflags);
	void Disable();
	void Enable();

	inline bool Enabled()        { return ((*crreg & 1) != 0); }

  // the pure EN flag based termination check does not always work on G4
  // using the NDTR register for the termination is not reliable
  // because sometimes it might overflow due bursts
  inline bool Active()         { return ((*crreg & 1) && (regs->CNDTR)); }  // combined termination check

	inline uint16_t Remaining()  { return regs->CNDTR; }

	void PrepareTransfer(THwDmaTransfer * axfer);
	inline void StartPreparedTransfer() { Enable(); }

	inline void ClearIrqFlag()
	{
		*irqstclrreg = (0x0F << irqstshift);
	}

public:
  __IO unsigned *    crreg;

  __IO unsigned *    irqstreg;
  __IO unsigned *    irqstclrreg;
  unsigned           irqstshift;

};

#define HWDMACHANNEL_IMPL  THwDmaChannel_py32

#endif // def HWDMA_PY32_H_
