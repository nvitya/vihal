/* -----------------------------------------------------------------------------
 * This file is a part of the NVCM project: https://github.com/nvitya/nvcm
 * Copyright (c) 2018 Viktor Nagy, nvitya
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
 *  file:     hwi2cslave_msp.h
 *  brief:    MSPM0 I2C Slave
 *  created:  2025-11-14
 *  authors:  nvitya
*/

#ifndef HWI2CSLAVE_MSP_H_
#define HWI2CSLAVE_MSP_H_

#include "platform.h"

#define HWI2CSLAVE_PRE_ONLY
#include "hwi2cslave.h"

class THwI2cSlave_msp : public THwI2cSlave_pre
{
public:
	bool         call_address_rw = false;
	uint8_t      ack_value = 0;

	I2C_Regs *   regs = nullptr;

	bool InitHw(int adevnum);

	void HandleIrq();

	inline void SetNak()  { ack_value = 1; }  // will be used in the IRQ handler
};

#define HWI2CSLAVE_IMPL THwI2cSlave_msp

#endif // def HWI2CSLAVE_MSP_H_
