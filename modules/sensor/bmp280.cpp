/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2022 Viktor Nagy, nvitya
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
 *  file:     bmp280.cpp
 *  brief:    BMP280 I2C air pressure and temperature sensor handler
 *  version:  1.00
 *  date:     2022-02-13
 *  authors:  nvitya
*/

#include <bmp280.h>
#include "traces.h"

bool TBmp280::Init(TI2cManager * ai2cmgr, uint8_t aaddr)
{
  pi2cmgr = ai2cmgr;
  addr = aaddr;

  state = 0;
  measure_count = 0;
  prev_measure_count = 0;
  measure_iv_clocks = 3 * SystemCoreClock;  // 3s
  last_measure = CLOCKCNT;
  tra.completed = true; // required for the first Run() cycle
  tra.errorcode = 0;

  initialized = false;  // will be initialized in the state machine
  return true;
}

// Original BOSH functions with different names only:

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
int32_t TBmp280::CalculateTemp(int32_t adc_T)
{
  int32_t var1, var2, T;
  var1 = ((((adc_T >> 3) - ((int32_t)dig.T1 << 1))) * ((int32_t)dig.T2)) >> 11;
  var2 = (((((adc_T >> 4) - ((int32_t)dig.T1)) * ((adc_T >> 4) - ((int32_t)dig.T1))) >> 12) * ((int32_t)dig.T3)) >> 14;
  t_fine = var1 + var2;
  T = (t_fine * 5 + 128) >> 8;
  return T;
}
// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
uint32_t TBmp280::CalculatePressure(int32_t adc_P)
{
  int64_t var1, var2, p;
  var1 = ((int64_t)t_fine) - 128000;

  var2 = var1 * var1 * (int64_t)dig.P6;
  var2 = var2 + ((var1*(int64_t)dig.P5) << 17);
  var2 = var2 + (((int64_t)dig.P4) << 35);
  var1 = ((var1 * var1 * (int64_t)dig.P3) >> 8) + ((var1 * (int64_t)dig.P2) << 12);
  var1 = (((((int64_t)1) << 47)+var1))*((int64_t)dig.P1) >> 33;
  if (var1 == 0)
  {
    return 0; // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)dig.P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)dig.P8) * p) >> 19;
  p = ((p + var1 + var2) >> 8) + (((int64_t)dig.P7) << 4);
  return (uint32_t)p;
}

void TBmp280::Run()  // non-blocking I2C state machine
{
  if (!pi2cmgr)
  {
    return;
  }

  pi2cmgr->Run();

  if (!tra.completed) // always wait the running transaction to finish
  {
    return;
  }

  if (tra.errorcode) // primitive handling of I2C errors: re-start initialization
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
  else if (1 == state)  // start the measurement
  {
    // burst read results always consistent reading, no status check is necessary.

    pi2cmgr->AddRead(&tra, addr, 0xF3 | I2CEX_1, &buf[3], 10);  // index corresponds to the register address
    ++state;
  }
  else if (2 == state) // process measurement results
  {
    // process the results

    ic_status = buf[3];
    ic_control = buf[4];
    ic_config = buf[5];

    p_raw = (buf[9] >> 4) | (buf[8] << 4) | (buf[7] << 12);
    t_raw = (buf[12] >> 4) | (buf[11] << 4) | (buf[10] << 12);

    t_celsius_x100 = CalculateTemp(t_raw);
    p_pascal_Q24 = CalculatePressure(p_raw);
    p_pascal = (p_pascal_Q24 >> 8);

    last_measure = CLOCKCNT;

    ++measure_count;

    state = 10; // wait for the next measurement
  }
  else if (10 == state) // delay
  {
    if (CLOCKCNT - last_measure >= measure_iv_clocks)
    {
      state = 1;  // repeat the measurement
    }
  }

  // initialization

  else if (100 == state) // start initialization
  {
    initialized = false;

    // read ID code
    pi2cmgr->AddRead(&tra, addr, 0xD0 | I2CEX_1, &ic_id, 1);
    ++state;
  }
  else if (101 == state) // read calibration data
  {
    // read calibration data / "dig" values
    pi2cmgr->AddRead(&tra, addr, 0x88 | I2CEX_1, &dig, sizeof(dig));
    ++state;
  }
  else if (102 == state) // stop first to be able to set the configuration
  {
    // stop first to be able to set the configuration
    buf[0] = 0;
    pi2cmgr->AddWrite(&tra, addr, 0xF4 | I2CEX_1, &buf[0], 1);
    ++state;
  }
  else if (103 == state)  // set F5 CONFIG
  {
    buf[0] = (0
      | (0  <<  0)  // spi3w_en:
      | (8  <<  2)  // filter(3): time constant for the IIR filter
      | (3  <<  5)  // t_sb(3): 2 = 125us, 3 = 250 us, 4 = 500 us, 5 = 1000 us
    );
    pi2cmgr->AddWrite(&tra, addr, 0xF5 | I2CEX_1, &buf[0], 1);
    ++state;
  }
  else if (104 == state)
  {
    // start, set F4 CTRL:
    buf[0] = (0
      | (3  <<  0)  // mode(2): 3 = normal mode
      | (7  <<  2)  // osrs_p(3): 3 = 4x oversampling (pressure), 4 = 8x oversampling, 7 = 16x oversampling
      | (7  <<  5)  // osrs_t(3): 3 = 4x oversampling (temp), 4 = 8x oversampling, 7 = 16x oversampling
    );
    pi2cmgr->AddWrite(&tra, addr, 0xF4 | I2CEX_1, &buf[0], 1);
    ++state;
  }
  else if (105 == state) // initialization finished
  {
    initialized = true;

    state = 1; // start the normal operation
  }
}
