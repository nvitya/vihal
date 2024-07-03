/*
 * hwspi_am.h
 *
 *  Created on: Jun 30, 2024
 *      Author: vitya
 */

#ifndef HWSPI_AM_H_
#define HWSPI_AM_H_

#include "platform.h"
#include "stdint.h"

typedef struct
{
	volatile uint32_t  CONF;             // 00
	volatile uint32_t  STAT;             // 04
	volatile uint32_t  CTRL;             // 08
	volatile uint32_t  TX;               // 0C
	volatile uint32_t  RX;               // 10
//
} THwSpiChRegs;

typedef struct
{
	volatile uint32_t  REVISION;         // 000
	uint32_t  _pad_004[(0x110-0x004)/4]; // 004
	volatile uint32_t  SYSCONFIG;        // 110
	volatile uint32_t  SYSSTATUS;        // 114
	volatile uint32_t  IRQSTATUS;        // 118
	volatile uint32_t  IRQENABLE;        // 11C
	uint32_t  _pad_120[(0x124-0x120)/4]; // 120
	volatile uint32_t  SYST;             // 124
	volatile uint16_t  MODULCTRL;        // 128

	THwSpiChRegs       CH[4];            // 12C

	volatile uint32_t  XFERLEVEL;        // 17C
	volatile uint32_t  DAFTX[8];         // 180
	volatile uint32_t  DAFRX[8];         // 1A0
//
} THwSpiRegs;

class THwSpi_am
{
public:
	uint32_t           speed = 4000000;
	unsigned char      databits = 8; // frame length
	bool               tx_only = false;
	bool               lsb_first = false;
	bool               idleclk_high = false;
	bool               datasample_late = true;

public:
	THwSpiChRegs *     regs  = nullptr;
	THwSpiRegs *       mregs = nullptr;
	int                devnum = -1;
	bool               initialized = false;

	bool Init(int adevnum);
	void SetSpeed(uint32_t aspeed);

	bool TrySendData(uint8_t adata);
	bool TryRecvData(uint8_t * dstptr);
	bool SendFinished();


};


#endif /* HWSPI_AM_H_ */
