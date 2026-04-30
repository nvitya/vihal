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

#include "hwadc.h"
//#include "hwadc_py32.h"
#include "clockcnt.h"
#include "py32_utils.h"

bool THwAdc_py32::Init(int adevnum, uint32_t achannel_map)
{
	uint32_t tmp;

	initialized = false;

	devnum = adevnum;
	initialized = false;
	channel_map = achannel_map;

	regs = nullptr;
	if  (1 == devnum)
	{
		regs = ADC1;
		RCC->APBENR2 |= RCC_APBENR2_ADCEN;
	}
	else
	{
		return false;
	}

	// DMA
  dmach.Init(1, dmaalloc & 15, 0);  // dma request 0 = ADC
	dmach.Prepare(false, (void *)&regs->DR, 0);

	ADC1_COMMON->CCR |= ADC_CCR_TSVREFINTEN;  // enable temp sensor + internal reference

	regs->CFGR2 = 0
	  | (0 << 30)  // CLKMODE(2), 0 = ADCCLK (14 MHz), 1 = PCLK / 2, 2 = PCLK / 4
	;

	// disable the ADC first
	while (regs->CR & 1)
	{
		regs->CR |= (1 << 1); // disable the ADC first
	}
	delay_us(10);

	uint32_t cr = (regs->CR & 0x7FFFFF00);
	// ADC CR reg bits:
	//   31 | ADCAL
	//  7:6 | VREFBUF_SEL(2): 0 = 1.024V, 1 = 1.5V, 2 = 2.048V, 3 = 2.5V
	//    5 | VREFBUF_EN
	//    4 | ADSTP
	//    3 | MSBSEL
	//    2 | ADSTART
	//    1 | ADDIS
	//    0 | ADEN

	cr |= (7 << 5); // enable the internal reference for 2.5V
	regs->CR = cr;
	delay_us(10);
	regs->CR |= (1u << 31); // start calibration
	delay_us(10);
	while (regs->CR & (1u << 31))
	{
		// wait until the calibration is ready
	}

	delay_us(10);

	regs->CR |= (1 << 0); // enable the ADC

	delay_us(10);

	while ((regs->CR & 1) == 0)
	{
		// wait until enabled
	}

	regs->IER = 0; // disable interrupts

	// CFGR1

	regs->CFGR1 = 0
	  | (0 << 26)  // AWDCH(5):  analogue watchdog channel select
	  | (0 << 23)  // AWDEN: 1 = enable AWD
	  | (0 << 22)  // AWDSGL: single ch. AWD
	  | (0 << 16)  // DISCEN: 1 = discontinous mode enabled
	  | (0 << 15)  // AUTOFF: 1 = auto off mode enabled
	  | (0 << 14)  // WAIT: 1 = wait conversion mode on
	  | (1 << 13)  // CONT: 1 = continous conversion mode
	  | (1 << 12)  // OVRMOD: 1 = the ADC_DR will be overwritten on overrun
	  | (0 << 10)  // EXTEN(2): external trigger enable
	  | (0 <<  6)  // EXTSEL(3): external trigger select
	  | (1 <<  5)  // ALIGN: 0 = right data alignment, 1 = left data aligment
	  | (0 <<  3)  // RES(2): resolution, 0 = 12 bit, 1 = 10 bit, 2 = 8 bit, 3 = 6 bit
	  | (0 <<  2)  // SCANDIR: 0 = upward scan (ch. 0 -> 18), 1 = backward scan (ch. 18 -> 0)
	  | (1 <<  1)  // DMACFG: 0 = one shot DMA mode, 1 = circular DMA mode
	  | (1 <<  0)  // DMAEN: 1 = DMA enabled
	;

	// Set the ADC clock to HSI/2 = 12 MHz
	regs->CFGR2 = (0
		| (9 << 28)  // CKMODE(4): 0-6 = PCLK / (2^CLKMODE[2:0]), 8-14 = HSI / (2^CLKMODE[2:0])
	);

	adc_clock = 12000000;

	// setup channel sampling time registers

	// setup sampling time

	uint32_t stcode = 0; // sampling time code, index for the following array
  uint32_t sampling_clocks[8] = {4, 6, 8, 14, 29, 42, 135, 240};  // actually its 0.5 CLK less
	uint32_t target_sampling_clocks = (sampling_time_ns * 1000) / (adc_clock / 1000);
	while ((stcode < 7) && (sampling_clocks[stcode] < target_sampling_clocks))
	{
		++stcode;
	}

	// same sampling time for all the channels:
	tmp = 0;
	int i;
	for (i = 0; i < 10; ++i)
	{
		tmp |= (stcode << (i * 3));
	}
	regs->SMPR0 = tmp;
	regs->SMPR1 = tmp;

  regs->TR = 0; // no analogue watchdog

	// calculate the actual conversion rate

	// total conversion cycles:  12.5 ADC clocks + sampling time (= 1.5 ADC clocks)
	conv_adc_clocks = 14;
	act_conv_rate = adc_clock / conv_adc_clocks;

	// setup the regular sequence based on the channel map and start the cyclic conversion
	StartFreeRun(channel_map);

	initialized = true;
	return true;
}

#define HWADC_SQREG_SHIFT  4

void THwAdc_py32::SetupChannels(uint32_t achsel)
{
	channel_map = achsel;

	uint32_t    ch;
	uint32_t    sqr[2] = {0, 0};
	uint32_t *  psqr = &sqr[0];
	uint32_t    bitshift = 0;

	for (ch = 0; ch < HWADC_MAX_CHANNELS; ++ch)
	{
		dmadata[ch] = 0x1111 + ch; // set some markers for diagnostics
		databyid[ch] = &dmadata[ch]; // initialize with valid pointers
	}

	dmadatacnt = 0;
	for (ch = 0; ch < HWADC_MAX_CHANNELS; ++ch)
	{
		if (channel_map & (1 << ch))
		{
			// add this channel
			*psqr |= (ch << bitshift);
			databyid[ch] = &dmadata[dmadatacnt]; // set the decode map

			++dmadatacnt;

			bitshift += HWADC_SQREG_SHIFT;
			if (bitshift > 24)
			{
				// go to the next register
				bitshift = 0;
				++psqr;
			}
		}
	}

	regs->CHSELR = channel_map;
	regs->SEQR0 = sqr[0];
	regs->SEQR1 = sqr[1];
}

void THwAdc_py32::StartFreeRun(uint32_t achsel)
{
	SetupChannels(achsel);

  // prepare the DMA transfer

	dmaxfer.bytewidth = 2;
	dmaxfer.count = dmadatacnt;
	dmaxfer.dstaddr = &dmadata[0];
	dmaxfer.flags = DMATR_CIRCULAR;
	dmach.StartTransfer(&dmaxfer);

	StartContConv();
}

void THwAdc_py32::StopFreeRun()
{
	dmach.Disable();

	// disable continuous mode
	regs->CR |= ADC_CR_ADSTP;
	while (regs->CR & ADC_CR_ADSTP)
	{
		// wait until stopped
	}
	///regs->CR2 &= ~ADC_CR2_CONT;  // disable the continuous mode
	delay_us(10);
}

void THwAdc_py32::StartContConv()
{
	// and start the conversion
	regs->CR |= ADC_CR_ADSTART; // start the ADC
}


void THwAdc_py32::StartRecord(uint32_t achsel, uint32_t abufsize, uint16_t * adstptr)
{
	StopFreeRun();

	SetupChannels(achsel); // this makes now a little bit more than what is necessary, but it is simpler this way

	dmaxfer.dstaddr = adstptr;
	dmaxfer.bytewidth = 2;
  dmaxfer.count = (abufsize >> 1);
	dmaxfer.flags = 0; // not circular this time
	dmach.StartTransfer(&dmaxfer);

	StartContConv();
}

