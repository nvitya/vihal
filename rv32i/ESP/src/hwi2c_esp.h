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
 *  file:     hwdma_i2c.h
 *  brief:    ESP32-C3 I2C
 *  created:  2023-03-25
 *  authors:  nvitya
*/

#ifndef HWI2C_ESP_H_
#define HWI2C_ESP_H_

#define HWI2C_PRE_ONLY
#include "hwi2c.h"

class THwI2c_esp : public THwI2c_pre
{
public:
  esp_i2c_regs_t * regs = nullptr;

	bool Init(int adevnum);

	void RunTransaction();

  //void DmaAssign(bool istx, THwDmaChannel * admach);

	void SetSpeed(unsigned aspeed);

};

#define HWI2C_IMPL THwI2c_esp

#endif // def HWI2C_ESP_H_
