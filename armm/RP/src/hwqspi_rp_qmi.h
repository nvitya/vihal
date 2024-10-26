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
 *  file:     hwqspi_rp_qmi.h
 *  brief:    RP2350 QSPI driver using the QMI unit
 *  created:  2024-10-13
 *  authors:  nvitya
*/

#ifndef HWQSPI_RP_QMI_H_
#define HWQSPI_RP_QMI_H_

#include "platform.h"

#if defined(qmi_hw)

#define HWQSPI_PRE_ONLY
#include "hwqspi.h"

#define QMI_TXFIFO_NOPUSH  (1 << 20)
#define QMI_TXFIFO_OE      (1 << 19)
#define QMI_TXFIFO_D16     (1 << 18)
#define QMI_TXFIFO_MLPOS   16

class THwQspi_rp : public THwQspi_pre
{
public:
	unsigned char  txdmachannel = 7;
	unsigned char  rxdmachannel = 6;

	qmi_hw_t *     regs = nullptr;

	uint32_t       mlcode = 0;

	bool Init();

	virtual bool InitInterface(); // override

	int  StartReadData(unsigned acmd, unsigned address, void * dstptr, unsigned len);
	int  StartWriteData(unsigned acmd, unsigned address, void * srcptr, unsigned len);
	void Run();

	unsigned       runstate = 0;

protected:
	uint32_t       csr_base = 0;
  uint32_t       addrdata = 0;
  uint32_t       addr_len = 0;
  uint32_t       remaining_transfers = 0;

  uint8_t        cmdbuflen = 0;
  uint32_t       cmdbuf[16];

};

#define HWQSPI_IMPL THwQspi_rp

#endif

#endif // def HWQSPI_RP_QMI_H_
