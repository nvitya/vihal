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
 *  file:     hwadc_py32.cpp
 *  brief:    PY32 Simple Internal ADC
 *  date:     2026-04-30
 *  authors:  nvitya
*/

#ifndef HWADC_PY32_H_
#define HWADC_PY32_H_

#define HWADC_PRE_ONLY
#include "hwadc.h"

#include "hwdma.h"

#define HWADC_REGS  ADC_TypeDef

#define HWADC_MAX_CHANNELS  15
#define HWADC_DATA_LSHIFT    0

class THwAdc_py32 : public THwAdc_pre
{
public:
	uint8_t               dmaalloc = 3;  // use the DMA channel 3 by default

	ADC_TypeDef *         regs = nullptr;
	ADC_Common_TypeDef *  commonregs = nullptr;

	THwDmaChannel   dmach;
	THwDmaTransfer  dmaxfer;

	uint32_t        channel_map = 0;  // by default convert only ch 0

	uint8_t         dmadatacnt = 0;
	uint16_t        dmadata[HWADC_MAX_CHANNELS];    // puffer for data storage (transferred with the DMA)
	uint16_t *      databyid[HWADC_MAX_CHANNELS];


	bool            Init(int adevnum, uint32_t achannel_map);
	inline uint16_t ChValue(uint8_t ach) { return *(databyid[ach]); }

	void            StartFreeRun(uint32_t achsel);
	void            StopFreeRun();
	void            StartRecord(uint32_t achsel, uint32_t abufsize, uint16_t * adstptr);
	bool            RecordFinished() { return !dmach.Active(); }

public: // STM32 helper functions
	void            SetupChannels(uint32_t achsel);
	void            StartContConv();
};

#define HWADC_IMPL THwAdc_py32

#endif /* HWADC_PY32_H_ */
