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
 *  file:     hwuart_esp.h
 *  brief:    ESP UART
 *  version:  1.00
 *  date:     2022-01-29
 *  authors:  nvitya
*/

#pragma once

#define HWUART_PRE_ONLY
#include "hwuart.h"

// the ESP official definitions are not really useable, so using own definitions

class THwUart_esp : public THwUart_pre
{
public:
	bool Init(int adevnum);  // 0 = UART0

	bool TrySendChar(char ach);
	bool TryRecvChar(char * ach);

	bool SendFinished();

	void DmaAssign(bool istx, THwDmaChannel * admach);

	bool DmaStartSend(THwDmaTransfer * axfer);
	bool DmaStartRecv(THwDmaTransfer * axfer);

public:
	esp_uart_t *     regs = nullptr;
	esp_uhci_t *     uhci = nullptr;

	uint32_t         tx_fifo_size = 128;
	uint32_t         rx_fifo_size = 128;
};

#define HWUART_IMPL THwUart_esp


