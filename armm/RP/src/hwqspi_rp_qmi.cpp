/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     hwdma_rp_qmi.cpp
 *  brief:    RP2350 QSPI driver using the QMI unit
 *  created:  2024-10-13
 *  authors:  nvitya
 *  notes:
 *    WARNING: this code is not compatible with XIP,
 *             the code must run from RAM to use the THwQspi on the RP2350
 *
 *    The code is rather specialized for the SPI Flash usage, which is usally
 *    fix soldered to the board.
 *    Only 32-bit transfers are supported.
*/

#include "platform.h"

#include "hwpins.h"
#include "hwqspi_rp_ssi.h"

#if defined(qmi_hw)

#include "rp_utils.h"

#include "hwqspi.h" // for eclipse indexer
#include "hwqspi_rp_qmi.h" // for eclipse indexer

#include "clockcnt.h"
#include "traces.h" // temporary, remove me !!!!!!!!!!!!!!!!!!

bool THwQspi_rp::InitInterface()
{
  int i;
	// QSPI / SPIFI pins

  // Then mux XIP block onto internal QSPI flash pads
  io_rw_32 * iobank1 = (io_rw_32 *) IO_QSPI_BASE;
  for (int i = 0; i < 6; ++i)
  {
    iobank1[2 * i + 5] = 0;  // select the QSPI function
  }

	txdma.Init(txdmachannel, DREQ_XIP_QMITX);
	rxdma.Init(rxdmachannel, DREQ_XIP_QMIRX);

	return true;
}

bool THwQspi_rp::Init()
{
  int i;
	unsigned tmp;

	initialized = false;

  regs = qmi_hw;

  rp_reset_control(RESETS_RESET_IO_QSPI_BITS | RESETS_RESET_PADS_QSPI_BITS, true); // issue reset
  for (i = 0; i < 100; ++i)
  {
    __NOP();
  }
  rp_reset_control(RESETS_RESET_IO_QSPI_BITS | RESETS_RESET_PADS_QSPI_BITS, false); // remove reset
  for (i = 0; i < 100; ++i)
  {
    __NOP();
  }

	if (!InitInterface())
	{
		return false;
	}

	if (!txdma.initialized || !rxdma.initialized)
	{
		return false;
	}

	xip_ctrl_hw->ctrl &= ~(XIP_CTRL_EN_NONSECURE_BITS | XIP_CTRL_EN_SECURE_BITS);  // disable XIP cache

#if 0
	regs->ssienr = 0; // disable to allow configuration
  regs->ser = 0;

  // Clear sticky errors (clear-on-read)
  if (regs->sr) { }
  if (regs->icr) { }

  unsigned baseclock = SystemCoreClock;
  unsigned speeddiv = baseclock / speed;
  if (speeddiv * speed < baseclock)  ++speeddiv;

  regs->baudr = speeddiv;

  regs->rx_sample_dly = 0;

	rxdma.Prepare(false, (void *)&regs->dr0, 0);
	txdma.Prepare(true,  (void *)&regs->dr0, 0);

  // do not enable yet enable
  // regs->ssienr = 1;

	if (4 == multi_line_count)
	{
	  mlcode = 2;
	}
	else if (2 == multi_line_count)
	{
	  mlcode = 1;
	}
	else
	{
	  mlcode = 0;
	}


	initialized = true;

	return true;
#else
	return false;
#endif
}

int THwQspi_rp::StartReadData(unsigned acmd, unsigned address, void * dstptr, unsigned len)
{
#if 0
  if (busy)
  {
    return HWERR_BUSY;
  }

  istx = false;
  byte_width = 1;
  dataptr = (uint8_t *)dstptr;
  datalen = len;
  remainingbytes = datalen;

  regs->ssienr = 0; // disable for re-configuration, also flushes the fifo-s

  uint32_t cr0 = (0
    | (0  << 24)  // SSTE: slave select toggle
    | (0  << 21)  // SPI_FRF(2): 0 = single, 1 = dual, 2 = quad
    | (7  << 16)  // DFS_32(5): 31 = 32-bit frames, 7 = 8-bit frames (will be set later)
    | (0  << 12)  // CFS(4): control frame size - ??? not documented
    | (0  << 11)  // SRL: shift register loop
    | (0  << 10)  // SLV_OE: slave output enable (???, not well documented)
    | (3  <<  8)  // TMOD(2): 3 = eeprom mode: RX starts after control data
    | (0  <<  7)  // SCPOL serial clock polarity
    | (0  <<  6)  // SCPH: serial clock phase
    | (0  <<  4)  //   FRF(2): ??? not documented, the SDK and the BOOTROM uses only the SPI_FRF field
    | (0  <<  0)  //   DFS(4): ??? not documented, the SDK and the BOOTROM uses only the DFS_32 field
  );

  uint32_t spi_cr0 = (0
//    | ((acmd & 0xFF) << 24)  // XIP_CMD(8): instruction code in XIP mode when INST_L = 8-bit
    | (0  << 24)  // XIP_CMD(8): instruction code in XIP mode when INST_L = 8-bit
    | (0  << 18)  // SPI_RXDS_EN: read data strobe enable
    | (0  << 17)  // INST_DDR_EN
    | (0  << 16)  // SPI_DDR_EN
    | (0  << 11)  // WAIT_CYCLES(5)
    | (2  <<  8)  // INST_L(2): 0 = no instruction, 2 = 8-bit instruction, 3 = 16-bit
    | (0  <<  2)  // ADDR_L(4): in 4 bit increments
    | (0  <<  0)  // TRANS_TYPE(2): 0 = all single, 1 = cmd single and addr multi-line, 2 = cmd and address multi-line
  );

  //unsigned fields = ((acmd >> 8) & 0xF);

  // check data transfer width
  if (acmd & (1 << QSPICM_LN_DATA_POS))
  {
    // use 32 bit mode !
    byte_width = 4;
    cr0 |= (0
      | (mlcode << 21)  // SPI_FRF(2): multi line data
      | (31     << 16)  // DFS_32(5): 31 = 32 bit mode
    );

    // control (cmd + address) transfer width
    if (acmd & (1 << QSPICM_LN_CMD_POS)) // multiline command ?
    {
      spi_cr0 |= (2 << 0); // TRANS_TYPE(2): 2 = multi line command and address (and data)
    }
    else if (acmd & (1 << QSPICM_LN_ADDR_POS)) // multiline address ?
    {
      spi_cr0 |= (1 << 0); // TRANS_TYPE(2): 1 = single line command, multi-line address
    }
    else
    {
      // leave it all single line
    }
  }
  else
  {
    // single line, 8 bit mode, normal spi
    // dummy bytes must be pushed too
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
    spi_cr0 |= (rqdummyc << 11);
  }

  // address
  addr_mode_len = ((acmd >> QSPICM_ADDR_POS) & QSPICM_ADDR_SMASK);
  if (addr_mode_len)
  {
    if (addr_mode_len > 4)
    {
      addr_mode_len = addrlen;  // default addrlen
    }

    addrdata = address;
  }
  else
  {
    addr_mode_len = 0;
    addrdata = 0;
  }

  // mode / alternate bytes
  if (acmd & QSPICM_MODE)
  {
    // only 8-bit supported here
    addr_mode_len += 1;
    addrdata = ((addrdata << 8) | (modedata & 0xFF));
  }

  spi_cr0 |= ((addr_mode_len << 1) << 2); // requested address length

  // push addr and command into the fifo

  if (byte_width > 1)
  {
    remaining_transfers = ((remainingbytes + 3) >> 2);
  }
  else
  {
    remaining_transfers = remainingbytes;
  }

  dmaused = (remaining_transfers > 0);
  //dmaused = false; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  regs->ctrlr0 = cr0;
  regs->spi_ctrlr0 = spi_cr0;
  regs->ctrlr1 = remaining_transfers-1;  // one less !

  if (dmaused)
  {
    regs->dmacr = SSI_DMACR_RDMAE_BITS;  // enable only the RX DMA
  }
  else
  {
    regs->dmacr = 0;
  }

  regs->ssienr = 1; // enable
  regs->ser = 1;

  if (dmaused)
  {
    xfer.bytewidth = byte_width;
    xfer.count = remaining_transfers;
    xfer.dstaddr = dstptr;
    xfer.flags = DMATR_BYTESWAP;
    rxdma.StartTransfer(&xfer);

    remaining_transfers = 0;
  }

  // start the transfer writing the control registers
  regs->dr0 = (acmd & 0xFF);
  if (addr_mode_len > 0)
  {
    if (byte_width > 1)
    {
      regs->dr0 = addrdata; // writing this starts the transfer
    }
    else
    {
      // put the data byte-wise into the fifo
      // beware of the reversed byte order!
      unsigned am_remaining = addr_mode_len;
      unsigned am_d = addrdata;
      while (am_remaining)
      {
        regs->dr0 = am_d >> (8 * (am_remaining - 1));
        --am_remaining;
      }
    }
  }

  if (rqdummyc && (1 == byte_width))
  {
    // in 8-bit single line mode specifying only the wait cycles in the SPI_CR0 does not seem to be enough
    // correct results were observed, if the wait cycles are pushed into the TX fifo too:
    regs->dr0 = dummydata;
  }

  while (0 == (regs->sr & SSI_SR_BUSY_BITS))
  {
    // wait until it is really started
  }

  busy = true;
  runstate = 0;

  return HWERR_OK;
#else
  return HWERR_NOTIMPL;
#endif
}

int THwQspi_rp::StartWriteData(unsigned acmd, unsigned address, void * srcptr, unsigned len)
{
#if 0
  if (busy)
  {
    return HWERR_BUSY;
  }

  uint8_t   cmdbuf[4];  // for fast fifo push we collect the bytes here
  unsigned  cmdlen = 1;

  // WARNING: no DUAL/QUAD support here!

  // the requested 0x32 (quad page program) is converted to 0x02 = single page program:
  if ((acmd & 0xFF) == 0x32)
  {
    acmd = ((acmd & 0xFFFFFF00) | 0x02);
  }

  cmdbuf[0] = (acmd & 0xFF);

  istx = true;
  byte_width = 1;
  dataptr = (uint8_t *)srcptr;
  datalen = len;
  remainingbytes = datalen;

  regs->ssienr = 0; // disable for re-configuration, also flushes the fifo-s

  uint32_t cr0 = (0
    | (0  << 24)  // SSTE: slave select toggle
    | (0  << 21)  // SPI_FRF(2): 0 = single, 1 = dual, 2 = quad
    | (7  << 16)  // DFS_32(5): 31 = 32-bit frames, 7 = 8-bit frames (will be set later)
    | (0  << 12)  // CFS(4): control frame size - ??? not documented
    | (0  << 11)  // SRL: shift register loop
    | (0  << 10)  // SLV_OE: slave output enable (???, not well documented)
    | (1  <<  8)  // TMOD(2): 0 = tx+rx, 1 = transmit only
    | (0  <<  7)  // SCPOL serial clock polarity
    | (0  <<  6)  // SCPH: serial clock phase
    | (0  <<  4)  //   FRF(2): ??? not documented, the SDK and the BOOTROM uses only the SPI_FRF field
    | (0  <<  0)  //   DFS(4): ??? not documented, the SDK and the BOOTROM uses only the DFS_32 field
  );

  // the spi_cr0 is not used here (?)
  uint32_t spi_cr0 = (0
//    | ((acmd & 0xFF) << 24)  // XIP_CMD(8): instruction code in XIP mode when INST_L = 8-bit
    | (0  << 24)  // XIP_CMD(8): instruction code in XIP mode when INST_L = 8-bit
    | (0  << 18)  // SPI_RXDS_EN: read data strobe enable
    | (0  << 17)  // INST_DDR_EN
    | (0  << 16)  // SPI_DDR_EN
    | (0  << 11)  // WAIT_CYCLES(5)
    | (0  <<  8)  // INST_L(2): 0 = no instruction, 2 = 8-bit instruction, 3 = 16-bit
    | (0  <<  2)  // ADDR_L(4): in 4 bit increments
    | (0  <<  0)  // TRANS_TYPE(2): 0 = all single, 1 = cmd single and addr multi-line, 2 = cmd and address multi-line
  );

  // no dummy support here

  // address
  addr_mode_len = ((acmd >> QSPICM_ADDR_POS) & QSPICM_ADDR_SMASK);
  if (addr_mode_len)
  {
    if (addr_mode_len > 4)
    {
      addr_mode_len = addrlen;  // default addrlen
    }

    addrdata = address;
    unsigned am_remaining = addr_mode_len;
    unsigned am_d = address;
    while (am_remaining)
    {
      cmdbuf[cmdlen++] = (am_d >> (8 * (am_remaining - 1)));
      --am_remaining;
    }
    addrdata = ((cmdbuf[0] << 24) | address);
  }
  else
  {
    addr_mode_len = 0;
    addrdata = 0;
  }

#if 1
  if ((4 == cmdlen) && (len > 8)) // use the 32-bit mode
  {
    // use 32 bit mode !
    byte_width = 4;
    cr0 |= (31 << 16);  // DFS_32(5): 31 = 32 bit mode
  }
#endif

  // push addr and command into the fifo

  if (byte_width > 1)
  {
    remaining_transfers = ((remainingbytes + 3) >> 2);
  }
  else
  {
    remaining_transfers = remainingbytes;
  }

  dmaused = (remaining_transfers > 0);

  regs->ctrlr0 = cr0;
  regs->spi_ctrlr0 = spi_cr0;
  regs->ctrlr1 = 0; // no recevied bytes expected, one word will be put into the fifo

  if (dmaused)
  {
    regs->dmacr = SSI_DMACR_TDMAE_BITS;  // enable only the TX DMA

    xfer.bytewidth = byte_width;
    xfer.count = remaining_transfers;
    xfer.srcaddr = srcptr;
    xfer.flags = DMATR_BYTESWAP;
    txdma.PrepareTransfer(&xfer);

    remaining_transfers = 0;
  }
  else
  {
    regs->dmacr = 0;
  }

  // special start required here because if the TX FIFO goes empty
  // the CS goes high, so for high speeds better to use 32-bit mode
  // DMA is mandatory.

  regs->ssienr = 1; // enable

  // fill the fifo with the command and address first:
  if (byte_width > 1)
  {
    regs->dr0 = addrdata;
  }
  else
  {
    unsigned n = 0;
    while (n < cmdlen)
    {
      regs->dr0 = cmdbuf[n++];
    }
  }

  if (dmaused)
  {
    txdma.StartPreparedTransfer();
  }

  // delayed slave enable required for higher transfer rates
  regs->ser = 1; // starts the actual transfer

  busy = true;
  runstate = 0;

  return HWERR_OK;
#else
  return HWERR_NOTIMPL;
#endif
}

void THwQspi_rp::Run()
{
#if 0
  if (!busy)
  {
    return;
  }

  uint32_t sr = regs->sr;

  if (0 == runstate)
  {
    if (istx)
    {
      if (dmaused)
      {
        if (txdma.Active())
        {
          return;
        }
      }
      else
      {
        while (remaining_transfers)
        {
          if (0 == (regs->sr & SSI_SR_RFNE_BITS))
          {
            return;
          }

          if (byte_width > 1)
          {
            regs->dr0 = __builtin_bswap32(*((uint32_t *)dataptr));
            dataptr += 4;
          }
          else
          {
            regs->dr0 = *((uint8_t *)dataptr);
            dataptr += 1;
          }
          remaining_transfers -= 1;
        }
      }

      if ((sr & SSI_SR_BUSY_BITS) != 0) // transfer complete ?
      {
        return;
      }
    }
    else
    {
      if (dmaused)
      {
        if (rxdma.Active())
        {
          return;
        }
      }
      else
      {
        while (remaining_transfers)
        {
          if (0 == (regs->sr & SSI_SR_RFNE_BITS))
          {
            return;
          }

          if (byte_width > 1)
          {
            *((uint32_t *)dataptr) = __builtin_bswap32(regs->dr0);
            dataptr += 4;
          }
          else
          {
            *((uint8_t *)dataptr) = regs->dr0;
            dataptr += 1;
          }
          remaining_transfers -= 1;
        }
      }
    }

    runstate = 1;
  }

  regs->ssienr = 0; // disable to allow configuration
  regs->ser = 0;

  busy = false;
#endif
}

#endif
