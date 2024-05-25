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
 *  file:     hwextirq_sg.h
 *  brief:    SG200x/CV1800 (Sophgo) Extenal Pin Interrupt driver for VIHAL
 *  created:  2024-05-24
 *  authors:  nvitya
*/

#ifndef _HWEXTIRQ_SG_H
#define _HWEXTIRQ_SG_H

#include "platform.h"

#define HWEXTIRQ_PRE_ONLY
#include "hwextirq.h"

class THwExtIrq_sg : public THwExtIrq_pre
{
public:
  gpio_regs_t *         gpio_regs = nullptr;

	volatile uint32_t *   irqack_reg = nullptr;
	volatile uint32_t *   irqpend_reg = nullptr;

	uint8_t               portnum = 0;
	uint8_t               pinnum = 0;
  uint32_t              bitmask = 0;

	// platform specific
	bool Init(int aportnum, int apinnum, unsigned flags);

	void Enable();
	void Disable();

	ALWAYS_INLINE void IrqBegin()   {  } // not reqired for this MCU
	ALWAYS_INLINE bool IrqPending() { return (*irqpend_reg & bitmask); }
	ALWAYS_INLINE void IrqAck()     { *irqack_reg = bitmask; }

};

#define HWEXTIRQ_IMPL   THwExtIrq_sg

#endif /* HWEXTIRQ_SG_H_ */
