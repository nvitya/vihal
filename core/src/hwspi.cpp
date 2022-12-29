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
// file:     hwspi.cpp
// brief:    SPI vendor-independent implementations
//           only SPI master mode supported so far
// created:  2018-02-10
// authors:  nvitya

#include "hwspi.h"

static_assert(HWSPI_MAX_XFER_BLOCK >= 4, "Minimum 4 SPI transfer blocks are required!");

void THwSpi::BeginTransaction()
{
	if (manualcspin)
	{
		manualcspin->Set0();
	}
}

void THwSpi::EndTransaction()
{
	if (manualcspin)
	{
		manualcspin->Set1();
	}
}

void THwSpi::WaitSendFinish()
{
	while (!SendFinished())
	{
		// wait
	}
}

void THwSpi_pre::PrepareTransfer(uint32_t acmd, uint32_t aaddr, uint32_t aflags,
		                             uint32_t alen, uint8_t * asrc, uint8_t * adst)
{
	TSpiXferBlock * pblock = &xferblock[0];

	blockcnt = 0;
	finished = false;
	state = 0; // sets back to start state

	if (aflags & SPITR_CMD_MASK) // send command ?
	{
		data_cmd = acmd;
		pblock->src = (uint8_t *)&data_cmd;
		pblock->dst = nullptr; // may be redirected to data_void
		if (aflags & SPITR_CMD) // use default size?
		{
			pblock->len = default_cmd_len;
		}
		else
		{
			pblock->len = (aflags & 7);
			if (pblock->len > 4)  pblock->len = 4;
		}
		++blockcnt;
		++pblock;
	}

	if (aflags & SPITR_ADDR_MASK) // send address ?
	{
		data_addr = aaddr;
		pblock->src = (uint8_t *)&data_addr;
		pblock->dst = nullptr; // may be redirected to data_void
		if (aflags & SPITR_ADDR) // use default size?
		{
			pblock->len = default_addr_len;
		}
		else
		{
			pblock->len = ((aflags >> 4) & 7);
			if (pblock->len > 4)  pblock->len = 4;
		}
		if (bigendian_addr)
		{
			data_addr = (__builtin_bswap32(data_addr) >> (8 * (4 - pblock->len)));
		}
		++blockcnt;
		++pblock;
	}

	if (aflags & SPITR_EXTRA_MASK) // send extra ?
	{
		pblock->src = (uint8_t *)&data_extra;
		pblock->dst = nullptr; // may be redirected to data_void
		if (aflags & SPITR_EXTRA) // use default size?
		{
			pblock->len = default_extra_len;
		}
		else
		{
			pblock->len = ((aflags >> 8) & 7);
			if (pblock->len > 4)  pblock->len = 4;
		}
		++blockcnt;
		++pblock;
	}

	// finally the data block
	if (alen > 0)
	{
    pblock->src = asrc;
    pblock->dst = adst;
    pblock->len = alen;
    ++blockcnt;
	}
}

void THwSpi::WaitFinish()
{
	while (!finished)
	{
		Run();
	}
}

#ifndef HWSPI_OWN_RUN

void THwSpi::Run()
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

    dmaused = (rxdma && txdma);
    if (dmaused)
    {
      rxdma->Disable();
      txdma->Disable();
    }

    SetCs(0);

    curblock = &xferblock[0];
    lastblock = &xferblock[blockcnt - 1];
    rx_fifo_wait = 0;

    if (dmaused)
    {
      state = 1;
    }
    else
    {
      tx_remaining = curblock->len; // also for rx only mode cmds are still required
      rx_remaining = curblock->len;
      state = 10;
    }
  }

  // With DMA

  //TODO: handle DMA chunking (the DMA transfer counts are limited to 65536)

  if (1 == state) // start dma transfers
  {
    chunksize = curblock->len;
    if (chunksize > HW_DMA_MAX_COUNT)  chunksize = HW_DMA_MAX_COUNT;

    txfer.srcaddr = curblock->src;
    txfer.bytewidth = 1;
    txfer.count = chunksize;
    txfer.flags = 0;
    if (!txfer.srcaddr)
    {
      txfer.srcaddr = &datazero;
      txfer.flags = DMATR_NO_SRC_INC;
    }

    rxfer.dstaddr = curblock->dst;
    rxfer.bytewidth = 1;
    rxfer.flags = 0;
    rxfer.count = chunksize;
    if (!rxfer.dstaddr)
    {
      rxfer.dstaddr = &datavoid;
      rxfer.flags = DMATR_NO_DST_INC;
    }

    DmaStartRecv(&rxfer);
    DmaStartSend(&txfer);

    state = 2;
  }
  else if (2 == state)
  {
    if (rxdma->Active() || txdma->Active())  // wait until the DMAs finish
    {
      // todo: timeout handling
      return;
    }

    curblock->len -= chunksize;
    if (curblock->src)  curblock->src += chunksize;
    if (curblock->dst)  curblock->dst += chunksize;

    if (curblock->len)
    {
      state = 1;  Run(); // jump to state 1 to process the next chunk
      return;
    }

    // go to the next block

    if (curblock == lastblock)
    {
      state = 3; // all finished
    }
    else
    {
      ++curblock;
      state = 1;  Run(); // jump to state 1 to process the next block
      return;
    }
  }
  else if (3 == state)
  {
    SetCs(1);
    finished = true;
    state = 100;
  }

  // without DMA

  else if (10 == state)  // start / run block
  {
    uint8_t txdata;
    uint8_t rxdata;

    while (tx_remaining && (rx_fifo_wait < fifo_size))
    {
      if (curblock->src)
      {
        txdata = *curblock->src;
      }
      else
      {
        txdata = 0;
      }
      if (TrySendData(txdata))
      {
        if (curblock->src)
        {
          ++(curblock->src);
        }
        ++rx_fifo_wait;
        --tx_remaining;
      }
      else
      {
        break;
      }
    }

    while (rx_remaining && rx_fifo_wait)
    {
      if (TryRecvData(&rxdata))
      {
        if (curblock->dst)
        {
          *curblock->dst = rxdata;
          ++(curblock->dst);
        }

        --rx_remaining;
        --rx_fifo_wait;
      }
      else
      {
        break;
      }
    }

    if (rx_remaining)
    {
      return;
    }

    // go to the next block

    if (curblock == lastblock)
    {
      SetCs(1);
      finished = true;
      state = 100;
    }
    else
    {
      ++curblock;
      tx_remaining = curblock->len; // also for rx only mode cmds are still required
      rx_remaining = curblock->len;
    }
  }
}

#endif
