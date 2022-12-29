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
 *  file:     bmp280.h
 *  brief:    BMP280 I2C air pressure and temperature sensor handler
 *  version:  1.00
 *  date:     2022-02-13
 *  authors:  nvitya
*/

#ifndef _BMP280_H_
#define _BMP280_H_

#include "stdint.h"
#include "hwi2c.h"

typedef struct
{
  uint16_t   T1; // 0x88/0x89
  int16_t    T2;
  int16_t    T3;

  uint16_t   P1;
  int16_t    P2;
  int16_t    P3;
  int16_t    P4;
  int16_t    P5;
  int16_t    P6;
  int16_t    P7;
  int16_t    P8;
  int16_t    P9;

  uint16_t   res;
//
} bmp280_dig_t;

class TBmp280
{
public:
  bool      initialized = false;
  uint8_t   addr = 0x77;

  THwI2c *  pi2c = nullptr;

  uint32_t  last_measure = 0;
  uint32_t  measure_iv_clocks = 0;

  int32_t   t_celsius_x100 = 0;  // temperature in 0.01 Celsius degrees
  uint32_t  p_pascal_Q24 = 0;
  uint32_t  p_pascal     = 0;

  int32_t   p_raw = 0;
  int32_t   t_raw = 0;

  uint8_t   ic_id = 0;
  uint8_t   ic_status = 0;
  uint8_t   ic_control = 0;
  uint8_t   ic_config = 0;

  int32_t   t_fine = 0;

  bmp280_dig_t  dig; // use the "dig" like in the calculation formulas

  uint32_t  measure_count = 0; // incremented after every successful measurement
  uint32_t  prev_measure_count = 0; // variable provided for the application

  bool      Init(THwI2c * ai2c, uint8_t aaddr);
  void      Run();

  int32_t   CalculateTemp(int32_t adc_T);
  uint32_t  CalculatePressure(int32_t adc_P);

protected:
  int       state = 0;

  TI2cTransaction tra;

  uint8_t   buf[16];
};

#endif
