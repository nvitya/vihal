/*
 * hwecap.h
 *
 *  Created on: Jun 30, 2024
 *      Author: vitya
 */

#ifndef HWECAP_H_
#define HWECAP_H_

#include "stdint.h"
#include "platform.h"

// The ECAP runs with 100 MHz

typedef struct
{
	volatile uint32_t  TSCTR;            // 00
	volatile uint32_t  CTRPHS;           // 04
	volatile uint32_t  CAP1;             // 08
	volatile uint32_t  CAP2;             // 0C
	volatile uint32_t  CAP3;             // 10
	volatile uint32_t  CAP4;             // 14
	uint32_t   _pad_018[(0x28-0x18)/4];  // 18
	volatile uint16_t  ECCTL1;           // 28
	volatile uint16_t  ECCTL2;           // 2A
	volatile uint16_t  ECEINT;           // 2C
	volatile uint16_t  ECFLG;            // 2E
	volatile uint16_t  ECCLR;            // 30
	volatile uint16_t  ECFRC;            // 32
	uint32_t   _pad_034[(0x5C-0x34)/4];  // 34
	volatile uint32_t  REVID;            // 5C
//
} THwEcapRegs;

class THwEcap
{
public:
	uint32_t           devnum = 0;
	THwEcapRegs *      regs = nullptr;

	bool               Init(uint32_t adevnum);
	virtual            ~THwEcap() { }

	inline uint32_t    GetCounter() { return regs->TSCTR; } // 100 MHz clock, 10 ns per tick
	bool               InitPwm(uint32_t adevnum, uint32_t aperclocks, uint32_t adutyclocks); // 100 MHz clock, 10 ns per tick
};

#endif /* HWECAP_H_ */
