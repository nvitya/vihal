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
 *  file:     hwdma_esp.h
 *  brief:    ESP32-C3 DMA
 *  created:  2023-03-25
 *  authors:  nvitya
*/

#ifndef HWDMA_ESP_H_
#define HWDMA_ESP_H_

#include "platform.h"

#define HWDMA_PRE_ONLY
#include "hwdma.h"

class THwDmaChannel_esp : public THwDmaChannel_pre
{
public:
	int                  periph = 0;
	esp_dmainout_t *     regs = nullptr;
	esp_gdma_t *         dmaregs = nullptr;
	volatile uint32_t *  irqreg = nullptr;
	volatile uint32_t *  irqclr = nullptr;

	esp_dmadesc_t        desc __attribute__((aligned(16)));

	bool Init(int achannel, int aperiph);

	void Prepare(bool aistx, void * aperiphaddr, unsigned aflags);
	void Disable();
	void Enable();

	inline bool Enabled()        { return 1 == (desc.size_len_flags & DMADESC_FLAG_OWN); }
  inline bool Active()         { return mactive && (0 == (*irqreg & done_flag)); }

	uint16_t    Remaining();

	//bool StartTransfer(THwDmaTransfer * axfer);
	//bool StartMemToMem(THwDmaTransfer * axfer);

	void PrepareTransfer(THwDmaTransfer * axfer);
	inline void StartPreparedTransfer() { Enable(); }

	inline void ClearIrqFlag()
	{
			*irqclr = irq_mask;
	}

public:
  uint32_t  done_flag = 1;
  uint32_t  irq_mask  = 0;
  bool      mactive = false;

};

#define HWDMACHANNEL_IMPL  THwDmaChannel_esp

#endif // def HWDMA_ESP_H_
