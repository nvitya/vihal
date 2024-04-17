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
 *  file:     stormanager.h
 *  brief:    Storage Manager, transaction manager for non-volatile storage devices (SDCARD, Serial FLASH)
 *  date:     2024-04-17
 *  authors:  nvitya
*/

#ifndef STORMANAGER_H_
#define STORMANAGER_H_

#include "platform.h"
#include "hwerrors.h"

#define ESTOR_NOTIMPL    1
#define ESTOR_INV_SIZE   2

typedef enum
{
	STRA_READ,
	STRA_WRITE,
	STRA_ERASE
//
} TStorTransType;

typedef void (* PStorCbFunc)(void * arg);

struct TStorTrans
{
	bool              completed;
	TStorTransType    trtype;

	uint8_t *         dataptr;  // usually DMA target, so better be 8-byte aligned (IXMRT QSPI requirement)
	unsigned          datalen;
	uint64_t          address;

	int               errorcode;

  PCbClassCallback  callback = nullptr;      // class method callback
  void *            callbackobj = nullptr;   // object for the class callback, or simple function when callback=nullptr
  void *            callbackarg = nullptr;

  TStorTrans *      next = nullptr;  // used internally

  void              ExecCallBack();
//
};

class TStorManager
{
public:
	unsigned          erase_unit = 4096;
	unsigned          smallest_block = 1;  // must be power of two !

	virtual           ~TStorManager() { }

	void              Init(uint8_t * apbuf, unsigned abufsize);

	void              AddTransaction(TStorTrans * atra);
	void              AddTransaction(TStorTrans * atra, TStorTransType atype, uint64_t aaddr,
			                             void * adataptr, uint32_t adatalen);
	void              WaitTransaction(TStorTrans * atra); // blocking, only for initializations!

public:  // virtual functions, they must be implemented by the slave

	virtual void      Run()      {}
	virtual uint64_t  ByteSize() { return 0; }  // 0 when not available

protected:

  int               state = 0;
  uint8_t *         pbuf = nullptr;
  unsigned          bufsize = 0;
  TStorTrans *      firsttra = nullptr;
  TStorTrans *      lasttra = nullptr;
  TStorTrans *      curtra = nullptr;
	unsigned          trastarttime = 0;

  void              FinishCurTra();
  void              FinishCurTraError(int aerror);

};

#define GSTORMAN_MAX_MANAGERS  4

class TGlobalStorMan
{
public:
  unsigned          smcount = 0;
  TStorManager *    smlist[GSTORMAN_MAX_MANAGERS];

  int               AddManager(TStorManager * amanager);  // returns the storage manager index / id
  void              Run();

  void              AddTransaction(uint8_t devid, TStorTrans * atra);
  void              AddTransaction(uint8_t devid, TStorTrans * atra, TStorTransType atype, uint64_t aaddr,
                                   void * adataptr, uint32_t adatalen);
  void              WaitTransaction(TStorTrans * atra); // blocking, use only for initializations

};

extern TGlobalStorMan  g_storman;

#endif /* STORMANAGER_H_ */
