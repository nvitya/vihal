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
 *  file:     i2cmanager.cpp
 *  brief:    Scheduleing I2C transactions for a shared I2C
 *  version:  1.00
 *  date:     2022-02-13
 *  authors:  nvitya
*/

#include <i2cmanager.h>

bool TI2cManager::Init(THwI2c * ai2c)
{
  pi2c = ai2c;
  curtra = nullptr;
  state = 0;

  return true;
}

void TI2cManager::Run()
{
  if (!pi2c)
  {
    return;
  }

  pi2c->Run();

  if (!curtra)
  {
    return;
  }

  if (0 == state)
  {
    if (curtra->iswrite)
    {
      if (!pi2c->StartWriteData(curtra->address, curtra->extra, curtra->dataptr, curtra->datalen))
      {
        curtra->errorcode = pi2c->error;
        curtra->completed = true;
        state = 10;
      }
    }
    else
    {
      if (!pi2c->StartReadData(curtra->address, curtra->extra, curtra->dataptr, curtra->datalen))
      {
        curtra->errorcode = pi2c->error;
        curtra->completed = true;
        state = 10;
      }
    }

    if (state != 10) // go on closing the transaction
    {
      state = 1;  // wait for complete
      return;
    }
  }
  else if (1 == state) // wait for finish
  {
    if (pi2c->busy)
    {
      return;
    }

    curtra->errorcode = pi2c->error;
    state = 10;
  }

  if (10 == state)  // request completed, remove from the queue
  {
    // the callback function might add the same transaction object as new
    // therefore we have to remove the transaction from the chain before we call the callback
    TI2cTransaction * ptra = curtra; // save the transaction pointer for the callback

    curtra->completed = true;
    curtra = curtra->next; // advance to the next transaction
    state = 0;

    // call the callback
    PCbClassCallback pcallback = PCbClassCallback(ptra->callback);
    if (pcallback)
    {
      TCbClass * obj = (TCbClass *)(ptra->callbackobj);
      (obj->*pcallback)(ptra->callbackarg);
    }
  }
}

void TI2cManager::AddWrite(TI2cTransaction * atra, uint8_t aaddr, uint32_t aextra, void * dataptr, unsigned len)
{
  atra->iswrite = 1;
  atra->address = aaddr;
  atra->extra = aextra;
  atra->dataptr = (uint8_t *)dataptr;
  atra->datalen = len;

  AddTransaction(atra);
}

void TI2cManager::AddRead(TI2cTransaction * atra, uint8_t aaddr, uint32_t aextra, void * dataptr, unsigned len)
{
  atra->iswrite = 0;
  atra->address = aaddr;
  atra->extra = aextra;
  atra->dataptr = (uint8_t *)dataptr;
  atra->datalen = len;

  AddTransaction(atra);
}

void TI2cManager::AddTransaction(TI2cTransaction * atra)
{
  atra->next = nullptr;
  atra->completed = false;
  atra->errorcode = 0;

  if (curtra)
  {
    // search the last tra
    TI2cTransaction * tra = curtra;
    while (tra->next)
    {
      tra = tra->next;
    }
    tra->next = atra;
  }
  else
  {
    // set as first
    curtra = atra;
  }
}

void TI2cManager::WaitFinish(TI2cTransaction * atra)
{
  while (!atra->completed)
  {
    Run();
  }
}
