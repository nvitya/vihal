/*
 *  file:     hwpins_am.h
 *  brief:    Broadcom Pin/Pad and GPIO configuration
 *  date:     2020-09-27
 *  authors:  nvitya
*/

#ifndef _HWPINS_AM_H
#define _HWPINS_AM_H

#include "stdint.h"

#define HWPINS_PRE_ONLY
#include "hwpins.h"

struct THwGpioRegs  // gpio register definition for the AM355x
{
	volatile uint32_t   REVISION;           // 000
	uint32_t   _pad_004[(0x010-0x004)/4];   // 004
	volatile uint32_t   SYSCONFIG;          // 010
	uint32_t   _pad_014[(0x020-0x014)/4];   // 014
	volatile uint32_t   EOI;                // 020
	volatile uint32_t   IRQSTATUS_RAW[2];   // 024
	volatile uint32_t   IRQSTATUS[2];       // 02C
	volatile uint32_t   IRQSTATUS_SET[2];   // 034
	volatile uint32_t   IRQSTATUS_CLR[2];   // 03C
	volatile uint32_t   IRQWAKEN[2];        // 044
	uint32_t   _pad_04C[(0x114-0x04C)/4];   // 04C
	volatile uint32_t   SYSSTATUS;          // 114
	uint32_t   _pad_118[(0x130-0x118)/4];   // 118
	volatile uint32_t   CTRL;               // 130
	volatile uint32_t   OE;                 // 134
	volatile uint32_t   DATAIN;             // 138
	volatile uint32_t   DATAOUT;            // 13C
	volatile uint32_t   LEVELDETECT[2];     // 140
	volatile uint32_t   RISINGDETECT;       // 148
	volatile uint32_t   FALLINGDETECT;      // 14C
	volatile uint32_t   DEBOUNCENABLE;      // 150
	volatile uint32_t   DEBOUNCINGTIME;     // 154
	uint32_t   _pad_158[(0x190-0x158)/4];   // 158
	volatile uint32_t   CLEARDATAOUT;       // 190
	volatile uint32_t   SETDATAOUT;         // 194
};

class THwPinCtrl_am : public THwPinCtrl_pre
{
public:
	// platform specific
  bool PadSetup(uint32_t aconfreg_offs, uint32_t agpio, unsigned flags);

	// platform specific
	bool PinSetup(int aportnum, int apinnum, unsigned flags);

	THwGpioRegs * GetGpioRegs(int aportnum);

	bool GpioPortEnable(int aportnum);

	void GpioSet(int aportnum, int apinnum, int value);

	inline bool GpioSetup(int aportnum, int apinnum, unsigned flags)  { return PinSetup(aportnum, apinnum, flags); }

public:
	uint32_t *       GetConfReg(uint32_t aconfreg_offs);

	THwGpioRegs *    mapped_gpio_regs[HW_GPIO_COUNT] = {0};
	uint8_t *        mm_cmodule = nullptr;  // uint8_t because the offsets are specified in bytes
};

class TGpioPin_am : public TGpioPin_common
{
public:
	THwGpioRegs *    regs = nullptr;

	bool Setup(unsigned flags);
	void Assign(int aportnum, int apinnum, bool ainvert);

	void Toggle();

	void SwitchDirection(int adirection);
};

#define HWPINCTRL_IMPL   THwPinCtrl_am
#define HWGPIOPIN_IMPL   TGpioPin_am

#endif /* HWPINS_AM_H_ */
