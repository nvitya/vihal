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
 * brief:    SPI Master for ESP32-C3
 * created:  2022-06-30
 * authors:  nvitya
*/

#include "platform.h"
#include "hwspi.h"
//#include "hwspi_esp.h"

bool THwSpi_esp::Init(int adevnum)
{
  uint32_t sys_rst_mask;

	devnum = adevnum;
	initialized = false;

	gregs = nullptr;
	mregs = nullptr;

	if (0 == devnum)  // not useable
	{
		// total different, works only with memory mapped mode
	}
	else if (1 == devnum)
	{
    mregs = SPIMEM1;
    SYSTEM->PERIP_CLK_EN0 |= SYSTEM_SPI01_CLK_EN;
    sys_rst_mask = SYSTEM_SPI01_RST;
	}
	else if (2 == devnum)
	{
    gregs = SPI2;
    SYSTEM->PERIP_CLK_EN0 |= SYSTEM_SPI2_CLK_EN;
    sys_rst_mask = SYSTEM_SPI2_RST;
	}

	if (!gregs && !mregs)
	{
		return false;
	}

  SYSTEM->PERIP_RST_EN0 |= sys_rst_mask;   // set system SPI reset
  if (SYSTEM->PERIP_RST_EN0) {} // some sync
  SYSTEM->PERIP_RST_EN0 &= ~sys_rst_mask;  // remove system SPI reset

  if (gregs)
  {
    pcmdreg = &gregs->CMD;
    pwregs = &gregs->W[0];

    cmd_usr_bit = (1 << 24);
    *pcmdreg = 0;
    //regs->MS_DLEN = 0;

    gregs->CLK_GATE = (0
      | (1  <<  0)  // CLK_EN
      | (1  <<  1)  // MST_CLK_ACTIVE
      | (1  <<  2)  // MST_CLK_SEL: 1 = PLL_80M_CLK
    );

    // extra timing registers
    gregs->DIN_MODE = 0;
    gregs->DIN_NUM = 0;
    gregs->DOUT_MODE = 0;

    // other registers
    gregs->DMA_CONF = 0;
    gregs->DMA_INT_ENA = 0;
    gregs->SLAVE = 0;
    gregs->SLAVE1 = 0;

    gregs->MISC = (0
      | (0  <<  0)  // CS0_DIS
      | (1  <<  1)  // CS1_DIS
      | (1  <<  2)  // CS2_DIS
      | (1  <<  3)  // CS3_DIS
      | (1  <<  4)  // CS4_DIS
      | (1  <<  5)  // CS5_DIS
      | (0  <<  6)  // CK_DIS
      | (0  <<  7)  // MASTER_CS_POL(6)
      | (0  << 23)  // SLAVE_CS_POL
      | (0  << 29)  // CK_IDLE_EDGE
      | (0  << 30)  // CS_KEEP_ACTIVE
      | (0  << 31)  // QUAD_DIN_PIN_SWAP
    );

    gregs->CTRL = (0
      | (0  <<  3)  // DUMMY_OUT
      | (0  <<  5)  // FADDR_DUAL
      | (0  <<  6)  // FADDR_QUAD
      | (0  <<  8)  // FCMD_DUAL
      | (0  <<  9)  // FCMD_QUAD
      | (0  << 14)  // FREAD_DUAL
      | (0  << 15)  // FREAD_QUAD
      | (1  << 18)  // Q_POL
      | (1  << 19)  // D_POL
      | (1  << 20)  // HOLD_POL
      | (1  << 21)  // WP_POL
      | (0  << 25)  // RD_BIT_ORDER
      | (0  << 26)  // WR_BIT_ORDER
    );

    gregs->USER = (0
      | (1  <<  0)  // DOUTDIN
      | (0  <<  3)  // QPI_MODE
      | (0  <<  5)  // TSCK_I_EDGE
      | (0  <<  6)  // CS_HOLD
      | (1  <<  7)  // CS_SETUP
      | (0  <<  8)  // RSCK_I_EDGE
      | (0  <<  9)  // CK_OUT_EDGE
      | (0  << 12)  // FWRITE_DUAL
      | (0  << 13)  // FWRITE_QUAD
      | (0  << 15)  // USR_CONF_NXT: 0 = no segmented transfer
      | (0  << 17)  // SIO: 1 = half duplex shared MISO/MOSI mode
      | (0  << 24)  // USR_MISO_HIGHPART
      | (0  << 25)  // USR_MOSI_HIGHPART
      | (0  << 26)  // USR_DUMMY_IDLE
      | (1  << 27)  // USR_MOSI
      | (1  << 28)  // USR_MISO
      | (0  << 29)  // USR_DUMMY
      | (0  << 30)  // USR_ADDR
      | (0  << 31)  // USR_COMMAND
    );

    gregs->USER1 = (0
      | (0  <<  0)  // USR_DUMMY_CYCLELEN(8)
      | (0  << 16)  // MST_WFULL_ERR_END_EN
      | (0  << 17)  // CS_SETUP_TIME(5)
      | (0  << 22)  // CS_HOLD_TIME(5)
      | (23 << 27)  // USR_ADDR_BITLEN(5)
    );

    gregs->USER2 = (0
      | (0  <<  0)  // USR_COMMAND_VALUE(16)
      | (0  << 27)  // MST_REMPTY_ERR_END_EN
      | (7  << 28)  // USR_COMMAND_BITLEN(4)
    );

    gregs->ADDR = 0;
  }

  if (mregs)
  {
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
      | (1  << 27)  // USR_MOSI: This bit enable the write-data phase of an operation.
      | (1  << 28)  // USR_MISO: This bit enable the read-data phase of an operation.
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

  }

  // clock configuration

  basespeed = 80000000;
  if (speed > basespeed)  speed = basespeed;
  uint32_t clkdiv = basespeed / speed;

  if (gregs)
  {
    uint32_t prediv = 1;
    while (clkdiv > 64)
    {
      ++prediv;
      clkdiv = basespeed / (prediv * speed);
    }
    uint32_t clkequ = (speed == basespeed ? 1 : 0);
    uint32_t clkcnt_h = clkdiv / 2;
    if (clkcnt_h > 1)  --clkcnt_h;

    gregs->CLOCK = (0
      | ((clkdiv - 1)  <<  0)  // CLKCNT_L(6): must be the same as CLKCNT_N
      | (clkcnt_h      <<  6)  // CLKCNT_H(6)
      | ((clkdiv - 1)  << 12)  // CLKCNT_N(6)
      | ((prediv - 1)  << 18)  // CLKDIV_PRE
      | (clkequ        << 31)  // CLK_EQU_SYSCLK
    );
  }

  if (mregs)
  {
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
  }



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
	if (*pcmdreg & cmd_usr_bit) // is the USR bit still active ?
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
	      uint8_t * dst8  = (uint8_t *)pwregs;  // byte addressable !
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

	  if (mregs)
	  {
	    mregs->USER = (0
	      //| (0  <<  6)  // CS_HOLD: spi cs keep low when spi is in  done  phase. 1: enable 0: disable.
	      //| (0  <<  7)  // CS_SETUP: spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable.
	      | (0  <<  9)  // CK_OUT_EDGE: the bit combined with spi_mem_mosi_delay_mode bits to set mosi signal delay mode.
	      | (0  << 12)  // FWRITE_DUAL: In the write operations read-data phase apply 2 signals
	      | (0  << 13)  // FWRITE_QUAD: In the write operations read-data phase apply 4 signals
	      | (0  << 14)  // FWRITE_DIO: In the write operations address phase and read-data phase apply 2 signals.
	      | (0  << 15)  // FWRITE_QIO: In the write operations address phase and read-data phase apply 4 signals.
	      | (0  << 24)  // USR_MISO_HIGHPART: read-data phase only access to high-part of the buffer spi_mem_w8~spi_mem_w15. 1: enable 0: disable.
	      | (0  << 25)  // USR_MOSI_HIGHPART: write-data phase only access to high-part of the buffer spi_mem_w8~spi_mem_w15. 1: enable 0: disable.
	      | (0  << 26)  // USR_DUMMY_IDLE: SPI clock is disable in dummy phase when the bit is enable.
	      | (1  << 27)  // USR_MOSI: This bit enable the write-data phase of an operation.
	      | (0  << 28)  // USR_MISO: This bit enable the read-data phase of an operation.
	      | (0  << 29)  // USR_DUMMY: This bit enable the dummy phase of an operation.
	      | (0  << 30)  // USR_ADDR: This bit enable the address phase of an operation.
	      | (1  << 31)  // USR_COMMAND: This bit enable the command phase of an operation.
	    );

      mregs->MOSI_DLEN = (chunksize << 3) - 1;
      mregs->MISO_DLEN = (chunksize << 3) - 1;

      //mregs->CTRL2 |= SPI_MEM_SYNC_RESET_M;


	  }
	  else // gregs
	  {
      // set the transfer bit length
      gregs->MS_DLEN = (chunksize << 3) - 1;

      // synchronize the transaction parameters
      gregs->CMD |= SPI_UPDATE_M;
      while (gregs->CMD & SPI_UPDATE_M)
      {
      }
	  }

    // start the transaction
	  if (gregs)
	  {
	    CmdStart();
	  }
	  else
	  {
      //CmdStart();
	    //mregs->CMD |= (1 << 28); //start reading the ID
	    mregs->CMD = (1 << 18); //start reading the ID
	  }

	  state = 2; // wait to finish
	}
	else if (2 == state) // wait the previous transaction to finish
	{
	  if (CmdRunning()) // still running ?
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
        uint32_t * src32 = (uint32_t *)pwregs;
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
