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
 *  file:     hwadc_msp.h
 *  brief:    Simple Internal ADC for MSPM0G
 *  date:     2023-12-06
 *  authors:  nvitya
*/

#ifndef HWADC_MSP_H_
#define HWADC_MSP_H_

#define HWADC_PRE_ONLY
#include "hwadc.h"

#define HWADC_MAX_CHANNELS  16
#define HWADC_DATA_LSHIFT    4  // minus hw averaging

class THwAdc_msp : public THwAdc_pre
{
public: // settings
	// Warning: EXTERNAL reference (=1) must be used for the highest precision
	// The internal 2.5V VREF buffer can be used to present the external reference (1 uF external required)
	uint8_t         reference = 0;  // 0 = VDDA, 1 = EXTREF, 2 = internal reference
	uint8_t         averaging = 0;  // 0 = no averaging, 1 = 2x, 2 = 4x ... 7 = 128x
	uint8_t         avg_denom = 0;  // averaging denominator (right shift count)

protected:
	int8_t          data_lshift = HWADC_DATA_LSHIFT;

public:
	uint32_t        channel_map = 0;  // by default convert only ch 0

	THwDmaChannel   dmach;
	THwDmaTransfer  dmaxfer;

	int             dmachannel = 12;

	uint8_t         dmarqid = 0;

	uint8_t         chmap[16];
	uint8_t         chcnt = 0;

	ADC12_ULLMEM_Regs *  regs = nullptr;
	volatile uint16_t *  databyid[HWADC_MAX_CHANNELS];

	uint16_t        zero_data = 0;


	bool            Init(int adevnum, uint32_t achannel_map);
	inline uint16_t ChValue(uint8_t ach) { return (*databyid[ach] << data_lshift); }

	void StartFreeRun(uint32_t achsel);
	void StopFreeRun();
	void StartRecord(uint32_t achsel, uint32_t abufsize, uint16_t * adstptr);
	bool RecordFinished() { return !dmach.Active(); }
};

#define HWADC_IMPL THwAdc_msp

#endif /* HWADC_MSP_H_ */
