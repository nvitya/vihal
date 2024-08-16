/* -----------------------------------------------------------------------------
 * This file is a part of the NVCM project: https://github.com/nvitya/nvcm
 * Copyright (c) 2018 Viktor Nagy, nvitya
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
 *  file:     hwqspi.h
 *  brief:    Internal QSPI/SPIFI vendor-independent definitions
 *  date:     2018-02-10
 *  authors:  nvitya
 *  notes:
 *    Interface coding changed in 2024-08-14 to support two-byte instructions
*/

#ifndef _HWQSPI_H_PRE_
#define _HWQSPI_H_PRE_

#include "platform.h"
#include "hwpins.h"
#include "hwdma.h"
#include "hwerrors.h"

// line format S = single line, M = multi line
#define QSPICM_SSS          0x00000000  // S command, S address + dummy, S data (default)
#define QSPICM_SSM          0x00040000  // S command, S address + dummy, M data
#define QSPICM_SMM          0x00060000  // S command, M address + dummy, M data
#define QSPICM_MMM          0x00070000  // M command, M address + dummy, M data
#define QSPICM_CMD_SMASK    0x0000FFFF  // command byte mask
#define QSPICM_LN_MASK      0x00070000
#define QSPICM_LN_POS               16
#define QSPICM_LN_POS               16
#define QSPICM_LN_DATA_POS          18
#define QSPICM_LN_ADDR_POS          17
#define QSPICM_LN_CMD_POS           16
#define QSPICM_2B           0x00080000  // force 2-byte command (otherwise on second byte non-null)

// Addres byte count
#define QSPICM_ADDR         0x00700000  // send address with the default size
#define QSPICM_ADDR0        0x00000000  // do not send address (default)
#define QSPICM_ADDR1        0x00100000
#define QSPICM_ADDR2        0x00200000
#define QSPICM_ADDR3        0x00300000
#define QSPICM_ADDR4        0x00400000
#define QSPICM_ADDR_MASK    0x00700000
#define QSPICM_ADDR_SMASK         0x07
#define QSPICM_ADDR_POS             20

// dummy cycle count (2 x [0-30])
#define QSPICM_DUMMYC       0x1F000000  // send dummy cycles with the default size
#define QSPICM_DUMMYC0      0x00000000  // do not send dummy (default)
#define QSPICM_DUMMYC2      0x01000000  // 2 dummy cycles
#define QSPICM_DUMMYC4      0x02000000  // 4 dummy cycles
#define QSPICM_DUMMYC6      0x03000000  // 6 dummy cycles
#define QSPICM_DUMMYC8      0x04000000  // 8 dummy cycles
#define QSPICM_DUMMYC10     0x05000000  // 10 dummy cycles
#define QSPICM_DUMMYC12     0x06000000  // 12 dummy cycles
#define QSPICM_DUMMYC14     0x07000000  // 14 dummy cycles
#define QSPICM_DUMMYC16     0x08000000  // 16 dummy cycles
#define QSPICM_DUMMYC18     0x09000000  // 18 dummy cycles
#define QSPICM_DUMMYC20     0x0A000000  // 20 dummy cycles
#define QSPICM_DUMMYC22     0x0B000000  // 22 dummy cycles
#define QSPICM_DUMMYC24     0x0C000000  // 24 dummy cycles
#define QSPICM_DUMMYC26     0x0D000000  // 26 dummy cycles
#define QSPICM_DUMMYC28     0x0E000000  // 28 dummy cycles
#define QSPICM_DUMMYC30     0x0F000000  // 30 dummy cycles
#define QSPICM_DUMMYC_MASK  0x1F000000
#define QSPICM_DUMMYC_SMASK       0x1F
#define QSPICM_DUMMYC_POS           24

// mode bits
#define QSPICM_MODE         0x80000000  // send mode bits with the default size
#define QSPICM_MODE_POS             31

class THwQspi_pre
{
public:	// settings
	bool 					 initialized = false;

	bool           idleclk_high = true;
	bool           datasample_late = false;

	uint8_t        addrlen = 3;           // default address byte count
	uint8_t        modelen = 1;           // mode bytes count
	uint8_t        dummycycles = 8;       // default dummy cycles = 1 byte
	uint8_t        multi_line_count = 2;  // 4 = quad, 2 = dual mode, 8 = octal, 1 = disable multi line mode

	uint32_t       speed = 8000000;  // default speed = 8MHz

  uint32_t       modedata = 0;
  uint32_t       dummydata = 0;

public: // Required HW resources
  THwDmaChannel  txdma;
  THwDmaChannel  rxdma;

  THwDmaTransfer xfer;

public: // run state
	bool           istx = false;
	bool           dmaused = false;
	uint8_t *      dataptr = nullptr;
	uint32_t       datalen = 0;
	uint32_t       remainingbytes = 0;

	bool           busy = false;

public:
	virtual        ~THwQspi_pre() { } // virtual destructor to avoid compiler warning

	virtual bool   InitInterface() { return true; }
};

#endif // ndef HWQSPI_H_PRE_

#ifndef HWQSPI_PRE_ONLY

//-----------------------------------------------------------------------------

#ifndef HWQSPI_H_
#define HWQSPI_H_

#include "mcu_impl.h"

#ifndef HWQSPI_IMPL

//#warning "HWQSPI is not implemented!"

class THwQspi_noimpl : public THwQspi_pre
{
public: // mandatory
	bool Init()                   { return false; }

	int  StartReadData(unsigned acmd, unsigned address, void * dstptr, unsigned len)   { return HWERR_NOTIMPL; }
	int  StartWriteData(unsigned acmd, unsigned address, void * srcptr, unsigned len)  { return HWERR_NOTIMPL; }
	void SetMemMappedMode() { }
	void Run()  { }
};

#define HWQSPI_IMPL   THwQspi_noimpl

#endif // ndef HWQSPI_IMPL

//-----------------------------------------------------------------------------

class THwQspi : public HWQSPI_IMPL
{
public:
	bool Finished();
	int WaitFinish();
};

#endif // HWQSPI_H_

#else
  #undef HWQSPI_PRE_ONLY
#endif
