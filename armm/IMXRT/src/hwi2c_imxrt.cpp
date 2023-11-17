/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     hwi2c_imxrt.cpp
 *  brief:    IMXRT I2C Driver for VIHAL
 *  created:  2020-02-07
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"
#include "hwi2c.h"
#include "imxrt_utils.h"


bool THwI2c_imxrt::Init(int adevnum)
{
	unsigned tmp;
	unsigned perid;

	initialized = false;

	devnum = adevnum;

	regs = nullptr;

	if (false)
	{

	}
#ifdef LPI2C1
	else if (1 == devnum)
	{
		regs = LPI2C1;
		imxrt_set_clock_gate(2, 6, 3);
	}
#endif
#ifdef LPI2C2
	else if (2 == devnum)
	{
		regs = LPI2C2;
		imxrt_set_clock_gate(2, 8, 3);
	}
#endif
#ifdef LPI2C3
	else if (3 == devnum)
	{
		regs = LPI2C3;
		imxrt_set_clock_gate(2, 10, 3);
	}
#endif
#ifdef LPI2C4
	else if (4 == devnum)
	{
		regs = LPI2C4;
		imxrt_set_clock_gate(6, 24, 3);
	}
#endif
	if (!regs)
	{
		return false;
	}

	// Reset
	regs->MCR = (3 << 8) | (1 << 1); // reset + reset fifos

	if (regs->MCR) { } // some delay
	if (regs->MCR) { }

	// remove reset, setup master control register:
	regs->MCR = 0
		| (0 <<  0) // MEN: 1 = enable master mode, do not enable yet, otherwise some registers can not be written
		| (0 <<  1) // RST: 1 = software reset
		| (0 <<  2) // DOZE: 0 = doze mode disabled
		| (1 <<  3) // DBGEN: 1 = enable in debug mode
		| (0 <<  8) // RTF: 1 = reset transfer fifo
		| (0 <<  9) // RRF: 1 = reset receive fifo
	;

	regs->MIER = 0; // disable interrupts

	regs->MDER = 0
		| (0 << 0)  // TDDE: 1 = Transmit DMA Enable
		| (0 << 1)  // RDDE: 1 = Receive DMA Enable
	;

	unsigned baseclock = 60000000; // 480 MHz / 8
	unsigned prescaler = 2;
	unsigned prescaler_code = 1;
	unsigned halfclockdiv;
	while (1)
	{
		 halfclockdiv = ((baseclock / prescaler / speed) >> 1);
		 if (halfclockdiv <= 63)
		 {
			 break;
		 }
		 prescaler <<= 1;
		 ++prescaler_code;
	}

	regs->MCCR0 = 0
		| ((halfclockdiv - 1)   <<  0)  // CLKLO(6): Clock Low Period - 1
		| ((halfclockdiv - 1)   <<  8)  // CLKHI(6): Clock High Period - 1
		| ((halfclockdiv >> 1)  << 16)  // SETHOLD(6): Setup Hold Delay
		| ((halfclockdiv >> 2)  << 24)  // DATAVD(6): Data Valid Delay
	;

	regs->MCCR1 = regs->MCCR0;

	regs->MCFGR0 = 0
		| (0  <<  9)  // RDMO: 0 = Received data is stored in the receive FIFO
		| (0  <<  8)  // CIRFIFIO: 0 = Circular FIFO is disabled
		| (0  <<  2)  // HRSEL: Host Request Select
		| (0  <<  1)  // HRPOL: Host Request Polarity
		| (0  <<  0)  // HREN: 0 = Host request input is disabled
	;

	regs->MCFGR1 = 0
		| (prescaler_code  <<  0)  // PRESCALE(3)
		| (0  <<  8)  // AUTOSTOP: 0 = no automatic stop generation
		| (0  <<  9)  // IGNACK: 0 = normal ACK/NACK receive
		| (0  << 10)  // TIMECFG:
		| (0  << 16)  // MATCFG(3): 0 = match is disabled
		| (0  << 24)  // PINCFG(3): 0 = 2 pin open drain mode
  ;

	regs->MCFGR2 = 0
		| (0  <<  0)  // BUSIDLE(12): 0 = disable bus idle timeout
		| (2  << 16)  // FILTSCL(4): Glitch Filter SCL
		| (2  << 24)  // FILTSDA(4): Glitch Filter SDA
  ;

	regs->MCFGR3 = 0
		| (0  <<  8)  // PINLOW(12): 0 = disable pin low timeout check
  ;

	regs->MCR |= (1 << 0); // enable master mode

	initialized = true;

	return true;
}

void THwI2c_imxrt::DmaAssign(bool istx, THwDmaChannel * admach)
{
  if (istx)
  {
    txdma = admach;
    if (admach)
    {
      txdma->Prepare(true, (void *)&regs->MTDR, 0);
    }
  }
  else
  {
    rxdma = admach;
    if (admach)
    {
      rxdma->Prepare(false, (void *)&(regs->MRDR), 0);
    }
  }
}

void THwI2c_imxrt::RunTransaction()
{
  uint32_t tmp;

  unsigned msr = regs->MSR;

  if (0 == trastate)
  {
    istx = (0 != curtra->iswrite);
    dataptr = (uint8_t *)curtra->dataptr;
    datalen = curtra->datalen;
    remainingbytes = datalen;
    reloadremaining = remainingbytes;

    if (msr & LPI2C_MSR_MBF_MASK)
    {
      curtra->error = ERR_I2C_BUS;
      runstate = 90;
      return;
    }

    // clear all interrupt flags
    regs->MSR = 0x00007F00;


    if (regs->MFSR & 0x70007)
    {
      regs->MCR |= (3 << 8); // clear FIFOs
      if (regs->MCR) { }
    }

    unsigned extracnt = ((curtra->extra >> 24) & 3);

    // generate the START condition
    if (extracnt || istx)
    {
      regs->MTDR = (4 << 8) | (curtra->address << 1) | 0;  // bit0: R/W bit, 0 = write bytes
    }
    else
    {
      regs->MTDR = (4 << 8) | (curtra->address << 1) | 1;  // bit0: R/W bit, 1 = read bytes
    }

    if (extracnt)
    {
      if (extracnt > 2)
      {
        regs->MTDR = (0 << 8) | ((curtra->extra >> 16) & 0xFF);
        regs->MTDR = (0 << 8) | ((curtra->extra >>  8) & 0xFF);
      }
      else if (extracnt > 1)
      {
        regs->MTDR = (0 << 8) | ((curtra->extra >> 8) & 0xFF);
      }
      regs->MTDR = (0 << 8) | (curtra->extra & 0xFF);
    }

    if (istx)
    {
      dmaused = (txdma && (datalen > 0));
      if (dmaused)
      {
        regs->MDER = 0
          | (1 << 0)  // TDDE: 1 = Transmit DMA Enable
          | (0 << 1)  // RDDE: 1 = Receive DMA Enable
        ;

        xfer.srcaddr = dataptr;
        xfer.bytewidth = 1;
        xfer.count = remainingbytes;
        xfer.flags = 0; // peripheral transfer with defaults
        txdma->StartTransfer(&xfer);

        trastate = 12;
      }
      else  // dma-less operation, manual feed
      {
        regs->MDER = 0
          | (0 << 0)  // TDDE: 1 = Transmit DMA Enable
          | (0 << 1)  // RDDE: 1 = Receive DMA Enable
        ;

        trastate = 11;
        RunTransaction();
        return;
      }
    }
    else
    {
      dmaused = (rxdma && (datalen > 0));
      if (dmaused)
      {
        regs->MDER = 0
          | (0 << 0)  // TDDE: 1 = Transmit DMA Enable
          | (1 << 1)  // RDDE: 1 = Receive DMA Enable
        ;

        xfer.dstaddr = dataptr;
        xfer.bytewidth = 1;
        xfer.count = remainingbytes;
        xfer.flags = 0; // peripheral transfer with defaults
        rxdma->StartTransfer(&xfer);
      }
      else
      {
        regs->MDER = 0
          | (0 << 0)  // TDDE: 1 = Transmit DMA Enable
          | (0 << 1)  // RDDE: 1 = Receive DMA Enable
        ;
      }

      if (extracnt > 0)
      {
        trastate = 4; // issue restart
        RunTransaction();
        return;
      }
      else
      {
        trastate = 1; // process the RX
      }
    }
  }  // 0 == trastate


  if (trastate <= 50)  // error handling
  {
    if (!curtra->error)
    {
      if (msr & LPI2C_MSR_NDF_MASK)
      {
        curtra->error = ERR_I2C_ACK;
      }
      else if (msr & LPI2C_MSR_ALF_MASK)
      {
        curtra->error = ERR_I2C_ARBLOST;
      }
      else if (msr & LPI2C_MSR_PLTF_MASK)
      {
        curtra->error = ERR_I2C_BUS;
      }
      else if (msr & LPI2C_MSR_FEF_MASK)
      {
        curtra->error = ERR_I2C_BUS;
      }

      if (curtra->error)
      {
        // jump to error handling
        runstate = 90;
        return;
      }
    }
  }

  if (1 == trastate) // RX states (with and without DMA) send and receive main data
  {
    // maximum 256 byte receive can be preprogrammed, therefore this cycle here
    while (reloadremaining > 0)
    {
      if (IsTxFifoFull())
      {
        break; // no more place in the cmd fifo, continue with byte receive then
      }

      uint32_t reload = reloadremaining;
      if (reload > 256)  reload = 256;

      regs->MTDR = (1 << 8) | (reload - 1); // program to receive "reload" bytes

      reloadremaining -= reload;
    }

    // read the actual bytes
    if (dmaused)
    {
      if (rxdma->Active())  return;
    }
    else
    {
      while (remainingbytes > 0)
      {
        if (regs->MFSR & 0x70000)  // is there any bytes in FIFO ?
        {
          *dataptr++ = regs->MRDR;
          --remainingbytes;
        }
        else
        {
          return;
        }
      }
    }

    if (IsTxFifoFull())
    {
      return; // no more place in the command fifo (should not happen)
    }

    regs->MTDR = (2 << 8); // generate STOP condition
    trastate = 20; // wait until the STOP detection
    return;
  }
  else if (4 == trastate) // send re-start, for read
  {
    if (IsTxFifoFull())
    {
      return; // no more place in the fifo
    }

    // issue re-start
    regs->MTDR = (4 << 8) | (curtra->address << 1) | 1;  // bit0: R/W bit, 1 = read bytes
    runstate = 1; // go to receive
    return;
  }
  else if (11 == trastate) // DMA-less TX
  {
    while (remainingbytes > 0)
    {
      if (IsTxFifoFull())
      {
        return; // no more place in the fifo
      }

      regs->MTDR = (0 << 8) | *dataptr;
      ++dataptr;
      --remainingbytes;
    }

    if (IsTxFifoFull())
    {
      return; // no more place in the fifo
    }

    regs->MTDR = (2 << 8); // generate STOP condition
    trastate = 20; // wait until the STOP detection
    return;
  }
  else if (12 == trastate) // DMA Based TX
  {
    if (txdma->Active())
    {
      return;
    }

    if (IsTxFifoFull())
    {
      return; // no more place in the fifo
    }

    regs->MTDR = (2 << 8); // generate STOP condition
    trastate = 20; // wait until the STOP detection
    return;
  }
  else if (20 == trastate)  // wait for the STOP condition
  {
    if (msr & (LPI2C_MSR_SDF_MASK | LPI2C_MSR_FEF_MASK)) // stop detected, or fifo error ?
    {
      runstate = 50;
      curtra->completed = true;
      return;
    }
    else
    {
      return; // wait for STOP
    }
  }
  else if (90 == trastate) // handling errors
	{
    if (dmaused)
    {
      if (rxdma && rxdma->Active())  rxdma->Disable();
      if (txdma && txdma->Active())  txdma->Disable();
    }

    // disable master and clear fifo probably will recover to the default state
    tmp = regs->MCR;
    tmp &= ~1; // disable master
    tmp |= (3 << 8); // clear fifos
    regs->MCR = tmp;

    runstate = 91; // wait until the master busy flag disappears, it takes a while
	}
	else if (91 == trastate)  // wait until the master busy flag disappears, it takes a while
	{
    if (regs->MSR & LPI2C_MSR_MBF_MASK)
    {
      return;
    }

    // re-enable the master
    regs->MCR |= 1;
    regs->MSR = 0x00007F00; // clear all status flags

    // finish
    curtra->completed = true;	// transaction finished.
    runstate = 50;
  }
}
