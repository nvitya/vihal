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
 *  file:     hwi2c_rp.h
 *  brief:    R2040 I2C Master Driver
 *  created:  2022-06-10
 *  authors:  nvitya
*/

#ifndef HWI2C_RP_H_
#define HWI2C_RP_H_

#define HWI2C_PRE_ONLY
#include "hwi2c.h"

#include "hwdma.h"

// my macro definition with somewhat better names
#define HWI2C_CMD_RESTART   (1 << 10)
#define HWI2C_CMD_STOP      (1 <<  9)
#define HWI2C_CMD_RXDATA    (1 <<  8)

#define HWI2C_TXDMA_BUF_SIZE   128

class THwI2c_rp : public THwI2c_pre
{
public:
	i2c_hw_t *    regs = nullptr;

	bool Init(int adevnum);

	void RunTransaction();

  void DmaAssign(bool istx, THwDmaChannel * admach);

protected:
  bool             rxcmd_dmaused = false;
  uint32_t         rxcmd_remaining = 0;
  uint32_t         rxcmd_data = HWI2C_CMD_RXDATA;
  THwDmaTransfer   cmdxfer;

  uint16_t         txdmabuf[HWI2C_TXDMA_BUF_SIZE]; // collect the TXDMA commands here, because 2 byte writes are required

  void             FillAndStartTxDma();

};

#define HWI2C_IMPL THwI2c_rp

#endif // def HWI2C_RP_H_
