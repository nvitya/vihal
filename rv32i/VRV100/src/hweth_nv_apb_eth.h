/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2024 Viktor Nagy, nvitya
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
 *  file:     hweth_nv_apb_eth.h
 *  brief:    VIHAL driver for the NV APB Ethernet: https://github.com/nvitya/nv_apb_eth
 *  created:  2024-02-11
 *  authors:  nvitya
*/

#ifndef HWETH_NV_APB_ETH_H_
#define HWETH_NV_APB_ETH_H_

#include "platform.h"

#if defined(NV_APB_ETH_BASE_ADDR)

#define HWETH_PRE_ONLY
#include "hweth.h"

typedef struct TNvApbEthRegs
{
  volatile uint32_t    VERSION;     // 00
  volatile uint32_t    INFO;        // 04
  volatile uint32_t    TXCTRL;      // 08
  volatile uint32_t    RXCTRL;      // 0C
  volatile uint32_t    PHYCTRL;     // 10
  uint32_t  _reserved_14;           // 14
  volatile uint32_t    MACADDR1;    // 18
  volatile uint32_t    MACADDR2;    // 1C
  volatile uint32_t    MDIO_CMD;    // 20
  volatile uint32_t    MDIO_STATUS; // 24
  volatile uint32_t    MDIO_SPEED;  // 28
  uint32_t  _reserved_2C;           // 2C
  volatile uint32_t    TXPUT;       // 30
  uint32_t  _reserved_34[3];        // 34
  volatile uint32_t    RXGET;       // 40

  uint32_t  _reg_tail_44[(0x4000-0x44)/4];

  uint32_t             TXMEM[0x1000];  // 4000

  uint32_t             RXMEM[0x1000];  // 8000
//
} TNvApbEthRegs;

#define NV_APB_ETH_MDIO_BUSY     (1 << 31)
#define NV_APB_ETH_TXCTRL_EN     (1 << 31)
#define NV_APB_ETH_RXCTRL_EN     (1 << 31)
#define NV_APB_ETH_RXGET_FILLED  (1 << 31)
#define NV_APB_ETH_TXPUT_FILLED  (1 << 31)

typedef struct
{
	uint32_t *  ADDR;
	uint32_t    LEN;
//
} HW_ETH_DMA_DESC;  // used for RX data

class THwEth_nv_apb_eth : public THwEth_pre
{
public: // special settings
  bool               ignore_rx_errors = false;
  bool               manual_crc = false;
  uint32_t           apb_address = NV_APB_ETH_BASE_ADDR;
  uint32_t           rx_slot_words = (HWETH_MAX_PACKET_SIZE >> 2);
  uint32_t           tx_slot_words = (HWETH_MAX_PACKET_SIZE >> 2);

public:
	bool               InitMac(void * prxdesclist, uint32_t rxcnt, void * ptxdesclist, uint32_t txcnt);

  void               AssignRxBuf(uint32_t idx, TPacketMem * pmem, uint32_t datalen);

  bool               TryRecv(TPacketMem * * pmem);
  void               ReleaseRxBuf(TPacketMem * pmem);
	bool               TrySend(uint32_t * pidx, void * pdata, uint32_t datalen);
  bool               SendFinished(uint32_t idx);

	void               Start();
	void               Stop();

	void               SetMdcClock(void);
	void               SetMacAddress(uint8_t * amacaddr);
	void               SetSpeed(bool speed100);
	void               SetDuplex(bool full);

	void               InitDescList(bool istx, int bufnum, HW_ETH_DMA_DESC * pdesc_list);

	void               SetupMii(uint32_t div, uint8_t addr);
	void               StartMiiWrite(uint8_t reg, uint16_t data);
	void               StartMiiRead(uint8_t reg);
	inline uint16_t    MiiData() { return (regs->MDIO_STATUS & 0xFFFF); }
	bool               IsMiiBusy();

	uint64_t           GetTimeStamp(uint32_t idx); // must be called within 2 s to get the right upper 32 bit
	void               NsTimeStart();
	uint64_t           NsTimeRead();

public:
	TNvApbEthRegs *    regs = nullptr;

	uint8_t            rx_idx = 0;
	uint8_t            tx_idx = 0;

	HW_ETH_DMA_DESC *  rx_desc_list = nullptr;
	uint32_t           rx_desc_count = 0;
	HW_ETH_DMA_DESC *  tx_desc_list = nullptr;
  uint32_t           tx_desc_count = 0;
};

#define HWETH_IMPL THwEth_nv_apb_eth

#endif // defined NV_APB_ETH_BASE_ADDR

#endif // def HWETH_NV_APB_ETH_H_
