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
 *  file:     hwuart_vrv100.cpp
 *  brief:    VexRiscV / SpinalHDL UART for VRV100
 *  version:  1.00
 *  date:     2021-10-02
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>

#include "hwuart.h"

bool THwUart_vrv100::Init(int adevnum)
{
	devnum = adevnum;
	initialized = false;

	regs = nullptr;

	if (1 == devnum)
	{
		regs = (vexriscv_uart_t *)UART1_BASE;
	}
	else if (2 == devnum)
	{
		regs = (vexriscv_uart_t *)UART2_BASE;
	}
#if defined(UART3_BASE)
	else if (3 == devnum)
	{
		regs = (vexriscv_uart_t *)UART3_BASE;
	}
#endif
#if defined(UART4_BASE)
	else if (4 == devnum)
	{
		regs = (vexriscv_uart_t *)UART4_BASE;
	}
#endif

	if (!regs)
	{
		return false;
	}

	// Frame Setup

	uint32_t frame_cfg = (0
		| (7  <<  0)  // DATA_LENGTH(8): data length - 1, 7 = 8 bit
		| (0  <<  8)  // PARITY(2): 0 = none, 1 = even, 2 = odd
		| (0  << 16)  // STOPBITS(2): 0 = one, 1 = two
	);

	if (halfstopbits == 4)
	{
		frame_cfg |= (1 << 16);
	}

	if (parity)
	{
		if (oddparity)
		{
			frame_cfg |= (2 << 8);
		}
		else
		{
			frame_cfg |= (1 << 8);
		}
	}

	regs->FRAME_CONFIG = frame_cfg;

	// Clock Setup

	uint32_t baseclock = (SystemCoreClock >> 3);

	regs->CLOCK_DIVIDER = (baseclock / baudrate) - 1;

	initialized = true;

	return true;
}

bool THwUart_vrv100::TrySendChar(char ach)
{
	if (0 == ((regs->STATUS >> 16) & 0xFF)) // no more free space in the Tx fifo?
	{
		return false;
	}

	regs->DATA = ach;

	return true;
}

bool THwUart_vrv100::TryRecvChar(char * ach)
{
	if ((regs->STATUS >> 24) & 0xFF) // is something in the rx fifo ?
	{
	  *ach = regs->DATA;
		return true;
	}

	return false;
}

bool THwUart_vrv100::SendFinished() // not 100% perfect !
{
  if ((regs->STATUS >> 16) & 0xFF) // is something in the tx fifo ?
  {
    return false;
  }

	return true;
}

