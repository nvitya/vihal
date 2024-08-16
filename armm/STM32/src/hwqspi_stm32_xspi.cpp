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
 *  file:     hwqspi_stm32_xspi.cpp
 *  brief:    STM32 OSPI/XSPI driver (H723, H730, H7RS)
 *  created:  2024-08-14
 *  authors:  nvitya
*/

#include "platform.h"

#if defined(OCTOSPI1) || defined(XSPI) || defined(XSPI1)

#if defined(OCTOSPI1)
  #define XSPI_SR_BUSY  OCTOSPI_SR_BUSY
  #define XSPI_SR_TCF   OCTOSPI_SR_TCF
  #define XSPI_SR_FTF   OCTOSPI_SR_FTF
#endif

#include "stm32_utils.h"
#include "hwqspi.h"
#include "clockcnt.h"

#include "traces.h"

bool THwQspi_stm32::InitInterface() // the pins must be configured before, because of the alternatives
{
  #if defined(MCUSF_H7RS)

    dmanum = HWDMA_HPDMA;
    unsigned dmarq = 2;
    if (2 == devnum)  dmarq = 3;

    txdma.Init(dmanum, dmach, dmarq);
    rxdma.Init(dmanum, dmach, dmarq);  // use the same channel for tx and rx

  #else

    dmanum = HWDMA_MDMA;
    unsigned dmarq = 22;
    if (2 == devnum)  dmarq = 32;

    txdma.Init(dmanum, dmach, dmarq);
    rxdma.Init(dmanum, dmach, dmarq);  // use the same channel for tx and rx

  #endif

	return true;
}

bool THwQspi_stm32::Init(uint8_t adevnum)
{
	initialized = false;

	if (false)
	{
	}
#if defined(XSPI1)
	else if (1 == adevnum)
	{
	  RCC->AHB5ENR |= (RCC_AHB5ENR_XSPI1EN | RCC_AHB5ENR_XSPIMEN);
    regs = XSPI1;
	}
#endif
#if defined(XSPI2)
	else if (2 == adevnum)
	{
    RCC->AHB5ENR |= (RCC_AHB5ENR_XSPI2EN | RCC_AHB5ENR_XSPIMEN);
    regs = XSPI2;
	}
#endif
#if defined(OCTOSPI1)
  else if (1 == adevnum)
  {
    RCC->AHB3ENR |= (RCC_AHB3ENR_OSPI1EN | RCC_AHB3ENR_IOMNGREN);
    regs = OCTOSPI1;
  }
#endif
#if defined(OCTOSPI2)
  else if (2 == adevnum)
  {
    RCC->AHB3ENR |= (RCC_AHB3ENR_OSPI2EN | RCC_AHB3ENR_IOMNGREN);
    regs = OCTOSPI2;
  }
#endif
	else
	{
	  return false;
	}

  if (!InitInterface())
  {
    return false;
  }

  if (!txdma.initialized || !rxdma.initialized)
  {
    return false;
  }

	unsigned baseclock = stm32_bus_speed(0);
	unsigned speeddiv = baseclock / speed;
	if (speeddiv * speed < baseclock)  ++speeddiv;

	uint32_t tmp;

	// CR
	cr_base = (0
	  | (0     << 30) // MSEL(2): io line selection
	  | (0     << 28) // FMODE(2): 0 = indirect write, 1 = indirect read, 2 = status polling, 3 = memory mapped
	  | (cs_line << 24) // CSEL: 0 = use CS0, 1 = use CS1
	  | (0     << 23) // PMM: polling match mode
	  | (0     << 22) // APMS: automatic poll mode stop
	  | (0     << 20) // TOIE: timeout interrupt enable
	  | (0     << 19) // SMIE: status match interrupt enable
	  | (0     << 18) // FTIE: FIFO threshold interrupt enable
	  | (0     << 17) // TCIE: transfer complete interrupt enable
	  | (0     << 16) // TEIE: transfer error interrupt enable
	  | (0     <<  8) // FTHRES(5): FIFO threshold level
	  | (0     <<  7) // FSEL: flash memory selection, 0 = FLASH1, 1 = FLASH2
	  | (0     <<  6) // DMM: dual memory mode
	  | (0     <<  3) // TCEN: timeout counter enable
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  | (0     <<  2) // DMAEN: enable DMA
	  | (0     <<  1) // ABORT: abort request
	  | (0     <<  0) // EN: enable, enable later
	);
	regs->CR = cr_base;

	tcr_base = 0;
  if (datasample_late)  tcr_base |= (1 << 30);  // SSHIFT: 1 = data sample shift by 1 cycle

	// configure device type
	tmp = (0
	  | (2  << 24)  // MTYP(3): 2 = standard mode
	  | (0x18 << 16)  // DEVSIZE(5): device size (in 2^(FSIZE+1) bytes)
	  | (0  <<  8)  // CSHT(6): chip select high time
	  | (0  <<  1)  // FRCK: 1 = free running clock
	  | (0  <<  0)  // CKMODE: 0 = idle clock low, 1 = idle clock high
	);
  if (idleclk_high)  tmp |= (1 << 0);
	regs->DCR1 = tmp;

  tmp = (0
    | (0              << 16)  // WRAPSIZE(3)
    | ((speeddiv - 1) <<  0)  // PRESCALER(8)
  );
  regs->DCR2 = tmp;

  tmp = (0
    | (0              << 16)  // CSBOUND(5)
    | (0              <<  0)  // MAXTRAN(8)
  );
  regs->DCR3 = tmp;

  tmp = (0
    | (0              <<  0)  // REFRESH(32)
  );
  regs->DCR4 = tmp;

	while (regs->SR & XSPI_SR_BUSY) { } // wait until busy

	regs->ABR  = 0x0;  	// dummy data

	if      ( 4 == multi_line_count)  mlcode = 3;  // quad
  else if ( 2 == multi_line_count)  mlcode = 2;  // dual
	else if ( 8 == multi_line_count)  mlcode = 4;  // octal
  else if (16 == multi_line_count)  mlcode = 5;
	else 													    mlcode = 1;  // single line mode

	if (ddr_mode)  mlcode |= (1 << 3);  // The DDR mode flag always follows the 3 multi-line code

	rxdma.Prepare(false, (void *)&(regs->DR), 0);
	txdma.Prepare(true,  (void *)&(regs->DR), 0);

	regs->FCR = 0x0000001F; // clear flags

	cr_base |= (1 << 0);  // set the enable bit
	regs->CR = cr_base;

	while (regs->SR & XSPI_SR_BUSY) { } // wait until busy

	initialized = true;

	return true;
}

void THwQspi_stm32::SetMemMappedMode()
{
#if 0
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
#endif
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
		| (0 << 29) // DQSE: 1 = DQS enabled

		| (0 << 27) // DDTR: 1 = DDR enabled for DATA phase
		| (0 << 24) // DMODE(3): DATA phase line count, 0 = disable

		| (0 << 20) // ABSIZE(2): ALTERNANATE BYTE size - 1
    | (0 << 19) // ABDTR: 1 = DDR enabled for ALTERNATE BYTEs
    | (0 << 16) // ABMODE(3): ALTERNATE BYTE phase line count, 0 = disable

    | (0 << 12) // ADSIZE(2): ADDRESS byte count - 1
    | (0 << 11) // ADDTR: 1 = DDR enabled for ADDRESS phase
    | (0 <<  8) // ADMODE(3): ADDRESS phase line count, 0 = disable

    | (0 <<  4) // ISIZE(2): INSTRUCTION byte count - 1
    | (0 <<  3) // IDTR: 1 = DDR enabled for INSTRUCTION phase
    | (0 <<  0) // IMODE(3): INSTRUCTION phase line count, 0 = disable
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
		// do not write the address here, it must be written at the end,
	  // because it starts the execution.

		if (acmd & (1 << QSPICM_LN_ADDR_POS))
		{
			ccr |= (mlcode << 8); // multi line address
		}
		else
		{
			ccr |= (1 << 8); // single line address
		}

		if (rqaddrlen > 4)
		{
      rqaddrlen = addrlen;  // default addrlen
		}
		ccr |= ((rqaddrlen-1) << 12);
	}

	// mode / alternate bytes
	if (acmd & QSPICM_MODE)
	{
    if (acmd & (1 << QSPICM_LN_ADDR_POS))
    {
      ccr |= (mlcode << 16); // multi line alternate / mode bytes
    }
    else
    {
			ccr |= (1 << 16); // single line alternate bytes
		}

		ccr |= ((modelen-1) << 20); // default addrlen
		regs->ABR = modedata;
	}

	// dummy
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
	}

	// command, must be present always !
	if (acmd & (1 << QSPICM_LN_CMD_POS))
	{
	  ccr |= (mlcode << 0); // multi line command, always QUAD
	}
	else
	{
		ccr |= (1 << 0); // single line command
	}

  regs->CR = (cr_base | (1 << 28)); // FMODE(2): 1 = indirect read

  regs->DLR = (datalen > 0 ? (datalen - 1) : 0);
  regs->TCR = (tcr_base | (rqdummyc << 0));  // DCYC(5): dummy cycles
  regs->CCR = ccr;

	dmaused = (remainingbytes > 0);
	//dmaused = false; //(remainingbytes > 0);


	regs->FCR = 0x1F; // clear flags

	TRACE("QSPI start with CCR = %08X\r\n", ccr);

	regs->IR = (acmd & QSPICM_CMD_SMASK);
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

#if 0
	// wait until the transfer completes.
	while (0 == (regs->SR & XSPI_SR_TCF))
	{
	}

	TRACE("QSPI transfer completed, SR = %08X\r\n", regs->SR);

	unsigned fcnt = (regs->SR >> 8) & 0x1F;
	if (fcnt)
	{
	  TRACE("  FIFO count = %u\r\n", fcnt);
	  uint32_t d = regs->DR;
	  TRACE("  FIFO data = %08X\r\n", d);
	}

#endif


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

#if 0

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

	unsigned fields = ((acmd >> 8) & 0xF);

	// data
	if (datalen > 0)
	{
		if (fields & 8)
		{
			ccr |= (mlcode << 24); // multi line data
		}
		else
		{
			ccr |= (1 << 24); // single line data
		}
	}
	// address
	unsigned rqaddrlen = ((acmd >> 16) & 0xF);
	if (rqaddrlen)
	{
		// do not write the address here
		if (fields & 2)
		{
			ccr |= (mlcode << 10); // multi line address
		}
		else
		{
			ccr |= (1 << 10); // single line address
		}

		if (8 == rqaddrlen)
		{
			ccr |= ((addrlen-1) << 12); // default addrlen
		}
		else
		{
			ccr |= ((rqaddrlen-1) << 12); // requested address length
		}
	}

	// mode / alternate bytes
	unsigned ablen = ((acmd >> 28) & 0xF);
	if (ablen)
	{
		if (fields & 2)
		{
			ccr |= (mlcode << 14); // multi line alternate bytes
		}
		else
		{
			ccr |= (1 << 14); // single line alternate bytes
		}

		if (8 == ablen)
		{
			ccr |= ((modelen-1) << 16); // default addrlen
		}
		else
		{
			ccr |= ((ablen-1) << 16); // requested address length
		}
		regs->ABR = modedata;
	}

	unsigned dummybytes = ((acmd >> 20) & 0xF);
	if (dummybytes)
	{
		// dummy required
		if (8 == dummybytes)
		{
			dummybytes = dummysize;
		}

		unsigned dummybits = (dummybytes * 8);
		if (fields & 2) // multiline address ?
		{
			if (multi_line_count == 4)
			{
				dummybits = (dummybytes << 1);  // *2
			}
			else if (multi_line_count == 2)
			{
				dummybits = (dummybytes << 2);  // *4
			}
		}

		ccr |= (dummybits << 18);
	}

	// command
	if (fields & 1)
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

#endif

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


			if ((sr & XSPI_SR_TCF) == 0) // transfer complete ?
			{
				return;
			}
		}
		else
		{

			if ((sr & XSPI_SR_TCF) == 0) // transfer complete ?
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
		if ((regs->SR & (XSPI_SR_BUSY | XSPI_SR_FTF)))
		{
			// after a successful (DMA) transaction the BUSY bit and FIFO threshold stays set
			// this is a silicon bug:, extra data written in the FIFO at the end of a read transfer
			regs->CR |= (1 << 1); // request abort...
			runstate = 2;
			return;
		}
	}

	if (2 == runstate)
	{
		// abort requested, wait until the busy flag goes back
		if (regs->SR & XSPI_SR_BUSY)
		{
			return;
		}
	}

	busy = false;
}

#endif
