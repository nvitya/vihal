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
    SYSTEM->PERIP_CLK_EN0 |= SYSTEM_I2C_EXT0_CLK_EN;

    SYSTEM->PERIP_RST_EN0 |=  SYSTEM_I2C_EXT0_RST;
    SYSTEM->PERIP_RST_EN0 &= ~SYSTEM_I2C_EXT0_RST;
  }

  if (!regs)
  {
    return false;
  }

  ctr_reg_base = (0
    | (0  <<  0)  // SDA_FORCE_OUT: 1 = open-drain output
    | (0  <<  1)  // SCL_FORCE_OUT: 1 = open-drain output
    | (0  <<  2)  // SAMPLE_SCL_LEVEL
    | (1  <<  3)  // RX_FULL_ACK_LEVEL: 1 = send no ack
    | (1  <<  4)  // MS_MODE: 1 = master
    | (0  <<  5)  // TRANS_START
    | (0  <<  6)  // TX_LSB_FIRST
    | (0  <<  7)  // RX_LSB_FIRST
    | (1  <<  8)  // CLK_EN
    | (1  <<  9)  // ARBITRATION_EN
    | (0  << 10)  // FSM_RST
    | (0  << 11)  // CONF_UPGATE
    | (0  << 12)  // SLV_TX_AUTO_START_EN
    | (0  << 13)  // ADDR_10BIT_RW_CHECK_EN
    | (0  << 14)  // ADDR_BROADCASTING_EN
  );

  regs->CTR = ctr_reg_base | (1 << 10); // FSM RESET
  ConfUpgate();

  tmp = (0
    | (11 <<  0)  // RXFIFO_WM_THRHD
    | (4  <<  5)  // TXFIFO_WM_THRHD
    | (0  << 10)  // NONFIFO_EN: 1 = non-fifo mode
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

  ConfUpgate();

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

  // These calculations and some comments were taken from Espressif IDF:
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

void THwI2c_esp::ResetComd()
{
  comdidx = 0;
  wrcnt = 0;
  rdidx = 0;
}

void THwI2c_esp::AddComd(unsigned acomd)
{
  if (comdidx > 7)  return;

  regs->COMD[comdidx] = acomd;
  ++comdidx;
}

void THwI2c_esp::RunTransaction()
{
  if (0 == trastate) // prepare for the first time
  {
    istx = (0 != curtra->iswrite);
    dataptr = (uint8_t *)curtra->dataptr;
    datalen = curtra->datalen;
    remainingbytes = datalen;

    regs->INT_CLR = 0x1FFFF; // clear all interrupts

    #if 1
      regs->FIFO_CONF |=  (3 << 12);  // reset TX and RX fifo
      regs->FIFO_CONF &= ~(3 << 12);  // remove the reset (is it necessary?)
    #endif

    unsigned extracnt = ((curtra->extra >> 24) & 3);

    ResetComd();
    AddComd(COMD_OP_RSTART);

    uint32_t wrc = COMD_OP_WRITE | COMD_ACK_EXP_0 | COMD_ACK_CHECK | 1;

    if (extracnt || istx)
    {
      PushData((curtra->address << 1) | 0); // write first
    }
    else
    {
      PushData((curtra->address << 1) | 1);  // go to read immediately
    }

    if (extracnt)
    {
      uint8_t   extradata[4];
      uint32_t  eshift = ((extracnt - 1) << 3);  // reverse byte order
      unsigned  ecnt = extracnt;
      while (ecnt)
      {
        PushData(curtra->extra >> eshift);
        eshift -= 8;
        wrc += 1;  // increments the byte count
        --ecnt;
      }
    }

    AddComd(wrc); // START + EXTRA

    trastate = 10;  // wait for finish by default

    if (istx)
    {
      rxremaining = 0;
      if (remainingbytes < 32 - wrcnt)
      {
        // single transaction

        AddComd(COMD_OP_WRITE | COMD_ACK_EXP_0 | remainingbytes);
        AddComd(COMD_OP_STOP);

        while (remainingbytes)
        {
          PushData(*dataptr++);
          --remainingbytes;
        }
      }
      else // multiple transactions
      {
        AddComd(COMD_OP_WRITE | COMD_ACK_EXP_0 | (32 - wrcnt));
        AddComd(COMD_OP_END); // will be continued

        while (wrcnt < 32)
        {
          PushData(*dataptr++);
          --remainingbytes;
        }

        trastate = 1; // process multiple chunks
      }
    }
    else // RX
    {
      if (extracnt)
      {
        AddComd(COMD_OP_RSTART);
        AddComd(COMD_OP_WRITE | COMD_ACK_EXP_0 | 1);  // send the address again
        PushData((curtra->address << 1) | 1);
      }

      if (remainingbytes <= 32) // single transaction is possible
      {
        rxremaining = remainingbytes;
        if (rxremaining > 0)
        {
          if (rxremaining > 1)
          {
            AddComd(COMD_OP_READ | COMD_ACK_0 | (rxremaining - 1));  // reads with ACK
          }
          AddComd(COMD_OP_READ | COMD_ACK_1 | 1); // the last one without ACK
        }
        AddComd(COMD_OP_STOP); // finish
      }
      else // multiple transactions are necessary
      {
        rxremaining = 32;

        AddComd(COMD_OP_READ | COMD_ACK_0 | 32);  // fill the whole fifo (32 bytes)
        AddComd(COMD_OP_END);                     // wait (stretching SCL) for the next chunk

        trastate = 1; // process multiple chunks
      }
    }


    StartComdSequence();
    return; // wait for the next call

  } // if (0 == trastate)

  // check the interrupt status first

	uint32_t st = regs->INT_RAW;

  if (!curtra->error)
  {
    if (st & I2C_INT_NACK)
    {
      curtra->error = ERR_I2C_ACK;
    }
    else if (st & I2C_INT_ARBITRATION_LOST)
    {
      curtra->error = ERR_I2C_ARBLOST;
    }
    else if (st & (I2C_INT_TIME_OUT | I2C_INT_SCL_ST_TO | I2C_INT_SCL_MAINST_TO) )
    {
      curtra->error = HWERR_TIMEOUT;
    }
    else if (st & (I2C_INT_RXFIFO_OVF | I2C_INT_MST_TXFIFO_UDF | I2C_INT_TXFIFO_OVF | I2C_INT_RXFIFO_UDF))
    {
      curtra->error = ERR_I2C_OVRUDR;
    }

    if (curtra->error)
    {
      trastate = 90;  // causes transaction finish
    }
  }

  if (1 == trastate)  // multiple chunk handling
  {
    if (0 == (st & I2C_INT_END_DETECT))  // wait until the previous finishes
    {
      return;
    }

    while (rxremaining)  // get the rx bytes
    {
      *dataptr++ = PopData();
      --remainingbytes;
      --rxremaining;
    }

    // add the next chunk

    ResetComd();
    trastate = 10;  // wait for finish by default

    if (istx)
    {
      rxremaining = 0;

      while ((wrcnt < 32) && (remainingbytes > 0))
      {
        PushData(*dataptr++);
        --remainingbytes;
      }

      AddComd(COMD_OP_WRITE | COMD_ACK_EXP_0 | wrcnt);
      if (remainingbytes)
      {
        AddComd(COMD_OP_END);
        trastate = 1; // will be continued
      }
      else
      {
        AddComd(COMD_OP_STOP); // this is the last one
      }
    }
    else // RX
    {
      if (remainingbytes <= 32)  // single transaction is possible
      {
        rxremaining = remainingbytes;
        if (rxremaining > 0)
        {
          if (rxremaining > 1)
          {
            AddComd(COMD_OP_READ | COMD_ACK_0 | (rxremaining - 1));  // reads with ACK
          }
          AddComd(COMD_OP_READ | COMD_ACK_1 | 1); // the last one without ACK
        }
        AddComd(COMD_OP_STOP); // finish
      }
      else // multiple transactions are necessary
      {
        rxremaining = 32;

        AddComd(COMD_OP_READ | COMD_ACK_0 | 32);  // fill the whole fifo (32 bytes)
        AddComd(COMD_OP_END);                     // wait (stretching SCL) for the next chunk

        trastate = 1; // process multiple chunks
      }
    }

    regs->INT_CLR = I2C_INT_END_DETECT;

    StartComdSequence();
    return;
  }
  else if (10 == trastate) // wait for the transaction complete
  {
    if (0 == (st & I2C_INT_TRANS_COMPLETE))
    {
      return;
    }

    while (rxremaining)  // get the rx bytes
    {
      *dataptr++ = PopData();
      --remainingbytes;
      --rxremaining;
    }

    // no return, finish the transaction
  }

  if (90 == trastate) // if an error was detected
  {
    if (0 == (st & I2C_INT_TRANS_COMPLETE))
    {
      return;
    }

    // if an error happens like ACK error, the I2C state machine seems to stuck in a special state.
    // The next transactions won't be successful,
    // the HW frankly does not sends the first byte that programmed

    // Resetting the state machine and the fifos do not help
    // this is not documented, probably a silicon error

    // TODO: find out a better error reset
    #if 1
      Init(devnum);  // full (peripheral) reset, but it takes long time
    #else
      // this does not work

      regs->FIFO_CONF |=  (3 << 12);  // reset TX and RX fifo
      regs->FIFO_CONF &= ~(3 << 12);  // remove the reset (is it necessary?)

      regs->CTR = (1 << 10); // FSM RESET
      regs->CTR = (3 << 10); // FSM RESET + CONF UPGATE
      while (regs->CTR & (1 << 10))
      {
        // wait
      }

      for (int i = 0; i < 8; ++i)
      {
        regs->COMD[i] = 0x80000000;  // set the done bits
      }

      regs->CTR = ctr_reg_base;

      regs->INT_CLR = 0x1FFFF; // clear all interrupts
    #endif

    // continue with closing the transaction
  }

  curtra->completed = true; // transaction finished.
}

