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
 *  file:     hwextirq_rp.cpp
 *  brief:    RP2040 Extenal Pin Interrupt
 *  created:  2022-06-12
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwextirq.h"

bool THwExtIrq_rp::Init(int aportnum, int apinnum, unsigned flags)
{
	apinnum = (apinnum & 0x1F);

	regidx = (apinnum >> 3);  // 8 pins / register
	bitpos = ((apinnum & 7) << 2);  // 4 bits / pin

	io_irq_ctrl_hw_t * pirqctrl = &iobank0_hw->proc0_irq_ctrl;

	irqpend_reg = &pirqctrl->ints[regidx];
	irqack_reg  = &iobank0_hw->intr[regidx];
  irqen_reg   = &pirqctrl->inte[regidx];

	ack_mask = 0;
	if (flags & HWEXTIRQ_FALLING)
	{
		ack_mask |= (1 << (bitpos + 2));
	}

	if (flags & HWEXTIRQ_RISING)
	{
    ack_mask |= (1 << (bitpos + 3));
	}

	IrqAck();
	Enable();

  return true;
}

void THwExtIrq_rp::Enable()
{
  uint32_t ier = *irqen_reg;
  ier &= ~(15 << bitpos);
  ier |= ack_mask;
  *irqen_reg = ier;
}

void THwExtIrq_rp::Disable()
{
  uint32_t ier = *irqen_reg;
  ier &= ~(15 << bitpos);
  *irqen_reg = ier;
}
