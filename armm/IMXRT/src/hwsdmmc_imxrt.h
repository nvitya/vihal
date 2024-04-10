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
 *  file:     hwsdmmc_imxrt.h
 *  brief:    IMXRT Internal SD/SDIO/MMC driver
 *  created:  2024-04-10
 *  authors:  nvitya
*/

#ifndef HWSDMMC_IMXRT_H_
#define HWSDMMC_IMXRT_H_

#define HWSDMMC_PRE_ONLY
#include "hwsdmmc.h"

class THwSdmmc_imxrt : public THwSdmmc_pre
{
public:

  USDHC_Type *       regs = nullptr;

	int                trstate = 0;
	uint8_t            dma_channel = 6;

	bool Init(int adevnum);

	void SetSpeed(uint32_t speed);
	void SetBusWidth(uint8_t abuswidth);

	void SendCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags);
	bool CmdFinished();
	bool CmdResult32Ok();

	void StartDataReadCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags, void * dataptr, uint32_t datalen);
	void StartDataWriteCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags, void * dataptr, uint32_t datalen);
	void StartDataWriteTransmit(void * dataptr, uint32_t datalen);

	//void RunTransfer(); // the internal state machine for managing multi block reads

	uint32_t GetCmdResult32();
	void GetCmdResult128(void * adataptr);

  bool TransferFinished();
  void CloseTransfer();

protected:
  uint32_t   protctl_base = 0;
  uint32_t   mixctl_base = 0;
};

#define HWSDMMC_IMPL THwSdmmc_imxrt

#endif // def HWSDMMC_IMXRT_H_
