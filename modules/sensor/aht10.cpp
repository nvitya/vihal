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
 *  file:     aht10.cpp
 *  brief:    AHT10 I2C air temperature and humidity sensor handler
 *  version:  1.00
 *  date:     2022-02-13
 *  authors:  nvitya
*/

#include <aht10.h>
#include "clockcnt.h"

bool TAht10::Init(THwI2c * ai2c, uint8_t aaddr)
{
  initialized = false;

  pi2c = ai2c;
  addr = aaddr;

  state = 0;
  measure_count = 0;
  measure_iv_clocks = SystemCoreClock * 3;  // 3s
  command_delay_clocks = 100 * (SystemCoreClock / 1000);
  last_measure = CLOCKCNT;
  tra.completed = true; // required for the first Run() cycle
  tra.error = 0;

  initialized = false; // will be initialized in the state machine
  return true;
}

void TAht10::Run()
{
  if (!pi2c)
  {
    return;
  }

  pi2c->Run();

  if (!tra.completed) // always wait the running transaction to finish
  {
    return;
  }

  if (tra.error) // primitive handling of I2C errors: re-start initialization
  {
    state = 100;  // go back to initialization start
  }

  if (0 == state) // wait a little for the initialization
  {
    if (CLOCKCNT - last_measure > SystemCoreClock / 10) // wait 100ms
    {
      state = 100; // start initialization
    }
  }
  else if (1 == state) // start measurement
  {
    buf[0] = 0xAC; // write start measurement register
    buf[1] = 0x33; // start measurement control
    buf[2] = 0x00; // nop

    pi2c->StartWrite(&tra, addr, 0, &buf[0], 3);
    state = 2;
  }
  else if (2 == state) // wait for i2c transaction end
  {
    // delay 10 ms after the command
    last_measure = CLOCKCNT;
    state = 3;
  }
  else if (3 == state) // 10 ms delay
  {
    if (CLOCKCNT - last_measure < command_delay_clocks)
    {
      return;
    }

    // read status and data
    pi2c->StartRead(&tra, addr, 0, &buf[0], 6);
    state = 4;
  }
  else if (4 == state)
  {
    // check status
    if (buf[0] & 0x80)  // bit7: 1=busy
    {
      // repeat the read out when it was busy
      state = 3;
      return;
    }

    // measurement ok, process the data
    ic_status = buf[0];
    rh_raw = (buf[1] << 12) | (buf[2] << 4) | ((buf[3] >> 4) & 0x0F);  // 20 bit
    t_raw  = ((buf[3] & 0x0F) << 16) | (buf[4] << 8) | buf[5];

    // convert to integers
    rh_percent_x100 = (10000 * (rh_raw >> 4)) >> 16;
    t_deg_x100 = ((20000 * (t_raw >> 4)) >> 16) - 5000;

    ++measure_count;

    state = 10; // delay for the next measurement
  }

  // measure delay

  else if (10 == state) // start measurement delay
  {
    last_measure = CLOCKCNT;
    state = 11;
  }
  else if (11 == state)
  {
    if (CLOCKCNT - last_measure >= measure_iv_clocks)
    {
      state = 1; // repeat the measurement
    }
  }

  // initialization

  else if (100 == state) // start initialziation
  {
    initialized = false;

    buf[0] = 0xBA; // soft reset register
    pi2c->StartWrite(&tra, addr, 0, &buf[0], 1);
    state = 101;
    last_measure = CLOCKCNT;
  }
  else if (101 == state) // wait reset complete
  {
    last_measure = CLOCKCNT;
    state = 102; // delay after the reset
  }
  else if (102 == state)  // wait for mode set
  {
    if (CLOCKCNT - last_measure < 20 * (SystemCoreClock / 1000))
    {
      return;
    }

    // set normal mode
    buf[0] = 0xE1; // initialization register
    buf[1] = 0x08; // normal mode, calibration on
    buf[2] = 0x00; // nop

    pi2c->StartWrite(&tra, addr, 0, &buf[0], 3);
    state = 103;
  }
  else if (103 == state) // wait for mode complete
  {
    initialized = true;

    last_measure = CLOCKCNT;
    state = 110; // delay after init
  }
  else if (110 == state)
  {
    if (CLOCKCNT - last_measure >= 20 * (SystemCoreClock / 1000))
    {
      state = 1; // start the normal measurement cycle
    }
  }

}
