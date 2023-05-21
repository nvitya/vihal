/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     hwadc_esp.h
 *  brief:    Simple ESP32-C3 ADC Driver
 *  date:     2023-04-09
 *  authors:  nvitya
*/

#ifndef HWADC_ESP_H_
#define HWADC_ESP_H_

#define HWADC_PRE_ONLY
#include "hwadc.h"

#define HWADC_MAX_CHANNELS  6
#define HWADC_DATA_LSHIFT   4

class THwAdc_esp : public THwAdc_pre
{
public:
  esp_saradc_regs_t *  regs = nullptr;

  uint32_t        sampling_time_us = 50;  // 50 us = 20 kHz, minimum 1 us = 1000 kHz
  uint32_t        channel_map = 0;

  bool            enable_temp_sensor = false;

  uint8_t         dmachannel = 2; // default dma channel
	THwDmaChannel   dmach;
	THwDmaTransfer  dmaxfer;

  uint8_t         dmadatacnt = 0;
  uint16_t        dmadata[HWADC_MAX_CHANNELS];    // puffer for data storage (transferred with the DMA)
  uint16_t *      databyid[HWADC_MAX_CHANNELS];

	bool            Init(int adevnum, uint32_t achannel_map);

  uint16_t        ChValue(uint8_t ach)  { return ( *(databyid[ach]) << HWADC_DATA_LSHIFT ); }  // always left aligned

	void            StartFreeRun(uint32_t achannel_map);
	void            StopFreeRun();
	void            StartRecord(uint32_t achsel, uint32_t abufsize, uint16_t * adstptr);
	bool            RecordFinished();

public:
	uint32_t        adcdiv = 0;

	void            SetupChannels(uint32_t achannel_map);
};

#define HWADC_IMPL THwAdc_esp

#endif /* HWADC_ESP_H_ */
