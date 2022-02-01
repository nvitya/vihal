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
// file:     hwuart_kendryte.h
// brief:    Kendryte UART driver
// created:  2022-02-01
// authors:  nvitya

#pragma once

#define HWUART_PRE_ONLY
#include "hwuart.h"

typedef struct kendryte_usart_t
{
  volatile uint32_t  STAT;    // 0x00: USART status register
  volatile uint32_t  DATA;    // 0x04: USART data register
  volatile uint32_t  BAUD;    // 0x08: USART baud rate register
  volatile uint32_t  CTL0;    // 0x0C: USART control register 0
  volatile uint32_t  CTL1;    // 0x10: USART control register 1
  volatile uint32_t  CTL2;    // 0x14: USART control register 2
  volatile uint32_t  GP;      // 0x18: USART guard time and prescaler register
//
} kendryte_usart_t;

class THwUart_kendryte : public THwUart_pre
{
public:
	bool Init(int adevnum);

	bool TrySendChar(char ach);
	bool TryRecvChar(char * ach);

	bool SendFinished();

	//void DmaAssign(bool istx, THwDmaChannel * admach);

	//bool DmaStartSend(THwDmaTransfer * axfer);
	//bool DmaStartRecv(THwDmaTransfer * axfer);

public:
	kendryte_usart_t *    regs = nullptr;
};

#define HWUART_IMPL THwUart_kendryte

