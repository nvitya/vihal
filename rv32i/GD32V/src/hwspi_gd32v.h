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
 *  file:     hwspi_gd32v.h
 *  brief:    STM32 SPI (master only)
 *  created:  2022-07-04
 *  authors:  nvitya
*/

#ifndef HWSPI_GD32V_H_
#define HWSPI_GD32V_H_

#define HWSPI_PRE_ONLY
#include "hwspi.h"

class THwSpi_gd32v : public THwSpi_pre
{
public:
	bool Init(int adevnum);

	bool TrySendData(uint8_t adata);
	bool TryRecvData(uint8_t * dstptr);
	bool SendFinished();

	void Run();

	void DmaAssign(bool istx, THwDmaChannel * admach);

	bool DmaStartSend(THwDmaTransfer * axfer);
	bool DmaStartRecv(THwDmaTransfer * axfer);
	bool DmaSendCompleted();
	bool DmaRecvCompleted();

public:
	unsigned  					basespeed;
	gd32v_spi_t * 			regs;


	void SetCs(unsigned value);
};


#define HWSPI_IMPL THwSpi_gd32v

#endif // def HWSPI_GD32V_H_
