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
 *  file:     hwpwm_imxrt.h
 *  brief:    IMXRT PWM controller
 *  date:     2023-12-16
 *  authors:  nvitya
*/

#ifndef SRC_HWPWM_IMXRT_H_
#define SRC_HWPWM_IMXRT_H_

#define HWPWM_PRE_ONLY
#include "hwpwm.h"

// The submodule structure is missing from the official register definitions

typedef struct
{
    __I  uint16_t CNT;                               /**< Counter Register, array offset: 0x0, array step: 0x60 */
    __IO uint16_t INIT;                              /**< Initial Count Register, array offset: 0x2, array step: 0x60 */
    __IO uint16_t CTRL2;                             /**< Control 2 Register, array offset: 0x4, array step: 0x60 */
    __IO uint16_t CTRL;                              /**< Control Register, array offset: 0x6, array step: 0x60 */
         uint8_t RESERVED_0[2];
    __IO uint16_t VAL0;                              /**< Value Register 0, array offset: 0xA, array step: 0x60 */
    __IO uint16_t FRACVAL1;                          /**< Fractional Value Register 1, array offset: 0xC, array step: 0x60 */
    __IO uint16_t VAL1;                              /**< Value Register 1, array offset: 0xE, array step: 0x60 */
    __IO uint16_t FRACVAL2;                          /**< Fractional Value Register 2, array offset: 0x10, array step: 0x60 */
    __IO uint16_t VAL2;                              /**< Value Register 2, array offset: 0x12, array step: 0x60 */
    __IO uint16_t FRACVAL3;                          /**< Fractional Value Register 3, array offset: 0x14, array step: 0x60 */
    __IO uint16_t VAL3;                              /**< Value Register 3, array offset: 0x16, array step: 0x60 */
    __IO uint16_t FRACVAL4;                          /**< Fractional Value Register 4, array offset: 0x18, array step: 0x60 */
    __IO uint16_t VAL4;                              /**< Value Register 4, array offset: 0x1A, array step: 0x60 */
    __IO uint16_t FRACVAL5;                          /**< Fractional Value Register 5, array offset: 0x1C, array step: 0x60 */
    __IO uint16_t VAL5;                              /**< Value Register 5, array offset: 0x1E, array step: 0x60 */
    __IO uint16_t FRCTRL;                            /**< Fractional Control Register, array offset: 0x20, array step: 0x60 */
    __IO uint16_t OCTRL;                             /**< Output Control Register, array offset: 0x22, array step: 0x60 */
    __IO uint16_t STS;                               /**< Status Register, array offset: 0x24, array step: 0x60 */
    __IO uint16_t INTEN;                             /**< Interrupt Enable Register, array offset: 0x26, array step: 0x60 */
    __IO uint16_t DMAEN;                             /**< DMA Enable Register, array offset: 0x28, array step: 0x60 */
    __IO uint16_t TCTRL;                             /**< Output Trigger Control Register, array offset: 0x2A, array step: 0x60 */
    __IO uint16_t DISMAP[2];                         /**< Fault Disable Mapping Register 0..Fault Disable Mapping Register 1, array offset: 0x2C, array step: index*0x60, index2*0x2 */
    __IO uint16_t DTCNT0;                            /**< Deadtime Count Register 0, array offset: 0x30, array step: 0x60 */
    __IO uint16_t DTCNT1;                            /**< Deadtime Count Register 1, array offset: 0x32, array step: 0x60 */
    __IO uint16_t CAPTCTRLA;                         /**< Capture Control A Register, array offset: 0x34, array step: 0x60 */
    __IO uint16_t CAPTCOMPA;                         /**< Capture Compare A Register, array offset: 0x36, array step: 0x60 */
    __IO uint16_t CAPTCTRLB;                         /**< Capture Control B Register, array offset: 0x38, array step: 0x60 */
    __IO uint16_t CAPTCOMPB;                         /**< Capture Compare B Register, array offset: 0x3A, array step: 0x60 */
    __IO uint16_t CAPTCTRLX;                         /**< Capture Control X Register, array offset: 0x3C, array step: 0x60 */
    __IO uint16_t CAPTCOMPX;                         /**< Capture Compare X Register, array offset: 0x3E, array step: 0x60 */
    __I  uint16_t CVAL0;                             /**< Capture Value 0 Register, array offset: 0x40, array step: 0x60 */
    __I  uint16_t CVAL0CYC;                          /**< Capture Value 0 Cycle Register, array offset: 0x42, array step: 0x60 */
    __I  uint16_t CVAL1;                             /**< Capture Value 1 Register, array offset: 0x44, array step: 0x60 */
    __I  uint16_t CVAL1CYC;                          /**< Capture Value 1 Cycle Register, array offset: 0x46, array step: 0x60 */
    __I  uint16_t CVAL2;                             /**< Capture Value 2 Register, array offset: 0x48, array step: 0x60 */
    __I  uint16_t CVAL2CYC;                          /**< Capture Value 2 Cycle Register, array offset: 0x4A, array step: 0x60 */
    __I  uint16_t CVAL3;                             /**< Capture Value 3 Register, array offset: 0x4C, array step: 0x60 */
    __I  uint16_t CVAL3CYC;                          /**< Capture Value 3 Cycle Register, array offset: 0x4E, array step: 0x60 */
    __I  uint16_t CVAL4;                             /**< Capture Value 4 Register, array offset: 0x50, array step: 0x60 */
    __I  uint16_t CVAL4CYC;                          /**< Capture Value 4 Cycle Register, array offset: 0x52, array step: 0x60 */
    __I  uint16_t CVAL5;                             /**< Capture Value 5 Register, array offset: 0x54, array step: 0x60 */
    __I  uint16_t CVAL5CYC;                          /**< Capture Value 5 Cycle Register, array offset: 0x56, array step: 0x60 */
         uint8_t RESERVED_1[8];
//
} PWM_SubModule_Type;

class THwPwmChannel_imxrt : public THwPwmChannel_pre
{
public:
	bool          Init(int adevnum, int achnum, int aoutnum);

	void          SetOnClocks(uint16_t aclocks);
	void          SetOnClocksFrac(uint32_t aclocks_x16);
	void          EnableFracMode(bool aenable);
	void          Enable();
	void          Disable();
	inline bool   Enabled() { return ((commonregs->OUTEN & outenbit) != 0); }

	void          SetFrequency(uint32_t afrequency);

	inline void   EnableDma() { regs->DMAEN = (1 << 9); } // enable the DMA for the VALx updates

public:
	PWM_SubModule_Type *    regs = nullptr;
	PWM_Type *              commonregs = nullptr;

	uint16_t						    outenbit = 0;
	volatile uint16_t *     valreg = nullptr;
	volatile uint16_t *     valregfrac = nullptr;
};

#define HWPWM_IMPL THwPwmChannel_imxrt

#endif /* SRC_HWPWM_IMXRT_H_ */
