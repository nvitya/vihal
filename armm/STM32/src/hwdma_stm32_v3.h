/* -----------------------------------------------------------------------------
 * This file is a part of the NVCM project: https://github.com/nvitya/nvcm
 * Copyright (c) 2018 Viktor Nagy, nvitya
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
 *  file:     hwdma_stm32_v3.h
 *  brief:    DMA for the STM32H7R/S series: GPDMA + HPDMA
 *  date:     2024-08-10
 *  authors:  nvitya
*/

#ifndef HWDMA_STM32_V3_H_
#define HWDMA_STM32_V3_H_

#include "platform.h"

#if defined(HWDMA_V3)

#define HWDMA_PRE_ONLY
#include "hwdma.h"

#define HWDMA_GPDMA  0
#define HWDMA_HPDMA  1

typedef struct  // the 3 item LLI structure used by VIHAL
{
  uint32_t   CBR;    // byte count
  uint32_t   SA_DA;  // source or destination address
  uint32_t   NEXTLLI;
  uint32_t   _reserved;
//
} THwDmaLli3;

class THwDmaChannel_stm32 : public THwDmaChannel_pre
{
public: // special STM32 specific settings
	uint8_t                 per_flow_controller = 0;  // 0 = DMA, 1 = peripheral
  uint8_t                 per_bus_port = 1; // 0 = AXI/AHB, 1 = AHB

public:
	DMA_Channel_TypeDef *   regs = nullptr;
	DMA_TypeDef *           gregs = nullptr;
	THwDmaLli3 *            plli3 = nullptr;

	int                     dmanum = 1;
	unsigned                rqnum;

  bool Init(int admanum, int achannel, int arequest); // admanum: 0=GPDMA, 1=HPDMA

	void Prepare(bool aistx, void * aperiphaddr, unsigned aflags);
	void Disable();
	void Enable();

	inline bool Enabled()        { return ((regs->CCR & 1) != 0); }
	inline bool Active()         { return ((regs->CCR & 1) != 0); }

	inline uint16_t Remaining()  { return (regs->CBR1 & 0xFFFF); }
	inline void ClearIrqFlag()   { regs->CFCR = irqclrmask; }

	bool StartTransfer(THwDmaTransfer * axfer);

	void PrepareTransfer(THwDmaTransfer * axfer);
	inline void StartPreparedTransfer() { Enable(); }

protected:
	uint32_t               irqclrmask = 0x7F00;  // this clears all the interrupts
	uint32_t               ccr_base   = 0;

};

#define HWDMACHANNEL_IMPL  THwDmaChannel_stm32

#endif // defined(HWDMA_V3)

#endif // def HWDMA_STM32_V3_H_
