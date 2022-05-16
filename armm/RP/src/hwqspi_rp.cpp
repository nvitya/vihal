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
 *  file:     hwdma_rp.h
 *  brief:    RP2040 QSPI driver
 *  version:  1.00
 *  date:     2022-05-07
 *  authors:  nvitya
 *  notes:
 *    WARNING: this code is not compatible with XIP,
 *             the code must run from RAM to use the THwQspi on the RP2040
 *
 *    The code is rather specialized for the SPI Flash usage, which is usally
 *    fix soldered to the board.
 *    Only 32-bit transfers are supported.
*/


/* nvitya Personal comments:

The RP2040 QSPI (=SSI) unit is poorly and badly documented.
There are big differences in behaviour between single and multi-line modes and 8-bit and 32-bit data transfer.

The wait cycles seems not properly working in SSS 8-bit mode.

The SSI unit sends the command part in 32 bits when SSS mode selected in 32 bit mode.
So using SSS mode with 32-bit mode not recommended.
 (Theoretically the cmd and address could be put into the first word, but where to put the wait states?)

*/


#include "platform.h"

#include "hwpins.h"
#include "hwqspi_rp.h"
#include "rp_utils.h"

#include "hwqspi.h" // for eclipse indexer

#include "clockcnt.h"
#include "traces.h" // temporary, remove me !!!!!!!!!!!!!!!!!!

// Annoyingly, structs give much better code generation, as they re-use the base
// pointer rather than doing a PC-relative load for each constant pointer.

static ssi_hw_t *const ssi = (ssi_hw_t *) XIP_SSI_BASE;

typedef enum
{
    OUTOVER_NORMAL = 0,
    OUTOVER_INVERT,
    OUTOVER_LOW,
    OUTOVER_HIGH
//
} outover_t;

// GCC produces some heinous code if we try to loop over the pad controls,
// so structs it is
struct sd_padctrl
{
    io_rw_32 sd0;
    io_rw_32 sd1;
    io_rw_32 sd2;
    io_rw_32 sd3;
};

// Flash code may be heavily interrupted (e.g. if we are running USB MSC
// handlers concurrently with flash programming) so we control the CS pin
// manually
static void flash_cs_force(outover_t over)
{
    io_rw_32 *reg = (io_rw_32 *) (IO_QSPI_BASE + IO_QSPI_GPIO_QSPI_SS_CTRL_OFFSET);
#ifndef GENERAL_SIZE_HACKS
    *reg = (*reg & ~IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_BITS)
         | (over << IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_LSB);
#else
    // The only functions we want are FSEL (== 0 for XIP) and OUTOVER!
    *reg = over << IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_LSB;
#endif
    // Read to flush async bridge
    (void) *reg;
}


// Set up the SSI controller for standard SPI mode,i.e. for every byte sent we get one back
// This is only called by flash_exit_xip(), not by any of the other functions.
// This makes it possible for the debugger or user code to edit SPI settings
// e.g. baud rate, CPOL/CPHA.
void flash_init_spi()
{
    // Disable SSI for further config
    ssi->ssienr = 0;
    // Clear sticky errors (clear-on-read)
    (void) ssi->sr;
    (void) ssi->icr;
    // Hopefully-conservative baud rate for boot and programming
    ssi->baudr = 32;
    ssi->ctrlr0 =
            (SSI_CTRLR0_SPI_FRF_VALUE_STD << SSI_CTRLR0_SPI_FRF_LSB) | // Standard 1-bit SPI serial frames
            (7 << SSI_CTRLR0_DFS_32_LSB) | // 8 clocks per data frame
            (SSI_CTRLR0_TMOD_VALUE_TX_AND_RX << SSI_CTRLR0_TMOD_LSB);  // TX and RX FIFOs are both used for every byte
    // Slave selected when transfers in progress
    ssi->ser = 1;
    // Re-enable
    ssi->ssienr = 1;
}

// Also allow any unbounded loops to check whether the above abort condition
// was asserted, and terminate early
int flash_was_aborted()
{
    return *(io_rw_32 *) (IO_QSPI_BASE + IO_QSPI_GPIO_QSPI_SD1_CTRL_OFFSET)
           & IO_QSPI_GPIO_QSPI_SD1_CTRL_INOVER_BITS;
}


// Put bytes from one buffer, and get bytes into another buffer.
// These can be the same buffer.
// If tx is NULL then send zeroes.
// If rx is NULL then all read data will be dropped.
//
// If rx_skip is nonzero, this many bytes will first be consumed from the FIFO,
// before reading a further count bytes into *rx.
// E.g. if you have written a command+address just before calling this function.
void flash_put_get(const uint8_t * tx, uint8_t * rx, unsigned count, unsigned rx_skip)
{
    // Make sure there is never more data in flight than the depth of the RX
    // FIFO. Otherwise, when we are interrupted for long periods, hardware
    // will overflow the RX FIFO.
    const unsigned max_in_flight = 16 - 2; // account for data internal to SSI
    unsigned tx_count = count;
    unsigned rx_count = count;

    while (tx_count || rx_skip || rx_count)
    {
        // NB order of reads, for pessimism rather than optimism
        uint32_t tx_level = ssi_hw->txflr;
        uint32_t rx_level = ssi_hw->rxflr;
        bool did_something = false; // Expect this to be folded into control flow, not register
        if (tx_count && tx_level + rx_level < max_in_flight)
        {
            ssi->dr0 = (uint32_t) (tx ? *tx++ : 0);
            --tx_count;
            did_something = true;
        }

        if (rx_level)
        {
            uint8_t rxbyte = ssi->dr0;
            did_something = true;
            if (rx_skip) {
                --rx_skip;
            }
            else
            {
                if (rx)  *rx++ = rxbyte;
                --rx_count;
            }
        }
        // APB load costs 4 cycles, so only do it on idle loops (our budget is 48 cyc/byte)
        if (!did_something && __builtin_expect(flash_was_aborted(), 0))
            break;
    }
    flash_cs_force(OUTOVER_HIGH);
}

// Convenience wrapper for above
// (And it's hard for the debug host to get the tight timing between
// cmd DR0 write and the remaining data)
void flash_do_cmd(uint8_t cmd, const uint8_t *tx, uint8_t *rx, unsigned count)
{
    flash_cs_force(OUTOVER_LOW);
    ssi->dr0 = cmd;
    flash_put_get(tx, rx, count, 1);
}

// Timing of this one is critical, so do not expose the symbol to debugger etc
static inline void flash_put_cmd_addr(uint8_t cmd, uint32_t addr)
{
    flash_cs_force(OUTOVER_LOW);
    addr |= cmd << 24;
    for (int i = 0; i < 4; ++i)
    {
        ssi->dr0 = addr >> 24;
        addr <<= 8;
    }
}

// Sequence:
// 1. CSn = 1, IO = 4'h0 (via pulldown to avoid contention), x32 clocks
// 2. CSn = 0, IO = 4'hf (via pullup to avoid contention), x32 clocks
// 3. CSn = 1 (brief deassertion)
// 4. CSn = 0, MOSI = 1'b1 driven, x16 clocks
//
// Part 4 is the sequence suggested in W25X10CL datasheet.
// Parts 1 and 2 are to improve compatibility with Micron parts

void flash_exit_xip()
{
    struct sd_padctrl * qspi_sd_padctrl = (struct sd_padctrl *) (PADS_QSPI_BASE + PADS_QSPI_GPIO_QSPI_SD0_OFFSET);
    io_rw_32 *qspi_ss_ioctrl = (io_rw_32 *) (IO_QSPI_BASE + IO_QSPI_GPIO_QSPI_SS_CTRL_OFFSET);
    uint8_t buf[2];
    buf[0] = 0xff;
    buf[1] = 0xff;

    flash_init_spi();

    uint32_t padctrl_save = qspi_sd_padctrl->sd0;
    uint32_t padctrl_tmp = (padctrl_save
                            & ~(PADS_QSPI_GPIO_QSPI_SD0_OD_BITS | PADS_QSPI_GPIO_QSPI_SD0_PUE_BITS |
                                PADS_QSPI_GPIO_QSPI_SD0_PDE_BITS)
                           ) | PADS_QSPI_GPIO_QSPI_SD0_OD_BITS | PADS_QSPI_GPIO_QSPI_SD0_PDE_BITS;

    // First two 32-clock sequences
    // CSn is held high for the first 32 clocks, then asserted low for next 32
    flash_cs_force(OUTOVER_HIGH);
    for (int i = 0; i < 2; ++i)
    {
        // This gives 4 16-bit offset store instructions. Anything else seems to
        // produce a large island of constants
        qspi_sd_padctrl->sd0 = padctrl_tmp;
        qspi_sd_padctrl->sd1 = padctrl_tmp;
        qspi_sd_padctrl->sd2 = padctrl_tmp;
        qspi_sd_padctrl->sd3 = padctrl_tmp;

        // Brief delay (~6000 cyc) for pulls to take effect
        uint32_t delay_cnt = 1u << 11;
        asm volatile (
        "1: \n\t"
        "sub %0, %0, #1 \n\t"
        "bne 1b"
        : "+r" (delay_cnt)
        );

        flash_put_get(nullptr, nullptr, 4, 0);

        padctrl_tmp = (padctrl_tmp
                       & ~PADS_QSPI_GPIO_QSPI_SD0_PDE_BITS)
                      | PADS_QSPI_GPIO_QSPI_SD0_PUE_BITS;

        flash_cs_force(OUTOVER_LOW);
    }

    // Restore IO/pad controls, and send 0xff, 0xff. Put pullup on IO2/IO3 as
    // these may be used as WPn/HOLDn at this point, and we are now starting
    // to issue serial commands.

    qspi_sd_padctrl->sd0 = padctrl_save;
    qspi_sd_padctrl->sd1 = padctrl_save;
    padctrl_save = ((padctrl_save & ~PADS_QSPI_GPIO_QSPI_SD0_PDE_BITS) | PADS_QSPI_GPIO_QSPI_SD0_PUE_BITS);
    qspi_sd_padctrl->sd2 = padctrl_save;
    qspi_sd_padctrl->sd3 = padctrl_save;

    flash_cs_force(OUTOVER_LOW);
    flash_put_get(buf, nullptr, 2, 0);

    *qspi_ss_ioctrl = 0;
}

// ----------------------------------------------------------------------------
// Read

void flash_read_data(uint32_t addr, uint8_t * rx, unsigned count)
{
    flash_put_cmd_addr(0x03, addr);
    flash_put_get(nullptr, rx, count, 4);
}

uint32_t flash_read_jedec_id()
{
  uint32_t jid = 0;
  flash_do_cmd(0x9F, nullptr, (unsigned char *)&jid, 4);
  return jid;
}

uint32_t flash_read_jedec_id_32()
{
  uint32_t jid = 0;

  //flash_cs_force(OUTOVER_NORMAL);

  // Disable SSI for further config
  ssi->ssienr = 0;
  // Clear sticky errors (clear-on-read)
  (void) ssi->sr;
  (void) ssi->icr;
  // Hopefully-conservative baud rate for boot and programming
  ssi->baudr = 32;


  uint32_t cr0 = (0
    | (0  << 24)  // SSTE: slave select toggle
    | (0  << 21)  // SPI_FRF(2): 0 = single, 1 = dual, 2 = quad
    | (7  << 16)  // DFS_32(5): 31 = 32 bit frames
    | (0  << 12)  // CFS(4): control frame size
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
    | (0xCC  << 24)  // XIP_CMD(8): instruction code in XIP mode when INST_L = 8-bit
    | (0  << 18)  // SPI_RXDS_EN: read data strobe enable
    | (0  << 17)  // INST_DDR_EN
    | (0  << 16)  // SPI_DDR_EN
    | (0  << 11)  // WAIT_CYCLES(5)
    | (2  <<  8)  // INST_L(2): 0 = no instruction, 2 = 8-bit instruction, 3 = 16-bit
    | (6  <<  2)  // ADDR_L(4): in 4 bit increments
    | (0  <<  0)  // TRANS_TYPE(2): 0 = all single, 1 = cmd single and addr multi-line, 2 = cmd and address multi-line
  );


  ssi->ctrlr0 = cr0;
  ssi->spi_ctrlr0 = spi_cr0;
  ssi->dmacr = 0;
  ssi->ctrlr1 = 3;

  // Slave selected when transfers in progress
  ssi->ser = 1;

  // Re-enable
  ssi->ssienr = 1;

  //ssi->ser = 1;

  ssi->dr0 = 0x9F;
  ssi->dr0 = 0x555555;
  ssi->dr0 = 0xCCCCCC;
  ssi->dr0 = 0x111111;

  // wait for the start
  while (0 == (ssi->sr & SSI_SR_BUSY_BITS))
  {
    // wait until it is really started
  }

  // wait for the end
  while (0 != (ssi->sr & SSI_SR_BUSY_BITS))
  {
    // wait until it is really started
  }

  while (ssi->sr & SSI_SR_RFNE_BITS)
  {
    jid = ssi->dr0;
    TRACE(" RX: %08X\r\n", jid);
  }

  return jid;
}

bool THwQspi_rp::InitInterface()
{
  int i;
	// QSPI / SPIFI pins

  // Then mux XIP block onto internal QSPI flash pads
  io_rw_32 * iobank1 = (io_rw_32 *) IO_QSPI_BASE;
  for (int i = 0; i < 6; ++i)
  {
    iobank1[2 * i + 1] = 0;
  }

	txdma.Init(txdmachannel, DREQ_XIP_SSITX);
	rxdma.Init(rxdmachannel, DREQ_XIP_SSIRX);

	return true;
}

bool THwQspi_rp::Init()
{
  int i;
	unsigned tmp;

	initialized = false;

  regs = ssi_hw;

  //flash_exit_xip();
  //flash_cs_force(OUTOVER_NORMAL);

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

#if 0
  TRACE("QSPI init...\r\n");

  uint32_t jid;

  //uint32_t jid = flash_read_jedec_id();
  //TRACE("Jedec ID=%08X\r\n", jid);

  //flash_cs_force(OUTOVER_NORMAL);

  //delay_us(100);

  jid = flash_read_jedec_id_32();

  TRACE("my jedec ID=%08X\r\n", jid);
#endif

	if (!txdma.initialized || !rxdma.initialized)
	{
		return false;
	}

	xip_ctrl_hw->ctrl &= ~XIP_CTRL_EN_BITS;  // disable XIP cache

	regs->ssienr = 0; // disable to allow configuration

  // Clear sticky errors (clear-on-read)
  if (regs->sr) { }
  if (regs->icr) { }

  unsigned baseclock = SystemCoreClock;
  unsigned speeddiv = baseclock / speed;
  if (speeddiv * speed < baseclock)  ++speeddiv;

  regs->baudr = speeddiv;

/*
  ssi->ctrlr0 =
          (SSI_CTRLR0_SPI_FRF_VALUE_STD << SSI_CTRLR0_SPI_FRF_LSB) | // Standard 1-bit SPI serial frames
          (7 << SSI_CTRLR0_DFS_32_LSB) | // 8 clocks per data frame
          (SSI_CTRLR0_TMOD_VALUE_TX_AND_RX << SSI_CTRLR0_TMOD_LSB);  // TX and RX FIFOs are both used for every byte
*/

  // Slave selected when transfers in progress
  regs->ser = 1;

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
}

int THwQspi_rp::StartReadData(unsigned acmd, unsigned address, void * dstptr, unsigned len)
{
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

  unsigned fields = ((acmd >> 8) & 0xF);

  // check data transfer width
  if (fields & 8)
  {
    // use 32 bit mode !
    byte_width = 4;
    cr0 |= (0
      | (mlcode << 21)  // SPI_FRF(2): multi line data
      | (31     << 16)  // DFS_32(5): 31 = 32 bit mode
    );

    // control (cmd + address) transfer width
    if (fields & 1) // multiline command ?
    {
      spi_cr0 |= (2 << 0); // TRANS_TYPE(2): 2 = multi line command and address (and data)
    }
    else if (fields & 2) // multiline address ?
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
  unsigned dummybytes = ((acmd >> 20) & 0xF);
  if (dummybytes)
  {
    // dummy required
    if (8 == dummybytes)
    {
      dummybytes = dummysize;
    }

    unsigned dummybits = (dummybytes * 8);
    spi_cr0 |= (dummybits << 11);
  }

  // address
  unsigned rqaddrlen = ((acmd >> 16) & 0xF);
  if (rqaddrlen)
  {
    if (8 == rqaddrlen)
    {
      addr_mode_len = addrlen;
    }
    else
    {
      addr_mode_len = rqaddrlen;
    }
    addrdata = address;
  }
  else
  {
    addr_mode_len = 0;
    addrdata = 0;
  }

  // mode / alternate bytes
  unsigned ablen = ((acmd >> 28) & 0xF);
  if (ablen)
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

  if (dummybytes && (1 == byte_width))
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
}

int THwQspi_rp::StartWriteData(unsigned acmd, unsigned address, void * srcptr, unsigned len)
{
  return HWERR_NOTIMPL;
}


void THwQspi_rp::Run()
{
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

      if ((sr & SSI_SR_BUSY_BITS) == 0) // transfer complete ?
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

  busy = false;
}
