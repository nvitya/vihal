/* -----------------------------------------------------------------------------
 * This file is a part of the NVCM project: https://github.com/nvitya/nvcm
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
 *  file:     hwcan_atsam_v2.cpp
 *  brief:    ATSAM_V2 CAN
 *  version:  1.00
 *  date:     2019-01-13
 *  authors:  nvitya
*/

#include "string.h"
#include "platform.h"
#include "hwcan_atsam_v2.h"
#include "clockcnt.h"
#include "atsam_v2_utils.h"

#ifdef HWCAN_IMPL

#include "hwcan.h" // for the eclipse indexer

bool THwCan_atsam_v2::HwInit(int adevnum)
{
	uint32_t tmp;
	unsigned gclkid;

	if (false)  { }
#ifdef CAN0
	else if (adevnum == 0)
	{
		devnum = 0;
		regs = (HW_CAN_REGS *)CAN0;
		MCLK->AHBMASK.bit.CAN0_ = 1;
		gclkid = CAN0_GCLK_ID;

    #ifdef CCFG_CAN0_CAN0DMABA_Pos
			// set the upper address of the message buffers
		  MATRIX->CCFG_CAN0 &= 0x0000FFFF;
		  MATRIX->CCFG_CAN0 |= ((uint32_t)(&stdfilters) & 0xFFFF0000);
    #else
		  if ((uint32_t)(&stdfilters) & 0x000F0000)
		  {
		  	__BKPT(); // invalid address for the stdfilters, they must reside in the lower memory
		  }
    #endif
	}
#endif
#ifdef CAN1
	else if (adevnum == 1)
	{
		devnum = 1;
		regs = (HW_CAN_REGS *)CAN1;
		MCLK->AHBMASK.bit.CAN1_ = 1;
		gclkid = CAN1_GCLK_ID;

		#ifdef CCFG_SYSIO_CAN1DMABA_Pos
			// set the upper address of the message buffers
			MATRIX->CCFG_SYSIO &= 0x0000FFFF;
			MATRIX->CCFG_SYSIO |= ((uint32_t)(&stdfilters) & 0xFFFF0000);
		#else
			if ((uint32_t)(&stdfilters) & 0x000F0000)
			{
				__BKPT(); // invalid address for the stdfilters, they must reside in the lower memory
			}
		#endif
	}
#endif
	else
	{
		regs = nullptr;
		devnum = -1;
		return false;
	}

	// setup peripheral clock
	GCLK->PCHCTRL[gclkid].reg = ((1 << 6) | (0 << 0));   // select main clock frequency + enable

	// set to init mode
	regs->CCCR.bit.INIT = 1;
	while (regs->CCCR.bit.INIT == 0) { } // wait for ack

	regs->CCCR.bit.CCE = 1;  // set config change enable

	// initialize memory addresses

	memset((void *)&stdfilters[0], 0, sizeof(stdfilters));
	memset((void *)&rxfifo[0], 0, sizeof(rxfifo));
	memset((void *)&txfifo[0], 0, sizeof(txfifo));

	regs->CCCR.reg = 0
		| (1 <<  0)  // INIT: 1 = initialization mode
		| (1 <<  1)  // CCE: Configuration Change Enable, 0 = protected, 1 = configurable
		| (0 <<  2)  // ASM: Restricted Operation Mode, 0 = normal mode
		| (0 <<  3)  // CSA: Clock Stop Acknowledge(ro)
		| (0 <<  4)  // CSR: Clock Stop Request
		| (0 <<  5)  // MON: Bus Monitoring Mode, 0 = disabled
		| (0 <<  6)  // DAR: Disable Automatic Retransmission, 0 = automati retransmit enabled
		| (0 <<  7)  // TEST: Test Mode Enable
		| (0 <<  8)  // FDOE: CAN FD Operation Enable, 0 = disabled
		| (0 <<  9)  // BRSE: Bit Rate Switching Enable, 0 = disabled
		| (0 << 12)  // PXHD: 0 = Protocol exception handling enabled
		| (0 << 13)  // EFBI: Edge Filtering during Bus Integration, 0 = disabled
		| (0 << 14)  // TXP: Tranmit Pause, 0 = disabled
		| (0 << 15)  // NISO: Non-Iso Operation, 0 = ISO operation
	;

	regs->MRCFG.reg = 0; // no sensitive operation

	regs->SIDFC.reg = 0 // standard filters
		| (((uint32_t)(&stdfilters[0]) & 0xFFFF) << 0)
		| (HWCAN_MAX_FILTERS << 16)
	;

	regs->XIDFC.reg = 0; // no extended filters

	regs->GFC.reg = 0
		| (1 << 0)  // RRFE: 1 = reject remote frames with extended ID
		| (1 << 1)  // RRFS: 1 = reject remote frames with standard ID
		| (3 << 2)  // ANFE(2): 3 = reject non-matching extended ID
		| (3 << 4)  // ANFS(2): 3 = reject non-matching standard ID
	;

	regs->RXF0C.reg = 0 // RX FIFO
		| (((uint32_t)(&rxfifo[0]) & 0xFFFF) << 0)
		| (HWCAN_RX_FIFO_SIZE << 16)
		| (0 << 24) // F0WM(7): fifo watermark, 0 = watermark interrupt disabled
		| (1 << 31) // F0OM: Operatin Mode, 0 = blocking, 1 = overwrite
	;

	regs->RXF1C.reg = 0; // no RX FIFO1, use only single FIFO
	regs->RXBC.reg = 0;  // no dedicated RX BUFFERS
	regs->RXESC.reg = 0; // all 8 byte data size

	regs->TXBC.reg = 0 // TX FIFO
		| (((uint32_t)(&txfifo[0]) & 0xFFFF) << 0)
		| (0                  << 16) // NDTB(6): Number of Dedicated Transmit Buffers
		| (HWCAN_TX_FIFO_SIZE << 24) // TFQS(6): Transmit FIFO/Queue Size
		| (0                  << 31) // TFQM: 0 = FIFO mode, 1 = queue mode
	;

	regs->TXEFC.reg = 0; // no TX event fifo

	regs->RWD.reg = 0;

	regs->TSCC.reg = 0
		| (1 <<  0)  // TSS(2): 1 = increment time stamp
		| (1 << 16)  // TCP(4): timestamp prescaler
	;

	regs->TOCC.reg = 0; // disable timeout counter

	SetSpeed(speed);

	regs->ILE.reg = (1 << devnum);  // assign interrupt line to the corresponding device
	regs->ILS.reg = (devnum == 0 ? 0x00000000 : 0xFFFFFFFF);

	regs->IR.reg = 1; // clear the interrupt
	regs->IE.reg = 1; // enable only RX FIFO0 interrupt

	return true;
}

void THwCan_atsam_v2::SetSpeed(uint32_t aspeed)
{
	bool wasenabled = Enabled();
	if (wasenabled)
	{
		Disable();
	}

	speed = aspeed;

	unsigned periphclock = SystemCoreClock;

	// timing:
	//   fixbits = 1x bit quanta reserved for sync (at the beginning)
	//   fixbits + ts1 + ts2 = total number of clocks for one CAN bit time
	//   ts1 = offset of sampling point (in bit quanta clocks)
	//   ts2 = bit quanta clocks from sampling point to next bit
	//   rjw = resynchronisation jump width

	uint32_t  ts1max = 256;  // (ts2 will fit surely)
	uint32_t  ts2max = 128;  // (ts2 will fit surely)

	uint32_t  bitclocks;
	int32_t   ts1, ts2, rjw;
	uint32_t  brp = 1;  // bit rate prescaler

	// correct very invalid settings
	if (smp_01_percent < HWCAN_SMP_01_PERCENT_MIN)  smp_01_percent = HWCAN_SMP_01_PERCENT_MIN;
	if (smp_01_percent > HWCAN_SMP_01_PERCENT_MAX)  smp_01_percent = HWCAN_SMP_01_PERCENT_MAX;
	if (rjw_01_percent > HWCAN_RJW_01_PERCENT_MAX)  rjw_01_percent = HWCAN_RJW_01_PERCENT_MAX;

	while (true)
	{
		bitclocks = periphclock / (brp * speed);

		rjw = (bitclocks * rjw_01_percent) / 1000;  // resynchronization jump width in 0.1% (rounded downwards)
		if (rjw < 1) rjw = 1;
		ts2 = ((bitclocks * (1000 - smp_01_percent)) / 1000);  // aim to the 87.5% sampling point (rounded downwards)
		if (ts2 < 1)  ts2 = 1;
		ts1 = bitclocks - rjw - ts2;
		if (ts1 < 1)
		{
			ts1 = 1;
			rjw = bitclocks - ts2 - ts1;
			if (rjw < 1)
			{
				rjw = 1;
				ts2 = bitclocks - ts1 - rjw;
			}
		}

		if ((ts1 >= 1) && (ts1 <= ts1max) && (ts2 >= 1) && (ts2 <= ts2max))
		{
			break; // solution found
		}
		else // something wrong
		{
			if (bitclocks >= 128)
			{
				++brp;  // increase the baud rate prescaler, try again
			}
			else  // no solution was found (settings are wrong probably)
			{
				// use fix 87.5 % sampling and 12.5 % RJW
				ts2 = ((7 * bitclocks) >> 3);
				rjw = ts2;
				ts1 = bitclocks - ts2 - rjw;
				break;
			}
		}
	}

	regs->NBTP.reg = 0
	  | (rjw        << 25)  // NSJW(7): Resynchronization jump width
	  | ((brp - 1)  << 16)  // NBRP(9): Bit Rate Prescaler
	  | ((ts1 - 1)  <<  8)  // NTSEG1(8): Time segment 1
	  | ((ts2 - 1)  <<  0)  // NTSEG2(7): Time segment 2
	;

	// do not change the (fast) Data Bit timing register, it won't be used anyway
	// regs->DBTP.reg = 0;

  canbitcpuclocks = SystemCoreClock / speed;

	if (wasenabled)
	{
		Enable();
	}
}

void THwCan_atsam_v2::Enable()
{
	regs->CCCR.bit.CCE = 0;
	regs->CCCR.bit.INIT = 0;
}

void THwCan_atsam_v2::Disable()
{
	regs->CCCR.bit.INIT = 1;
	while (!regs->CCCR.bit.INIT) { } // wait until it enters inactive mode
	regs->CCCR.bit.CCE = 1; // enable configuration change
}

void THwCan_atsam_v2::HandleTx()
{
  unsigned pm = __get_PRIMASK();  // save interrupt disable status
  __disable_irq();

	while (HasTxMessage())
	{
		uint32_t txfs = regs->TXFQS.reg;  // store Tx FIFO status register

		if (txfs & CAN_TXFQS_TFQF) // is the FIFO full?
		{
			break;
		}

		// ok, we can send the message
		TCanMsg msg;
		if (!TryGetTxMessage(&msg))
		{
			break; // should not happen.
		}

		uint8_t tpi = ((txfs >> 16) & 0x1F); // get the put index

		hwcan_tx_fifo_t * txmb = &txfifo[tpi];

		txmb->DATAL = *(uint32_t *)&msg.data[0]; // must be aligned
		txmb->DATAH = *(uint32_t *)&msg.data[4];
		txmb->DLC = (msg.len << 16);
		uint32_t idfl = ((msg.cobid & 0x7FF) << 18);
		if (msg.cobid & HWCAN_RTR_FLAG)  idfl |= (1 << 29);
		txmb->IDFL = idfl;

		regs->TXBAR.reg = (1 << tpi); // add the transmit request

		++tx_msg_counter;
	}

	bus_error_count += regs->ECR.bit.CEL;  // reading this clears the CEL (bits16..23) !

  __set_PRIMASK(pm); // restore interrupt disable status
}

void THwCan_atsam_v2::HandleRx()
{
	while (true)
	{
		uint32_t rxfs = regs->RXF0S.reg;  // store Rx FIFO status register

		if ((rxfs & 0x3F) == 0)
		{
			regs->IR.reg = 1; // clear the interrupt
			return; // there are no (more) messages
		}

		uint8_t rgi = ((rxfs >> 8) & 0x3F);

		// read the message
		hwcan_rx_fifo_t * rxmb = &rxfifo[rgi];

		TCanMsg msg;

		msg.cobid = ((rxmb->ID >> 18) & 0x7FF);
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
        cantimer = regs->TSCV.reg;
        t1 = CLOCKCNT;
      }
      while (t1-t0 > 100);  // repeat if it was interrupted

      msg.timestamp = t1 - canbitcpuclocks * uint16_t(cantimer - uint16_t(dt & 0xFFFF));
    }

		++rx_msg_counter;

		OnRxMessage(&msg); // call the virtual function

		regs->RXF0A.reg = rgi; // acknowledge the read
	}
}

void THwCan_atsam_v2::AcceptListClear()
{
  bool wasenabled = Enabled();
  if (wasenabled)
  {
    Disable();
  }

	filtercnt = 0;
	memset((void *)&stdfilters[0], 0, sizeof(stdfilters));

  if (wasenabled)
  {
    Enable();
  }
}

void THwCan_atsam_v2::AcceptAdd(uint16_t cobid, uint16_t amask)
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

uint32_t THwCan_atsam_v2::ReadPsr()  // updates the CAN error counters
{
  uint32_t  psr = regs->PSR.reg;  // resets the LEC to 7 when read
  unsigned  lec = (psr & 7);

	acterr_warning = (psr & (1 << 6));
	acterr_busoff  = (psr & (1 << 7));

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
	else if (4 == lec)
	{
		++errcnt_bit1;
	}
	else if (5 == lec)
	{
		++errcnt_bit0;
	}

  return psr;
}

void THwCan_atsam_v2::UpdateErrorCounters()
{
  uint32_t ecr = regs->ECR.reg;
  acterr_rx = ((ecr >>  8) & 0x7F);
  acterr_tx = ((ecr >>  0) & 0xFF);

  if (ReadPsr()) // updates the error counters inside
  {
  }
}

bool THwCan_atsam_v2::Enabled()
{
	return (regs->CCCR.bit.INIT == 0);
}

bool THwCan_atsam_v2::IsBusOff()
{
	ReadPsr();
	return acterr_busoff;
}

bool THwCan_atsam_v2::IsWarning()
{
	ReadPsr();
	return acterr_warning;
}

uint16_t THwCan_atsam_v2::TimeStampCounter()
{
  return regs->TSCV.bit.TSC;
}

#endif
