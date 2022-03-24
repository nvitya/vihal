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
/* file:     spipsram.h
 * brief:    PSRAM Memory Handling with SPI/QSPI
 * created:  2022-03-23
 * authors:  nvitya
 * notes:
 *   The PSRAM devices allow only 8 microsecond long transmission (tCEM = Maximum CE low time)
 *   so best driven with QSPI HW, otherwise hard to guarantee this tight timing
*/

#ifndef SPIPSRAM_H_
#define SPIPSRAM_H_

#include "platform.h"
#include "hwpins.h"
#include "hwspi.h"
#include "hwqspi.h"
#include "hwerrors.h"

#define PSRAM_STATE_READMEM   1
#define PSRAM_STATE_WRITEMEM  2

struct TPsramTra
{
  bool               completed;
  uint8_t            iswrite;  // 0 = read, 1 = write
  uint8_t            _pad[2];
  int                error;
  uint32_t           address;

  uint8_t *          dataptr;
  unsigned           datalen;

  PCbClassCallback   callback = nullptr;
  void *             callbackobj = nullptr;
  void *             callbackarg = nullptr;

  TPsramTra *  next;
};

class TSpiPsram
{
public: // settings
	// Required HW resources
	THwSpi *       spi = nullptr;
	THwQspi *      qspi = nullptr;
  unsigned       pagesize = 1024;
	unsigned       t_cem_ns = 8000;  // maximum allowed CE low time [ns]

public:
  unsigned       idcode = 0;
  unsigned       idcode2 = 0;
  unsigned       bytesize = 0; // auto-detected from JEDEC ID

  unsigned       pagemask = 0x3FF;

  bool           initialized = false;

  TPsramTra *    curtra = nullptr;

	bool           Init();
  void           Run();
  void           RunTransaction();
  void           WaitFinish(TPsramTra * atra);
  void           StartReadMem(TPsramTra * atra, unsigned aaddr, void * adstptr, unsigned alen);
  void           StartWriteMem(TPsramTra * atra, unsigned aaddr, void * asrcptr, unsigned alen); // must be erased before

  bool           AddTransaction(TPsramTra * atra);  // returns false if already added

public:

  unsigned       maxchunksize = 1024;

  bool           ReadIdCode(); // done automatically in init
	void           ResetChip();

protected:
  // state machine
  int            phase = 0;

  unsigned       effective_speed = 0;
  unsigned       chunksize = 0;
  unsigned       curcmdlen = 0;

  bool           istx = false;
  uint8_t *      dataptr = nullptr;
  unsigned       datalen = 0;
  unsigned       address = 0;
  unsigned       remaining = 0;

  // smaller buffers for simple things
  unsigned char  txbuf[16];
  unsigned char  rxbuf[16];

protected:

	void CmdRead();
	void CmdWrite();

	bool CmdFinished();

};

#endif /* SPIPSRAM_H_ */
