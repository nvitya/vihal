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
 *  file:     hwextirq_imxrt.h
 *  brief:    IMXRT Extenal Pin Interrupt
 *  date:     2023-12-16
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"
#include "hwextirq_imxrt.h"

bool THwExtIrq_imxrt::Init(int aportnum, int apinnum, unsigned flags)
{
	apinnum = (apinnum & 0x1F);

  regs = hwpinctrl.GetGpioRegs(aportnum);
  if (!regs)
  {
    return false;
  }

  irqack_reg  = &regs->ISR;
  irqpend_reg = &regs->ISR;

  pin_mask = (1 << apinnum);

  uint32_t * ireg = (uint32_t *)&regs->ICR1;
  uint32_t vshift = (apinnum << 1);
  if (apinnum > 15)
  {
    ++ireg;
    vshift -= 32;
  }

  // mode: 0 = low level, 1 = high level, 2 = rising edge, 3 = falling edge
  uint32_t mode;
  if      ((flags & (HWEXTIRQ_RISING | HWEXTIRQ_FALLING)) == (HWEXTIRQ_RISING | HWEXTIRQ_FALLING))  mode = 1; // high level
  else if (flags & HWEXTIRQ_FALLING)  mode = 3;
  else if (flags & HWEXTIRQ_RISING)   mode = 2;
  else                                mode = 0; // low level

  uint32_t tmp = *ireg;
  tmp &= ~(3 << vshift);
  tmp |=  (mode << vshift);
  *ireg = tmp;

	IrqAck();
	Enable();

  return true;
}

void THwExtIrq_imxrt::Enable()
{
  regs->IMR |= pin_mask;
}

void THwExtIrq_imxrt::Disable()
{
  regs->IMR &= ~pin_mask;
}
