/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2018 Viktor Nagy, nvitya
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
 *  file:     hweth_stm32_v2.h
 *  brief:    STM32 H7 Ethernet MAC
 *  created:  2023-11-26
 *  authors:  nvitya
*/

#ifndef HWETH_STM32_V2_H_
#define HWETH_STM32_V2_H_

#include "platform.h"

#if defined(MCUSF_H7)

#define HWETH_PRE_ONLY
#include "hweth.h"

typedef struct
{
  volatile uint32_t DES0;     // RDES status word
  volatile uint32_t DES1;     // Buffer 1/2 byte counts
  volatile uint32_t B1ADD;    // Buffer 1 address
  volatile uint32_t B2ADD;    // Buffer 2 or next descriptor address
  volatile uint32_t EXTSTAT;  // Extended Status
  volatile uint32_t _RES;     // Reserved
  volatile uint32_t TSL;      // Timestamp value low
  volatile uint32_t TSH;      // Timestamp value high
//
} HW_ETH_DMA_DESC;


class THwEth_stm32_v2 : public THwEth_pre
{
public:
  bool               InitMac(void * prxdesclist, uint32_t rxcnt, void * ptxdesclist, uint32_t txcnt);
  void               Start();
  void               Stop();

  bool               TryRecv(TPacketMem * * pmem);
  void               ReleaseRxBuf(TPacketMem * pmem);
  bool               TrySend(uint32_t * pidx, void * pdata, uint32_t datalen);
  bool               SendFinished(uint32_t idx);

  void               StartMiiWrite(uint8_t reg, uint16_t data);
  void               StartMiiRead(uint8_t reg);
  inline uint16_t    MiiData() { return regs->MACMDIODR; }
  bool               IsMiiBusy();

  uint64_t           GetTimeStamp(uint32_t idx); // must be called within 2 s to get the right upper 32 bit
  void               NsTimeStart();
  uint64_t           NsTimeRead();

public:
  uint32_t           CalcMdcClock(void);
  void               SetMacAddress(uint8_t * amacaddr);
  void               SetSpeed(bool speed100);
  void               SetDuplex(bool full);

  void               InitDescList(bool istx, int bufnum, HW_ETH_DMA_DESC * pdesc_list, uint8_t * pbuffer);

  void               SetupMii(uint32_t div, uint8_t addr);

public:
  ETH_TypeDef *      regs = nullptr;

  HW_ETH_DMA_DESC *  rx_desc_list = nullptr;
  HW_ETH_DMA_DESC *  tx_desc_list = nullptr;

  void               AssignRxBuf(uint32_t idx, TPacketMem * pmem, uint32_t datalen);

  HW_ETH_DMA_DESC *  actual_rx_desc;

};

#define HWETH_IMPL THwEth_stm32_v2

#endif // defined(MCUSF_H7)

#endif /* HWETH_STM32_V2_H_ */
