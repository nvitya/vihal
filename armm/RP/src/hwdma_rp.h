/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     hwdma_rp.h
 *  brief:    RP2040  DMA
 *  version:  1.00
 *  date:     2022-05-07
 *  authors:  nvitya
*/

#ifndef HWDMA_RP_H_
#define HWDMA_RP_H_

#define HWDMA_PRE_ONLY
#include "hwdma.h"

#define MAX_DMA_CHANNELS  12

#define DMA_CTRL_BUSY  (1 << 24)
#define DMA_CTRL_EN    (1 <<  0)

typedef struct
{
  volatile uint32_t  inte;
  volatile uint32_t  intf;
  volatile uint32_t  ints;
//
} dma_irq_regs_t;

typedef struct
{
  uint32_t   original_address;
//
} rp_dma_circ_helper_data_t;

class THwDmaChannel_rp : public THwDmaChannel_pre
{
public:
	unsigned             chbit = 0;
	int                  perid = -1;
	dma_channel_hw_t *   regs = nullptr;
	dma_hw_t *           gregs = nullptr;

	uint8_t              irq_line = 0;
	dma_irq_regs_t *     irq_regs = nullptr;

	bool Init(int achnum, int aperid);  // use only channels 0-7 if possible to leave some free for the helpers

	void Prepare(bool aistx, void * aperiphaddr, unsigned aflags);
	void Enable();
  void Disable();

  inline bool IrqFlag()       { return (0 != (irq_regs->ints & chbit)); }  // for shared IRQ line handling
	inline void ClearIrqFlag()  { irq_regs->ints = chbit; } // clear transfer complete IRQ

	inline bool Enabled() { return ((regs->ctrl_trig & DMA_CTRL_EN) != 0); }
	inline bool Active()  { return ((regs->ctrl_trig & DMA_CTRL_BUSY) != 0); }
	inline unsigned Remaining() { return regs->transfer_count; }

	void PrepareTransfer(THwDmaTransfer * axfer);
	inline void StartPreparedTransfer()              { Enable(); }

public:
	int                   helper_chnum = -1;  // helper channel for the circular buffers
	dma_channel_hw_t *    helper_regs = nullptr;

	rp_dma_circ_helper_data_t  circ_data;

	bool                  ConfigureCircular(THwDmaTransfer * axfer, uint32_t crreg);

  bool                  AllocateHelper();
};

#define HWDMACHANNEL_IMPL  THwDmaChannel_rp

#endif // def HWDMA_RP_H_
