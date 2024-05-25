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
 *  file:     hwextirq_sg.cpp
 *  brief:    SG200x/CV1800 (Sophgo) Extenal Pin Interrupt driver for VIHAL
 *  created:  2024-05-24
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"
#include "hwextirq.h"

bool THwExtIrq_sg::Init(int aportnum, int apinnum, unsigned flags)
{
  portnum = aportnum;
  pinnum = apinnum & 0x1F;

  gpio_regs = sg_gpio_regs(aportnum);
  if (!gpio_regs)
  {
    return false;
  }

	bitmask = (1 << pinnum);

	irqpend_reg  = &gpio_regs->RAW_INTSTATUS;
	irqack_reg  = &gpio_regs->PORTA_EOI;

  gpio_regs->INTTYPE_LEVEL |=  bitmask;  // select EDGE interrupts
	if (flags & HWEXTIRQ_FALLING)
	{
	  gpio_regs->INT_POLARITY  &= ~bitmask;  // select falling edge
	}
	else
	{
    gpio_regs->INT_POLARITY  |=  bitmask;  // select rising edge
	}
  gpio_regs->INTMASK &= ~bitmask;

	IrqAck();
	Enable();

  return true;
}

void THwExtIrq_sg::Enable()
{
  gpio_regs->INTEN |= bitmask;
}

void THwExtIrq_sg::Disable()
{
  gpio_regs->INTEN &= ~bitmask;
}
