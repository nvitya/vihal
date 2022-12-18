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
 *  file:     hwsdmmc.h
 *  brief:    Internal SD/SDIO/MMC controller
 *  created:  2018-06-07
 *  authors:  nvitya
*/

#ifndef HWSDMMC_H_PRE_
#define HWSDMMC_H_PRE_

#include "platform.h"
#include "hwdma.h"
#include "hwerrors.h"

#define SDCMD_RES_NO         0x0000
#define SDCMD_RES_48BIT      0x0001
#define SDCMD_RES_136BIT     0x0002
#define SDCMD_RES_R1B        0x0003
#define SDCMD_RES_MASK       0x0003

#define SDCMD_OPENDRAIN      0x0010

#define SD_SPECIAL_CMD_INIT       1

class THwSdmmc_pre
{
public:
	int           devnum = -1;
	bool          initialized = false;

  uint8_t       curcmd = 0;
  uint32_t      curcmdflags = 0;
  uint32_t      curcmdarg = 0;
  uint32_t      curcmdreg = 0;

	uint32_t      cmdtimeout = 0;
	bool          cmdrunning = false;
	bool          cmderror = false;
	uint32_t      lastcmdtime = 0;

	THwDmaChannel  dma; // must be initialized by the user
	THwDmaTransfer dmaxfer;
};

#endif // ndef HWSDMMC_H_PRE_

#ifndef HWSDMMC_PRE_ONLY

//-----------------------------------------------------------------------------

#ifndef HWSDMMC_H_
#define HWSDMMC_H_

#include "mcu_impl.h"

#ifndef HWSDMMC_IMPL

class THwSdmmc_noimpl : public THwSdmmc_pre
{
public: // mandatory
	bool Init()        { return false; }

	void SetSpeed(uint32_t speed) { }
	void SetBusWidth(uint8_t abuswidth) { }

	void SendSpecialCmd(uint32_t aspecialcmd) { }
	void SendCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags) { }
	bool CmdFinished() { return true; }

	void StartDataReadCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags, void * dataptr, uint32_t datalen) { }
	void StartDataWriteCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags, void * dataptr, uint32_t datalen) { }

	uint32_t GetCmdResult32() { return 0; }
	void GetCmdResult128(void * adataptr) { }

	bool TransferFinished() { return true; }
  void CloseTransfer() { }


};

#define HWSDMMC_IMPL   THwSdmmc_noimpl

#endif // ndef HWSDMMC_IMPL

//-----------------------------------------------------------------------------

class THwSdmmc : public HWSDMMC_IMPL
{

};

#endif /* HWSDMMC_H_ */

#else
  #undef HWSDMMC_PRE_ONLY
#endif
