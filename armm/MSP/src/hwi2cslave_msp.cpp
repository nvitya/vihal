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
 *  file:     hwi2cslave_msp.cpp
 *  brief:    MSPM0 I2C Slave
 *  created:  2025-11-14
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"
#include "hwi2cslave.h"

#include "msp_utils.h"

#include "traces.h"

#define I2C_EV_PREC    (1 <<  0) // STOP received
#define I2C_EV_AMATCH  (1 <<  1)
#define I2C_EV_DRDY    (1 <<  2)
#define I2C_EV_ERROR   (1 <<  7)

bool THwI2cSlave_msp::InitHw(int adevnum)
{
	unsigned tmp;
	unsigned perid;

	initialized = false;
	devnum = adevnum;
	regs = nullptr;

	runstate = 0;

	if (0 == devnum)
	{
		regs = I2C0;
	}
	else if (1 == devnum)
	{
		regs = I2C1;
	}
	else
	{
		return false;
	}

  regs->GPRCM.RSTCTL = (I2C_RSTCTL_KEY_UNLOCK_W | I2C_RSTCTL_RESETSTKYCLR_MASK);
  if (regs->GPRCM.RSTCTL) { } // do some syncing

  regs->GPRCM.PWREN = (I2C_PWREN_KEY_UNLOCK_W | I2C_PWREN_ENABLE_MASK);
  if (regs->GPRCM.PWREN) { } // do some syncing

  regs->CLKSEL = (1 << 3);  // select SYSCLK = BUS CLOCK as clock source
  regs->CLKDIV = 0;
  uint32_t periphclock = msp_bus_speed(0);

  // no clock divisor needed

  // setup the slave address
  regs->SLAVE.SOAR = (0
  	| (0       << 15)  // TMODE: 1 = 10-bit mode, 0 = 7-bit mode
  	| (1       << 14)  // OAREN: 1 = enable own address in the OAR
  	| (address <<  0)  // OAR(10): own address
  );

  if (addressmask)
  {
    regs->SLAVE.SOAR2 = (0
    	| (addressmask << 16)  // OAR2_MASK(7)
    	| (1           <<  7)  // OAR2EN: 1 = enable own address in the OAR2
    	| (address     <<  0)  // OAR2(10): own address 2
    );
  }
  else
  {
  	regs->SLAVE.SOAR2 = 0;
  }

  // setup interrupts
  regs->INT_EVENT0.IMASK = (0
  	| (0  << 31)  // INTR_OVFL: Interrupt Overflow
  	| (0  << 30)  // TARBLOST: Target Arbitration Lost
  	| (0  << 29)  // TRX_OVFL: Target RX FIFO overflow
  	| (0  << 28)  // TTX_UNFL: Target TX FIFO underflow
  	| (0  << 27)  // TPEC_RX_ERR: Target RX Pec Error Interrupt
  	| (0  << 26)  // TDMA_DONE_RX: Target DMA Done on Event Channel RX
  	| (0  << 25)  // TDMA_DONE_TX: Target DMA Done on Event Channel TX
  	| (0  << 24)  // TGENCALL: General Call
  	| (0  << 23)  // TSTOP: Stop Condition
  	| (0  << 22)  // TSTART: Start Condition
  	| (1  << 21)  // TTXEMPTY: Target Transmit FIFO Empty
  	| (0  << 20)  // TRXFIFOFULL: RXFIFO full event
  	| (0  << 19)  // TTXFIFOTRG: Target Transmit FIFO Trigger
  	| (0  << 18)  // TRXFIFOTRG: Target Receive FIFO Trigger
  	| (1  << 17)  // TTXDONE: Target Transmit Transaction completed Interrupt
  	| (1  << 16)  // TRXDONE: Target Receive Data Interrupt

		// 0-15: Controller Interrupts
  );

  regs->INT_EVENT0.ICLR = 0xFFFFFFFF;

  // turn-on the ACK override

#if 0
  regs->SLAVE.SACKCTL = (0
    | (1  <<  4)  // ACKOEN_ON_PECDONE
    | (1  <<  3)  // ACKOEN_ON_PECNEXT
    | (1  <<  2)  // ACKOEN_ON_START
    | (1  <<  1)  // ACKOVAL: 1 = send NACK, 0 = send ACK
    | (1  <<  0)  // ACKOEN: 1 = enable ACK override
  );
#else
  regs->SLAVE.SACKCTL = (0
    | (0  <<  4)  // ACKOEN_ON_PECDONE
    | (0  <<  3)  // ACKOEN_ON_PECNEXT
    | (0  <<  2)  // ACKOEN_ON_START
    | (0  <<  1)  // ACKOVAL: 1 = send NACK, 0 = send ACK
    | (0  <<  0)  // ACKOEN: 1 = enable ACK override
  );
#endif

  // activate the Slave (Target) mode

  regs->SLAVE.SCTR = (0
  	| (0  << 10)  // TWUEN
  	| (0  <<  9)  // EN_DEFDEVADR
  	| (0  <<  8)  // EN_ALRESPADR
  	| (0  <<  7)  // EN_DEFHOSTADR
  	| (1  <<  6)  // RXFULL_ON_RREQ
  	| (0  <<  5)  // TXWAIT_STALE_TXFIFO
  	| (0  <<  4)  // TXTRIG_TXMODE
  	| (1  <<  3)  // TXEMPTY_ON_TREQ
  	| (1  <<  2)  // TCLKSTRETCH: 1 = Target clock stretching is enabled
  	| (0  <<  1)  // GENCALL: 0 = Do not respond to a general call
  	| (1  <<  0)  // ACTIVE: 1 = Enables the I2C Target operation
  );

	initialized = true;

	return true;
}

// runstate:
//   0: idle
//   1: receive data
//   5: transmit data

void THwI2cSlave_msp::HandleIrq()
{
	uint32_t intflag = regs->INT_EVENT0.MIS;

	//TRACE("I2C %08X\r\n", intflag);

	__NOP();

	if (intflag & (1 << 21)) // TX_EMPTY
	{
		regs->SLAVE.STXDATA = test_data;
		++test_data;
	}

#if 0
  regs->SLAVE.SACKCTL = (0
    | (1  <<  4)  // ACKOEN_ON_PECDONE
    | (1  <<  3)  // ACKOEN_ON_PECNEXT
    | (1  <<  2)  // ACKOEN_ON_START
    | (0  <<  1)  // ACKOVAL: 1 = send NACK, 0 = send ACK
    | (1  <<  0)  // ACKOEN: 1 = enable ACK override
  );

#endif

	__NOP();

  regs->INT_EVENT0.ICLR = 0xFFFFFFFF;

#if 0
	if (intflag & I2C_EV_AMATCH)
	{
		if (regs->STATUS.bit.DIR)
		{
			istx = true;
			runstate = 5;  // go to transfer data
		}
		else
		{
			istx = false;
			runstate = 1;  // go to receive data
		}

		OnAddressRw(address); // there is no other info on this chip, use own address

		//regs->INTFLAG.reg = I2C_EV_AMATCH;
		regs->CTRLB.bit.CMD = 3;
	}

	if (intflag & I2C_EV_DRDY)
	{
		if (1 == runstate)
		{
			uint8_t d = *(uint8_t *)&regs->DATA.reg;
			OnByteReceived(d);
		}
		else if (5 == runstate)
		{
			uint8_t d = OnTransmitRequest();
			*(uint8_t *)&regs->DATA.reg = d;
		}

		//regs->INTFLAG.reg = I2C_EV_DRDY;
		regs->CTRLB.bit.CMD = 3;
	}

	if (intflag & I2C_EV_PREC)
	{
		runstate = 0;
		regs->INTFLAG.reg = I2C_EV_PREC;
	}

	if (intflag & I2C_EV_ERROR)
	{
		regs->INTFLAG.reg = I2C_EV_ERROR;
	}
#endif
}

void THwI2cSlave_msp::SetNak()
{
}
