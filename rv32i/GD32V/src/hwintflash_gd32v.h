/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     hwintflash_gd32v.h
 *  brief:    Internal Flash Handling for GD32V
 *  date:     2023-05-21
 *  authors:  nvitya
*/

#ifndef HWINTFLASH_GD32V_H_
#define HWINTFLASH_GD32V_H_

#define HWINTFLASH_PRE_ONLY
#include "hwintflash.h"

class THwIntFlash_gd32v : public THwIntFlash_pre
{
public:
	bool             HwInit();

public:
	gd32v_fmc_t *    regs = nullptr;
  uint8_t          fixblock_size_shift = 0;

	//bool           StartFlashCmd(uint8_t acmd);

	void             CmdEraseBlock(); // at address
	void             CmdWritePage();

	bool             CmdFinished();

	void             Run();

public:

	void             Unlock();
  uint32_t         EraseSize(uint32_t aaddress);

protected:

	//uint16_t *       src16;
	//uint16_t *       dst16;

	void             Write32(uint32_t * adst, uint32_t avalue);

	int              BlockIdFromAddress(uint32_t aaddress);

};

#define HWINTFLASH_IMPL     THwIntFlash_gd32v

#define HWINTFLASH_OWN_RUN

#endif // def HWINTFLASH_GD32V_H_
