/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     hwspi_rk.h
 *  brief:    Rockchip SPI driver for VIHAL
 *  created:  2025-09-12
 *  authors:  nvitya
*/

#ifndef HWSPI_RK_H_
#define HWSPI_RK_H_

//#define HWSPI_OWN_RUN

#define HWSPI_PRE_ONLY
#include "hwspi.h"
#include "hwdma.h"

class THwSpi_rk : public THwSpi_pre
{
public: // special parameters
  uint8_t     rx_sample_delay = 0;  // 0-3

public:
	bool Init(int adevnum);

	bool TrySendData(uint8_t adata);
	bool TryRecvData(uint8_t * dstptr);
	bool SendFinished();

	//void Run();

  void SetSpeed(unsigned aspeed);
  void SetCs(unsigned avalue);

	bool DmaStartSend(THwDmaTransfer * axfer) { return false; }
	bool DmaStartRecv(THwDmaTransfer * axfer) { return false; }

public:
	uint32_t   					basespeed = 0;
	SPI_REG * 			    regs = nullptr;
};


#define HWSPI_IMPL THwSpi_rk

#endif // def HWSPI_RK_H_
