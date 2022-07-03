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
 * file:     hwspi_esp.h
 * brief:    SPI Master for ESP32
 * created:  2022-06-30
 * authors:  nvitya
*/

#ifndef HWSPI_ESP_H_
#define HWSPI_ESP_H_

#define HWSPI_OWN_RUN  // special handling, own Run() implementation

#define HWSPI_PRE_ONLY
#include "hwspi.h"

class THwSpi_esp : public THwSpi_pre
{
public:
	bool Init(int adevnum);

	void Run();

	bool TrySendData(unsigned short adata);
	bool TryRecvData(unsigned short * dstptr);
	bool SendFinished();

public:
	unsigned  					basespeed;
	esp32_spi_t *	      regs = nullptr;

	volatile uint32_t * pwregs = nullptr;  // pointer to the 16 data registers

	uint32_t            remaining = 0;
	uint32_t            chunksize = 0;

	TSpiXferBlock *     curblock = nullptr;
	TSpiXferBlock *     lastblock = nullptr;

	void SetCs(unsigned value);
};


#define HWSPI_IMPL THwSpi_esp

#endif // def HWSPI_ESP_H_
