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
 *  file:     hwadc_sg.cpp
 *  brief:    Simple Internal ADC for the SG200x/CV1800
 *  date:     2025-02-13
 *  authors:  nvitya
*/

#include "string.h"
#include "hwadc.h"
#include "hwadc_sg.h"
#include "clockcnt.h"
#include "sg_utils.h"

bool THwAdc_sg::Init(int adevnum, uint32_t achannel_map)
{
	initialized = false;

	devnum = adevnum;
	initialized = false;

	set_periph_clock_enable(0, 10, true);  // enable the SARADC clock

  regs = (saradc_regs_t *)map_hw_addr(SARADC_BASE_ADDR, sizeof(saradc_regs_t), (void * *)&regs);

  // leave the timing settings at default
  // regs->CYC_SET = ...

	initialized = true;
	return true;
}

uint16_t THwAdc_sg::ChValue(uint8_t ach)
{
	StartMeasure(1 << ach);

  while (!MeasureFinished())
  {
  	// wait
  }

	return MeasuredValue(ach);
}

void THwAdc_sg::StartMeasure(uint32_t achsel)
{
  regs->CTRL = (0
  	| (1      <<  0)  // EN: 1 = enable the adc and start the measurements
  	| (achsel <<  5)  // SEL(3): select adc channels, bit5 = first adc channel = ADC1 !!!
  );
}
