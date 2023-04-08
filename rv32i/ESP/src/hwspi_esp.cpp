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

	regs = nullptr;

	if (0 == devnum)  // not useable
	{
		// total different, works only with memory mapped mode
	}
	else if (2 == devnum)
	{
    regs = SPI2;
    SYSTEM->PERIP_CLK_EN0 |= SYSTEM_SPI2_CLK_EN;
    sys_rst_mask = SYSTEM_SPI2_RST;
	}

	if (!regs)
	{
		return false;
	}

  SYSTEM->PERIP_RST_EN0 |= sys_rst_mask;   // set system SPI reset
  if (SYSTEM->PERIP_RST_EN0) {} // some sync
  SYSTEM->PERIP_RST_EN0 &= ~sys_rst_mask;  // remove system SPI reset

  pcmdreg = &regs->CMD;
  pwregs = &regs->W[0];

  cmd_usr_bit = (1 << 24);
  *pcmdreg = 0;
  //regs->MS_DLEN = 0;

  regs->CLK_GATE = (0
    | (1  <<  0)  // CLK_EN
    | (1  <<  1)  // MST_CLK_ACTIVE
    | (1  <<  2)  // MST_CLK_SEL: 1 = PLL_80M_CLK
  );

  // extra timing registers
  regs->DIN_MODE = 0;
  regs->DIN_NUM = 0;
  regs->DOUT_MODE = 0;

  // other registers
  regs->DMA_CONF = 0;
  regs->DMA_INT_ENA = 0;
  regs->SLAVE = 0;
  regs->SLAVE1 = 0;

  regs->MISC = (0
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

  regs->CTRL = (0
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

  regs->USER = (0
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

  regs->USER1 = (0
    | (0  <<  0)  // USR_DUMMY_CYCLELEN(8)
    | (0  << 16)  // MST_WFULL_ERR_END_EN
    | (0  << 17)  // CS_SETUP_TIME(5)
    | (0  << 22)  // CS_HOLD_TIME(5)
    | (23 << 27)  // USR_ADDR_BITLEN(5)
  );

  regs->USER2 = (0
    | (0  <<  0)  // USR_COMMAND_VALUE(16)
    | (0  << 27)  // MST_REMPTY_ERR_END_EN
    | (7  << 28)  // USR_COMMAND_BITLEN(4)
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
    | ((prediv - 1)  << 18)  // CLKDIV_PRE
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
	if (*pcmdreg & cmd_usr_bit) // is the USR bit still active ?
	{
		return true;
	}
	else
	{
		return false;
	}
}

void THwSpi_esp::DmaAssign(bool istx, THwDmaChannel * admach)
{
  if (istx)
  {
    txdma = admach;
  }
  else
  {
    rxdma = admach;
  }

  admach->Prepare(istx, (void *)pwregs, 0);
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

#define WRITE_CHUNK_MAX  60


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

		curblock  = &xferblock[0];
		lastblock = &xferblock[blockcnt - 1];

		remaining = curblock->len;

		SetCs(0);

		state = 1;
	}

	if (1 == state) // start / continue block
	{
    chunksize = remaining;

	  if ((txdma && curblock->src) || (rxdma && curblock->dst))
	  {
	    // transmit with DMA
      if (remaining <= HW_DMA_MAX_COUNT)  chunksize = remaining;
      else chunksize = HW_DMA_MAX_COUNT;

	    if (txdma && curblock->src)
	    {
        regs->DMA_CONF |= (1 << 28); // enable the TX DMA

        txfer.bytewidth = 1;
        txfer.srcaddr = curblock->src;
        txfer.count = chunksize;
        txfer.flags = 0;
        txdma->StartTransfer(&txfer);
	    }
	    else  // send zeroes
	    {
        regs->DMA_CONF &= ~(1 << 28); // disable the TX DMA

        uint32_t *  dst32 = (uint32_t *)pwregs;
        uint32_t *  end32 = dst32 + 16;
        while (dst32 < end32)
        {
          *dst32++ = 0;
        }
	    }

      if (rxdma && curblock->dst)
      {
        regs->DMA_CONF |= (1 << 27); // enable the RX DMA

        rxfer.bytewidth = 1;
        rxfer.dstaddr = curblock->dst;
        rxfer.count = chunksize;
        rxfer.flags = 0;
        rxdma->StartTransfer(&rxfer);
      }
      else
      {
        regs->DMA_CONF &= ~(1 << 27); // disable the RX DMA

        // the RX data will be discarded
      }
	  }
	  else
	  {
      regs->DMA_CONF &= ~(3 << 27); // disable RX and TX DMA

      if (remaining <= 60)  chunksize = remaining;
      else                  chunksize = 60;

      uint32_t *  dst32 = (uint32_t *)pwregs;
      uint32_t    d32cnt = ((chunksize + 3) >> 2);
      uint32_t *  end32 = dst32 + d32cnt;

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

          if (chunksize < remaining)
          {
            // write ahead the the next transfer's first 4 bytes
            end8 += 4;
            while (dst8 < end8)
            {
              *dst8++ = *src8++;
            }
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

          if (chunksize < remaining)
          {
            *dst32++ = *src32++;  // write ahead the the next transfer's first word
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

        if (chunksize < remaining)
        {
          *dst32++ = 0;  // write ahead the the next transfer's first word
        }
      }
	  }

	  uint32_t rmisc = regs->MISC;
    if (chunksize < remaining)
    {
      rmisc |=  (1  << 30);  // CS_KEEP_ACTIVE
    }
    else
    {
      rmisc &= ~(1  << 30);  // CS_KEEP_ACTIVE
    }
    regs->MISC = rmisc;

    // set the transfer bit length
    regs->MS_DLEN = (chunksize << 3) - 1;

    // synchronize the transaction parameters
    regs->CMD |= SPI_UPDATE_M;
    while (regs->CMD & SPI_UPDATE_M)
    {
    }

    // start the transaction
    CmdStart();

    state = 2; // wait finish chunk
	}
	else if (2 == state) // wait the previous transaction to finish
	{
	  if (CmdRunning()) // still running ?
	  {
	    return;
	  }

	  // read and store the received data
	  if (!rxdma && curblock->dst)
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
	    else
	    {
        ++curblock;
        remaining = curblock->len;
        state = 1;
        Run();
        return;
	    }
	  }
	}
}
