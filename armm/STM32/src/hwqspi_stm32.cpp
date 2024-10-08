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
 *  file:     hwqspi_stm32.cpp
 *  brief:    STM32 QSPI
 *  date:     2018-09-21
 *  authors:  nvitya
 *  notes:
*/

#include "platform.h"

#if defined(QUADSPI)

#include "hwqspi.h"
#include "clockcnt.h"

#include "traces.h"

#if !defined(RCC_AHB3ENR_QSPIEN) || defined(RCC_AHB3ENR_QUADSPIEN)
  #define RCC_AHB3ENR_QSPIEN  RCC_AHB3ENR_QUADSPIEN
#endif

bool THwQspi_stm32::InitInterface() // the pins must be configured before, because of the alternatives
{
/* possible pins for the F746:

	unsigned qspipincfg = PINCFG_PULLUP;

	hwpinctrl.PinSetup(PORTNUM_B,  6, qspipincfg | PINCFG_AF_10);  // NCS
	hwpinctrl.PinSetup(PORTNUM_B,  2, qspipincfg | PINCFG_AF_9);   // CLK
	hwpinctrl.PinSetup(PORTNUM_F,  8, qspipincfg | PINCFG_AF_10);  // IO0
	hwpinctrl.PinSetup(PORTNUM_C,  9, qspipincfg | PINCFG_AF_9);   // IO0
	hwpinctrl.PinSetup(PORTNUM_D, 11, qspipincfg | PINCFG_AF_9);   // IO0
	hwpinctrl.PinSetup(PORTNUM_F,  9, qspipincfg | PINCFG_AF_10);  // IO1
	hwpinctrl.PinSetup(PORTNUM_C, 10, qspipincfg | PINCFG_AF_9);   // IO1
	hwpinctrl.PinSetup(PORTNUM_D, 12, qspipincfg | PINCFG_AF_9);   // IO1
	hwpinctrl.PinSetup(PORTNUM_E,  2, qspipincfg | PINCFG_AF_9);   // IO2
	hwpinctrl.PinSetup(PORTNUM_F,  7, qspipincfg | PINCFG_AF_9);   // IO2
	hwpinctrl.PinSetup(PORTNUM_A,  1, qspipincfg | PINCFG_AF_9);   // IO3
	hwpinctrl.PinSetup(PORTNUM_D, 13, qspipincfg | PINCFG_AF_9);   // IO3
	hwpinctrl.PinSetup(PORTNUM_F,  6, qspipincfg | PINCFG_AF_9);   // IO3

	G4: AF10
	--------
	A6: IO3
	A7: IO2
	B0: IO1
	B1: IO0
	B10: CLK
	B11: NCS
*/

#if defined(MCUSF_G4)
	txdma.Init(dmanum, dmach, 40);  // request 40 = QUADSPI
	rxdma.Init(dmanum, dmach, 40);  // use the same channel for tx and rx
#elif defined(MCUSF_WB)
  txdma.Init(0, dmach, 20);  // request 20 = QUADSPI
  rxdma.Init(0, dmach, 20);  // use the same channel for tx and rx
#elif defined(MCUSF_H7)
	txdma.Init(0, dmach, 22);  // MDMA22: fifo threshold, MDMA 23: QSPI Transfer Complete
	rxdma.Init(0, dmach, 22);  // use the same channel for tx and rx
#else
	// The DMA 2 / stream 7 / chanel 3 is assigned to the QSPI
	txdma.Init(2, 7, 3);
	rxdma.Init(2, 7, 3);  // use the same channel for tx and rx
#endif

	return true;
}

bool THwQspi_stm32::Init()
{
	initialized = false;

	if (!InitInterface())
	{
		return false;
	}

	if (!txdma.initialized || !rxdma.initialized)
	{
		return false;
	}

	// enable clock
	RCC->AHB3ENR |= RCC_AHB3ENR_QSPIEN;
	regs = QUADSPI;

	unsigned baseclock = SystemCoreClock;
	unsigned speeddiv = baseclock / speed;
	if (speeddiv * speed < baseclock)  ++speeddiv;

	uint32_t tmp;

	// CR
	tmp = 0
	  | ((speeddiv - 1) << 24) // PRESCALER(8): clock prescaler
	  | (0 << 23) // PMM: polling match mode
	  | (0 << 22) // APMS: automatic poll mode stop
	  | (0 << 20) // TOIE: timeout interrupt enable
	  | (0 << 19) // SMIE: status match interrupt enable
	  | (0 << 18) // FTIE: FIFO threshold interrupt enable
	  | (0 << 17) // TCIE: transfer complete interrupt enable
	  | (0 << 16) // TEIE: transfer error interrupt enable
	  | (0 <<  8) // FTHRES(5): FIFO threshold level
	  | (0 <<  7) // FSEL: flash memory selection, 0 = FLASH1, 1 = FLASH2
	  | (0 <<  6) // DFM: dual flash mode
	  | (0 <<  4) // SSHIFT: sample shift, 0 = no shift, 1 = 1/2 cycle shift
	  | (0 <<  3) // TCEN: timeout counter enable
	  | (1 <<  2) // DMAEN: enable DMA
	  | (0 <<  1) // ABORT: abort request
	  | (0 <<  0) // EN: enable, enable later
	;
	if (datasample_late)  tmp |= (1 << 4);
	regs->CR = tmp;

	tmp = 0
		| (23 << 16) // FSIZE(5): Flash memory size (in 2^(FSIZE+1) bytes)
		| (3  <<  8) // CSHT(3): chip select high time, 0 = 1 cycles, 1 = 2 cycles ...
		| (0  <<  0) // CKMODE: 0 = idle clock low, 1 = idle clock high
	;
	if (idleclk_high)  tmp |= (1 << 0);
	regs->DCR = tmp;

	while (regs->SR & QUADSPI_SR_BUSY) { } // wait until busy

	regs->ABR  = 0x0;  	// dummy data

	if      (4 == multi_line_count)  mlcode = 3;
	else if (2 == multi_line_count)  mlcode = 2;
	else 													   mlcode = 1;

	rxdma.Prepare(false, (void *)&(regs->DR), 0);
	txdma.Prepare(true,  (void *)&(regs->DR), 0);

	regs->FCR = 0x0000001F; // clear flags

	regs->CR |= QUADSPI_CR_EN;

	while (regs->SR & QUADSPI_SR_BUSY) { } // wait until busy

	initialized = true;

	return true;
}

void THwQspi_stm32::SetMemMappedMode()
{
	while (busy)
	{
		Run();
	}

	unsigned cmd = 0x0B; // fast read;
	unsigned dummy = 8;
	unsigned abmode = 0;

	if (multi_line_count == 2)
	{
		cmd = 0x3B; // dual data
	}
	else if (multi_line_count == 4)
	{
		cmd = 0x6B; // quad data
	}

	unsigned ccr = 0
		| (0 << 31) // DDRM: double data rate mode
		| (0 << 30) // DHHC: DDR hold
		| (0 << 28) // SIOO: send instruction only once, 0 = send inst. for every transaction
		| (3 << 26) // FMODE(2): functional mode, 0 = write mode, 1 = read mode, 2 = polling, 3 = memory mapped
		| (mlcode << 24) // DMODE(2): data mode, 0 = no data, 1 = single, 2 = dual, 4 = quad
		| (dummy  << 18) // DCYC(5): number of dummy cycles
		| (0 << 16) // ABSIZE(2): alternate byte size, 0 = 1 byte, 3 = 4 byte
		| (0 << 14) // ABMODE(2): alternate byte mode, 0 = no bytes, 1 = single, 2 = dual, 3 = quad
		| (2 << 12) // ADSIZE(2): address size, 0 = 1 byte, 3 = 4 byte
		| (1 << 10) // ADMODE(2): address mode, 0 = do not send, 1 = single, 2 = dual, 3 = quad
		| (1 <<  8) // IMODE(2): instruction mode, 0 = do not send, 1 = single, 2 = dual, 3 = quad
		| ((cmd & 0xFF) <<  0) // INSTRUCTION(8): command / instruction byte
	;

	regs->FCR = 0x1F;
	regs->ABR = 0;
	//regs->AR = 0;
	regs->CCR = ccr;
}

int THwQspi_stm32::StartReadData(unsigned acmd, unsigned address, void * dstptr, unsigned len)
{
	if (busy)
	{
		return HWERR_BUSY;
	}

	istx = false;
	dataptr = (uint8_t *)dstptr;
	datalen = len;
	remainingbytes = datalen;

	unsigned ccr = 0
		| (0 << 31) // DDRM: double data rate mode
		| (0 << 30) // DHHC: DDR hold
		| (0 << 28) // SIOO: send instruction only once, 0 = send inst. for every transaction
		| (1 << 26) // FMODE(2): functional mode, 0 = write mode, 1 = read mode, 2 = polling, 3 = memory mapped
		| (0 << 24) // DMODE(2): data mode, 0 = no data, 1 = single, 2 = dual, 4 = quad
		| (0 << 18) // DCYC(5): number of dummy cycles
		| (0 << 16) // ABSIZE(2): alternate byte size, 0 = 1 byte, 3 = 4 byte
		| (0 << 14) // ABMODE(2): alternate byte mode, 0 = no bytes, 1 = single, 2 = dual, 3 = quad
		| (0 << 12) // ADSIZE(2): address size, 0 = 1 byte, 3 = 4 byte
		| (0 << 10) // ADMODE(2): address mode, 0 = do not send, 1 = single, 2 = dual, 3 = quad
		| (0 <<  8) // IMODE(2): instruction mode, 0 = do not send, 1 = single, 2 = dual, 3 = quad
		| ((acmd & 0xFF) <<  0) // INSTRUCTION(8): command / instruction byte
	;

	// data
	if (datalen > 0)
	{
    if (acmd & (1 << QSPICM_LN_DATA_POS))
		{
			ccr |= (mlcode << 24); // multi line data
		}
		else
		{
			ccr |= (1 << 24); // single line data
		}
	}
	// address
  unsigned rqaddrlen = ((acmd >> QSPICM_ADDR_POS) & QSPICM_ADDR_SMASK);
	if (rqaddrlen)
	{
		// do not write the address here
    if (acmd & (1 << QSPICM_LN_ADDR_POS))
		{
			ccr |= (mlcode << 10); // multi line address
		}
		else
		{
			ccr |= (1 << 10); // single line address
		}

    if (rqaddrlen > 4)
		{
      rqaddrlen = addrlen;  // default addrlen
		}
		ccr |= ((rqaddrlen-1) << 12); // requested address length
	}

	// mode / alternate bytes
  if (acmd & QSPICM_MODE)
	{
    if (acmd & (1 << QSPICM_LN_ADDR_POS))
		{
			ccr |= (mlcode << 14); // multi line alternate bytes
		}
		else
		{
			ccr |= (1 << 14); // single line alternate bytes
		}

		ccr |= ((modelen-1) << 16); // default addrlen
		regs->ABR = modedata;
	}

	// dummy cycles
  unsigned rqdummyc = ((acmd >> QSPICM_DUMMYC_POS) & QSPICM_DUMMYC_SMASK);
  if (rqdummyc)
  {
    if (rqdummyc == QSPICM_DUMMYC_SMASK)
    {
      rqdummyc = dummycycles;
    }
    else
    {
      rqdummyc <<= 1;
    }

		ccr |= (rqdummyc << 18);
	}

  // command, must be present always !
  if (acmd & (1 << QSPICM_LN_CMD_POS))
	{
	  ccr |= (3 << 8); // multi line command, always QUAD
	}
	else
	{
		ccr |= (1 << 8); // single line command
	}

	dmaused = (remainingbytes > 0);

	regs->FCR = 0x1F; // clear flags
	regs->DLR = (datalen > 0 ? (datalen - 1) : 0);
	regs->CCR = ccr;

	if (rqaddrlen)
	{
		regs->AR = address; // expects the address sending after the command
	}

	if (dmaused)
	{
	  if (0 == (remainingbytes & 3))
	  {
      xfer.bytewidth = 4;
      xfer.count = (remainingbytes >> 2);
	  }
	  else
	  {
	    xfer.bytewidth = 1;
	    xfer.count = remainingbytes;
	  }
		xfer.dstaddr = dstptr;
		xfer.flags = 0;
		rxdma.StartTransfer(&xfer);

		remainingbytes = 0;
	}

	// the operation should be started...

	busy = true;
	runstate = 0;

	return HWERR_OK;
}

int THwQspi_stm32::StartWriteData(unsigned acmd, unsigned address, void * srcptr, unsigned len)
{
	if (busy)
	{
		return HWERR_BUSY;
	}

	istx = true;
	dataptr = (uint8_t *)srcptr;
	datalen = len;
	remainingbytes = datalen;

	unsigned ccr = 0
		| (0 << 31) // DDRM: double data rate mode
		| (0 << 30) // DHHC: DDR hold
		| (0 << 28) // SIOO: send instruction only once, 0 = send inst. for every transaction
		| (0 << 26) // FMODE(2): functional mode, 0 = write mode, 1 = read mode, 2 = polling, 3 = memory mapped
		| (0 << 24) // DMODE(2): data mode, 0 = no data, 1 = single, 2 = dual, 4 = quad
		| (0 << 18) // DCYC(5): number of dummy cycles
		| (0 << 16) // ABSIZE(2): alternate byte size, 0 = 1 byte, 3 = 4 byte
		| (0 << 14) // ABMODE(2): alternate byte mode, 0 = no bytes, 1 = single, 2 = dual, 3 = quad
		| (0 << 12) // ADSIZE(2): address size, 0 = 1 byte, 3 = 4 byte
		| (0 << 10) // ADMODE(2): address mode, 0 = do not send, 1 = single, 2 = dual, 3 = quad
		| (0 <<  8) // IMODE(2): instruction mode, 0 = do not send, 1 = single, 2 = dual, 3 = quad
		| ((acmd & 0xFF) <<  0) // INSTRUCTION(8): command / instruction byte
	;

  // data
  if (datalen > 0)
  {
    if (acmd & (1 << QSPICM_LN_DATA_POS))
    {
      ccr |= (mlcode << 24); // multi line data
    }
    else
    {
      ccr |= (1 << 24); // single line data
    }
  }
  // address
  unsigned rqaddrlen = ((acmd >> QSPICM_ADDR_POS) & QSPICM_ADDR_SMASK);
  if (rqaddrlen)
  {
    // do not write the address here
    if (acmd & (1 << QSPICM_LN_ADDR_POS))
    {
      ccr |= (mlcode << 10); // multi line address
    }
    else
    {
      ccr |= (1 << 10); // single line address
    }

    if (rqaddrlen > 4)
    {
      rqaddrlen = addrlen;  // default addrlen
    }
    ccr |= ((rqaddrlen-1) << 12); // requested address length
  }

  // mode / alternate bytes
  if (acmd & QSPICM_MODE)
  {
    if (acmd & (1 << QSPICM_LN_ADDR_POS))
    {
      ccr |= (mlcode << 14); // multi line alternate bytes
    }
    else
    {
      ccr |= (1 << 14); // single line alternate bytes
    }

    ccr |= ((modelen-1) << 16); // default addrlen
    regs->ABR = modedata;
  }

  // dummy cycles
  unsigned rqdummyc = ((acmd >> QSPICM_DUMMYC_POS) & QSPICM_DUMMYC_SMASK);
  if (rqdummyc)
  {
    if (rqdummyc == QSPICM_DUMMYC_SMASK)
    {
      rqdummyc = dummycycles;
    }
    else
    {
      rqdummyc <<= 1;
    }

    ccr |= (rqdummyc << 18);
  }

  // command, must be present always !
  if (acmd & (1 << QSPICM_LN_CMD_POS))
  {
    ccr |= (3 << 8); // multi line command, always QUAD
  }
  else
  {
    ccr |= (1 << 8); // single line command
  }

	dmaused = (remainingbytes > 0);

	regs->FCR = 0x1F; // clear flags
	regs->DLR = (datalen > 0 ? (datalen - 1) : 0);
	regs->CCR = ccr;

	if (rqaddrlen)
	{
		regs->AR = address; // expects the address sending after the command
	}

	if (dmaused)
	{
    if (0 == (remainingbytes & 3))
    {
      xfer.bytewidth = 4;
      xfer.count = (remainingbytes >> 2);
    }
    else
    {
      xfer.bytewidth = 1;
      xfer.count = remainingbytes;
    }

		xfer.srcaddr = srcptr;
		xfer.flags = 0;
		txdma.StartTransfer(&xfer);

		remainingbytes = 0;
	}

	// the operation should be started...

	busy = true;
	runstate = 0;

	return HWERR_OK;
}


void THwQspi_stm32::Run()
{
	if (!busy)
	{
		return;
	}

	uint32_t sr = regs->SR;

	if (0 == runstate)
	{
		if (istx)
		{
			if (dmaused && txdma.Active())
			{
				return;
			}


			if ((sr & QUADSPI_SR_TCF) == 0) // transfer complete ?
			{
				return;
			}
		}
		else
		{

			if ((sr & QUADSPI_SR_TCF) == 0) // transfer complete ?
			{
				return;
			}

			if (dmaused && rxdma.Active())
			{
				return;
			}
		}

		regs->FCR = 0x3; // clear transfer flags

		runstate = 1; // for abort handling
	}

	if (1 == runstate)
	{
		if ((regs->SR & (QUADSPI_SR_BUSY | QUADSPI_SR_FTF)))
		{
			// after a successful (DMA) transaction the BUSY bit and FIFO threshold stays set
			// this is a silicon bug:, extra data written in the FIFO at the end of a read transfer
			regs->CR |= QUADSPI_CR_ABORT; // request abort...
			runstate = 2;
			return;
		}
	}

	if (2 == runstate)
	{
		// abort requested, wait until the busy flag goes back
		if (regs->SR & QUADSPI_SR_BUSY)
		{
			return;
		}
	}

	busy = false;
}

#endif
