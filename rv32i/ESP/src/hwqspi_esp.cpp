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
  #if defined(MCUSF_32C3)
    SYSTEM->PERIP_CLK_EN0 |= SYSTEM_SPI01_CLK_EN;
    sys_rst_mask = SYSTEM_SPI01_RST;
    SYSTEM->PERIP_RST_EN0 |= sys_rst_mask;   // set system SPI reset
    SYSTEM->PERIP_RST_EN0 &= ~sys_rst_mask;  // remove system SPI reset
  #else
    PCR->MSPI_CONF |=  PCR_MSPI_CLK_EN;
    PCR->MSPI_CONF |=  PCR_MSPI_RST_EN;
    PCR->MSPI_CONF &= ~PCR_MSPI_RST_EN;
  #endif

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
  remainingbytes = len;
  if (len <= 64)  chunksize = len;
  else            chunksize = 64;

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

  uint32_t rmisc = (0
    | (0  <<  0)  // CS0_DIS
    | (1  <<  1)  // CS1_DIS
    | (0  <<  9)  // CK_IDLE_EDGE
    | (0  << 10)  // CS_KEEP_ACTIVE
  );

  // data
  if (chunksize > 0)
  {
    ruser |= (1  << 28);  // USR_MISO: This bit enable the read-data phase of an operation.

    if (acmd & (1 << QSPICM_LN_DATA_POS))  // multi line data ?
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

    mregs->MISO_DLEN = (chunksize << 3) - 1;

    if (chunksize < datalen)
    {
      rmisc |= (1  << 10);  // CS_KEEP_ACTIVE
    }
  }

  // address
  unsigned rqaddrlen = ((acmd >> QSPICM_ADDR_POS) & QSPICM_ADDR_SMASK);
  if (rqaddrlen)
  {
    if (rqaddrlen > 4)
    {
      rqaddrlen = addrlen;  // default addrlen
    }

    if (acmd & (1 << QSPICM_LN_ADDR_POS))  // multi-line address ?
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

    ruser  |= (1  << 30);  // USR_ADDR: This bit enable the address phase of an operation.
    ruser1 |= (((rqaddrlen << 3)-1) << 26); // requested address length

    mregs->ADDR = address;
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

    ruser1 |= ((rqdummyc-1)  <<  0);  // USR_DUMMY_CYCLELEN(6): The length in spi_mem_clk cycles of dummy phase. The register value shall be (cycle_num-1).
  }

  // command
  if (acmd & (1 << QSPICM_LN_CMD_POS))  // multi line command ?
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
  mregs->MISC = rmisc;

  mregs->CMD |= cmd_usr_bit; // start the CMD

  busy = true;
  runstate = 0;

  return HWERR_OK;
}

/* The ESP32 at writing reads ahead the next word and stores internally for consecutive write blocks
 * therefore the write happens only in 60 byte chunks and the last dword holds the first dword of the next transfer
 */

#define WRITE_CHUNK_MAX  60

int THwQspi_esp::StartWriteData(unsigned acmd, unsigned address, void * srcptr, unsigned len)
{

  if (busy)
  {
    return HWERR_BUSY;
  }

  istx = true;
  dataptr = (uint8_t *)srcptr;
  datalen = len;
  remainingbytes = len;
  if (len <= 64)  chunksize = len;
  else            chunksize = WRITE_CHUNK_MAX;  // use 60 byte chunks with write ahead

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

  uint32_t rmisc = (0
    | (0  <<  0)  // CS0_DIS
    | (1  <<  1)  // CS1_DIS
    | (0  <<  9)  // CK_IDLE_EDGE
    | (0  << 10)  // CS_KEEP_ACTIVE
  );

  // data
  if (chunksize > 0)
  {
    ruser |= (1  << 27);  // USR_MOSI: This bit enable the write-data phase of an operation.

    if (acmd & (1 << QSPICM_LN_DATA_POS))  // multi line data ?
    {
      ruser |= (0
        | (mldio << 12)  // FWRITE_DUAL: In the write operations read-data phase apply 2 signals
        | (mlqio << 13)  // FWRITE_QUAD: In the write operations read-data phase apply 4 signals
        | (0  << 14)  // FWRITE_DIO: In the write operations address phase and read-data phase apply 2 signals.
        | (0  << 15)  // FWRITE_QIO: In the write operations address phase and read-data phase apply 4 signals.
      );
    }
    else
    {
      // single line data
    }

    mregs->MOSI_DLEN = (chunksize << 3) - 1;

    if (chunksize < datalen)
    {
      rmisc |= (1  << 10);  // CS_KEEP_ACTIVE
    }
  }

  // address
  unsigned rqaddrlen = ((acmd >> QSPICM_ADDR_POS) & QSPICM_ADDR_SMASK);
  if (rqaddrlen)
  {
    if (rqaddrlen > 4)
    {
      rqaddrlen = addrlen;  // default addrlen
    }

    if (acmd & (1 << QSPICM_LN_ADDR_POS))  // multi-line address ?
    {
      ruser |= (0
        | (mldio << 12)  // FWRITE_DUAL: In the write operations read-data phase apply 2 signals
        | (mlqio << 13)  // FWRITE_QUAD: In the write operations read-data phase apply 4 signals
        | (mldio << 14)  // FWRITE_DIO: In the write operations address phase and read-data phase apply 2 signals.
        | (mlqio << 15)  // FWRITE_QIO: In the write operations address phase and read-data phase apply 4 signals.
      );
    }
    else
    {
      // single line address
    }

    ruser  |= (1  << 30);  // USR_ADDR: This bit enable the address phase of an operation.
    ruser1 |= (((rqaddrlen << 3)-1) << 26); // requested address length

    mregs->ADDR = address;
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

    ruser1 |= ((rqdummyc-1)  <<  0);  // USR_DUMMY_CYCLELEN(6): The length in spi_mem_clk cycles of dummy phase. The register value shall be (cycle_num-1).
  }

  // command
  if (acmd & (1 << QSPICM_LN_CMD_POS))  // multi line command ?
  {
    rctrl = (0
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
  mregs->MISC = rmisc;

  // fill the TX data

  uint32_t d32cnt = ((chunksize + 3) >> 2);
  uint32_t * src32 = (uint32_t *)dataptr;
  uint32_t * end32 = src32 + d32cnt;
  uint32_t * dst32 = (uint32_t *)pwregs;
  while (src32 < end32)
  {
    *dst32++ = *src32++;
  }

  if (chunksize < remainingbytes)
  {
    *dst32++ = *src32++;  // write ahead the the next transfer's first word
  }

  mregs->CMD |= cmd_usr_bit; // start the CMD

  busy = true;
  runstate = 0;

  return HWERR_OK;
}

void THwQspi_esp::StartNextChunk()
{
  uint32_t ruser = mregs->USER;
/*
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
*/

  ruser &= ~(7 << 29); // Clear DUMMY, ADDR, CMD, (keep MISO/MOSI)

  if (istx)
  {
    if (remainingbytes > WRITE_CHUNK_MAX)
    {
      chunksize = WRITE_CHUNK_MAX;
    }
    else
    {
      chunksize = remainingbytes;
      mregs->MISC &= ~(1 << 10); // remove CS_KEEP_ACTIVE
    }
  }
  else
  {
    if (remainingbytes > 64)
    {
      chunksize = 64;
    }
    else
    {
      chunksize = remainingbytes;
      mregs->MISC &= ~(1 << 10); // remove CS_KEEP_ACTIVE
    }
  }

  if (istx)
  {
    mregs->MOSI_DLEN = (chunksize << 3) - 1;

    // fill the TX data

    uint32_t d32cnt = ((chunksize + 3) >> 2);
    uint32_t * src32 = (uint32_t *)dataptr;
    uint32_t * end32 = src32 + d32cnt;
    uint32_t * dst32 = (uint32_t *)pwregs;
    while (src32 < end32)
    {
      *dst32++ = *src32++;
    }

    if (remainingbytes > chunksize)
    {
      *dst32++ = *src32++;  // write ahead the the next transfer's first word
    }
  }
  else
  {
    mregs->MISO_DLEN = (chunksize << 3) - 1;
  }

  mregs->USER = ruser;
  mregs->CMD |= cmd_usr_bit; // start the CMD
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
      // the data is already transferred
    }
    else
    {
      // read the data

      uint32_t d32cnt = ((chunksize + 3) >> 2);
      uint32_t * src32 = (uint32_t *)pwregs;
      uint32_t * end32 = src32 + d32cnt;
      uint32_t * dst32 = (uint32_t *)dataptr;
      while (src32 < end32)
      {
        *dst32++ = *src32++;
      }
    }

    dataptr += chunksize;
    remainingbytes -= chunksize;

    if (remainingbytes)
    {
      StartNextChunk();
      return;
    }
    else
    {
      // finished
      runstate = 1;
    }

  }

  busy = false;
}

