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
#include "hwqspi_stm32.h"
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

  devnum = adevnum;

	baseclock = stm32_bus_speed(0);
	unsigned speeddiv = baseclock / speed;
	if (speeddiv * speed < baseclock)  ++speeddiv;
	actual_speed = baseclock / speeddiv;

	uint32_t clk_period_ps = 1000000 / (actual_speed / 1000000);

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
	  | (1     <<  2) // DMAEN: enable DMA
	  | (0     <<  1) // ABORT: abort request
	  | (0     <<  0) // EN: enable, enable later
	);
	regs->CR = cr_base;

	tcr_base = 0;
  if (datasample_late)  tcr_base |= (1 << 30);  // SSHIFT: 1 = data sample shift by 1 cycle

	// configure device type

  // DCR1

  uint32_t dscode = 0; // device size code
  while ((1 << (dscode + 1)) < device_size)  ++dscode;

  uint32_t csht = 0;   // chip select high time
  if (cs_high_min_ns)
  {
    csht = ((cs_high_min_ns * 1000) + clk_period_ps-1 ) / clk_period_ps;
    if (csht > 63)  csht = 63;
  }

	tmp = (0
//	  | (2       << 24)  // MTYP(3): 2 = standard mode
	  | (0       << 24)  // MTYP(3): 0 = macronix mode, 2 = standard mode
	  | (dscode  << 16)  // DEVSIZE(5): device size (in 2^(FSIZE+1) bytes)
	  | (csht    <<  8)  // CSHT(6): chip select high time
	  | (0       <<  1)  // FRCK: 1 = free running clock
	  | (0       <<  0)  // CKMODE: 0 = idle clock low, 1 = idle clock high
	);
  if (idleclk_high)  tmp |= (1 << 0);
	regs->DCR1 = tmp;

	// DCR2

  tmp = (0
    | (0              << 16)  // WRAPSIZE(3)
    | ((speeddiv - 1) <<  0)  // PRESCALER(8)
  );
  regs->DCR2 = tmp;

  // DCR3

  uint32_t csbound = 0;
  while ((1 << csbound) < page_size)  ++csbound;
  tmp = (0
    | (csbound        << 16)  // CSBOUND(5): CS boundary on every 2^CSBOUND bytes
    | (0              <<  0)  // MAXTRAN(8)
  );
  regs->DCR3 = tmp;

  // DCR4

  uint32_t refrclk = 0;
  if (cs_low_max_ns)
  {
    refrclk = ((cs_low_max_ns * 1000) + clk_period_ps-1 ) / clk_period_ps;
  }

  tmp = (0
    | (refrclk        <<  0)  // REFRESH(32)
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

void THwQspi_stm32::SetMemMappedMode(uint32_t ardcmd, uint32_t awrcmd)
{
  regs->CR = (cr_base | (3 << 28)); // FMODE(2): 3 = memory mapped mode
  regs->FCR = 0x1F; // clear flags

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
  if (ardcmd & (1 << QSPICM_LN_DATA_POS))
  {
    ccr |= (mlcode << 24); // multi line data
  }
  else
  {
    ccr |= (1 << 24); // single line data
  }

  // address
  unsigned rqaddrlen = ((ardcmd >> QSPICM_ADDR_POS) & QSPICM_ADDR_SMASK);
  if (rqaddrlen > 4)
  {
    rqaddrlen = addrlen;  // default addrlen
  }
  if (ardcmd & (1 << QSPICM_LN_ADDR_POS))
  {
    ccr |= (mlcode << 8); // multi line address
  }
  else
  {
    ccr |= (1 << 8); // single line address
  }

  ccr |= ((rqaddrlen-1) << 12);

  // mode / alternate bytes
  if (ardcmd & QSPICM_MODE)
  {
    if (ardcmd & (1 << QSPICM_LN_ADDR_POS))
    {
      ccr |= (mlcode << 16); // multi line alternate / mode bytes
    }
    else
    {
      ccr |= (1 << 16); // single line alternate bytes
    }

    ccr |= ((modelen-1) << 20); // default modelen
    regs->ABR = modedata;
  }

  // dummy cycles
  unsigned rqdummyc = ((ardcmd >> QSPICM_DUMMYC_POS) & QSPICM_DUMMYC_SMASK);
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
  if (ardcmd & (1 << QSPICM_LN_CMD_POS))
  {
    ccr |= (mlcode << 0); // multi line command, always QUAD
  }
  else
  {
    ccr |= (1 << 0); // single line command
  }

  regs->TCR = (tcr_base | (rqdummyc << 0));  // DCYC(5): dummy cycles
  regs->CCR = ccr;
  regs->IR = (ardcmd & QSPICM_CMD_SMASK);

  regs->WPTCR = (tcr_base | (rqdummyc << 0));  // DCYC(5): dummy cycles
  regs->WPCCR = ccr;
  regs->WPIR = (ardcmd & QSPICM_CMD_SMASK);

  //------------------------------------------------
  // WRITE
  //------------------------------------------------

  ccr = 0
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
  if (awrcmd & (1 << QSPICM_LN_DATA_POS))
  {
    ccr |= (mlcode << 24); // multi line data
  }
  else
  {
    ccr |= (1 << 24); // single line data
  }

  // address
  rqaddrlen = ((awrcmd >> QSPICM_ADDR_POS) & QSPICM_ADDR_SMASK);
  if (rqaddrlen > 4)
  {
    rqaddrlen = addrlen;  // default addrlen
  }
  if (awrcmd & (1 << QSPICM_LN_ADDR_POS))
  {
    ccr |= (mlcode << 8); // multi line address
  }
  else
  {
    ccr |= (1 << 8); // single line address
  }

  ccr |= ((rqaddrlen-1) << 12);

  // mode / alternate bytes
  if (awrcmd & QSPICM_MODE)
  {
    if (awrcmd & (1 << QSPICM_LN_ADDR_POS))
    {
      ccr |= (mlcode << 16); // multi line alternate / mode bytes
    }
    else
    {
      ccr |= (1 << 16); // single line alternate bytes
    }

    ccr |= ((modelen-1) << 20); // default modelen
    regs->ABR = modedata;
  }

  // dummy cycles
  rqdummyc = ((awrcmd >> QSPICM_DUMMYC_POS) & QSPICM_DUMMYC_SMASK);
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

  if (awrcmd & (1 << QSPICM_LN_CMD_POS))
  {
    ccr |= (mlcode << 0); // multi line command, always QUAD
  }
  else
  {
    ccr |= (1 << 0); // single line command
  }

  regs->WTCR = (tcr_base | (rqdummyc << 0));  // DCYC(5): dummy cycles
  regs->WCCR = ccr;
  regs->WIR = (awrcmd & QSPICM_CMD_SMASK);
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

	istx = true;
	dataptr = (uint8_t *)srcptr;
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

  regs->CR = (cr_base | (0 << 28)); // FMODE(2): 0 = indirect write

  regs->DLR = (datalen > 0 ? (datalen - 1) : 0);
  regs->TCR = (tcr_base | (rqdummyc << 0));  // DCYC(5): dummy cycles
  regs->CCR = ccr;

	dmaused = (remainingbytes > 0);

	regs->FCR = 0x1F; // clear flags

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
