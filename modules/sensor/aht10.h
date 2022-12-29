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
 *  file:     aht10.h
 *  brief:    AHT10 I2C air temperature and humidity sensor handler
 *  version:  1.00
 *  date:     2022-02-13
 *  authors:  nvitya
*/

#ifndef _AHT10_H_
#define _AHT10_H_

#include "stdint.h"
#include "hwi2c.h"

class TAht10
{
public:
  bool      initialized = false;
  uint8_t   addr = 0x77;
  THwI2c *  pi2c = nullptr;

  int       t_deg_x100 = 0;
  uint32_t  rh_percent_x100 = 0;

  uint32_t  rh_raw = 0;  // 20 bit scaling: 0 = 0%,  0x100000 = 100 %
  uint32_t  t_raw = 0;   // 20 bit scaling: 0 = -50 C, 0x100000 = 150 C

  uint8_t   ic_status = 0;

  uint32_t  measure_count = 0; // incremented after every successful measurement
  uint32_t  prev_measure_count = 0; // variable provided for the application

  bool      Init(THwI2c * ai2c, uint8_t aaddr);
  void      Run();

protected:
  int       state = 0;

  uint32_t  last_measure = 0;
  uint32_t  measure_iv_clocks = 0;
  uint32_t  command_delay_clocks = 0;

  uint8_t   buf[8];  // internaly used

  TI2cTransaction tra;
};

#endif
