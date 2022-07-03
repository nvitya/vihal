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
 * file:     hwspi_esp.cpp
 * brief:    SPI Master for ESP32
 * created:  2022-06-30
 * authors:  nvitya
*/

#include "platform.h"
#include "hwspi.h"

bool THwSpi_esp::Init(int adevnum)
{
  uint32_t sys_rst_mask;

	devnum = adevnum;
	initialized = false;

	regs = nullptr;

	if (0 == devnum)  // not useable
	{
		regs = SPI0;
    DPORT->PERIP_CLK_EN |= DPORT_SPI01_CLK_EN;
    sys_rst_mask = DPORT_SPI01_RST;
	}
	else if (1 == devnum)
	{
    regs = SPI1;
    DPORT->PERIP_CLK_EN |= DPORT_SPI01_CLK_EN;
    sys_rst_mask = DPORT_SPI01_RST;
	}
	else if (2 == devnum)
	{
    regs = SPI2;
    DPORT->PERIP_CLK_EN |= DPORT_SPI2_CLK_EN;
    sys_rst_mask = DPORT_SPI2_RST;
	}
  else if (3 == devnum)
  {
    regs = SPI3;
    DPORT->PERIP_CLK_EN |= DPORT_SPI3_CLK_EN;
    sys_rst_mask = DPORT_SPI3_RST;
  }

	if (!regs)
	{
		return false;
	}

  // following the Espressif recommended reset sequence:

  DPORT->PERIP_RST_EN &= ~sys_rst_mask;  // clear system UART reset
  DPORT->PERIP_RST_EN |= sys_rst_mask;   // set system UART reset
  if (DPORT->PERIP_RST_EN) {} // some sync
  DPORT->PERIP_RST_EN &= ~sys_rst_mask;  // remove system UART reset

  pwregs = &regs->W[0];

  regs->CMD = 0;

  // other registers
  regs->DMA_CONF = 0;
  regs->DMA_INT_ENA = 0;
  regs->SLAVE = 0;
  regs->SLAVE1 = 0;
  regs->CACHE_FCTRL = 0;
  regs->CACHE_SCTRL = 0;
  regs->SRAM_CMD = 0;

  regs->CTRL = (0
    | (0  << 10)  // FCS_CRC_EN:         For SPI1  initialize crc32 module before writing encrypted data to flash. Active low.
    | (0  << 11)  // TX_CRC_EN:          For SPI1  enable crc32 when writing encrypted data to flash. 1: enable  0:disable
    | (0  << 12)  // WAIT_FLASH_IDLE_EN: wait flash idle when program flash or erase flash. 1: enable 0: disable.
    | (1  << 13)  // FASTRD_MODE:        This bit enable the bits: spi_fread_qio  spi_fread_dio  spi_fread_qout and spi_fread_dout. 1: enable 0: disable.
    | (0  << 14)  // FREAD_DUAL:         In the read operations  read-data phase apply 2 signals. 1: enable 0: disable.
    | (0  << 15)  // RESANDRES:          The Device ID is read out to SPI_RD_STATUS register, this bit combine with spi_flash_res bit. 1: enable 0: disable.
    | (0  << 20)  // FREAD_QUAD:         In the read operations read-data phase apply 4 signals. 1: enable 0: disable.
    | (1  << 21)  // WP:                 Write protect signal output when SPI is idle.  1: output high  0: output low.
    | (0  << 22)  // WRSR_2B:            two bytes data will be written to status register when it is set. 1: enable 0: disable.
    | (0  << 23)  // FREAD_DIO:          In the read operations address phase and read-data phase apply 2 signals. 1: enable 0: disable.
    | (0  << 24)  // FREAD_QIO:          In the read operations address phase and read-data phase apply 4 signals. 1: enable 0: disable.
    | (0  << 25)  // RD_BIT_ORDER:       In read-data (MISO) phase 1: LSB first 0: MSB first
    | (0  << 26)  // WR_BIT_ORDER:       In command address write-data (MOSI) phases 1: LSB firs 0: MSB first
  );

  regs->CTRL1 = (0
    | (0  << 16)  // CS_HOLD_DELAY_RES(12): Delay cycles of resume Flash when resume Flash is enable by spi clock.
    | (5  << 28)  // CS_HOLD_DELAY(4):      SPI cs signal is delayed by spi clock cycles
  );

  regs->CTRL2 = (0
    | (1  <<  0)  // SETUP_TIME(4):        (cycles-1) of ,prepare, phase by spi clock, this bits combined with spi_cs_setup bit.
    | (1  <<  4)  // HOLD_TIME(4):         delay cycles of cs pin by spi clock, this bits combined with spi_cs_hold bit.
    | (0  <<  8)  // CK_OUT_LOW_MODE(4):   modify spi clock duty ratio when the value is lager than 8, the bits are combined with spi_clkcnt_N bits and spi_clkcnt_L bits.
    | (0  << 12)  // CK_OUT_HIGH_MODE(4):  modify spi clock duty ratio when the value is lager than 8, the bits are combined with spi_clkcnt_N bits and spi_clkcnt_H bits.
    | (0  << 16)  // MISO_DELAY_MODE(2):   MISO signals are delayed by spi_clk. 0: zero  1: if spi_ck_out_edge or spi_ck_i_edge is set 1  delayed by half cycle    else delayed by one cycle  2: if spi_ck_out_edge or spi_ck_i_edge is set 1  delayed by one cycle  else delayed by half cycle  3: delayed one cycle
    | (0  << 18)  // MISO_DELAY_NUM(3):    MISO signals are delayed by system clock cycles
    | (0  << 21)  // MOSI_DELAY_MODE(2):   MOSI signals are delayed by spi_clk. 0: zero  1: if spi_ck_out_edge or spi_ck_i_edge is set 1  delayed by half cycle    else delayed by one cycle  2: if spi_ck_out_edge or spi_ck_i_edge is set 1  delayed by one cycle  else delayed by half cycle  3: delayed one cycle
    | (0  << 23)  // MOSI_DELAY_NUM(3):    MOSI signals are delayed by system clock cycles
    | (0  << 26)  // CS_DELAY_MODE(2):     spi_cs signal is delayed by spi_clk . 0: zero  1: if spi_ck_out_edge or spi_ck_i_edge is set 1  delayed by half cycle    else delayed by one cycle  2: if spi_ck_out_edge or spi_ck_i_edge is set 1  delayed by one cycle   else delayed by half cycle  3: delayed one cycle
    | (0  << 28)  // CS_DELAY_NUM(4):      spi_cs signal is delayed by system clock cycles
  );

  regs->USER = (0
    | (1  <<  0)  // DOUTDIN: Set the bit to enable full duplex communication. 1: enable 0: disable.
    | (0  <<  4)  // CS_HOLD: spi cs keep low when spi is in ,done, phase. 1: enable 0: disable.
    | (1  <<  5)  // CS_SETUP: spi cs is enable when spi is in ,prepare, phase. 1: enable 0: disable.
    | (0  <<  6)  // CK_I_EDGE: In the slave mode  the bit is same as spi_ck_out_edge in master mode. It is combined with  spi_miso_delay_mode bits.
    | (0  <<  7)  // CK_OUT_EDGE: the bit combined with spi_mosi_delay_mode bits to set mosi signal delay mode.
    | (0  << 10)  // RD_BYTE_ORDER: In read-data (MISO) phase 1: big-endian 0: little_endian*/
    | (0  << 11)  // WR_BYTE_ORDER: In command address write-data (MOSI) phases 1: big-endian 0: litte_endian*/
    | (0  << 12)  // FWRITE_DUAL: In the write operations read-data phase apply 2 signals
    | (0  << 13)  // FWRITE_QUAD: In the write operations read-data phase apply 4 signals
    | (0  << 14)  // FWRITE_DIO: In the write operations address phase and read-data phase apply 2 signals.
    | (0  << 15)  // FWRITE_QIO: In the write operations address phase and read-data phase apply 4 signals.
    | (0  << 16)  // SIO: Set the bit to enable 3-line half duplex communication  mosi and miso signals share the same pin. 1: enable 0: disable.
    | (0  << 24)  // USR_MISO_HIGHPART: read-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable 0: disable.
    | (0  << 25)  // USR_MOSI_HIGHPART: write-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable 0: disable.
    | (0  << 26)  // USR_DUMMY_IDLE: 1 = spi clock is disabled in dummy phase
    | (1  << 27)  // USR_MOSI: 1 = enable the write-data phase of an operation
    | (1  << 28)  // USR_MISO: 1 = enable the read-data phase of an operation
    | (0  << 29)  // USR_DUMMY: 1 = enable the dummy phase of an operation
    | (0  << 30)  // USR_ADDR: 1 = enable the address phase of an operation
    | (0  << 31)  // USR_COMMAND: 1 = enable the command phase of an operation
  );

  regs->USER1 = (0
    | (0  <<  0)  // USR_DUMMY_CYCLELEN(8): The length in spi_clk cycles of dummy phase. The register value shall be (cycle_num-1)
    | (23 << 26)  // USR_ADDR_BITLEN(6): The length in bits of address phase. The register value shall be (bit_num-1)
  );

  regs->USER2 = (0
    | (0  <<  0)  // USR_COMMAND_VALUE(16): The value of  command. Output sequence: bit 7-0 and then 15-8.
    | (7  << 28)  // USR_COMMAND_BITLEN(4): The length in bits of command phase. The register value shall be (bit_num-1)
  );

  regs->PIN = (0
    | (0  <<  0)  // CS0_DIS: SPI CS0 pin enable, 1: disable CS0, 0: spi_cs0 signal is from/to CS0 pin
    | (1  <<  1)  // CS1_DIS: SPI CS1 pin enable, 1: disable CS1, 0: spi_cs1 signal is from/to CS1 pin
    | (1  <<  2)  // CS2_DIS: SPI CS2 pin enable, 1: disable CS2, 0: spi_cs2 signal is from/to CS2 pin
    | (0  <<  5)  // CK_DIS: 1: spi clk out disable  0: spi clk out enable
    | (0  <<  6)  // MASTER_CS_POL(3): In the master mode  the bits are the polarity of spi cs line  the value is equivalent to spi_cs ^ spi_master_cs_pol.
    | (7  << 11)  // MASTER_CK_SEL(3): In the master mode  spi cs line is enable as spi clk  it is combined with spi_cs0_dis spi_cs1_dis spi_cs2_dis.
    | (0  << 29)  // CK_IDLE_EDGE: 1: spi clk line is high when idle     0: spi clk line is low when idle
    | (0  << 30)  // CS_KEEP_ACTIVE: spi cs line keep low when the bit is set.
  );

  regs->ADDR = 0;

  // clock configuration

  basespeed = 80000000;
  if (speed > basespeed)  speed = basespeed;
  uint32_t clkdiv = basespeed / speed;

  uint32_t prediv = 1;
  while (clkdiv > 64)
  {
    ++prediv;
    clkdiv = basespeed / (prediv * speed);
  }
  uint32_t clkequ = (speed == basespeed ? 1 : 0);
  uint32_t clkcnt_h = clkdiv / 2;
  if (clkcnt_h > 1)  --clkcnt_h;

  regs->CLOCK = (0
    | ((clkdiv - 1)  <<  0)  // CLKCNT_L(6): must be the same as CLKCNT_N
    | (clkcnt_h      <<  6)  // CLKCNT_H(6)
    | ((clkdiv - 1)  << 12)  // CLKCNT_N(6)
    | ((prediv - 1)  << 18)  // CLKDIV_PRE(13)
    | (clkequ        << 31)  // CLK_EQU_SYSCLK
  );

	//if (idleclk_high)     cfg |= (1 << 0);
	//if (datasample_late)  cfg |= (1 << 1);

	initialized = true;

	return true;
}

bool THwSpi_esp::TrySendData(unsigned short adata)
{
	return false;
}

bool THwSpi_esp::TryRecvData(unsigned short * dstptr)
{
	return false;
}

bool THwSpi_esp::SendFinished()
{
	if (regs->CMD & (1 << 18)) // is the USR bit still active ?
	{
		return true;
	}
	else
	{
		return false;
	}
}

void THwSpi_esp::SetCs(unsigned value)
{
  if (manualcspin)
  {
    manualcspin->SetTo(value);
  }
  else
  {
    //TODO: implement
  }
}

void THwSpi_esp::Run()
{
	if (finished)
	{
		return;
	}

	if (0 == state)  // start phase
	{
		if (blockcnt < 1)  // wrong config ?
		{
			finished = true;
			return;
		}

		curblock = &xferblock[0];
		lastblock = &xferblock[blockcnt - 1];

		remaining = curblock->len;

		SetCs(0);

		state = 1;
	}

	if (1 == state) // start / continue block
	{
	  chunksize = remaining;
	  if (remaining <= 64)  chunksize = remaining;
	  else                  chunksize = 64;

	  uint32_t * dst32 = (uint32_t *)pwregs;
	  uint32_t d32cnt = ((chunksize + 3) >> 2);
    uint32_t * end32 = dst32 + d32cnt;

	  // fill the tx data
	  if (curblock->src)
	  {
	    if ((unsigned(curblock->src) & 3) || (chunksize & 3))
	    {
	      // use 8-bit transfers
	      uint8_t * dst8  = (uint8_t *)&pwregs;  // byte addressable !
	      uint8_t * end8 = dst8 + chunksize;
	      uint8_t * src8 = curblock->src;
        while (dst8 < end8)
        {
          *dst8++ = *src8++;
        }
	    }
	    else
	    {
	      // use 32-bit transfers
	      uint32_t * src32 = (uint32_t *)curblock->src;
	      while (dst32 < end32)
	      {
	        *dst32++ = *src32++;
	      }
	    }
      curblock->src += chunksize;
	  }
	  else // zero tx data
	  {
	    // todo: optimize to do it once for longer transfers
	    while (dst32 < end32)
	    {
	      *dst32++ = 0;
	    }
	  }

    regs->MOSI_DLEN = (chunksize << 3) - 1;
    regs->MISO_DLEN = (chunksize << 3) - 1;

    // start the transaction
	  regs->CMD |= (1 << 18); // USR
	  //regs->CMD |= (1 << 28); // RDID
	  state = 2; // wait to finish
	}
	else if (2 == state) // wait the previous transaction to finish
	{
	  if (regs->CMD) // still running ?
	  //if (regs->CMD & (1 << 18)) // still running ?
	  {
	    return;
	  }

	  // read and store the received data
	  if (curblock->dst)
	  {
      if ((unsigned(curblock->dst) & 3) || (chunksize & 3))
      {
        // use 8-bit transfers
        uint8_t * src8  = (uint8_t *)pwregs;  // byte addressable !
        uint8_t * end8 = src8 + chunksize;
        uint8_t * dst8 = curblock->dst;
        while (src8 < end8)
        {
          *dst8++ = *src8++;
        }
      }
      else
      {
        // use 32-bit transfers
        uint32_t * src32 = (uint32_t *)&pwregs;
        uint32_t d32cnt = ((chunksize + 3) >> 2);
        uint32_t * end32 = src32 + d32cnt;
        uint32_t * dst32 = (uint32_t *)curblock->dst;
        while (src32 < end32)
        {
          *dst32++ = *src32++;
        }
      }
      curblock->dst += chunksize;
	  }

	  remaining -= chunksize;

	  if (remaining)
	  {
	    state = 1; // go on with the next chunk
	    Run();
	  }
	  else
	  {
	    if (curblock == lastblock)
	    {
	      SetCs(1);
	      finished = true;
	      state = 100;
	    }
	  }
	}
}
