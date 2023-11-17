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
 *  file:     hwi2c_imxrt.h
 *  brief:    IMXRT I2C Driver
 *  created:  2020-02-07
 *  authors:  nvitya
*/


#ifndef HWI2C_IMXRT_H_
#define HWI2C_IMXRT_H_

#define HWI2C_PRE_ONLY
#include "hwi2c.h"

class THwI2c_imxrt : public THwI2c_pre
{
public:
	unsigned       runstate = 0;
	LPI2C_Type *   regs = nullptr;

	uint32_t       reloadremaining = 0;

	bool Init(int adevnum);

  void RunTransaction();

  void DmaAssign(bool istx, THwDmaChannel * admach);


	int  StartReadData(uint8_t  adaddr, unsigned aextra, void * dstptr, unsigned len);
	int  StartWriteData(uint8_t adaddr, unsigned aextra, void * srcptr, unsigned len);
	void Run();

	void Reset();

	ALWAYS_INLINE bool IsTxFifoFull() { return ((regs->MFSR & 0x7) >= 4); }
};

#define HWI2C_IMPL THwI2c_imxrt

#endif // def HWI2C_IMXRT_H_
