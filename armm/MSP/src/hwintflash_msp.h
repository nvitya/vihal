/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2023 Viktor Nagy, nvitya
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
 *  file:     hwintflash_msp.h
 *  brief:    TI MSPM0G Internal Flash Driver
 *  date:     2023-12-13
 *  authors:  nvitya
*/

#ifndef HWINTFLASH_MSP_H_
#define HWINTFLASH_MSP_H_

#define HWINTFLASH_PRE_ONLY
#include "hwintflash.h"

#define HWINTFLASH_OWN_RUN 1

class THwIntFlash_msp : public THwIntFlash_pre
{
public:
	bool           HwInit();

public:
	FLASHCTL_GEN_Regs *  regs = nullptr;

	bool           StartFlashCmd(uint8_t acmd);

	void           CmdEraseBlock(); // at address
	void           CmdWritePage(uint32_t wordcnt);  // uses the dstaddr

  uint32_t       EraseSize(uint32_t aaddress);

  void           Run(); // own run

public:
  uint32_t       wprot_reg[2];  // CMDWEPROTA, CMDWEPROTB contents
  void           PrepareWriteProtection();
  void           ClearWriteProtection();  // must be called after every write
};

#define HWINTFLASH_IMPL THwIntFlash_msp

#endif // def HWINTFLASH_MSP_H_
