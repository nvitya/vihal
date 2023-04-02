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
 *  file:     hwdma_i2c.h
 *  brief:    ESP32-C3 I2C
 *  created:  2023-03-25
 *  authors:  nvitya
*/

#ifndef HWI2C_ESP_H_
#define HWI2C_ESP_H_

#define HWI2C_PRE_ONLY
#include "hwi2c.h"

#define COMD_DONE       (1u << 31)

#define COMD_OP_WRITE   (1 << 11)
#define COMD_OP_STOP    (2 << 11)
#define COMD_OP_READ    (3 << 11)
#define COMD_OP_END     (4 << 11)
#define COMD_OP_RSTART  (6 << 11)

#define COMD_ACK_1      (1 << 10)
#define COMD_ACK_0      (0 << 10)

#define COMD_ACK_CHECK  (1 <<  8)
#define COMD_ACK_EXP_1  ((1 <<  9) | COMD_ACK_CHECK)
#define COMD_ACK_EXP_0  ((0 <<  9) | COMD_ACK_CHECK)

#define COMD_ACK_CHECK  (1 <<  8)

#define I2C_INT_RXFIFO_WM        (1 <<  0)
#define I2C_INT_TXFIFO_WM        (1 <<  1)
#define I2C_INT_RXFIFO_OVF       (1 <<  2)
#define I2C_INT_END_DETECT       (1 <<  3)
#define I2C_INT_BYTE_TRANS_DONE  (1 <<  4)
#define I2C_INT_ARBITRATION_LOST (1 <<  5)
#define I2C_INT_MST_TXFIFO_UDF   (1 <<  6)
#define I2C_INT_TRANS_COMPLETE   (1 <<  7)
#define I2C_INT_TIME_OUT         (1 <<  8)
#define I2C_INT_TRANS_START      (1 <<  9)
#define I2C_INT_NACK             (1 << 10)
#define I2C_INT_TXFIFO_OVF       (1 << 11)
#define I2C_INT_RXFIFO_UDF       (1 << 12)
#define I2C_INT_SCL_ST_TO        (1 << 13)
#define I2C_INT_SCL_MAINST_TO    (1 << 14)
#define I2C_INT_DET_START        (1 << 15)
#define I2C_INT_SLAVE_STRETCH    (1 << 16)
#define I2C_INT_GENERAL_CALL     (1 << 17)

class THwI2c_esp : public THwI2c_pre
{
public:
  esp_i2c_regs_t * regs = nullptr;

	bool Init(int adevnum);

	void RunTransaction();

  //void DmaAssign(bool istx, THwDmaChannel * admach);

	void SetSpeed(unsigned aspeed);

protected:
	uint8_t    comdidx = 0;
	uint8_t    wrcnt = 0;
	unsigned   rxremaining = 0;

	uint32_t   ctr_reg_base = 0;

	void       ResetComd();
	void       AddComd(unsigned acomd);
	inline void PushData(uint8_t adata)
	{
	  regs->DATA = adata;
	  ++wrcnt;
	}

	inline void ConfUpgate()
	{
	  regs->CTR = ctr_reg_base | (1 << 11); // CONF_UPGATE: update the configuration to the slow clock domain
	  while (regs->CTR & (1 << 11)) // is this wait necessary ?
	  {
	    // wait
	  }
	}

  inline void StartComdSequence()
  {
    ConfUpgate();
    regs->CTR = ctr_reg_base | (1 << 5); // TRANS_START
  }

};

#define HWI2C_IMPL THwI2c_esp

#endif // def HWI2C_ESP_H_
