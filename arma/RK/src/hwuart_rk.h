/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2023 Viktor Nagy, nvitya
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
 *  file:     hwuart_rk.h
 *  brief:    RK 3506 UART
 *  date:     2025-09-08
 *  authors:  nvitya
*/


#ifndef HWUART_RK_H_
#define HWUART_RK_H_

#define HWUART_PRE_ONLY
#include "hwuart.h"

class THwUart_rk : public THwUart_pre
{
public:
	bool Init(int adevnum);

  bool SetBaudRate(int abaudrate);

	bool TrySendChar(char ach);
	bool TryRecvChar(char * ach);
	bool SendFinished();

public:

	UART_REG *      regs = nullptr;
};

#define HWUART_IMPL THwUart_rk

#endif // def HWUART_RK_H_
