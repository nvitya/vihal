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
// file:     spipsram.h
// brief:    PSRAM Memory Handling with SPI/QSPI
// created:  2022-03-23
// authors:  nvitya

#ifndef SPIPSRAM_H_
#define SPIPSRAM_H_

#include "platform.h"
#include "hwpins.h"
#include "hwspi.h"
#include "hwqspi.h"
#include "hwerrors.h"

#define PSRAM_STATE_READMEM   1
#define PSRAM_STATE_WRITEMEM  2
#define PSRAM_STATE_ERASE     3

//#define PSRAM_MAX_CHUNK  (16*1024*1024)

class TSpiPsram
{
public: // settings
	// Required HW resources
	THwSpi *       spi = nullptr;
	THwQspi *      qspi = nullptr;

public:
  unsigned       idcode = 0;
  unsigned       bytesize = 0; // auto-detected from JEDEC ID

  unsigned       pagesize = 1024;
  unsigned       pagemask = 0x3FF;

  bool           initialized = false;
  bool           completed = true;
  int            errorcode = 0;

	bool           Init();
	void           Run();
  void           WaitForComplete();

  bool           StartReadMem(unsigned aaddr, void * adstptr, unsigned alen);
  bool           StartWriteMem(unsigned aaddr, void * asrcptr, unsigned alen); // must be erased before

public:

  bool           ReadIdCode(); // done automatically in init
	void           ResetChip();

protected:
  // state machine
  int            state = 0;
  int            phase = 0;

  unsigned       curcmdlen = 0;
  unsigned       chunksize = 0;
  unsigned       maxchunksize = 1024;

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
	bool CmdReadStatus();

	bool CmdFinished();

};

#endif /* SPIPSRAM_H_ */
