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
 *  file:     hwadc_sg.h
 *  brief:    Simple Internal ADC for the SG200x/CV1800
 *  date:     2025-02-13
 *  authors:  nvitya
*/

#ifndef HWADC_SG_H_
#define HWADC_SG_H_

#define HWADC_PRE_ONLY
#include "hwadc.h"

#define HWADC_MAX_CHANNELS   3
#define HWADC_DATA_LSHIFT    4

class THwAdc_sg : public THwAdc_pre
{
public:
	saradc_regs_t *  regs = nullptr;

	bool             Init(int adevnum, uint32_t achannel_map);
	uint16_t         ChValue(uint8_t ach);  // performs and waits one measurement !

	// no free-running in background, no DMA support, so these do nothing:
	void             StartFreeRun(uint32_t achsel) { }
	void             StopFreeRun() { }
	void             StartRecord(uint32_t achsel, uint32_t abufsize, uint16_t * adstptr) { }
	bool             RecordFinished() { return true; }

public: // SG200x specific
	void             StartMeasure(uint32_t achsel);
	inline bool      MeasureFinished() { return (0 == (regs->STATUS & 1)); }
	inline uint16_t  MeasuredValue(uint8_t ach) { return (regs->RESULT[ach] & 0x0FFF) << HWADC_DATA_LSHIFT; }
};

#define HWADC_IMPL THwAdc_sg

#endif /* HWADC_SG_H_ */
