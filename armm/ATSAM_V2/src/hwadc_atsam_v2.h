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
 *  file:     hwadc_atsam_v2.cpp
 *  brief:    ATSAM V2 Internal ADC
 *            relative complex, the sequencing also controlled with DMA
 *  version:  1.00
 *  date:     2021-11-14
 *  authors:  nvitya
*/

#ifndef HWADC_ATSAM_V2_H_
#define HWADC_ATSAM_V2_H_

#define HWADC_PRE_ONLY
#include "hwadc.h"

#define HWADC_MAX_CHANNELS  32
#define HWADC_DATA_LSHIFT    0

class THwAdc_atsam_v2 : public THwAdc_pre
{
public:
  Adc *            regs = nullptr;

	uint32_t         channel_map = 0;  // by default convert only ch 0
  uint8_t          dmadatacnt = 0;

	uint8_t          dmachannel     = 12;
	uint8_t          seq_dmachannel = 13;  // DMA channel used for sequencing control

  THwDmaChannel    dmach;
  THwDmaTransfer   dmaxfer;

  THwDmaChannel    seq_dmach;
  THwDmaTransfer   seq_dmaxfer;

  uint16_t         dmadata[HWADC_MAX_CHANNELS];    // puffer for data storage (transferred with the DMA)
  uint16_t *       databyid[HWADC_MAX_CHANNELS];

  uint32_t         seq_data[HWADC_MAX_CHANNELS];   // sequencing data

	bool             Init(int adevnum, uint32_t achannel_map);

  inline uint16_t  ChValue(unsigned ach) { return *(databyid[ach]); }

	void             StartFreeRun(uint32_t achsel);
	void             StopFreeRun();
	void             StartRecord(uint32_t achsel, uint32_t abufsize, uint16_t * adstptr);
	bool             RecordFinished() { return !dmach.Active(); }

public: // helper functions
  void             SetupChannels(uint32_t achsel);
  void             StartContConv();

};

#define HWADC_IMPL THwAdc_atsam_v2

#endif /* HWADC_ATSAM_V2_H_ */
