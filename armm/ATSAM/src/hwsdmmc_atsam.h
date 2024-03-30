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
 *  file:     hwsdmmc_atsam.cpp
 *  brief:    ATSAM internal SDCARD driver
 *  created:  2018-06-07
 *  authors:  nvitya
*/

#ifndef HWSDMMC_ATSAM_H_
#define HWSDMMC_ATSAM_H_

#define HWSDMMC_PRE_ONLY
#include "hwsdmmc.h"

#define HW_SDCARD_REGS  Hsmci

class THwSdmmc_atsam : public THwSdmmc_pre
{
public:
  uint8_t       dma_channel = 9;

public:
  Hsmci *       regs = nullptr;

	int           trstate = 0;

	bool Init();

	void SetSpeed(uint32_t speed);
	void SetBusWidth(uint8_t abuswidth);

	void SendSpecialCmd(uint32_t aspecialcmd);
	void SendCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags);
	bool CmdFinished();
  bool CmdResult32Ok();

	void StartDataReadCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags, void * dataptr, uint32_t datalen);
	//void StartBlockReadCmd();

  void StartDataWriteCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags, void * dataptr, uint32_t datalen);
  void StartDataWriteTransmit(void * dataptr, uint32_t datalen);

	//void RunTransfer(); // the internal state machine for managing multi block reads

	uint32_t GetCmdResult32();
	void GetCmdResult128(void * adataptr);

  bool TransferFinished();
  void CloseTransfer();

};

#define HWSDMMC_IMPL THwSdmmc_atsam

#endif // def HWSDMMC_ATSAM_H_
