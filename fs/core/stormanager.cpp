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
 *  file:     stormanager.cpp
 *  brief:    Storage Manager, transaction manager for non-volatile storage devices (SDCARD, Serial FLASH)
 *  date:     2024-04-17
 *  authors:  nvitya
*/

#include "string.h"
#include "stormanager.h"
#include "traces.h"

TGlobalStorMan  g_storman;

void TStorTrans::ExecCallBack()
{
  PCbClassCallback pcallback = PCbClassCallback(callback);
  if (pcallback)
  {
    TCbClass * obj = (TCbClass *)(callbackobj);
    (obj->*pcallback)(callbackarg);
  }
  else if (callbackobj)
  {
    PStorCbFunc cbfunc = PStorCbFunc(callbackobj);
    (* cbfunc)(callbackarg);
  }
}

//--------------------------------------------------------------------------

void TStorManager::Init(uint8_t * apbuf, unsigned abufsize)
{
  pbuf = apbuf;
  bufsize = abufsize;

	firsttra = nullptr;
	lasttra = nullptr;
	state = 0;
}

void TStorManager::AddTransaction(TStorTrans * atra)
{
	atra->next = nullptr;
	atra->completed = false;
	atra->errorcode = 0;

	if (lasttra)
	{
		lasttra->next = atra;
		lasttra = atra;
	}
	else
	{
		// set as first
		firsttra = atra;
		lasttra = atra;
	}
}

void TStorManager::AddTransaction(TStorTrans * atra, TStorTransType atype, uint64_t aaddr,
		                             void * adataptr, uint32_t adatalen)
{
	atra->trtype = atype;
	atra->address = aaddr;
	atra->dataptr = (uint8_t *)adataptr;
	atra->datalen = adatalen;

	AddTransaction(atra);
}

void TStorManager::WaitTransaction(TStorTrans * atra)
{
	while (!atra->completed)
	{
		Run();
	}
}

void TStorManager::FinishCurTra()
{
  // request completed
  // the callback function might add the same transaction object as new
  // therefore we have to remove the transaction from the chain before we call the callback

  curtra->completed = true;
  firsttra = firsttra->next; // advance to the next transaction
  if (!firsttra)  lasttra = nullptr;

  state = 0;

  curtra->ExecCallBack();
}

void TStorManager::FinishCurTraError(int aerror)
{
  curtra->errorcode = aerror;
  FinishCurTra();
}

//--------------------------------------------------------------------------

int TGlobalStorMan::AddManager(TStorManager * amanager)
{
  if (smcount >= GSTORMAN_MAX_MANAGERS)
  {
    return -1;
  }

  int r = smcount;
  smlist[smcount] = amanager;
  ++smcount;
  return r;
}

void TGlobalStorMan::Run()
{
  for (unsigned n = 0; n < smcount; ++n)
  {
    smlist[n]->Run();
  }
}

void TGlobalStorMan::AddTransaction(uint8_t devid, TStorTrans * atra)
{
  if (devid >= smcount)
  {
    atra->completed = true;
    atra->errorcode = HWERR_INVALID;
    atra->ExecCallBack();
    return;
  }

  TStorManager * pman = smlist[devid];
  pman->AddTransaction(atra);
}

void TGlobalStorMan::AddTransaction(uint8_t devid, TStorTrans * atra,
    TStorTransType atype, uint64_t aaddr, void * adataptr, uint32_t adatalen)
{
  if (devid >= smcount)
  {
    atra->completed = true;
    atra->errorcode = HWERR_INVALID;
    atra->ExecCallBack();
    return;
  }

  TStorManager * pman = smlist[devid];
  pman->AddTransaction(atra, atype, aaddr, adataptr, adatalen);
}

void TGlobalStorMan::WaitTransaction(TStorTrans * atra)
{
  while (!atra->completed)
  {
    Run();
  }
}
