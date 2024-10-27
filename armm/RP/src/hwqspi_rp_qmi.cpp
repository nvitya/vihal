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

  unsigned baseclock = SystemCoreClock;
  unsigned speeddiv = baseclock / speed;
  if (speeddiv * speed < baseclock)  ++speeddiv;

	// set to direct mode

	csr_base = (0
	  | (rx_delay << 30)  // RXDELAY(2)
	  | (speeddiv << 22)  // CLKDIV(8)
	  | (0  << 18)  // RXLEVEL(3)
	  | (0  << 17)  // RXFULL
	  | (0  << 16)  // RXEMPTY
	  | (0  << 12)  // TXLEVEL(3)
	  | (0  << 11)  // TXEMPTY
	  | (0  << 10)  // TXFULL
	  | (0  <<  7)  // AUTO_CS1N
	  | (0  <<  6)  // AUTO_CS0N
	  | (0  <<  3)  // ASSERT_CS1N
	  | (0  <<  2)  // ASSERT_CS0N
	  | (0  <<  1)  // BUSY
	  | (1  <<  0)  // EN: 1 = enable direct mode
	);

  regs->direct_csr = csr_base;

	rxdma.Prepare(false, (void *)&regs->direct_rx, 0);
	txdma.Prepare(true,  (void *)&regs->direct_tx, 0);

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
}

void THwQspi_rp::SetCs(uint8_t alevel)
{
  if (alevel)
  {
    regs->direct_csr = csr_base;
  }
  else
  {
    regs->direct_csr = (csr_base | (1 << (2 + cs_index)));
  }
}

void THwQspi_rp::FinishReadWrite()
{
  SetCs(1);
  busy = false;
}

int THwQspi_rp::StartReadData(unsigned acmd, unsigned address, void * dstptr, unsigned len)
{
  if (busy)
  {
    return HWERR_BUSY;
  }

  istx = false;
  dataptr = (uint8_t *)dstptr;
  datalen = len;
  addrdata = address;
  remainingbytes = datalen;

  regs->direct_csr = csr_base;

  StartReadWrite(acmd);

  return HWERR_OK;
}

int THwQspi_rp::StartWriteData(unsigned acmd, unsigned address, void * srcptr, unsigned len)
{
  if (busy)
  {
    return HWERR_BUSY;
  }

  istx = true;
  dataptr = (uint8_t *)srcptr;
  datalen = len;
  addrdata = address;
  remainingbytes = datalen;

  regs->direct_csr = csr_base;

  StartReadWrite(acmd);

  return HWERR_OK;
}

void THwQspi_rp::StartReadWrite(unsigned acmd)
{
  uint32_t fcmd;
  uint32_t iwidth;

  regs->direct_csr = csr_base;

  // prepare the command buffer
  cmdbuflen = 0;
  cmdbufidx = 0;

  // COMMAND
  fcmd = (0
    | (1  <<  20)  // NOPUSH: 0 = push rx byte into the rx fifo, 1 = do not push rx into rx fifo for this tx
    | (1  <<  19)  // OE: 0 = disable output, 1 = enable output
    | (0  <<  18)  // DWIDTH: 0 = 8, 1 = 16
    | (0  <<  16)  // IWIDTH(2): 0 = single line, 1 = dual, 2 = quad
  );
  if (acmd & QSPICM_2B)
  {
    fcmd |= QMI_TXFIFO_D16;
  }
  if (acmd & (1 << QSPICM_LN_CMD_POS))
  {
    fcmd |= (mlcode << QMI_TXFIFO_MLPOS);
  }
  cmdbuf[cmdbuflen++] = (fcmd | (acmd & QSPICM_CMD_SMASK));

  // ADDRESS
  addr_len = ((acmd >> QSPICM_ADDR_POS) & QSPICM_ADDR_SMASK);
  if (addr_len)
  {
    if (addr_len > 4)
    {
      addr_len = addrlen;  // default addrlen
    }

    fcmd = (0
      | (1  <<  20)  // NOPUSH: 0 = push rx byte into the rx fifo, 1 = do not push rx into rx fifo for this tx
      | (1  <<  19)  // OE: 0 = disable output, 1 = enable output
      | (0  <<  18)  // DWIDTH: 0 = 8, 1 = 16
      | (0  <<  16)  // IWIDTH(2): 0 = single line, 1 = dual, 2 = quad
    );
    if (acmd & (1 << QSPICM_LN_ADDR_POS))
    {
      fcmd |= (mlcode << QMI_TXFIFO_MLPOS);
    }
    if (addr_len > 3)   cmdbuf[cmdbuflen++] = (fcmd | ((addrdata >> 24) & 0xFF));
    if (addr_len > 2)   cmdbuf[cmdbuflen++] = (fcmd | ((addrdata >> 16) & 0xFF));
    if (addr_len > 1)   cmdbuf[cmdbuflen++] = (fcmd | ((addrdata >>  8) & 0xFF));
    if (addr_len > 0)   cmdbuf[cmdbuflen++] = (fcmd | ((addrdata >>  0) & 0xFF));
  }
  else
  {
    addr_len = 0;
    addrdata = 0;
  }

  // MODE / ALTERNATE BYTES
  if (acmd & QSPICM_MODE)
  {
    fcmd = (0
      | (1  <<  20)  // NOPUSH: 0 = push rx byte into the rx fifo, 1 = do not push rx into rx fifo for this tx
      | (1  <<  19)  // OE: 0 = disable output, 1 = enable output
      | (0  <<  18)  // DWIDTH: 0 = 8, 1 = 16
      | (0  <<  16)  // IWIDTH(2): 0 = single line, 1 = dual, 2 = quad
    );
    if (acmd & (1 << QSPICM_LN_ADDR_POS))
    {
      fcmd |= (mlcode << QMI_TXFIFO_MLPOS);
    }
    if (modelen > 3)   cmdbuf[cmdbuflen++] = (fcmd | ((modedata >> 24) & 0xFF));
    if (modelen > 2)   cmdbuf[cmdbuflen++] = (fcmd | ((modedata >> 16) & 0xFF));
    if (modelen > 1)   cmdbuf[cmdbuflen++] = (fcmd | ((modedata >>  8) & 0xFF));
    if (modelen > 0)   cmdbuf[cmdbuflen++] = (fcmd | ((modedata >>  0) & 0xFF));
  }

  // DUMMY
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

    uint32_t dummybytes = (rqdummyc >> 3);
    fcmd = (0
      | (1  <<  20)  // NOPUSH: 0 = push rx byte into the rx fifo, 1 = do not push rx into rx fifo for this tx
      | (0  <<  19)  // OE: 0 = disable output, 1 = enable output
      | (0  <<  18)  // DWIDTH: 0 = 8, 1 = 16
      | (0  <<  16)  // IWIDTH(2): 0 = single line, 1 = dual, 2 = quad
    );
    fcmd = (0 | QMI_TXFIFO_NOPUSH); // disable output !
    if (acmd & (1 << QSPICM_LN_DATA_POS))
    {
      fcmd |= (mlcode << QMI_TXFIFO_MLPOS);
    }
    while (dummybytes)
    {
      cmdbuf[cmdbuflen++] = fcmd;
      --dummybytes;
    }
  }

  // DATA
  fcmd = (0
    | (0  <<  20)  // NOPUSH: 0 = push rx byte into the rx fifo, 1 = do not push rx into rx fifo for this tx
    | (0  <<  19)  // OE: 0 = disable output, 1 = enable output
    | (0  <<  18)  // DWIDTH: 0 = 8, 1 = 16
    | (0  <<  16)  // IWIDTH(2): 0 = single line, 1 = dual, 2 = quad
  );
  if (acmd & (1 << QSPICM_LN_DATA_POS))
  {
    fcmd |= (mlcode << QMI_TXFIFO_MLPOS);
  }
  if (istx)
  {
    fcmd |= (0
      | (1 << 20)  // NOPUSH: 1 = do not push rx into rx fifo for this tx
      | (1 << 19)  // OE: 0 = disable output, 1 = enable output
    );
  }
  data_fcmd = fcmd;
  remaining_fcmd = remainingbytes;

  //dmaused = (remaining_transfers > 0);
  dmaused = false; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  // empty rx fifo
  while (0 == (regs->direct_csr & QMI_DIRECT_CSR_RXEMPTY_BITS))
  {
    if (regs->direct_rx) { }
  }

  SetCs(0);  // assert CS

  busy = true;
  runstate = 0;

  Run();
}

void THwQspi_rp::Run()
{
  if (!busy)
  {
    return;
  }

  if (0 == runstate)  // command phase
  {
    if (dmaused)
    {
      //TODO: implement
    }
    else
    {
      while (cmdbufidx < cmdbuflen)
      {
        if (regs->direct_csr & QMI_DIRECT_CSR_TXFULL_BITS)
        {
          return;
        }

        regs->direct_tx = cmdbuf[cmdbufidx];
        ++cmdbufidx;
      }
    }

    // go to data phase

    if (remainingbytes)
    {
      if (istx)
      {
        runstate = 5;
      }
      else
      {
        runstate = 1;
      }
      Run();
      return;
    }
    else
    {
      FinishReadWrite();
      return;
    }
  }

  else if (1 == runstate)  // RX data phase without DMA
  {
    while (remaining_fcmd)
    {
      if (regs->direct_csr & QMI_DIRECT_CSR_TXFULL_BITS)
      {
        break;
      }

      regs->direct_tx = data_fcmd;
      --remaining_fcmd;
    }

    while (remainingbytes)
    {
      if (regs->direct_csr & QMI_DIRECT_CSR_RXEMPTY_BITS)
      {
        return;
      }

      *((uint8_t *)dataptr) = regs->direct_rx;
      ++dataptr;

      --remainingbytes;
    }

    FinishReadWrite();
    return;
  }

  else if (5 == runstate)  // TX data phase without DMA
  {
    while (remainingbytes)
    {
      if (regs->direct_csr & QMI_DIRECT_CSR_TXFULL_BITS)
      {
        return;
      }

      regs->direct_tx = (data_fcmd | *dataptr);
      ++dataptr;
      --remainingbytes;
    }

    FinishReadWrite();
    return;
  }

  else // unhandled phase !
  {
    //TODO: implement
  }
}


#endif
