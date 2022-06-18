/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     hwextirq_gd32v.h
 *  brief:    GD32V Extenal Pin Interrupt
 *  date:     2022-06-16
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwextirq_gd32v.h"

bool THwExtIrq_gd32v::Init(int aportnum, int apinnum, unsigned flags)
{
	apinnum = (apinnum & 0x1F);

	regs = EXTI;

	irqpend_reg = &regs->PD;
	irqack_reg  = &regs->PD;
	pin_mask = (1 << apinnum);

	// select the EXTI line for this port
	RCU->APB2EN |= RCU_APB2EN_AFEN;
  int cridx  = (apinnum >> 2);
  int rshift = ((apinnum & 3) << 2);
  volatile uint32_t * pcfgreg = &AFIO->EXTISS[cridx];
  uint32_t tmp = *pcfgreg;
  tmp &= ~(15 << rshift);
  tmp |= ((aportnum & 15) << rshift);
  *pcfgreg = tmp;

	if (flags & HWEXTIRQ_FALLING)
	{
		regs->FTEN |= pin_mask;
	}
	else
	{
    regs->FTEN &= ~pin_mask;
	}

	if (flags & HWEXTIRQ_RISING)
  {
    regs->RTEN |= pin_mask;
  }
  else
  {
    regs->RTEN &= ~pin_mask;
  }

	IrqAck();
	Enable();

  return true;
}

void THwExtIrq_gd32v::Enable()
{
  regs->EVEN  |= pin_mask;
  regs->INTEN |= pin_mask;
}

void THwExtIrq_gd32v::Disable()
{
  regs->EVEN  &= ~pin_mask;
  regs->INTEN &= ~pin_mask;
}
