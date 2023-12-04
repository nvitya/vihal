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
 *  file:     hwcan_msp.h
 *  brief:    MSP CAN driver for the MCAN (FD-CAN)
 *  created:  2023-11-28
 *  authors:  nvitya
*/

#ifndef HWCAN_MSP_H_
#define HWCAN_MSP_H_

#include "platform.h"

#if defined(CANFD0_BASE)

#define HWCAN_PRE_ONLY
#include "hwcan.h"

#define HWCAN_MAX_FILTERS   16
#define HWCAN_RX_FIFO_SIZE   8
#define HWCAN_TX_FIFO_SIZE   8

typedef struct  // MCAN register definitions without "MCAN_" prefix
{
  __I  uint32_t CREL;                         /* !< (@ 0x00007000) MCAN Core Release Register */
  __I  uint32_t ENDN;                         /* !< (@ 0x00007004) MCAN Endian Register */
       uint32_t RESERVED0;
  __IO uint32_t DBTP;                         /* !< (@ 0x0000700C) MCAN Data Bit Timing and Prescaler Register */
  __IO uint32_t TEST;                         /* !< (@ 0x00007010) MCAN Test Register */
  __IO uint32_t RWD;                          /* !< (@ 0x00007014) MCAN RAM Watchdog */
  __IO uint32_t CCCR;                         /* !< (@ 0x00007018) MCAN CC Control Register */
  __IO uint32_t NBTP;                         /* !< (@ 0x0000701C) MCAN Nominal Bit Timing and Prescaler Register */
  __IO uint32_t TSCC;                         /* !< (@ 0x00007020) MCAN Timestamp Counter Configuration */
  __IO uint32_t TSCV;                         /* !< (@ 0x00007024) MCAN Timestamp Counter Value */
  __IO uint32_t TOCC;                         /* !< (@ 0x00007028) MCAN Timeout Counter Configuration */
  __IO uint32_t TOCV;                         /* !< (@ 0x0000702C) MCAN Timeout Counter Value */
       uint32_t RESERVED1[4];
  __I  uint32_t ECR;                          /* !< (@ 0x00007040) MCAN Error Counter Register */
  __I  uint32_t PSR;                          /* !< (@ 0x00007044) MCAN Protocol Status Register */
  __IO uint32_t TDCR;                         /* !< (@ 0x00007048) MCAN Transmitter Delay Compensation Register */
       uint32_t RESERVED2;
  __IO uint32_t IR;                           /* !< (@ 0x00007050) MCAN Interrupt Register */
  __IO uint32_t IE;                           /* !< (@ 0x00007054) MCAN Interrupt Enable */
  __IO uint32_t ILS;                          /* !< (@ 0x00007058) MCAN Interrupt Line Select */
  __IO uint32_t ILE;                          /* !< (@ 0x0000705C) MCAN Interrupt Line Enable */
       uint32_t RESERVED3[8];
  __IO uint32_t GFC;                          /* !< (@ 0x00007080) MCAN Global Filter Configuration */
  __IO uint32_t SIDFC;                        /* !< (@ 0x00007084) MCAN Standard ID Filter Configuration */
  __IO uint32_t XIDFC;                        /* !< (@ 0x00007088) MCAN Extended ID Filter Configuration */
       uint32_t RESERVED4;
  __IO uint32_t XIDAM;                        /* !< (@ 0x00007090) MCAN Extended ID and Mask */
  __I  uint32_t HPMS;                         /* !< (@ 0x00007094) MCAN High Priority Message Status */
  __IO uint32_t NDAT1;                        /* !< (@ 0x00007098) MCAN New Data 1 */
  __IO uint32_t NDAT2;                        /* !< (@ 0x0000709C) MCAN New Data 2 */
  __IO uint32_t RXF0C;                        /* !< (@ 0x000070A0) MCAN Rx FIFO 0 Configuration */
  __I  uint32_t RXF0S;                        /* !< (@ 0x000070A4) MCAN Rx FIFO 0 Status */
  __IO uint32_t RXF0A;                        /* !< (@ 0x000070A8) MCAN Rx FIFO 0 Acknowledge */
  __IO uint32_t RXBC;                         /* !< (@ 0x000070AC) MCAN Rx Buffer Configuration */
  __IO uint32_t RXF1C;                        /* !< (@ 0x000070B0) MCAN Rx FIFO 1 Configuration */
  __I  uint32_t RXF1S;                        /* !< (@ 0x000070B4) MCAN Rx FIFO 1 Status */
  __IO uint32_t RXF1A;                        /* !< (@ 0x000070B8) MCAN Rx FIFO 1 Acknowledge */
  __IO uint32_t RXESC;                        /* !< (@ 0x000070BC) MCAN Rx Buffer / FIFO Element Size Configuration */
  __IO uint32_t TXBC;                         /* !< (@ 0x000070C0) MCAN Tx Buffer Configuration */
  __I  uint32_t TXFQS;                        /* !< (@ 0x000070C4) MCAN Tx FIFO / Queue Status */
  __IO uint32_t TXESC;                        /* !< (@ 0x000070C8) MCAN Tx Buffer Element Size Configuration */
  __I  uint32_t TXBRP;                        /* !< (@ 0x000070CC) MCAN Tx Buffer Request Pending */
  __IO uint32_t TXBAR;                        /* !< (@ 0x000070D0) MCAN Tx Buffer Add Request */
  __IO uint32_t TXBCR;                        /* !< (@ 0x000070D4) MCAN Tx Buffer Cancellation Request */
  __I  uint32_t TXBTO;                        /* !< (@ 0x000070D8) MCAN Tx Buffer Transmission Occurred */
  __I  uint32_t TXBCF;                        /* !< (@ 0x000070DC) MCAN Tx Buffer Cancellation Finished */
  __IO uint32_t TXBTIE;                       /* !< (@ 0x000070E0) MCAN Tx Buffer Transmission Interrupt Enable */
  __IO uint32_t TXBCIE;                       /* !< (@ 0x000070E4) MCAN Tx Buffer Cancellation Finished Interrupt
                                                      Enable */
       uint32_t RESERVED5[2];
  __IO uint32_t TXEFC;                        /* !< (@ 0x000070F0) MCAN Tx Event FIFO Configuration */
  __I  uint32_t TXEFS;                        /* !< (@ 0x000070F4) MCAN Tx Event FIFO Status */
  __IO uint32_t TXEFA;                        /* !< (@ 0x000070F8) MCAN Tx Event FIFO Acknowledge */
//
} mcan_regs_t;

#define HW_CAN_REGS     mcan_regs_t

typedef struct
{
	__IO uint32_t  ID;    // ID
	__IO uint32_t  DLCTS; // Length Code + TimeStamp
	__IO uint32_t  DATAL; // DATA
	__IO uint32_t  DATAH;
	__IO uint32_t  DATAX[14];
//
} __attribute__((packed)) hwcan_rx_fifo_t;

typedef struct
{
	__IO uint32_t  IDFL;  // ID + Flags
	__IO uint32_t  DLC;   // Length code
	__IO uint32_t  DATAL; // data low
	__IO uint32_t  DATAH; // data high
	__IO uint32_t  DATAX[14];
//
} __attribute__((packed)) hwcan_tx_fifo_t;

typedef struct
{
	__IO uint32_t  IDFL;   // ID + Flags
	__IO uint32_t  DLCTS;  // Length code, Timestamp, MM (mark)
//
} __attribute__((packed)) hwcan_txev_fifo_t;

class THwCan_msp : public THwCan_pre
{
public: // mandatory
	bool HwInit(int adevnum);

	void Enable();
	void Disable();
	bool Enabled();

	void HandleTx();
	void HandleRx();

	void AcceptListClear();
	void AcceptAdd(uint16_t cobid, uint16_t amask);

	void SetSpeed(uint32_t aspeed);
	bool IsBusOff();
	bool IsWarning();

	void UpdateErrorCounters();

public:
	HW_CAN_REGS *        regs = nullptr;
	MCAN_MCANSS_Regs *   ssregs = nullptr;

	uint8_t              filtercnt = 0;

	uint32_t             canram_base = 0;
	uint32_t *           stdfilters = nullptr;
	hwcan_rx_fifo_t *    rxfifo = nullptr;
	hwcan_tx_fifo_t *    txfifo = nullptr;
	hwcan_txev_fifo_t *  txevfifo = nullptr;

	ALWAYS_INLINE uint32_t           ReadPsr(); // special function to handle the reset on read fields
};

#define HWCAN_IMPL THwCan_msp

#endif // defined(CANFD0_BASE)

#endif /* HWCAN_MSP_H_ */
