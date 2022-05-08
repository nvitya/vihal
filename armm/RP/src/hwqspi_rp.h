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
 *  file:     hwqspi_rp.h
 *  brief:    RP2040 QSPI driver
 *  version:  1.00
 *  date:     2022-05-07
 *  authors:  nvitya
*/

#ifndef HWQSPI_RP_H_
#define HWQSPI_RP_H_

#include "platform.h"

#define HWQSPI_PRE_ONLY
#include "hwqspi.h"

class THwQspi_rp : public THwQspi_pre
{
public:
	unsigned char  txdmachannel = 6;
	unsigned char  rxdmachannel = 7;

	ssi_hw_t *     regs = nullptr;

	uint32_t       mlcode = 0;

	bool Init();

	virtual bool InitInterface(); // override

	int  StartReadData(unsigned acmd, unsigned address, void * dstptr, unsigned len);
	int  StartWriteData(unsigned acmd, unsigned address, void * srcptr, unsigned len);
	void Run();

	unsigned       runstate = 0;

protected:
  uint32_t       addrdata = 0;
  uint32_t       addr_mode_len = 0;
  uint32_t       remaining_words = 0;

};

#define HWQSPI_IMPL THwQspi_rp

#endif // def HWQSPI_RP_H_
