/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     hwdma_esp.h
 *  brief:    ESP32-C3 I2C
 *  created:  2023-03-25
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"
#include "hwi2c.h"
#include "esp_utils.h"

bool THwI2c_esp::Init(int adevnum)
{
	unsigned tmp;

	initialized = false;

	devnum = adevnum;
	regs = nullptr;

  if (0 == devnum)
  {
    regs = I2C0;
  }

  if (!regs)
  {
    return false;
  }

  uint32_t ctr = (0
    | (1  <<  0)  // SDA_FORCE_OUT: 1 = open-drain output
    | (1  <<  1)  // SCL_FORCE_OUT: 1 = open-drain output
    | (0  <<  2)  // SAMPLE_SCL_LEVEL
    | (1  <<  3)  // RX_FULL_ACK_LEVEL: 1 = send no ack
    | (1  <<  4)  // MS_MODE: 1 = master
    | (0  <<  5)  // TRANS_START
    | (0  <<  6)  // TX_LSB_FIRST
    | (0  <<  7)  // RX_LSB_FIRST
    | (1  <<  8)  // CLK_EN
    | (1  <<  9)  // ARBITRATION_EN
    | (1  << 10)  // FSM_RST
    | (0  << 11)  // CONF_UPGATE
    | (0  << 12)  // SLV_TX_AUTO_START_EN
    | (0  << 13)  // ADDR_10BIT_RW_CHECK_EN
    | (0  << 14)  // ADDR_BROADCASTING_EN
  );
  regs->CTR = ctr;

  tmp = (0
    | (11 <<  0)  // RXFIFO_WM_THRHD
    | (4  <<  5)  // TXFIFO_WM_THRHD
    | (0  << 10)  // NONFIFO_EN
    | (0  << 11)  // FIFO_ADDR_CFG_EN
    | (1  << 12)  // RX_FIFO_RST
    | (1  << 13)  // TX_FIFO_RST
    | (0  << 14)  // FIFO_PRT_EN
  );
  regs->FIFO_CONF = tmp;
  tmp &= ~(3 << 12); // remove the reset bits
  regs->FIFO_CONF = tmp;

  regs->FILTER_CFG = (0
    | (0  <<  0)  // SCL_FILTER_THRES
    | (0  <<  4)  // SDA_FILTER_THRES
    | (1  <<  8)  // SCL_FILTER_EN
    | (1  <<  9)  // SDA_FILTER_EN
  );

  regs->SCL_SP_CONF = 0; // no idle pulses, no power down

  SetSpeed(speed);

	initialized = true;

	return true;
}

void THwI2c_esp::SetSpeed(unsigned aspeed)
{
  speed = aspeed;

  unsigned periphclock = 40000000;  // fix 40 MHz
  unsigned half_cycle = ((periphclock / speed) >> 1);
  unsigned clkm_div = 0;
  while (half_cycle > 511)
  {
    clkm_div += 1;
    half_cycle = (half_cycle >> 1);
  }

  // These calculations and comments were taken from Espressif IDF:
  //   i2c_ll_cal_bus_clk()
  //   i2c_ll_set_bus_timing()

  //SCL
  uint32_t scl_low = half_cycle;
  // default, scl_wait_high < scl_high
  uint32_t scl_wait_high = (speed <= 50000) ? 0 : (half_cycle / 8); // compensate the time when freq > 50K
  uint32_t scl_high = half_cycle - scl_wait_high;

  uint32_t sda_hold = half_cycle / 4;
  // scl_wait_high < sda_sample <= scl_high
  uint32_t sda_sample = half_cycle / 2;
  uint32_t setup = half_cycle;
  uint32_t hold = half_cycle;
  //default we set the timeout value to about 10 bus cycles
  // log(20*half_cycle)/log(2) = log(half_cycle)/log(2) +  log(20)/log(2)
  uint32_t tout = (int)(sizeof(half_cycle) * 8 - __builtin_clz(5 * half_cycle)) + 2;

  regs->CLK_CONF = (0
    | (clkm_div <<  0)  // SCLK_DIV_NUM: integer part (one less)
    | (0     <<  8)  // SCLK_DIV_A(6): fractional numerator
    | (0     << 14)  // SCLK_DIV_B(6): fractional denumerator
    | (0     << 20)  // SCLK_SEL: 0 = XTAL_CLK, 1 = RC_FAST_CLK
    | (1     << 21)  // SCLK_ACTIVE
  );

  // SCL period
  regs->SCL_LOW_PERIOD  = (scl_low - 2);
  regs->SCL_HIGH_PERIOD = (0
    | ((scl_high - 3) << 0)  // SCL_HIGH_PERIOD(9)
    | (scl_wait_high  << 9)  // SCL_WAIT_HIGH_PERIOD(7)
  );
  // SDA sampling
  regs->SDA_HOLD   = (sda_hold - 1);
  regs->SDA_SAMPLE = (sda_sample - 1);
  // setup
  regs->SCL_RSTART_SETUP = (setup - 1);
  regs->SCL_STOP_SETUP   = (setup - 1);
  // hold
  regs->SCL_START_HOLD = (hold - 1);
  regs->SCL_STOP_HOLD  = (hold - 1);
  // timeout
  regs->TO = (0
    | (tout <<  0)  // TIME_OUT_VALUE
    | (1    <<  5)  // TIME_OUT_EN
  );
}

void THwI2c_esp::RunTransaction()
{
#if 0

  if (0 == trastate)
  {
    istx = (0 != curtra->iswrite);
    dataptr = (uint8_t *)curtra->dataptr;
    datalen = curtra->datalen;
    remainingbytes = datalen;

    unsigned extracnt = ((curtra->extra >> 24) & 3);
    if (extracnt)
    {
      regs->IADR = (curtra->extra & 0x00FFFFFF);
    }

    if (istx)
    {
      regs->MMR = 0
        | (curtra->address << 16)
        | (0 << 12)  // MREAD: 0 = write
        | (extracnt <<  8)  // IADRSZ: internal address bytes
      ;

      dmaused = (txdma && (datalen > 1));
      if (dmaused)
      {
        txdma->Prepare(true,  (void *)&(regs->THR), 0);

        xfer.srcaddr = dataptr;
        xfer.bytewidth = 1;
        xfer.count = remainingbytes - 1;
        xfer.flags = 0; // peripheral transfer with defaults

        dataptr += xfer.count;
        remainingbytes = 1;
      }
    }
    else
    {
      regs->MMR = 0
        | (curtra->address << 16)
        | (1 << 12)  // MREAD: 1 = read
        | (extracnt <<  8)  // IADRSZ: internal address bytes
      ;

      dmaused = (rxdma && (datalen > 2));
      if (dmaused)
      {
        rxdma->Prepare(false, (void *)&(regs->RHR), 0);

        xfer.dstaddr = dataptr;
        xfer.bytewidth = 1;
        xfer.count = remainingbytes - 2;
        xfer.flags = 0; // peripheral transfer with defaults

        dataptr += xfer.count;
        remainingbytes = 2;

        rxdma->StartTransfer(&xfer);
      }
    }

    if (regs->SR)  { }  // read the SR register to clear flags

    // Start

    unsigned tmp = (1 << 0);  // send a start condition
    if (remainingbytes <= 1)
    {
      tmp |= (1 << 0); // send a stop condition too
    }
    regs->CR = tmp;

    if (istx && dmaused)
    {
      txdma->StartTransfer(&xfer);
    }

    trastate = 1;
  }

	uint32_t sr = regs->SR; // clears flags on read !

  // check error flags
  if (!curtra->error)
  {
    if (sr & (1 << 8))
    {
      curtra->error = ERR_I2C_ACK;
    }
    else if (sr & (1 << 9))
    {
      curtra->error = ERR_I2C_ARBLOST;
    }
    else if (sr & (1 << 6))
    {
      curtra->error = ERR_I2C_OVERRUN;
    }
#if 0
    else if (sr & (1 << xxx))
    {
      curtra->error = ERR_I2C_BUS;
    }
#endif

    if (curtra->error)
    {
      regs->CR = (1 << 1);  // send stop condition
      trastate = 90;
    }
  }

  if (1 == trastate)
  {
    if (istx)
    {
      if (dmaused && txdma->Active())
      {
        return;
      }

      if (remainingbytes > 0)
      {
        if ((sr & (1 << 2)) == 0)  // TX Ready?
        {
          return;
        }

        if (remainingbytes == 1)
        {
          regs->CR = (1 << 1);  // send stop condition after the following read
        }

        regs->THR = *dataptr++;
        --remainingbytes;

        if (remainingbytes > 0)
        {
          return;
        }
      }
    }
    else
    {
      if (dmaused && rxdma->Active())
      {
        return;
      }

      if (remainingbytes > 0)
      {
        if ((sr & (1 << 1)) == 0)  // RX Ready?
        {
          return;
        }

        if (remainingbytes == 2)
        {
          regs->CR = (1 << 1);  // send stop condition after the following read
        }

        *dataptr++ = regs->RHR;
        --remainingbytes;

        if (remainingbytes > 0)
        {
          return;
        }
      }
    }
  }

	if ((sr & 1) == 0)   // transfer completed?
	{
		return;
	}

  curtra->completed = true; // transaction finished.
#endif
}

