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

#if defined(MCUSF_H7) || defined(MCUSF_H7RS)

#define HWETH_PRE_ONLY
#include "hweth.h"

#if 0
typedef struct
{
  volatile uint32_t DES0;     // RDES status word
  volatile uint32_t DES1;     // Buffer 1/2 byte counts
  volatile uint32_t DES2;    // Buffer 1 address
  volatile uint32_t DES3;    // Buffer 2 or next descriptor address
  volatile uint32_t EXTSTAT;  // Extended Status
  volatile uint32_t _RES;     // Reserved
  volatile uint32_t TSL;      // Timestamp value low
  volatile uint32_t TSH;      // Timestamp value high
//
} HW_ETH_DMA_DESC;
#endif


#define HWETH_DMADES_OWN        (1 << 31)  // 1 = owned by the DMA
#define HWETH_DMADES_CTX        (1 << 30)  // contex descriptor
#define HWETH_DMADES_IOC        (1 << 30)  // IOC = interrupt on completition
#define HWETH_DMADES_BUF1V      (1 << 24)  // BUF 1 Address Valid

typedef struct
{
  __IO uint32_t  DESC0;
  __IO uint32_t  DESC1;
  __IO uint32_t  DESC2;
  __IO uint32_t  DESC3;
  uint32_t       BackupAddr0; /* used to store rx buffer 1 address */
  uint32_t       BackupAddr1; /* used to store rx buffer 2 address */
//
} ETH_DMADescTypeDef;

typedef ETH_DMADescTypeDef  HW_ETH_DMA_DESC;

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

  void               SetupMii(uint32_t divmask, uint8_t addr);

public:
  ETH_TypeDef *      regs = nullptr;

  HW_ETH_DMA_DESC *  cur_rx_desc = nullptr;
  HW_ETH_DMA_DESC *  cur_tx_desc = nullptr;

  HW_ETH_DMA_DESC *  rx_desc_list = nullptr;
  HW_ETH_DMA_DESC *  tx_desc_list = nullptr;

  HW_ETH_DMA_DESC *  rx_desc_list_end = nullptr;
  HW_ETH_DMA_DESC *  tx_desc_list_end = nullptr;

  uint32_t           rxdesc_irq_flag = 0;

  void               AssignRxBuf(uint32_t idx, TPacketMem * pmem, uint32_t datalen);

  inline void        RestartStoppedRx(uint32_t idx)
  {
    if (4 == ((regs->DMADSR >> 8) & 0xF)) // is the receive suspended because of descriptor unavailability?
    {
      // try to restart
      regs->DMACRDTPR = idx;  // Set Receive Descriptor Tail pointer Address
    }
  }

  inline void        RestartStoppedTx(uint32_t idx)
  {
    if (6 == ((regs->DMADSR >> 12) & 0xF)) // is the transmit suspended because of no active descriptors?
    {
      regs->DMACTDTPR = idx;  // Set the Transmit Descriptor Tail pointer Address
    }
  }

};

#define HWETH_IMPL THwEth_stm32_v2

#endif // defined(MCUSF_H7)

#endif /* HWETH_STM32_V2_H_ */
