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
 *  brief:    ATSAM V2 Simple Internal ADC
 *  version:  1.00
 *  date:     2021-11-14
 *  authors:  nvitya
*/

#include "hwadc_atsam_v2.h"
#include "atsam_v2_utils.h"
#include "clockcnt.h"

bool THwAdc_atsam_v2::Init(int adevnum, uint32_t achannel_map)
{
  initialized = false;

  devnum = adevnum;

  uint32_t tmp;
  unsigned perid;
	unsigned periphclock = SystemCoreClock;
	unsigned gclkid = 0;
	if (periphclock > 100000000)
	{
	  periphclock = (periphclock >> 1);
	  gclkid = 5; // use the half speed generator
	}

	regs = nullptr;
	if (0 == devnum)
	{
		regs = ADC0;
	  atsam2_set_periph_gclk(ADC0_GCLK_ID, gclkid);
    MCLK->APBDMASK.reg |= MCLK_APBDMASK_ADC0;
		perid = ID_ADC0;

    dmach.Init(dmachannel, ADC0_DMAC_ID_RESRDY);
	  seq_dmach.Init(seq_dmachannel, ADC0_DMAC_ID_SEQ);
	}
	else if (1 == devnum)
	{
    regs = ADC1;
    atsam2_set_periph_gclk(ADC1_GCLK_ID, gclkid);
    MCLK->APBDMASK.reg |= MCLK_APBDMASK_ADC1;
    perid = ID_ADC1;

    dmach.Init(dmachannel, ADC1_DMAC_ID_RESRDY);
    seq_dmach.Init(seq_dmachannel, ADC1_DMAC_ID_SEQ);
	}
	else
	{
		return false;
	}

	// reset

	regs->CTRLA.bit.SWRST = 1;
	while (regs->SYNCBUSY.bit.SWRST || regs->CTRLA.bit.SWRST)
	{
	  // wait until the reset completes
	}

	unsigned adc_clock = (periphclock >> 1);
	unsigned adc_presc = 0;
	while (adc_clock > 16000000)  // max 16 MHz
	{
	  adc_clock >>= 1;
	  ++adc_presc;
	}

	regs->CTRLA.reg = (0
	  | (0  << 15)  // R2R: 0 = single ended mode
	  | (adc_presc <<  8)  // PRESCALER(3):
	  | (0  <<  7)  // ONDEMAND
	  | (0  <<  6)  // RUNSTDBY
	  | (0  <<  5)  // SLAVEEN
	  | (0  <<  3)  // DUALSEL(2)
	  | (0  <<  1)  // ENABLE
	  | (0  <<  0)  // SWRST
	);

	regs->CTRLB.reg = (0
	  | (0  << 11)  // WINSS
	  | (0  <<  8)  // WINMODE(3)
	  | (0  <<  3)  // RESSEL(2): 0 = 12 bit, 1 = 16 bit for averaging mode
	  | (0  <<  2)  // CORREN: 0 = no gain or offset correction
	  | (0  <<  1)  // FREERUN: 0 = disable freerun, it works only with single channel
	  | (1  <<  0)  // LEFTADJ: 0 = right adjusted result, 1 = left adjusted result
	);

	regs->EVCTRL.reg = 0; // disable events
	regs->REFCTRL.reg = (0
	  | (0  <<  7)  // REFCOMP: 0 = disable compensation
	  | (3  <<  0)  // REFSEL(4): 0 = INTDEF, 3 = INTVCC1 = VDDANA
	);

	regs->AVGCTRL.reg = 0; // no averaging

  // sampling time
  uint32_t sampling_clocks = (sampling_time_ns * 1000) / (adc_clock / 1000);
  if       (sampling_clocks <  4)  sampling_clocks = 4;
  else if (sampling_clocks  > 64)  sampling_clocks = 64;

  regs->SAMPCTRL.reg = sampling_clocks; // no offset compensation

  // calculate the actual conversion rate
  conv_adc_clocks = sampling_clocks + 12;
  act_conv_rate = adc_clock / conv_adc_clocks;

  // no gain or offset connection
  regs->GAINCORR.reg = 0;
  regs->OFFSETCORR.reg = 0;

  regs->INTENCLR.reg = 7; // disable interrupts

  regs->DSEQCTRL.reg = 0; // disable the sequencer

  // Prepare DMA for sequencing, update only the INPUTCTRL
  seq_dmach.Prepare(true, (void *)&regs->DSEQDATA.reg, 0);

  // Pepare DMA for result transfer
  dmach.Prepare(false, (void *)&regs->RESULT.reg, 0);

  // enable the ADC
  regs->CTRLA.bit.ENABLE = 1;

  // setup the regular sequence based on the channel map and start the cyclic conversion
  StartFreeRun(achannel_map);

	initialized = true;
	return true;
}

void THwAdc_atsam_v2::SetupChannels(uint32_t achsel)
{
  channel_map = achsel;

  uint32_t ch;

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
      seq_data[dmadatacnt] = (0
        | (0  << 15)  // DSEQSTOP: 0 = do not stop
        | (0  <<  8)  // MUXNEG(5): for differential mode only
        | (0  <<  7)  // DIFFMODE: 1 = differential mode
        | (ch <<  0)  // MUXPOS(5): channel selection
      );

      databyid[ch] = &dmadata[dmadatacnt]; // set the decode map
      ++dmadatacnt;
    }
  }

  // Setup DMA for sequencing
  seq_dmaxfer.bytewidth = 4;
  seq_dmaxfer.count = dmadatacnt;
  seq_dmaxfer.srcaddr = &seq_data[0];
  seq_dmaxfer.flags = DMATR_CIRCULAR;
  seq_dmach.StartTransfer(&seq_dmaxfer);
}


void THwAdc_atsam_v2::StartFreeRun(uint32_t achsel)
{
  SetupChannels(achsel);

  // Setup DMA for the results
  dmaxfer.bytewidth = 2;
  dmaxfer.count = dmadatacnt;
  dmaxfer.dstaddr = &dmadata[0];
  dmaxfer.flags = DMATR_CIRCULAR;
  dmach.StartTransfer(&dmaxfer);

  // start the sequencer

  regs->DSEQCTRL.reg = (0
    | (1  << 31)  // AUTOSTART
    | (0  <<  0)  //
    | (0  <<  8)  // OFFSETCORR
    | (0  <<  7)  // GAINCORR
    | (0  <<  6)  // WINUT
    | (0  <<  5)  // WINLT
    | (0  <<  4)  // SAMPCTRL
    | (0  <<  3)  // AVGCTRL
    | (0  <<  2)  // REFCTRL
    | (0  <<  1)  // CTRLB
    | (1  <<  0)  // INPUTCTRL
  );

  //regs->SWTRIG.reg = (ADC_SWTRIG_START); // start the conversions
}

void THwAdc_atsam_v2::StopFreeRun()
{
  regs->DSEQCTRL.bit.AUTOSTART = 0; // stop the DMA sequencer
  while (regs->STATUS.bit.ADCBUSY)
  {
    // wait until it is stopped propery
  }
  dmach.Disable();
}

void THwAdc_atsam_v2::StartRecord(uint32_t achsel, uint32_t abufsize, uint16_t * adstptr)
{
	StopFreeRun();

	SetupChannels(achsel);

  // Setup DMA for the results
  dmaxfer.bytewidth = 2;
  dmaxfer.count = (abufsize >> 1);
  dmaxfer.dstaddr = adstptr;
  dmaxfer.flags = 0; // normal transfer
  dmach.StartTransfer(&dmaxfer);

  // start the sequencer

  regs->DSEQCTRL.reg = (0
    | (1  << 31)  // AUTOSTART
    | (0  <<  0)  //
    | (0  <<  8)  // OFFSETCORR
    | (0  <<  7)  // GAINCORR
    | (0  <<  6)  // WINUT
    | (0  <<  5)  // WINLT
    | (0  <<  4)  // SAMPCTRL
    | (0  <<  3)  // AVGCTRL
    | (0  <<  2)  // REFCTRL
    | (0  <<  1)  // CTRLB
    | (1  <<  0)  // INPUTCTRL
  );
}


