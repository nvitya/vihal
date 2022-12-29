/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2022 Viktor Nagy, nvitya
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
 *  file:     hwqspi_esp.h
 *  brief:    Espressif QSPI driver
 *  date:     2022-12-23
 *  authors:  nvitya
*/

#include "hwqspi.h" // for eclipse indexer
//#include "hwqspi_esp.h" // for eclipse indexer

#include "esp_utils.h"


#include "clockcnt.h"
#include "traces.h" // temporary, remove me !!!!!!!!!!!!!!!!!!

bool THwQspi_esp::InitInterface()
{

	return true;
}

bool THwQspi_esp::Init()
{
  uint32_t sys_rst_mask;
  int i;
	unsigned tmp;

	initialized = false;

  mregs = SPIMEM1;
  SYSTEM->PERIP_CLK_EN0 |= SYSTEM_SPI01_CLK_EN;
  sys_rst_mask = SYSTEM_SPI01_RST;

  SYSTEM->PERIP_RST_EN0 |= sys_rst_mask;   // set system SPI reset
  if (SYSTEM->PERIP_RST_EN0) {} // some sync
  SYSTEM->PERIP_RST_EN0 &= ~sys_rst_mask;  // remove system SPI reset

  pcmdreg = &mregs->CMD;
  pwregs = &mregs->W[0];

  cmd_usr_bit = (1 << 18);

  mregs->USER = 0;

  mregs->CLOCK_GATE = 1;

  mregs->CTRL = (0
    | (0  <<  3)  // DUMMY_OUT: In the dummy phase the signal level of spi is output by the spi controller
    | (0  <<  7)  // FCMD_DUAL: Apply 2 signals during command phase 1:enable 0: disable
    | (0  <<  8)  // FCMD_QUAD: Apply 4 signals during command phase 1:enable 0: disable
    | (0  << 10)  // FCS_CRC_EN: initialize crc32 module before writing encrypted data to flash. Active low.
    | (0  << 11)  // TX_CRC_EN: initialize crc32 module before writing encrypted data to flash. Active low.
    | (0  << 13)  // FASTRD_MODE: This bit enable the bits: spi_mem_fread_qio  spi_mem_fread_dio  spi_mem_fread_qout and spi_mem_fread_dout. 1: enable 0: disable
    | (0  << 14)  // FREAD_DUAL: In the read operations  read-data phase apply 2 signals. 1: enable 0: disable
    | (0  << 15)  // RESANDRES: The Device ID is read out to SPI_MEM_RD_STATUS register   this bit combine with spi_mem_flash_res bit. 1: enable 0: disable
    | (0  << 18)  // Q_POL: The bit is used to set MISO line polarity  1=high, 0=low
    | (0  << 19)  // D_POL: The bit is used to set MOSI line polarity  1=high, 0=low
    | (0  << 20)  // FREAD_QUAD: In the read operations read-data phase apply 4 signals. 1: enable 0: disable
    | (0  << 21)  // WP: Write protect signal output when SPI is idle.  1: output high  0: output low
    | (0  << 22)  // WRSR_2B: two bytes data will be written to status register when it is set. 1: enable 0: disable
    | (0  << 23)  // FREAD_DIO: In the read operations address phase and read-data phase apply 2 signals. 1: enable 0: disable
    | (0  << 24)  // FREAD_QIO: In the read operations address phase and read-data phase apply 4 signals. 1: enable 0: disable
  );

  mregs->CTRL1 = (0
    | (0  <<  0)  // CLK_MODE(2): SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3: SPI clock is alwasy on.
    | (0  <<  2)  // CS_HOLD_DLY_RES(10): After RES/DP/HPM command is sent  SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 512) SPI_CLK cycles.
    | (0  << 30)  // RXFIFO_RST: SPI0 RX FIFO reset signal
    | (0  << 31)  // RXFIFO_WFULL_ERR: 1: SPI0 RX FIFO write full error  Cache/EDMA do not read all the data out. 0: Not error.
  );

  mregs->CTRL2 = (0
    | (0  <<  0)  // CS_SETUP_TIME(5): (cycles-1) of prepare phase by spi clock this bits are combined with spi_mem_cs_setup bit
    | (0  <<  5)  // CS_HOLD_TIME(5): Spi cs signal is delayed to inactive by spi clock this bits are combined with spi_mem_cs_hold bit
    | (0  << 25)  // CS_HOLD_DELAY(6): These bits are used to set the minimum CS high time tSHSL between SPI burst transfer when accesses to flash. tSHSL is (SPI_MEM_CS_HOLD_DELAY[5:0] + 1) MSPI core clock cycles.
    | (0  << 31)  // SYNC_RESET: The FSM will be reset.
  );

  mregs->USER = (0
    | (0  <<  6)  // CS_HOLD: spi cs keep low when spi is in  done  phase. 1: enable 0: disable.
    | (0  <<  7)  // CS_SETUP: spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable.
    | (0  <<  9)  // CK_OUT_EDGE: the bit combined with spi_mem_mosi_delay_mode bits to set mosi signal delay mode.
    | (0  << 12)  // FWRITE_DUAL: In the write operations read-data phase apply 2 signals
    | (0  << 13)  // FWRITE_QUAD: In the write operations read-data phase apply 4 signals
    | (0  << 14)  // FWRITE_DIO: In the write operations address phase and read-data phase apply 2 signals.
    | (0  << 15)  // FWRITE_QIO: In the write operations address phase and read-data phase apply 4 signals.
    | (0  << 24)  // USR_MISO_HIGHPART: read-data phase only access to high-part of the buffer spi_mem_w8~spi_mem_w15. 1: enable 0: disable.
    | (0  << 25)  // USR_MOSI_HIGHPART: write-data phase only access to high-part of the buffer spi_mem_w8~spi_mem_w15. 1: enable 0: disable.
    | (0  << 26)  // USR_DUMMY_IDLE: SPI clock is disable in dummy phase when the bit is enable.
    | (0  << 27)  // USR_MOSI: This bit enable the write-data phase of an operation.
    | (0  << 28)  // USR_MISO: This bit enable the read-data phase of an operation.
    | (0  << 29)  // USR_DUMMY: This bit enable the dummy phase of an operation.
    | (0  << 30)  // USR_ADDR: This bit enable the address phase of an operation.
    | (0  << 31)  // USR_COMMAND: This bit enable the command phase of an operation.
  );

  mregs->USER1 = (0
    | (0  <<  0)  // USR_DUMMY_CYCLELEN(6): The length in spi_mem_clk cycles of dummy phase. The register value shall be (cycle_num-1).
    | (23 << 26)  // USR_ADDR_BITLEN(6): The length in bits of address phase. The register value shall be (bit_num-1).
  );

  mregs->USER2 = (0
    | (0x9F <<  0)  // USR_COMMAND_VALUE(16): The value of command
    | (7  << 28)  // USR_COMMAND_BITLEN(4): The length in bits of command phase. The register value shall be (bit_num-1)
  );

  //mregs->MOSI_DLEN = 7;
  //mregs->MISO_DLEN = 7;

  //mregs->RD_STATUS = 0;

  mregs->MISC = (0
    | (0  <<  0)  // CS0_DIS: SPI_CS0 pin enable  1: disable SPI_CS0  0: SPI_CS0 pin is active to select SPI device  such as flash  external RAM and so on.
    | (1  <<  1)  // CS1_DIS: SPI_CS1 pin enable  1: disable SPI_CS1  0: SPI_CS1 pin is active to select SPI device  such as flash  external RAM and so on.
    | (0  <<  9)  // CK_IDLE_EDGE:        1: spi clk line is high when idle     0: spi clk line is low when idle
    | (0  << 10)  // CS_KEEP_ACTIVE:      spi cs line keep low when the bit is set.
  );

  mregs->CACHE_FCTRL = (0
    | (0  <<  1)  // USR_ADDR_4BYTE: For SPI1   cache  read flash with 4 bytes address  1: enable  0:disable.
    | (0  <<  3)  // FDIN_DUAL: For SPI1  din phase apply 2 signals. 1: enable 0: disable. The bit is the same with spi_mem_fread_dio.
    | (0  <<  4)  // FDOUT_DUAL: For SPI1  dout phase apply 2 signals. 1: enable 0: disable. The bit is the same with spi_mem_fread_dio.
    | (0  <<  5)  // FADDR_DUAL: For SPI1  address phase apply 2 signals. 1: enable 0: disable.  The bit is the same with spi_mem_fread_dio.
    | (0  <<  6)  // FDIN_QUAD: For SPI1  din phase apply 4 signals. 1: enable 0: disable.  The bit is the same with spi_mem_fread_qio.
    | (0  <<  7)  // FDOUT_QUAD: For SPI1  dout phase apply 4 signals. 1: enable 0: disable.  The bit is the same with spi_mem_fread_qio.
    | (0  <<  8)  // FADDR_QUAD: For SPI1  address phase apply 4 signals. 1: enable 0: disable.  The bit is the same with spi_mem_fread_qio.
  );

  mregs->ADDR = 0;


  // clock configuration

  basespeed = 80000000;
  if (speed > basespeed)  speed = basespeed;
  uint32_t clkdiv = basespeed / speed;

  uint32_t clkequ = (speed == basespeed ? 1 : 0);
  uint32_t clkcnt_h = clkdiv / 2;
  if (clkcnt_h > 1)  --clkcnt_h;

  mregs->CLOCK = (0
    | ((clkdiv - 1)  <<  0)  // CLKCNT_L(8): must be the same as CLKCNT_N
    | (clkcnt_h      <<  8)  // CLKCNT_H(8)
    | ((clkdiv - 1)  << 16)  // CLKCNT_N(8)
    | (clkequ        << 31)  // CLK_EQU_SYSCLK
  );

  mregs->CLOCK_GATE = (0
    | (1  <<  0)  // CLK_EN
  );

  mlqio = (multi_line_count > 2);
  mldio = (multi_line_count == 2);

  initialized = true;
  return true;
}

int THwQspi_esp::StartReadData(unsigned acmd, unsigned address, void * dstptr, unsigned len)
{
  if (busy)
  {
    return HWERR_BUSY;
  }

  istx = false;
  dataptr = (uint8_t *)dstptr;
  datalen = len;
  remainingbytes = datalen;

  unsigned fields = ((acmd >> 8) & 0xF);

  uint32_t rctrl = (0
    | (0  <<  3)  // DUMMY_OUT: In the dummy phase the signal level of spi is output by the spi controller
    | (0  <<  7)  // FCMD_DUAL: Apply 2 signals during command phase 1:enable 0: disable
    | (0  <<  8)  // FCMD_QUAD: Apply 4 signals during command phase 1:enable 0: disable
    | (0  << 10)  // FCS_CRC_EN: initialize crc32 module before writing encrypted data to flash. Active low.
    | (0  << 11)  // TX_CRC_EN: initialize crc32 module before writing encrypted data to flash. Active low.
    | (0  << 13)  // FASTRD_MODE: This bit enable the bits: spi_mem_fread_qio  spi_mem_fread_dio  spi_mem_fread_qout and spi_mem_fread_dout. 1: enable 0: disable
    | (0  << 14)  // FREAD_DUAL: In the read operations  read-data phase apply 2 signals. 1: enable 0: disable
    | (0  << 15)  // RESANDRES: The Device ID is read out to SPI_MEM_RD_STATUS register   this bit combine with spi_mem_flash_res bit. 1: enable 0: disable
    | (0  << 18)  // Q_POL: The bit is used to set MISO line polarity  1=high, 0=low
    | (0  << 19)  // D_POL: The bit is used to set MOSI line polarity  1=high, 0=low
    | (0  << 20)  // FREAD_QUAD: In the read operations read-data phase apply 4 signals. 1: enable 0: disable
    | (0  << 21)  // WP: Write protect signal output when SPI is idle.  1: output high  0: output low
    | (0  << 22)  // WRSR_2B: two bytes data will be written to status register when it is set. 1: enable 0: disable
    | (0  << 23)  // FREAD_DIO: In the read operations address phase and read-data phase apply 2 signals. 1: enable 0: disable
    | (0  << 24)  // FREAD_QIO: In the read operations address phase and read-data phase apply 4 signals. 1: enable 0: disable
  );


  uint32_t ruser = (0
    | (0  <<  6)  // CS_HOLD: spi cs keep low when spi is in  done  phase. 1: enable 0: disable.
    | (0  <<  7)  // CS_SETUP: spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable.
    | (0  <<  9)  // CK_OUT_EDGE: the bit combined with spi_mem_mosi_delay_mode bits to set mosi signal delay mode.
    | (0  << 12)  // FWRITE_DUAL: In the write operations read-data phase apply 2 signals
    | (0  << 13)  // FWRITE_QUAD: In the write operations read-data phase apply 4 signals
    | (0  << 14)  // FWRITE_DIO: In the write operations address phase and read-data phase apply 2 signals.
    | (0  << 15)  // FWRITE_QIO: In the write operations address phase and read-data phase apply 4 signals.
    | (0  << 24)  // USR_MISO_HIGHPART: read-data phase only access to high-part of the buffer spi_mem_w8~spi_mem_w15. 1: enable 0: disable.
    | (0  << 25)  // USR_MOSI_HIGHPART: write-data phase only access to high-part of the buffer spi_mem_w8~spi_mem_w15. 1: enable 0: disable.
    | (0  << 26)  // USR_DUMMY_IDLE: SPI clock is disable in dummy phase when the bit is enable.
    | (0  << 27)  // USR_MOSI: This bit enable the write-data phase of an operation.
    | (0  << 28)  // USR_MISO: This bit enable the read-data phase of an operation.
    | (0  << 29)  // USR_DUMMY: This bit enable the dummy phase of an operation.
    | (0  << 30)  // USR_ADDR: This bit enable the address phase of an operation.
    | (1  << 31)  // USR_COMMAND: This bit enable the command phase of an operation.
  );

  uint32_t ruser1 = (0
    | (0  <<  0)  // USR_DUMMY_CYCLELEN(6): The length in spi_mem_clk cycles of dummy phase. The register value shall be (cycle_num-1).
    | (0  << 26)  // USR_ADDR_BITLEN(6): The length in bits of address phase. The register value shall be (bit_num-1).
  );

  // data
  if (datalen > 0)
  {
    ruser |= (1  << 28);  // USR_MISO: This bit enable the read-data phase of an operation.

    if (fields & 8) // multi line data ?
    {
      rctrl |= (0
        | (1     << 13)  // FASTRD_MODE: This bit enable the bits: spi_mem_fread_qio  spi_mem_fread_dio  spi_mem_fread_qout and spi_mem_fread_dout. 1: enable 0: disable
        | (mldio << 14)  // FREAD_DUAL: In the read operations  read-data phase apply 2 signals. 1: enable 0: disable
        | (mlqio << 20)  // FREAD_QUAD: In the read operations read-data phase apply 4 signals. 1: enable 0: disable
      );
    }
    else
    {
      // single line data
    }

    mregs->MISO_DLEN = (datalen << 3) - 1;
  }

  // address
  unsigned rqaddrlen = ((acmd >> 16) & 0xF);
  if (rqaddrlen)
  {
    ruser |= (1  << 30);  // USR_ADDR: This bit enable the address phase of an operation.

    if (fields & 2)
    {
      rctrl |= (0
        | (1     << 13)  // FASTRD_MODE: This bit enable the bits: spi_mem_fread_qio  spi_mem_fread_dio  spi_mem_fread_qout and spi_mem_fread_dout. 1: enable 0: disable
        | (mldio << 23)  // FREAD_DIO: In the read operations address phase and read-data phase apply 2 signals. 1: enable 0: disable
        | (mlqio << 24)  // FREAD_QIO: In the read operations address phase and read-data phase apply 4 signals. 1: enable 0: disable
      );
    }
    else
    {
      // single line address
    }

    if (8 == rqaddrlen)
    {
      ruser1 |= ((addrlen-1) << 26); // default address length
    }
    else
    {
      ruser1 |= ((rqaddrlen-1) << 26); // requested address length
    }

    mregs->ADDR = address;

    ruser |= (1 << 30); // enable the address
  }

  // dummy
  unsigned dummybytes = ((acmd >> 20) & 0xF);
  if (dummybytes)
  {
    ruser |= (1  << 29);  // USR_DUMMY: This bit enable the dummy phase of an operation.

    // dummy required
    if (8 == dummybytes)
    {
      dummybytes = dummysize;
    }

    unsigned dummybits = (dummybytes << 3); // *8
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

    ruser1 |= ((dummybits-1)  <<  0);  // USR_DUMMY_CYCLELEN(6): The length in spi_mem_clk cycles of dummy phase. The register value shall be (cycle_num-1).
  }

  // command
  if (fields & 1) // multi line command ?
  {
    rctrl = (0
      | (1     << 13)  // FASTRD_MODE: This bit enable the bits: spi_mem_fread_qio  spi_mem_fread_dio  spi_mem_fread_qout and spi_mem_fread_dout. 1: enable 0: disable
      | (mldio <<  7)  // FCMD_DUAL: Apply 2 signals during command phase 1:enable 0: disable
      | (mlqio <<  8)  // FCMD_QUAD: Apply 4 signals during command phase 1:enable 0: disable
    );
  }

  uint32_t ruser2 = (0
    | ((acmd & 0xFF) <<  0)  // USR_COMMAND_VALUE(16): The value of command
    | (7  << 28)  // USR_COMMAND_BITLEN(4): The length in bits of command phase. The register value shall be (bit_num-1)
  );

  mregs->CTRL = rctrl;
  mregs->USER = ruser;
  mregs->USER1 = ruser1;
  mregs->USER2 = ruser2;

  mregs->CMD |= cmd_usr_bit; // start the CMD

  busy = true;
  runstate = 0;

  return HWERR_OK;
}

int THwQspi_esp::StartWriteData(unsigned acmd, unsigned address, void * srcptr, unsigned len)
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
  //dmaused = false; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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

void THwQspi_esp::Run()
{
  if (!busy)
  {
    return;
  }

  if (0 == runstate)
  {
    if (mregs->CMD & cmd_usr_bit)
    {
      return; // command is still running
    }

    if (istx)
    {
#if 0
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
#endif
    }
    else
    {

      // read the data

      uint32_t d32cnt = ((remainingbytes + 3) >> 2);
      uint32_t * src32 = (uint32_t *)&pwregs;
      uint32_t * end32 = src32 + d32cnt;
      uint32_t * dst32 = (uint32_t *)dataptr;
      while (src32 < end32)
      {
        *dst32++ = *src32++;
      }
    }

    runstate = 1;
  }

  busy = false;
}
