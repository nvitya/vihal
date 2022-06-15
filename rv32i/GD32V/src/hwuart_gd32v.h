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
// file:     hwuart_gd32v.h
// brief:    GD32V UART driver
// created:  2021-10-21
// authors:  nvitya

#ifndef HWUART_GD32V_H_
#define HWUART_GD32V_H_

#define HWUART_PRE_ONLY
#include "hwuart.h"

class THwUart_gd32v : public THwUart_pre
{
public:
	bool Init(int adevnum);  // 0x101 = LPUART1

	bool TrySendChar(char ach);
	bool TryRecvChar(char * ach);

	bool SendFinished();

	void DmaAssign(bool istx, THwDmaChannel * admach);

	bool DmaStartSend(THwDmaTransfer * axfer);
	bool DmaStartRecv(THwDmaTransfer * axfer);

public:
	gd32v_usart_t *    regs = nullptr;
};

#define HWUART_IMPL THwUart_gd32v

#endif // def HWUART_GD32V_H_
