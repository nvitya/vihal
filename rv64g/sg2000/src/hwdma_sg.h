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
 *  file:     hwdma_sg.h
 *  brief:    SG200x/CV1800 (Sophgo) DMA
 *  created:  2024-05-25
 *  authors:  nvitya
*/

#ifndef HWDMA_SG_H_
#define HWDMA_SG_H_

#include "platform.h"

#define HWDMA_PRE_ONLY
#include "hwdma.h"

// LLI == Linked List Item; a.k.a. DMA block descriptor
typedef struct
{
  // values that are not changed by hardware
  uint64_t  SAR;
  uint64_t  DAR;
  uint64_t  BLOCK_TS;
  uint64_t  LLP;      // chain to next LLI
  uint64_t  CTL;

  // sstat and dstat can snapshot peripheral register state. silicon config may discard either or both...
  uint32_t  sstat;
  uint32_t  dstat;
  uint64_t  llp_status;
  uint64_t  reserved;
//
} dma_lli_t;  // 8 * 8 = 64 bytes

class THwDmaChannel_sg : public THwDmaChannel_pre
{
public:
	unsigned          perid = 0;
	unsigned          hsnum = 0;  // handshake number
	sdma_chregs_t *   regs = nullptr;
	sdma_regs_t *     dmaregs = nullptr;
	dma_lli_t *       lli = nullptr;
	uint32_t          chbit = 0;

	bool Init(int achnum, int aperid, int ahsnum);

	void Prepare(bool aistx, void * aperiphaddr, unsigned aflags);
	void Disable();
	void Enable();

	inline bool Enabled()        { return ((dmaregs->CH_EN & chbit) != 0); }
  inline bool Active()         { return Enabled(); }
	inline uint16_t Remaining()  { return regs->BLOCK_TS - (regs->STATUS & 0x3FFFFF); }

	bool StartTransfer(THwDmaTransfer * axfer);
	bool StartMemToMem(THwDmaTransfer * axfer);

	void PrepareTransfer(THwDmaTransfer * axfer);
	inline void StartPreparedTransfer() { Enable(); }

	inline void ClearIrqFlag()
	{
	  regs->INTCLEARREG = 0xFFFFFFFF;
	}

public:  // for debugging

	uint64_t  dbg_dma_en;
	uint32_t  dbg_dma_remap[2];

};

#define HWDMACHANNEL_IMPL  THwDmaChannel_sg

#endif // def HWDMA_SG_H_
