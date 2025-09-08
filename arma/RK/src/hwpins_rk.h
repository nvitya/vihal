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
 *  file:     hwpins_rk.h
 *  brief:    RK Pin/Pad and GPIO configuration
 *  created:  2025-07-29
 *  authors:  nvitya
*/

#ifndef _HWPINS_RK_H
#define _HWPINS_RK_H

#define HWPINS_PRE_ONLY
#include "hwpins.h"
#include "platform.h"

#define PINNUM_A0    0
#define PINNUM_A1    1
#define PINNUM_A2    2
#define PINNUM_A3    3
#define PINNUM_A4    4
#define PINNUM_A5    5
#define PINNUM_A6    6
#define PINNUM_A7    7
#define PINNUM_B0    8
#define PINNUM_B1    9
#define PINNUM_B2   10
#define PINNUM_B3   11
#define PINNUM_B4   12
#define PINNUM_B5   13
#define PINNUM_B6   14
#define PINNUM_B7   15
#define PINNUM_C0   16
#define PINNUM_C1   17
#define PINNUM_C2   18
#define PINNUM_C3   19
#define PINNUM_C4   20
#define PINNUM_C5   21
#define PINNUM_C6   22
#define PINNUM_C7   23
#define PINNUM_D0   24
#define PINNUM_D1   25
#define PINNUM_D2   26
#define PINNUM_D3   27
#define PINNUM_D4   28
#define PINNUM_D5   29
#define PINNUM_D6   30
#define PINNUM_D7   31

#define PINCFG_AF_RM_IO  (7 << PINCFG_AF_SHIFT)

typedef enum
{
    RMIO_NO_FUNCTION       = 0,

    RMIO_UART1_TX          = 0x1,
    RMIO_UART1_RX          = 0x2,
    RMIO_UART2_TX          = 0x3,
    RMIO_UART2_RX          = 0x4,
    RMIO_UART3_TX          = 0x5,
    RMIO_UART3_RX          = 0x6,
    RMIO_UART3_CTS_REN     = 0x7,
    RMIO_UART3_RTSN_DE     = 0x8,
    RMIO_UART4_TX          = 0x9,
    RMIO_UART4_RX          = 0xA,
    RMIO_UART4_CTS_REN     = 0xB,
    RMIO_UART4_RTSN_DE     = 0xC,
    RMIO_MIPITE            = 0xD,
    RMIO_CLK32_IOUT        = 0xE,
    RMIO_I2C0_SCL          = 0xF,
    RMIO_I2C0_SDA          = 0x10,
    RMIO_I2C1_SCL          = 0x11,
    RMIO_I2C1_SDA          = 0x12,
    RMIO_I2C2_SCL          = 0x13,
    RMIO_I2C2_SDA          = 0x14,
    RMIO_PDMCLK0           = 0x15,
    RMIO_PDM_SDI0          = 0x16,
    RMIO_PDM_SDI1          = 0x17,
    RMIO_PDM_SDI2          = 0x18,
    RMIO_PDM_SDI3          = 0x19,
    RMIO_CAN1_TX           = 0x1A,
    RMIO_CAN1_RX           = 0x1B,
    RMIO_CAN0_TX           = 0x1C,
    RMIO_CAN0_RX           = 0x1D,
    RMIO_PWM0_CH0          = 0x1E,
    RMIO_PWM0_CH1          = 0x1F,
    RMIO_PWM0_CH2          = 0x20,
    RMIO_PWM0_CH3          = 0x21,
    RMIO_PWM1_CH0          = 0x22,
    RMIO_PWM1_CH1          = 0x23,
    RMIO_PWM1_CH2          = 0x24,
    RMIO_PWM1_CH3          = 0x25,
    RMIO_PWM1_CH4          = 0x26,
    RMIO_PWM1_CH5          = 0x27,
    RMIO_PWM1_CH6          = 0x28,
    RMIO_PWM1_CH7          = 0x29,
    RMIO_TOUCH_KEY_DRIVE   = 0x2A,
    RMIO_TOUCH_KEY0        = 0x2B,
    RMIO_TOUCH_KEY1        = 0x2C,
    RMIO_TOUCH_KEY2        = 0x2D,
    RMIO_TOUCH_KEY3        = 0x2E,
    RMIO_TOUCH_KEY4        = 0x2F,
    RMIO_TOUCH_KEY5        = 0x30,
    RMIO_TOUCH_KEY6        = 0x31,
    RMIO_TOUCH_KEY7        = 0x32,
    RMIO_SAI0_MCLK         = 0x33,
    RMIO_SAI0_SCLK         = 0x34,
    RMIO_SAI0_LRCK         = 0x35,
    RMIO_SAI0_SDI0         = 0x36,
    RMIO_SAI0_SDI1         = 0x37,
    RMIO_SAI0_SDI2         = 0x38,
    RMIO_SAI0_SDI3         = 0x39,
    RMIO_SAI0_SDO          = 0x3A,
    RMIO_SAI1_MCLK         = 0x3B,
    RMIO_SAI1_SCLK         = 0x3C,
    RMIO_SAI1_LRCK         = 0x3D,
    RMIO_SAI1_SDI          = 0x3E,
    RMIO_SAI1_SDO0         = 0x3F,
    RMIO_SAI1_SDO1         = 0x40,
    RMIO_SAI1_SDO2         = 0x41,
    RMIO_SAI1_SDO3         = 0x42,
    RMIO_SPI0_CLK          = 0x43,
    RMIO_SPI0_MOSI         = 0x44,
    RMIO_SPI0_MISO         = 0x45,
    RMIO_SPI0_CSN0         = 0x46,
    RMIO_SPI0_CSN1         = 0x47,
    RMIO_SPI1_CLK          = 0x48,
    RMIO_SPI1_MOSI         = 0x49,
    RMIO_SPI1_MISO         = 0x4A,
    RMIO_SPI1_CSN0         = 0x4B,
    RMIO_SPI1_CSN1         = 0x4C,
    RMIO_WDT_TSADC_SHUT    = 0x4D,
    RMIO_PMU_SLEEP         = 0x4E,
    RMIO_CORE_POWER_OFF    = 0x4F,
    RMIO_SPDIF_TX          = 0x50,
    RMIO_SPDIF_RX          = 0x51,
    RMIO_BIP_CNTR_A0       = 0x52,
    RMIO_BIP_CNTR_A1       = 0x53,
    RMIO_BIP_CNTR_A2       = 0x54,
    RMIO_BIP_CNTR_A3       = 0x55,
    RMIO_BIP_CNTR_A4       = 0x56,
    RMIO_BIP_CNTR_A5       = 0x57,
    RMIO_BIP_CNTR_B0       = 0x58,
    RMIO_BIP_CNTR_B1       = 0x59,
    RMIO_BIP_CNTR_B2       = 0x5A,
    RMIO_BIP_CNTR_B3       = 0x5B,
    RMIO_BIP_CNTR_B4       = 0x5C,
    RMIO_BIP_CNTR_B5       = 0x5D,
    RMIO_PDMCLK1           = 0x5E,
    RMIO_RMII0_PTP         = 0x5F,
    RMIO_RMII0_MCGPST_TRIG = 0x60,
    RMIO_RMII1_PTP         = 0x61,
    RMIO_RMII1_MCGPST_TRI  = 0x62,
} eRMIO_Name;


// The register accesses are very slow on this platform!

class THwPinCtrl_rk : public THwPinCtrl_pre
{
public: // platform specific

	bool PinSetupRmio(int aportnum, int apinnum, int rmio_func, unsigned flags);
	bool RmioSetup(int rmionum, int funcnum);
	int  RmioByPortPin(int aportnum, int apinnum);

public: //
	bool PinSetup(int aportnum, int apinnum, unsigned flags);  // this does only GPIO setup

	void GpioSet(int aportnum, int apinnum, int value);
	void GpioDir(int aportnum, int apinnum, int value);

	inline bool GpioSetup(int aportnum, int apinnum, unsigned flags)  { return PinSetup(aportnum, apinnum, flags); }

	GPIO_REG *  GetGpioRegs(int aportnum);

public:
	GPIO_REG *           mapped_gpio_regs[5] = {0};
	uint8_t *            mapped_ioc_regs[5] = {0};     // using uint8_t for using byte offsets from the TRM
	uint8_t *            mapped_rm_io = nullptr;  // using uint8_t for using byte offsets from the TRM

protected:
	uint8_t *            GetIocRegs(int aportnum);
};

class TGpioPin_rk : public TGpioPin_pre
{
public:
	GPIO_REG *             regs = nullptr;

	uint32_t *             setbitptr = nullptr;
	uint32_t *             pindirptr = nullptr;
	uint32_t *             clrbitptr = nullptr;
	volatile uint32_t *    getbitptr = nullptr;
	volatile uint32_t *    getoutbitptr = nullptr;
	uint32_t               setbitvalue = 0;
	uint32_t               clrbitvalue = 0;
	uint32_t               getbitshift = 0;
	uint32_t               getbitshift16 = 0;


	void InitDummy(); // for unassigned ports
	void Assign(int aportnum, int apinnum, bool ainvert);

	inline void Set1()                 { *setbitptr = setbitvalue; }
	inline void Set0()                 { *clrbitptr = clrbitvalue; }
	inline void SetTo(unsigned value)  { if (value & 1) Set1(); else Set0(); }

	inline unsigned char Value()       { return ((*getbitptr >> getbitshift) & 1); }
	inline unsigned char OutValue()    { return ((*getoutbitptr >> getbitshift16) & 1); }

	void Toggle();
	void SwitchDirection(int adirection);

};

#define HWPINCTRL_IMPL   THwPinCtrl_rk
#define HWGPIOPORT_IMPL  TGpioPort_rk
#define HWGPIOPIN_IMPL   TGpioPin_rk

//extern GPIO_REG * rk_gpio_regs(int aportnum);


#endif
