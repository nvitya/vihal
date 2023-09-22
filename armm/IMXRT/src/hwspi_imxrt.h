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
 *  file:     hwspi_imxrt.h
 *  brief:    Vihal IMXRT SPI Master Driver
 *  created:  2023-09-21
 *  authors:  nvitya
*/

#ifndef HWSPI_IMXRT_H_
#define HWSPI_IMXRT_H_

#define HWSPI_PRE_ONLY
#include "hwspi.h"

class THwSpi_imxrt : public THwSpi_pre
{
public:
	bool Init(int adevnum);
  void SetSpeed(unsigned aspeed);

  bool TrySendData(uint8_t adata);
  bool TryRecvData(uint8_t * dstptr);
  bool SendFinished();

  //void Run() { } // runs the prepared transaction
  void SetCs(unsigned avalue) { }

	void DmaAssign(bool istx, THwDmaChannel * admach);

	bool DmaStartSend(THwDmaTransfer * axfer);
	bool DmaStartRecv(THwDmaTransfer * axfer);
	bool DmaSendCompleted();
	bool DmaRecvCompleted();

public:
	HW_SPI_REGS * 			regs = nullptr;
	unsigned            tcrbase = 0;
};


#define HWSPI_IMPL THwSpi_imxrt

#endif // def HWSPI_IMXRT_H_
