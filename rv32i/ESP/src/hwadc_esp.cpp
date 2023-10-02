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
 *  file:     hwadc_esp.cpp
 *  brief:    Simple ESP32-C3 ADC Driver
 *  date:     2023-04-09
 *  authors:  nvitya
*/

#include "hwadc.h"

#include "esp_utils.h"
#include "clockcnt.h"

bool THwAdc_esp::Init(int adevnum, uint32_t achannel_map)
{
	uint32_t tmp;

	initialized = false;

	devnum = adevnum;  // not used here
	channel_map = (achannel_map & 0x1F);
	initialized = false;

#if defined(MCUSF_32C3)
  SYSTEM->PERIP_CLK_EN0 |= SYSTEM_APB_SARADC_CLK_EN;
  SYSTEM->PERIP_RST_EN0 &= ~SYSTEM_APB_SARADC_RST;
#else
  PCR->SARADC_CONF |= (PCR_SARADC_REG_CLK_EN | PCR_SARADC_CLK_EN);
  PCR->SARADC_CONF &= ~(PCR_SARADC_RST_EN | PCR_SARADC_REG_RST_EN);
#endif

	regs = SARADC;

  regs->APB_ADC_CLKM_CONF = (0
    | (4  <<  0)  // CLKM_DIV_NUM(8): integer division
    | (0  <<  8)  // CLKM_DIV_B(6): fractional numerator
    | (0  << 14)  // CLKM_DIV_A(6): fractional denumerator
    | (1  << 20)  // CLK_EN:
    | (0  << 21)  // CLK_SEL(2): 0 = APB_CLK, 1: PLL_240
  );

	regs->CTRL = (0
	  | (0  <<  0)  // START_FORCE ADC:  0: select FSM to start SAR ADC. 1: Y select software to start SAR
	  | (0  <<  1)  // START: Write 1 here to start the SAR ADC by software.  Valid only when APB_SARADC_START_FORCE = 1
	  | (1  <<  6)  // SAR_CLK_GATED:  ADC clock gate enable bit.
	  | (4  <<  7)  // SAR_CLK_DIV(8): Clock divider, should not be less than 2
	  | (0  << 15)  // SAR_PATT_LEN(3): how many pattern entries used, one less must be set
	  | (0  << 23)  // SAR_PATT_P_CLEAR: resets the pattern table pointer
	  | (0  << 27)  // XPD_SAR_FORCE(2): Force select XPD SAR
	  | (1  << 30)  // WAIT_APB_CYCLE(2): The clock sycles of waiting arbitration signal stable after SAR_DONE
	);

	regs->CTRL2 = (0
	  | (0  <<  0)  // MEAS_NUM_LIMIT
	  | (0  <<  1)  // MAX_MEAS_NUM
	  | (0  <<  9)  // SAR1_INV
	  | (0  << 10)  // SAR2_INV
	  | (0  << 12)  // TIMER_TARGET
	  | (0  << 24)  // TIMER_EN
	);

  regs->FILTER_CTRL0 = (0
    | (13 << 18)  // FILTER_CHANNEL_1(4)
    | (13 << 22)  // FILTER_CHANNEL_0(4)
    | (0  << 31)  // FILTER_RESET
  );

  regs->FILTER_CTRL1 = (0
    | (0  << 26)  // FILTER_FACTOR_1(3)
    | (0  << 29)  // FILTER_FACTOR_0(3)
  );

	// calculate the actual conversion rate
  adc_clock = 80000000 / 4;
  uint32_t sampling_clocks = sampling_time_us;

	conv_adc_clocks = 25;
	act_conv_rate = adc_clock / sampling_clocks;

	// DMA setup
	dmach.Init(dmachannel, GDMA_PERI_SEL_ADC);
	dmach.Prepare(false, (void *)&(regs->DATA_STATUS_1), 0);

	regs->DMA_CONF = (0
	  | (0  <<  0)  // EOF_NUM(15)
	  | (0  << 30)  // RESET_FSM
	  | (1  << 31)  // TRANS: 1 = use DMA
	);

	StartFreeRun(channel_map);

	initialized = true;
	return true;
}

void THwAdc_esp::StartFreeRun(uint32_t achannel_map)
{
  SetupChannels(achannel_map & 0x3F);

  dmaxfer.bytewidth = sizeof(dmadata[0]);
  dmaxfer.count = dmadatacnt;
  dmaxfer.dstaddr = &dmadata[0];
  dmaxfer.flags = DMATR_CIRCULAR;
  dmach.StartTransfer(&dmaxfer);

/*
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
*/
}

void THwAdc_esp::StopFreeRun()
{
/*
  uint32_t cs = regs->cs;
  cs &= ~ADC_CS_START_MANY_BITS;
  regs->cs = cs;
  while (0 == (regs->cs & ADC_CS_READY_BITS))
  {
    // wait
  }
*/
  dmach.Disable();
}

void THwAdc_esp::StartRecord(uint32_t achsel, uint32_t abufsize, uint16_t * adstptr)
{
  SetupChannels(achsel & 0x3F);

  dmaxfer.bytewidth = 2;
  dmaxfer.count = (abufsize >> 1);
  dmaxfer.dstaddr = adstptr;
  dmaxfer.flags = 0;
  dmach.StartTransfer(&dmaxfer);


}

bool THwAdc_esp::RecordFinished()
{
  if (dmach.Active())
  {
    return false;
  }

/*
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
*/

  return true;
}

void THwAdc_esp::SetupChannels(uint32_t achannel_map)
{
  channel_map = (achannel_map & 0x3F);

  uint32_t pat_tab[2] = {0, 0};
  uint32_t ptidx = 0;
  uint32_t ptshift = 0;

  uint32_t ch;
  dmadatacnt = 0;

  for (ch = 0; ch < HWADC_MAX_CHANNELS; ++ch)
  {
    dmadata[ch] = 0x1111 + ch; // set some markers for diagnostics
    databyid[ch] = (uint16_t *)&dmadata[ch]; // initialize with valid pointers
  }

  for (ch = 0; ch < HWADC_MAX_CHANNELS; ++ch)
  {
    if (channel_map & (1 << ch))
    {
      pat_tab[ptidx] |= ((1 << ch) << ptshift);
      if (ptshift < 18)
      {
        ptshift += 6;
      }
      else
      {
        ptshift = 0;
        ptidx = 1;
      }

      // add this channel
      databyid[ch] = (uint16_t *)&dmadata[dmadatacnt]; // set the decode map
      ++dmadatacnt;
    }
  }

  regs->SAR_PATT_TAB1 = pat_tab[0];
  regs->SAR_PATT_TAB2 = pat_tab[1];
}
