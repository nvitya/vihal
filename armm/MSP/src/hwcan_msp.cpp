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
 *  file:     hwcan_msp.cpp
 *  brief:    MSP CAN driver for the MCAN (FD-CAN)
 *  date:     2023-11-28
 *  authors:  nvitya
 *  notes:
 *    Classic CAN only, FD features are disabled
 *    The same MCAN is used as in ATSAM_V2 (ATSAME5x) and STM32 FD (G4)
*/

#include "platform.h"

#if defined(CANFD0_BASE)

#include <hwcan_msp.h>
#include "string.h"
#include "clockcnt.h"

#include "msp_utils.h"

#include "hwcan.h" // for the eclipse indexer

// FIX MCAN SRAM allocation:

#define SRAMCAN_FLSSA  0x0004  // Filter List Standard Start Address
#define SRAMCAN_FLESA  0x0080  // Filter List Extended Start Address
#define SRAMCAN_RF0SA  0x0100  // Rx FIFO 0 Start Address
#define SRAMCAN_RF1SA  0x0200  // Rx FIFO 1 Start Address
#define SRAMCAN_TBSA   0x0300  // Tx FIFO/Queue Start Address
#define SRAMCAN_TEFSA  0x0400  // Tx Event FIFO Start Address
#define SRAMCAN_SIZE   0x0500  // closing address = used CAN SRAM size

bool THwCan_msp::HwInit(int adevnum)
{
	uint32_t tmp;

	// select PCLK as the CANFD base clock (=SystemCoreClock)
	SYSCTL->SOCLOCK.GENCLKCFG |= SYSCTL_GENCLKCFG_CANCLKSRC_MASK; // select CAN CLOCK as SYSPLL1 (80 MHz)
	if (SYSCTL->SOCLOCK.GENCLKCFG) { }

	if (false)  { }
	else if (adevnum == 0)
	{
		devnum = 0;
		ssregs = &CANFD0->MCANSS;
		regs = (HW_CAN_REGS *)&(ssregs->MCAN);
		canram_base = CANFD0_SRAM_BASE;
	}
	else
	{
		regs = nullptr;
		ssregs = nullptr;
		devnum = -1;
		return false;
	}
	
	instance_id = devnum - 1;

	ssregs->PWREN = (0
	  | (0x26 << 24)  // KEY: 0x26 = enable write
		| (1    <<  0)  // ENABLE: 1 = enable power
	);

	while (0 == (SYSCTL->SOCLOCK.SYSSTATUS & SYSCTL_SYSSTATUS_MCAN0READY_MASK))
	{
		// wait until the CAN HW is ready
	}

	// setup common clock divider
	ssregs->TI_WRAPPER.MSP.MCANSS_CLKEN = 1;
	if (ssregs->TI_WRAPPER.MSP.MCANSS_CLKEN) { }
	ssregs->TI_WRAPPER.MSP.MCANSS_CLKDIV = 0;
	if (ssregs->TI_WRAPPER.MSP.MCANSS_CLKDIV) { }

	ssregs->RSTCTL = (0
	  | (0xB1 << 24)
		| (2    <<  0)  // clear sticky reset
	);
	if (ssregs->RSTCTL) {}

	// reset SRAM
	memset((void *)(canram_base), 0, SRAMCAN_SIZE);

	// set CAN memory pointers
	stdfilters = (uint32_t *)(canram_base + SRAMCAN_FLSSA);
	rxfifo = (hwcan_rx_fifo_t *)(canram_base + SRAMCAN_RF0SA);
	txfifo = (hwcan_tx_fifo_t *)(canram_base + SRAMCAN_TBSA);
	txevfifo = (hwcan_txev_fifo_t *)(canram_base + SRAMCAN_TEFSA);

	// by default CAN is in sleep mode, so exit from sleep, and go inactive

	regs->CCCR &= ~MCAN_CCCR_CSR_MASK;
	while (regs->CCCR & MCAN_CCCR_CSA_MASK) { } // wait until it exits from sleep

	regs->CCCR |= MCAN_CCCR_INIT_MASK; // request inactive mode
	while (!(regs->CCCR & MCAN_CCCR_INIT_MASK)) { } // wait until it enters inactive mode

	regs->CCCR |= MCAN_CCCR_CCE_MASK; // enable configuration change
	if (regs->CCCR) { }  // some syncing

	// now it is ready to set up
	regs->CCCR = 0
		| (0  << 15)  // NISO: 0 = ISO compatible FD Frame format
		| (0  << 14)  // TXP: 0 = no 2 bit transmit pause
		| (0  << 13)  // EFBI: 0 = no edge filtering
		| (0  << 12)  // PXHD: 0 = protocol exception handling enabled (not disabled)
		| (0  <<  9)  // BRSE: 0 = disable bit rate switching
		| (0  <<  8)  // FDOE: 0 = disable FD operation
		| (0  <<  7)  // TEST: 0 = normal operation, 1 = enable write access to the TEST register
		| (0  <<  6)  // DAR: 0 = autoamic retransmission on failure
		| (0  <<  5)  // MON: 0 = Bus monitoring mode disabled
		| (0  <<  4)  // CSR: 0 = normal mode, 1 = power down
		| (0  <<  3)  // CSA: clock stop (power down) ack
		| (0  <<  2)  // ASM: 0 = normal CAN operation, 1 = ASM restricted operation
		| (1  <<  1)  // CCE: Configuration Change Enable (already set before)
		| (1  <<  0)  // INIT: Initialization (already set before)
	;

	if (loopback_mode)
	{
		regs->CCCR |= (1 << 7); // enable write access to the TEST register

		regs->TEST = 0
			| (0 << 5) // TX(2): 0 = normal mode
			| (1 << 4) // LBCK: 1 = enable loopback mode
		;
	}

#if 0 // test dominant on TX
	regs->CCCR |= (1 << 7); // enable write access to the TEST register
	regs->TEST = 0
		| (3 << 5) // TX(2): 0 = normal mode, 3 = force dominant on the TX
		| (1 << 4) // LBCK: 1 = enable loopback mode
	;

#endif

	regs->TSCC = 0
	  | (0 << 16)  // TCP(4): Timestamp Counter Prescaler
	  | (1 <<  0)  // TSS(2): Timestamp Select, 1 = Count CAN Bit times
	;

	regs->TOCC = 0xFFFF0000; // disable timeout counters

	SetSpeed(speed);

	// setup filtering
	AcceptListClear();

	// FILTERING

	regs->GFC = (0
	  | (3  <<  4)  // ANFS(2): 3 = reject non-mathching frames in standard filters
	  | (3  <<  2)  // ANFE(2): 3 = reject non-mathching frames in extended filters
	  | (0  <<  1)  // RRFS: 0 = filter remote frames (standard)
	  | (0  <<  0)  // RRFE: 0 = filter remote frames (extended)
	);

	regs->SIDFC = (0
  	| (HWCAN_MAX_FILTERS << 16) // LSS(5): Filter List Size Standard
		| (SRAMCAN_FLSSA     <<  0) // FLSAA(16): Filter List Standard Start Address
	);

	regs->XIDFC = (0
	 	| (0             << 16)  // LSA(5): Filter List Extended Size
	  | (SRAMCAN_FLESA <<  0)  // FLESA(16): Filter List Extended Start Address
	);

	// RX FIFO

	regs->RXESC = 0; // traditional CAN frames, maximum 8-byte data, adjust the hwcan_rx_fifo_t structure when this changed

	regs->RXF0C = 0 // RX FIFO
		| (SRAMCAN_RF0SA << 0)
		| (HWCAN_RX_FIFO_SIZE << 16)
		| (0 << 24) // F0WM(7): fifo watermark, 0 = watermark interrupt disabled
		| (1 << 31) // F0OM: Operating Mode, 0 = blocking, 1 = overwrite
	;

	regs->RXF1C = 0; // no RX FIFO1, use only single FIFO

  // TXFIFO

	regs->TXESC = 0; // traditional CAN frames, maximum 8-byte data, adjust the hwcan_tx_fifo_t structure when this changed

	regs->TXBC = 0 // TX FIFO
  	| (0                  << 30) // TFQM: 0 = FIFO mode, 1 = queue mode
		| (HWCAN_TX_FIFO_SIZE << 24) // TFQS(6): Transmit FIFO/Queue Size
		| (0                  << 16) // NDTB(6): Number of Dedicated Transmit Buffers
		| (SRAMCAN_TBSA       <<  0) // TBSA(16): Tx Buffers Start Address
	;

	regs->TXEFC = 0 // TX Event FIFO
  	| (0                  << 24) // EFWM: Event FIFO Watermark: 0 = interrupt disabled
		| (HWCAN_TX_FIFO_SIZE << 16) // EFS(6): Tx Event FIFO Size
		| (SRAMCAN_TEFSA      <<  0) // EFSA(16): Tx Event FIFO Start Address
	;

	regs->RWD = 0; // disable ram watchdog

	// setup interrupts
	regs->ILS = 0; // select line 0 for all the interrupts
	regs->ILE = 1; // enable only IRQ line 0
	regs->IE = 1;  // enable only RX FIFO0 New Message Interrupt

	return true;
}

void THwCan_msp::Enable()
{
	regs->CCCR &= ~(MCAN_CCCR_CCE_MASK | MCAN_CCCR_INIT_MASK);
}

bool THwCan_msp::Enabled()
{
	return ((regs->CCCR & (MCAN_CCCR_INIT_MASK | MCAN_CCCR_CCE_MASK)) == 0);
}

void THwCan_msp::Disable()
{
	regs->CCCR |= MCAN_CCCR_INIT_MASK; // request inactive mode
	while (!(regs->CCCR & MCAN_CCCR_INIT_MASK)) { } // wait until it enters inactive mode

	regs->CCCR |= MCAN_CCCR_CCE_MASK; // enable configuration change
}

void THwCan_msp::SetSpeed(uint32_t aspeed)
{
	bool wasenabled = Enabled();
	if (wasenabled)
	{
		Disable();
	}

	speed = aspeed;

	// set speed (nominal only)

	uint32_t periphclock = SystemCoreClock / 2;  // Uses SYSPLLCLK1, which is set to half CPU FREQ

	uint32_t brp = 1;  // bit rate prescaler
	uint32_t ts1, ts2;

	// timing:
  //   1 + ts1 + ts2 = total number of clocks for one CAN bit time
	//   1x bit quanta reserved for sync (at the beginning)
	//   ts1 = offset of sampling point (in bit quanta clocks)
	//   ts2 = bit quanta clocks from sampling point to next bit


	uint32_t bitclocks = periphclock / (brp * speed);
	while (bitclocks > 384)
	{
		++brp;
		bitclocks = periphclock / (brp * speed);
	}

	ts2 = (bitclocks - 1) / 3;
	if (ts2 > 128) ts2 = 128;
	if (ts2 < 1) ts2 = 1;
	ts1 = bitclocks - 1 - ts2;  // should not bigger than 16

	regs->NBTP = 0
	  | ((ts2 >> 1) << 25)  // NSJW(7): Resynchronization jump width
	  | ((brp - 1)  << 16)  // NBRP(9): Bit Rate Prescaler
	  | ((ts1 - 1)  <<  8)  // NTSEG1(8): Time segment 1
	  | ((ts2 - 1)  <<  0)  // NTSEG2(7): Time segment 2
	;

	// do not change the (fast) Data Bit timing register, it won't be used anyway
	// regs->DBTP = 0;

  canbitcpuclocks = SystemCoreClock / speed;

	if (wasenabled)
	{
		Enable();
	}
}

void THwCan_msp::HandleTx() // warning it can be called from multiple contexts!
{
	unsigned pm = __get_PRIMASK();  // save interrupt disable status
	__disable_irq();

	while (HasTxMessage())
	{
		uint32_t txfs = regs->TXFQS;  // store Tx FIFO status register

		if (txfs & MCAN_TXFQS_TFQF_MASK) // is the FIFO full?
		{
			break;
		}

		uint32_t tpi = ((txfs >> 16) & 0x1F); // get the put index
		if (tpi >= 0x1F)
		{
			break;
		}

		// ok, we can send the message
		TCanMsg msg;
		if (!TryGetTxMessage(&msg))
		{
			break; // should not happen.
		}

		hwcan_tx_fifo_t * txmb = (txfifo + tpi);

		txmb->DATAL = *(uint32_t *)&msg.data[0]; // must be aligned
		txmb->DATAH = *(uint32_t *)&msg.data[4];
		unsigned dlc = (msg.len << 16) | (tpi << 24);  // mark the message with the tx fifo index for tx timetamp
		if (receive_own)  dlc |= (1 << 23); // store the event in the event fifo
		txmb->DLC = dlc;
		uint32_t idfl = ((msg.cobid & 0x7FF) << 18);
		if (msg.cobid & HWCAN_RTR_FLAG)  idfl |= (1 << 29);
		txmb->IDFL = idfl;

		regs->TXBAR = (1 << tpi); // add the transmit request

		++tx_msg_counter;
	}

	// update the error counter here, because the HandleTx() called always regularly
	uint32_t ecnt_reg = regs->ECR; // reading this clears the CEL (bits16..23) !
	bus_error_count += ((ecnt_reg >> 16) & 0xFF);

 	__set_PRIMASK(pm); // restore interrupt disable status
}

void THwCan_msp::HandleRx()
{
	TCanMsg msg;

	while (true)
	{
		uint32_t rxfs = regs->RXF0S;  // store Rx FIFO(0) status register
		if ((rxfs & 0x2F) == 0)
		{
			regs->IR = 1; // clear the interrupt (RF0N = New Message in RxFIFO0)
			if (regs->IR) { } // some sync
			break; // there are no (more) messages
		}

		uint8_t rgi = ((rxfs >> 8) & 0x1F);

		// read the message
		hwcan_rx_fifo_t * rxmb = (rxfifo + rgi);

		msg.cobid = (
				((rxmb->ID >> 18) & 0x7FF)
				| ((rxmb->ID >> 14) & HWCAN_RTR_FLAG)  // move bit 29 to bit 15
		);

		*((uint32_t *)&(msg.data[0])) = rxmb->DATAL;
		*((uint32_t *)&(msg.data[4])) = rxmb->DATAH;
		uint32_t dt = rxmb->DLCTS;
		msg.len = ((dt >> 16) & 15);
		if (raw_timestamp)
		{
			msg.timestamp = (dt & 0xFFFF);
		}
		else
		{
			// get the time reference

			unsigned t0, t1;
			uint16_t cantimer;
			do
			{
				t0 = CLOCKCNT;
				cantimer = regs->TSCV;
				t1 = CLOCKCNT;
			}
			while (t1-t0 > 100);  // repeat if it was interrupted

			msg.timestamp = t1 - canbitcpuclocks * uint16_t(cantimer - uint16_t(dt & 0xFFFF));
		}

		++rx_msg_counter;

		OnRxMessage(&msg); // call the virtual function

		regs->RXF0A = rgi; // acknowledge the read
	}

	if (receive_own) // check TX event FIFO for the own msg timetamp
	{
		while (true)
		{
			uint32_t txefs = regs->TXEFS;  // Tx FIFO Event Status register

			if ((txefs & 0x3F) == 0)
			{
				break; // there are no sent messages
			}

			unsigned efgi = ((txefs >> 8) & 0x1F);  // event fifo get index

			// read the tx event
			hwcan_txev_fifo_t *  ptxe = (txevfifo + efgi);

			unsigned txdlcts = ptxe->DLCTS; // here is the Tx timestamp
			unsigned txidx = ((txdlcts >> 24) & 7); // the marker is the tx fifo index

			// read the message back from the TX FIFO
			hwcan_tx_fifo_t *  txmb = (txfifo + txidx);

			unsigned txdlc = txmb->DLC;
			if ((txdlc >> 24) == txidx) // marked properly?
			{
				unsigned idfl = txmb->IDFL;
				msg.cobid = ((idfl >> 18) & 0x7FF);
				if (idfl & (1 << 29))  msg.cobid |= HWCAN_RTR_FLAG;

				*((uint32_t *)&(msg.data[0])) = txmb->DATAL;
				*((uint32_t *)&(msg.data[4])) = txmb->DATAH;

				msg.len = ((txdlc >> 16) & 15);
				if (raw_timestamp)
				{
					msg.timestamp = (txdlcts & 0xFFFF);
				}
				else
				{
					// get the time reference

					unsigned t0, t1;
					uint16_t cantimer;
					do
					{
						t0 = CLOCKCNT;
						cantimer = regs->TSCV;
						t1 = CLOCKCNT;
					}
					while (t1-t0 > 100);  // repeat if it was interrupted

					msg.timestamp = t1 - canbitcpuclocks * uint16_t(cantimer - uint16_t(txdlcts & 0xFFFF));
				}

				++rx_msg_counter;

				OnRxMessage(&msg); // call the virtual function
			}

			regs->TXEFA = efgi; // acknowledge the Tx Event Fifo Read read
		}
	}
}

void THwCan_msp::AcceptListClear()
{
	bool wasenabled = Enabled();
	if (wasenabled)
	{
		Disable();
	}

	memset(stdfilters, 0, 4 * HWCAN_MAX_FILTERS);
	filtercnt = 0;

	if (wasenabled)
	{
		Enable();
	}
}

void THwCan_msp::AcceptAdd(uint16_t cobid, uint16_t amask)
{
	if (filtercnt >= HWCAN_MAX_FILTERS)
	{
		return;
	}

	// assembly the filter word
	uint32_t fw = 0
		| ((amask & 0x7FF) <<  0)
		| ((cobid & 0x7FF) << 16)
		| (1               << 27)  // SFEC(3): 1 = store in Rx FIFO 0 if filter matches
		| (2               << 30)  // SFT(2): 2 = mask + id, (0 = range, 1 = dual ID)
	;

	stdfilters[filtercnt] = fw;

	++filtercnt;
}

uint32_t THwCan_msp::ReadPsr()  // updates the CAN error counters
{
	uint32_t  psr = regs->PSR;  // resets the LEC to 7 when read
	unsigned  lec = (psr & 7);

	if ((0 == lec) || (7 == lec)) // the fast path, most probable case
	{
		//
	}
	else if (3 == lec)
	{
		++errcnt_ack;
	}
	else if (1 == lec)
	{
		++errcnt_stuff;
	}
	else if (2 == lec)
	{
		++errcnt_form;
	}
	else if (6 == lec)
	{
		++errcnt_crc;
	}

	return psr;
}

bool THwCan_msp::IsBusOff()
{
	uint32_t psr = ReadPsr();
	return (psr & MCAN_PSR_BO_MASK);
}

bool THwCan_msp::IsWarning()
{
	uint32_t psr = ReadPsr();
	return (psr & MCAN_PSR_EW_MASK);
}

void THwCan_msp::UpdateErrorCounters()
{
	uint32_t ecr = regs->ECR;
	acterr_rx = ((ecr >>  8) & 0xFF);
	acterr_tx = ((ecr >>  0) & 0xFF);

	if (ReadPsr()) // updates the error counters inside
	{
	}
}

#endif // defined(CANFD0)
