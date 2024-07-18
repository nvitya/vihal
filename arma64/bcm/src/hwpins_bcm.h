/*
 *  file:     hwpins_bcm.h
 *  brief:    Broadcom Pin/Pad and GPIO configuration
 *  date:     2024-07-18
 *  authors:  nvitya
*/

#ifndef _HWPINS_BCM_H
#define _HWPINS_BCM_H

#include "stdint.h"

#define HWPINS_PRE_ONLY
#include "hwpins.h"

struct gpio_regs_t  // gpio register definition for the BCM2711
{
	//  here the +1 are for padding only, they do not belong to the registers actually

	volatile uint32_t   FSEL[6+1];        // 00  Function select
	volatile uint32_t   SET[2+1];         // 1C  Pin output set
	volatile uint32_t   CLR[2+1];         // 28  Pin output clear
	volatile uint32_t   LEV[2+1];         // 34  Pin level
	volatile uint32_t   EDS[2+1];         // 40  Event detect status
	volatile uint32_t   REN[2+1];         // 4C  Rising Edge Detect Enable
	volatile uint32_t   FEN[2+1];         // 58  Falling Edge Detect Enable
	volatile uint32_t   HEN[2+1];         // 64  High Detect Enable
	volatile uint32_t   LEN[2+1];         // 70  Low Detect Enable
	volatile uint32_t   AREN[2+1];        // 7C  Async. Rising Edge Detect Enable
	volatile uint32_t   AFEN[2+1];        // 88  Async. Falling Edge Detect Enable
	uint32_t   _pad_8C[(0xE4-0x8C)/4];    // 8C
	volatile uint32_t   PUP_PDN_CNTRL[4]; // E4  Pull-up / Pull-down control register
};

class THwPinCtrl_bcm : public THwPinCtrl_pre
{
public:
	// platform specific
	bool PinSetup(int aportnum, int apinnum, unsigned flags);

	gpio_regs_t * GetGpioRegs();

	bool GpioPortEnable(int aportnum);

	void GpioSet(int aportnum, int apinnum, int value);

	inline bool GpioSetup(int aportnum, int apinnum, unsigned flags)  { return PinSetup(aportnum, apinnum, flags); }

public:
	gpio_regs_t *    mapped_gpio_regs = nullptr;
	uint8_t *        mm_cmodule = nullptr;  // uint8_t because the offsets are specified in bytes
};

class TGpioPin_bcm : public TGpioPin_common
{
public:
	gpio_regs_t *    regs = nullptr;

	bool Setup(unsigned flags);
	void Assign(int aportnum, int apinnum, bool ainvert);

	void Toggle();

	void SwitchDirection(int adirection);
};

#define HWPINCTRL_IMPL   THwPinCtrl_bcm
#define HWGPIOPIN_IMPL   TGpioPin_bcm

#endif /* HWPINS_BCM_H_ */
