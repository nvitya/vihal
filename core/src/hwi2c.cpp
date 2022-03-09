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
 *  file:     hwi2c.cpp
 *  brief:    Internal I2C / TWI vendor-independent implementations
 *  version:  1.00
 *  date:     2018-02-10
 *  authors:  nvitya
*/

#include "platform.h"
#include <hwi2c.h>

bool THwI2c_pre::AddTransaction(TI2cTransaction * atra)
{
  if (curtra)
  {
    // search the last tra
    TI2cTransaction * tra = curtra;
    while (tra->next)
    {
      if (tra == atra)
      {
        break;
      }
      tra = tra->next;
    }

    if (tra == atra) // already added
    {
      return false; // already added
    }

    tra->next = atra;
  }
  else
  {
    // set as first
    curtra = atra;
  }

  atra->completed = false;
  atra->error = 0;
  atra->next = nullptr;

  return true;
}

void THwI2c_pre::StartWrite(TI2cTransaction * atra, uint8_t aaddr, uint32_t aextra, void * dataptr, unsigned len)
{
  if (!AddTransaction(atra))
  {
    // application error: already added
    return;
  }

  atra->iswrite = 1;
  atra->address = aaddr;
  atra->extra = aextra;
  atra->dataptr = (uint8_t *)dataptr;
  atra->datalen = len;

}

void THwI2c_pre::StartRead(TI2cTransaction * atra, uint8_t aaddr, uint32_t aextra, void * dataptr, unsigned len)
{
  if (!AddTransaction(atra))
  {
    // application error: already added
    return;
  }

  atra->iswrite = 0;
  atra->address = aaddr;
  atra->extra = aextra;
  atra->dataptr = (uint8_t *)dataptr;
  atra->datalen = len;
}

//-----------------------------------------------------------------------------

void THwI2c::WaitFinish(TI2cTransaction * atra)
{
  while (!atra->completed)
  {
    Run();
  }
}

void THwI2c::Run()
{
  if (!curtra)
  {
    return;
  }

  if (!curtra->completed)
  {
    RunTransaction();
  }

  if (curtra->completed)
  {
    // the callback function might add the same transaction object as new
    // therefore we have to remove the transaction from the chain before we call the callback
    TI2cTransaction * ptra = curtra; // save the transaction pointer for the callback

    curtra->completed = true;
    curtra = curtra->next; // advance to the next transaction
    trastate = 0;

    // call the callback
    PCbClassCallback pcallback = PCbClassCallback(ptra->callback);
    if (pcallback)
    {
      TCbClass * obj = (TCbClass *)(ptra->callbackobj);
      (obj->*pcallback)(ptra->callbackarg);
    }
  }
}
