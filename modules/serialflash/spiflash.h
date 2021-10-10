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
// file:     spiflash.h
// brief:    SPI Flash Memory Implementation
// created:  2018-02-10
// authors:  nvitya

#ifndef SPIFLASH_H_
#define SPIFLASH_H_

#include "platform.h"
#include "hwpins.h"
#include "hwspi.h"
#include "serialflash.h"
#include "hwerrors.h"

class TSpiFlash : public TSerialFlash
{
public:
	typedef TSerialFlash super;

	// Required HW resources
	THwSpi *       spi = nullptr;

	// overrides
	virtual bool   InitInherited();
	virtual bool   ReadIdCode();
	virtual void   Run();

public:

	// smaller buffers for simple things
	unsigned char  txbuf[16];
	unsigned char  rxbuf[16];

	unsigned       curcmdlen = 0;

	void ResetChip();

	bool StartReadStatus();
	bool StartWriteEnable();

};

#endif /* SPIFLASH_H_ */
