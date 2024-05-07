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
 *  file:     hwadc_msp.cpp
 *  brief:    Simple Internal ADC for MSPM0G
 *  date:     2023-12-06
 *  authors:  nvitya
*/

#include "string.h"
#include "hwadc.h"
#include "hwadc_msp.h"
#include "clockcnt.h"

bool THwAdc_msp::Init(int adevnum, uint32_t achannel_map)
{
	uint32_t tmp;
	unsigned perid;
	unsigned periphclock = SystemCoreClock;

	initialized = false;

	devnum = adevnum;
	initialized = false;
	channel_map = achannel_map;

	for (tmp = 0; tmp < HWADC_MAX_CHANNELS; ++tmp)
	{
		chmap[tmp] = 0;
		databyid[tmp] = &zero_data;
	}

	regs = nullptr;
	if (0 == devnum)
	{
		regs = &ADC0->ULLMEM;
	}
#if defined(ADC1_BASE)
	else if (1 == devnum)
	{
		regs = &ADC1->ULLMEM;
	}
#endif
	else
	{
		return false;
	}

	// Enable the peripheral

	regs->GPRCM.PWREN = (ADC12_PWREN_KEY_UNLOCK_W | ADC12_PWREN_ENABLE_MASK);
	if (regs->GPRCM.PWREN) { }
	if (regs->GPRCM.PWREN) { }

	// Clock config: by default the ADC uses the ULPCLK, which is the half of the CPU clock, usually 40 MHz
	regs->GPRCM.CLKCFG = (0
		| ADC12_CLKCFG_KEY_UNLOCK_W
		| (0  <<  0)  // SAMPCLK(2): 0 = ULPCLK, 1 = SYSOSC, 2 = HFCLK
	);

	// sampling clock freq range
	unsigned cfreq;
	unsigned adc_mhz = SystemCoreClock / 2000000;  // the ULPICLK has the half of the CPU speed
	if      (adc_mhz <  4)  cfreq = 0;
	else if (adc_mhz <  8)  cfreq = 1;
	else if (adc_mhz < 16)  cfreq = 2;
	else if (adc_mhz < 20)  cfreq = 3;
	else if (adc_mhz < 24)  cfreq = 4;
	else if (adc_mhz < 32)  cfreq = 5;
	else if (adc_mhz < 40)  cfreq = 6;
	else                    cfreq = 7;
	regs->CLKFREQ = cfreq;

	regs->CTL0 = (0
	  | (0  << 24)  // SCLKDIV(3): 0 = do not divide the sampling clock, 1 = /2, 2 = /4, 3 = /8, 4 = 16, ! 5 = /24, 6 = /32, 7 = 48
	  | (1  << 16)  // PWRDN: 1 = do not power down the ADC
	  | (0  <<  0)  // ENC: 1 = enable conversion
	);

	regs->CTL1 = (0
	  | (0  << 28)  // AVGD(3): HW avg denominator (right shift count)
	  | (0  << 24)  // AVGN(3): 0 = disable HW AVG, 1 = 2x, 2 = 4x .. 7 = 128x
	  | (0  << 20)  // SAMPMODE: sampling mode (only, not the conversion): 0 = AUTO: sample timer is the trigger, 1 = MANUAL: SW trigger
	  | (0  << 16)  // CONSEQ(2): 0 = simple ch, 1 = simple sequence, 2 = repeated ch, 3 = repeated sequence
	  | (0  <<  8)  // SC: 1 = start sampling
	  | (0  <<  0)  // TRIGSRC: sample trigger source, 0 = SW, 1 = HW event trigger
	);

	regs->CTL2 = (0
	  | (0  << 24)  // ENDADD(5)
	  | (0  << 16)  // STARTADD(5)
	  | (0  << 11)  // SAMPCNT(5)
	  | (0  << 10)  // FIFOEN: 1 = enable FIFO based operation
	  | (0  <<  8)  // DMAEN: 1 = enable DMA trigger
	  | (0  <<  1)  // RES(2): resolution, 0 = 12-bit, 1 = 10-bit, 2 = 8-bit
	  | (0  <<  0)  // DF: 0 = unsigned right aligned, 1 = signed left aligned
	);

	// Ad-hoc single conversion (ASC) setup
	regs->CTL3 = (0
	  | (0  << 12)  // ASCVRSEL(2): ASC voltage reference, 0 = VDDA, 1 = EXTREV, 2 = internal reference
	  | (0  <<  8)  // ASCSTIME: 0 = SCOMP0, 1 = SCOMP1
	  | (0  <<  0)  // ASCCHSEL(5): channel select
	);

	uint32_t stime_clk = (sampling_time_ns * adc_mhz) / 1000;
	if (stime_clk < 4)  stime_clk = 4;

	regs->SCOMP0 = stime_clk; // VAL(10): sampling time clocks
	regs->SCOMP1 = stime_clk; // VAL(10): sampling time clocksg

	regs->REFCFG = 0; // disable the internal voltage reference

	// set ADC clock
	uint32_t baseclock = SystemCoreClock / 2; // = ULPICLK

	adc_clock = baseclock / 1;
	// calculate the actual conversion rate
	conv_adc_clocks = 8 + stime_clk;
	act_conv_rate = adc_clock / conv_adc_clocks;

	StartFreeRun(channel_map);

	initialized = true;
	return true;
}

void THwAdc_msp::StartFreeRun(uint32_t achsel)
{
	uint32_t ch;

	StopFreeRun();

	channel_map = achsel;

	chcnt = 0;
	memset(&chmap[0], 0, sizeof(chmap));

	for (ch = 0; ch < HWADC_MAX_CHANNELS; ++ch)
	{
		if (channel_map & (1 << ch))
		{
			chmap[ch] = chcnt;
			databyid[ch] = (volatile uint16_t *)&regs->MEMRES[chcnt];

			// setup the control reg

			uint32_t creg = (0
			  | (0         << 28)  // WINCOMP: 1 = enable window comparator
			  | (0         << 24)  // TRIG: 0 = automatic trigger, 1 = requires a trigger event
			  | (0         << 20)  // BCSEN: 1 = enable burn out current source
			  | (0         << 16)  // AVGEN: 1 = averaging enabled
			  | (0         << 12)  // STIME: sampling time select, 0 = SCOMP0, 1 = SCOMP1
			  | (reference <<  8) // VRSEL: 0 = VDDA, 1 = EXTREF, 2 = Internal reference
			  | (ch        <<  0)  // CHANSEL(5): channel
			);
			if (averaging)	creg |= (1 << 16);

			regs->MEMCTL[chcnt] = creg;
			++chcnt;
		}
		else
		{
			chmap[ch] = 0;
			databyid[ch] = &zero_data;
		}
	}

	if (chcnt)
	{
		regs->CTL2 = (0
			| ((chcnt - 1)  << 24)  // ENDADD(5)
			| (0            << 16)  // STARTADD(5)
			| (0            << 11)  // SAMPCNT(5)
			| (0            << 10)  // FIFOEN: 1 = enable FIFO based operation
			| (0            <<  8)  // DMAEN: 1 = enable DMA trigger
			| (0            <<  1)  // RES(2): resolution, 0 = 12-bit, 1 = 10-bit, 2 = 8-bit
			| (0            <<  0)  // DF: 0 = unsigned right aligned, 1 = signed left aligned
		);

		regs->CTL1 = (0
		  | (avg_denom    << 28)  // AVGD(3): HW avg denominator (right shift count)
		  | (averaging    << 24)  // AVGN(3): 0 = disable HW AVG, 1 = 2x, 2 = 4x .. 7 = 128x
		  | (0            << 20)  // SAMPMODE: sampling mode (only, not the conversion): 0 = AUTO: sample timer is the trigger, 1 = MANUAL: SW trigger
		  | (3            << 16)  // CONSEQ(2): 0 = simple ch, 1 = simple sequence, 2 = repeated ch, 3 = repeated sequence
		  | (1            <<  8)  // SC: 1 = start sampling
		  | (0            <<  0)  // TRIGSRC: sample trigger source, 0 = SW, 1 = HW event trigger
		);

		// start the conversions
		regs->CTL0 |= ADC12_CTL0_ENC_MASK;
		//regs->CTL1 |= ADC12_CTL1_SC_MASK;  // start the conversion
	}

	data_lshift = 16 - 12 - averaging;
}

void THwAdc_msp::StopFreeRun()
{
	// stop the freerun
	regs->CTL0 &= ~ADC12_CTL0_ENC_MASK;

	while (regs->STATUS & ADC12_STATUS_BUSY_MASK)
	{
		// wait
	}
}

void THwAdc_msp::StartRecord(uint32_t achsel, uint32_t abufsize, uint16_t * adstptr)
{
	StopFreeRun();

#if 0

	// enable the selected channels

	channel_map = achsel;
	regs->ADC_CHDR = (~channel_map & 0xFFFF);
	regs->ADC_CHER = channel_map; // enable channels

	dmaxfer.dstaddr = adstptr;
	dmaxfer.count = (abufsize >> 1);
	dmach.StartTransfer(&dmaxfer);

	regs->ADC_MR |= 0
		|	(1 <<  7)  // FREERUN: Free Run Mode, 1 = never wait for any trigger
	;

	// Start the conversion
	regs->ADC_CR = 0
		| (0 <<  0)  // SWRST: Software Reset
		| (1 <<  1)  // START: Start Conversion
	;
#endif
}

