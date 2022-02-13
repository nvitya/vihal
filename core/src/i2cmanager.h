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
 *  file:     i2cmanager.h
 *  brief:    Scheduleing I2C transactions for a shared I2C
 *  version:  1.00
 *  date:     2022-02-13
 *  authors:  nvitya
*/

#pragma once

#include "stdint.h"
#include "hwi2c.h"

struct TI2cTransaction
{
  bool               completed;
  uint8_t            iswrite;  // 0 = read, 1 = write
  uint8_t            address;
  int                errorcode;

  uint32_t           extra;
  uint8_t *          dataptr;
  unsigned           datalen;

  PCbClassCallback   callback = nullptr;
  void *             callbackobj = nullptr;
  void *             callbackarg = nullptr;

  TI2cTransaction *  next;
};


class TI2cManager
{
public:
  THwI2c *           pi2c = nullptr;
  int                state = 0;

  TI2cTransaction *  curtra = nullptr;

  bool               Init(THwI2c * ai2c);
  void               Run();

  void AddWrite(TI2cTransaction * atra, uint8_t aaddr, uint32_t aextra, void * dataptr, unsigned len);
  void AddRead(TI2cTransaction * atra, uint8_t aaddr, uint32_t aextra, void * dataptr, unsigned len);

  void WaitFinish(TI2cTransaction * atra);

protected:

  void AddTransaction(TI2cTransaction * atra);

};

