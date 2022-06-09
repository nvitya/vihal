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
 *  file:     hwadc_rp.h
 *  brief:    Simple RP2040 ADC Driver
 *  date:     2022-06-06
 *  authors:  nvitya
*/

#include "hwadc_rp.h"
#include "rp_utils.h"
#include "clockcnt.h"

bool THwAdc_rp::Init(int adevnum, uint32_t achannel_map)
{
	uint32_t tmp;

	initialized = false;

	devnum = adevnum;  // not used here
	channel_map = (achannel_map & 0x1F);
	initialized = false;

	clock_hw_t * pclk_adc = &clocks_hw->clk[clk_adc];
	pclk_adc->ctrl = (0
	  | (1  << 11)  // ENABLE
	  | (0  <<  5)  // AUXSRC(3): 0 = PLL_USB = 48 MHz
	);
	pclk_adc->div = (1 << 8);

	regs = adc_hw;
	rp_reset_control(RESETS_RESET_ADC_BITS, true);  // force reset
	rp_reset_control(RESETS_RESET_ADC_BITS, false); // remove reset
	for (tmp = 0; tmp < 100; ++tmp)
	{
	  __NOP();
	}

	regs->cs = (0
	  | (0  << 16)  // RROBIN(5):
	  | (0  << 12)  // AINSEL(5):
	  | (0  << 10)  // ERR_STICKY
	  | (0  <<  9)  // ERR
	  | (0  <<  8)  // READY
	  | (0  <<  3)  // START_MANY
	  | (0  <<  2)  // START_ONCE
	  | ((enable_temp_sensor ? 1 : 0) <<  1)  // TS_EN
	  | (1  <<  0)  // EN: 1 = enable the ADC clocks
	);

	regs->fcs = (0
	  | (1  << 24)  // THRESH(4): DREQ/IRQ asserted when LEVEL >= THRESH
	  | (0  << 16)  // LEVEL(4): number of conversion results actually waiting in the FIFO
	  | (0  << 11)  // OVER(ro): Overflow error
	  | (0  << 10)  // UNDER(ro): Underflow error
	  | (0  <<  9)  // FULL
	  | (0  <<  8)  // EMPTY
	  | (1  <<  3)  // DREQ_EN
	  | (0  <<  2)  // ERR: 1 = conversion error is signalized in the result
	  | (0  <<  1)  // SHIFT: 1 = 8-bit data mode
	  | (1  <<  0)  // EN: 1 = enable FIFO
	);

	// calculate the actual conversion rate
  adc_clock = 48000000;
  uint32_t sampling_clocks = sampling_time_us * 48;

	conv_adc_clocks = 96;
	act_conv_rate = adc_clock / sampling_clocks;

	regs->div = (sampling_clocks << 8);

	// DMA setup
	dmach.Init(dmachannel, DREQ_ADC);
	dmach.Prepare(false, (void *)&(regs->fifo), 0);

	StartFreeRun(channel_map);

	initialized = true;
	return true;
}

void THwAdc_rp::StartFreeRun(uint32_t achannel_map)
{
  channel_map = (achannel_map & 0x1F);

  uint32_t ch;
  for (ch = 0; ch < HWADC_MAX_CHANNELS; ++ch)
  {
    dmadata[ch] = 0x1111 + ch; // set some markers for diagnostics
    databyid[ch] = (uint16_t *)&dmadata[ch]; // initialize with valid pointers
  }

  dmadatacnt = 0;
  for (ch = 0; ch < HWADC_MAX_CHANNELS; ++ch)
  {
    if (channel_map & (1 << ch))
    {
      // add this channel
      databyid[ch] = (uint16_t *)&dmadata[dmadatacnt]; // set the decode map
      ++dmadatacnt;
    }
  }

  dmaxfer.bytewidth = sizeof(dmadata[0]);
  dmaxfer.count = dmadatacnt;
  dmaxfer.dstaddr = &dmadata[0];
  dmaxfer.flags = DMATR_CIRCULAR;  // requires continuous checking on RP2040 !
  dmach.StartTransfer(&dmaxfer);

  // search the first channel
  ch = 0;
  while ((ch < 5) && (0 == (channel_map & (1 << ch))))
  {
    ++ch;
  }

  regs->cs = (0
    | (channel_map  << 16)  // RROBIN(5):
    | (ch << 12)  // AINSEL(5):
    | (0  << 10)  // ERR_STICKY
    | (0  <<  9)  // ERR
    | (0  <<  8)  // READY
    | (1  <<  3)  // START_MANY
    | (0  <<  2)  // START_ONCE
    | ((enable_temp_sensor ? 1 : 0) <<  1)  // TS_EN
    | (1  <<  0)  // EN: 1 = enable the ADC clocks
  );
}

uint16_t THwAdc_rp::ChValue(uint8_t ach)
{
  if (dmach.Remaining())  // calling the Remaining() keeps the circular DMA running here on RP2040
  {
  }

  return ( *(databyid[ach]) << HWADC_DATA_LSHIFT );  // always left aligned
}

void THwAdc_rp::StopFreeRun()
{
  uint32_t cs = regs->cs;
  cs &= ~ADC_CS_START_MANY_BITS;
  regs->cs = cs;
  while (0 == (regs->cs & ADC_CS_READY_BITS))
  {
    // wait
  }

  dmach.Disable();
}

void THwAdc_rp::StartRecord(uint32_t achsel, uint32_t abufsize, uint16_t * adstptr)
{
  channel_map = (achsel & 0x1F);

  uint32_t ch;
  dmadatacnt = 0;
  for (ch = 0; ch < HWADC_MAX_CHANNELS; ++ch)
  {
    if (channel_map & (1 << ch))
    {
      // add this channel
      databyid[ch] = (uint16_t *)&dmadata[dmadatacnt]; // set the decode map
      ++dmadatacnt;
    }
  }

  dmaxfer.bytewidth = 2;
  dmaxfer.count = (abufsize >> 1);
  dmaxfer.dstaddr = adstptr;
  dmaxfer.flags = 0;
  dmach.StartTransfer(&dmaxfer);

  // search the first channel
  ch = 0;
  while ((ch < 5) && (0 == (channel_map & (1 << ch))))
  {
    ++ch;
  }

  regs->cs = (0
    | (channel_map  << 16)  // RROBIN(5):
    | (ch << 12)  // AINSEL(5):
    | (0  << 10)  // ERR_STICKY
    | (0  <<  9)  // ERR
    | (0  <<  8)  // READY
    | (1  <<  3)  // START_MANY
    | (0  <<  2)  // START_ONCE
    | ((enable_temp_sensor ? 1 : 0) <<  1)  // TS_EN
    | (1  <<  0)  // EN: 1 = enable the ADC clocks
  );
}

bool THwAdc_rp::RecordFinished()
{
  if (dmach.Active())
  {
    return false;
  }

  uint32_t cs = regs->cs;
  cs &= ~ADC_CS_START_MANY_BITS;
  regs->cs = cs;
  while (0 == (regs->cs & ADC_CS_READY_BITS))
  {
    // wait
  }

  while (0 == (regs->fcs & ADC_FCS_EMPTY_BITS))
  {
    if (regs->fifo)
    {
      // clear the fifo
    }
  }

  return true;
}
