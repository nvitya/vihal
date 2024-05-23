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
 *  file:     hwspi_sg.cpp
 *  brief:    SG200x/CV1800 (Sophgo) SPI driver for VIHAL
 *  created:  2024-05-22
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwspi_sg.h"

#include "sg_utils.h"

#define SPI_SR_DCOL   (0x40)
#define SPI_SR_TXE    (0x20)
#define SPI_SR_RFF    (0x10)
#define SPI_SR_RFNE   (0x08)
#define SPI_SR_TFE    (0x04)
#define SPI_SR_TFNF   (0x02)
#define SPI_SR_BUSY   (0x01)

bool THwSpi_sg::Init(int adevnum)
{
	devnum = adevnum;
  unsigned resetmask;

	initialized = false;

  basespeed = 187500000;

	//SYSCTL->CLK_EN_CENT |= SYSCTL_CLK_EN_CENT_APB0;

  if (0 == devnum)
  {
    regs = SPI0;
  }
  else if (1 == devnum)
  {
    regs = SPI1;
  }
  else if (2 == devnum)
  {
    regs = SPI2;
  }
  else if (3 == devnum)
  {
    regs = SPI3;
  }

  if (!regs)
  {
    return false;
  }

  regs->SPIENR = 0;
  if (regs->ICR) { } // reading this register clears the interrupts
  regs->SER = 1; // enable slave select

  uint32_t ctrl0 = (0
    | (0  << 12)  // MWCFS(4): microwire control frame size
    | (0  << 11)  // LOOPBACK: 1 = loopback output to input
    | (0  << 10)  // ?SLAVE? (only for slave mode)
    | (0  <<  8)  // TRMODE(2): transfer mode, 0 = Tx+Rx, 1 = Tx only, 2 = Rx only, 3 = EEPROM Read
    | (0  <<  7)  // CPOL: 0 = clock is low when inactive, 1 = clock is high when inactive
    | (0  <<  6)  // SCPH: 0 = serial clock toggles in middle of first data bit, 1 = serial clock toggles at start of first bit
    | (0  <<  4)  // FRF: frame format, 0 = Motorola, 1 = TI, 2 = Microwire
    | ((databits-1) <<  0)  // FSIZE(4): 3 = 4-bit, 7 = 8-bit, 15 = 16-bit
  );
  if (idleclk_high)      ctrl0 |= (1 << 7);
  if (datasample_late)   ctrl0 |= (1 << 6);
  regs->CTRLR0 = ctrl0;


  regs->CTRLR1 = (0
    | (0  <<  0)  // DFNUM(16): number of data frames
  );

  SetSpeed(speed);

  regs->SPIENR = 1;

	initialized = true;
	return true;
}

void THwSpi_sg::SetSpeed(unsigned aspeed)
{
  unsigned baudr = ((basespeed / speed) + 1) & 0xFFFE; // round up to two
  if (baudr < 2)  baudr = 2;
  regs->BAUDR = baudr;
}

bool THwSpi_sg::TrySendData(uint8_t adata)
{
  if (regs->SR & SPI_SR_TFNF)
  {
    regs->DR = adata;
    return true;
  }
  else
  {
	  return false;
  }
}

bool THwSpi_sg::TryRecvData(uint8_t * dstptr)
{
  if (regs->SR & SPI_SR_RFNE)
  {
    *dstptr = regs->DR;
    return true;
  }
  else
  {
    return false;
  }
}

bool THwSpi_sg::SendFinished()
{
  if (regs->SR & SPI_SR_BUSY)
  {
    return false;
  }
  else
  {
    return true;
  }
}

void THwSpi_sg::SetCs(unsigned avalue)
{
}

void THwSpi_sg::DmaAssign(bool istx, THwDmaChannel *admach)
{
}

bool THwSpi_sg::DmaStartSend(THwDmaTransfer * axfer)
{
  return true;
}

bool THwSpi_sg::DmaStartRecv(THwDmaTransfer *axfer)
{
  return true;
}

bool THwSpi_sg::DmaSendCompleted()
{
  return true;
}

bool THwSpi_sg::DmaRecvCompleted()
{
  return true;
}

#if 0
void THwSpi_sg::Run()
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

    // empty tx fifo
    while (regs->TXFLR)
    {
      // wait until the fifo will be empty
    }

    // empty rx fifo
    while (regs->RXFLR)
    {
      if (regs->DR[0]) {  }
    }

    txblock = &xferblock[0];
    rxblock = &xferblock[0];
    lastblock = &xferblock[blockcnt - 1];

    tx_remaining = txblock->len; // also for rx only mode cmds are still required
    rx_remaining = rxblock->len;

    tx_finished = false;
    rx_finished = false;

    state = 1;

    uint32_t totalbytes = 0;
    for (unsigned bc = 0; bc < blockcnt; ++bc)
    {
      totalbytes += xferblock[bc].len;
    }
    regs->CTRLR1 = (totalbytes - 1);
    regs->SSIENR = 1;
    regs->SER = 1;  // chip select control ?
  }

  if (1 == state) // run phase
  {
    // TX
    while (!tx_finished) // repeat until the TX fifo is full
    {
      if (0 == tx_remaining)
      {
        // go to the next block
        if (txblock == lastblock)
        {
          if (SendFinished())
          {
            tx_finished = true;
          }
          break;
        }
        else
        {
          ++txblock;
          tx_remaining = txblock->len;
        }
      }
      else
      {
        // the TX fifo is 32 frame big, so this is ompimized for multiple characters

        uint32_t pushcnt = (32 - regs->TXFLR);  // tx fifo free
        if (pushcnt > tx_remaining)  pushcnt = tx_remaining;
        tx_remaining -= pushcnt;
        if (txblock->src)  // send data ?
        {
          uint8_t * dp = txblock->src;
          uint8_t * endp = dp + pushcnt;
          while (dp < endp)
          {
            regs->DR[0] = *dp;
            ++dp;
          }
          txblock->src = endp;
        }
        else
        {
          while (pushcnt)
          {
            regs->DR[0] = 0xFFFFFFFF;
            --pushcnt;
          }
        }

        if (tx_remaining)
        {
          // tx fifo is full, wait another round
          break;
        }
      }
    }

    // RX
    while (!rx_finished) // repeat until there are RX bytes
    {
      if (0 == rx_remaining)
      {
        // go to the next block
        if (rxblock == lastblock)
        {
          rx_finished = true;
          break;
        }
        else
        {
          ++rxblock;
          rx_remaining = rxblock->len;
        }
      }
      else
      {
        // store the received characters, until the fifo is empty
        if (rx_remaining)
        {
          if (rxblock->dst)
          {
            while (regs->RXFLR)
            {
              *(rxblock->dst) = regs->DR[0];
              ++(rxblock->dst);
              --rx_remaining;
            }
          }
          else
          {
            while (regs->RXFLR)
            {
              // just pop
              if (regs->DR[0]) { }
              --rx_remaining;
            }
          }
        }

        if (rx_remaining)
        {
          // rx fifo is empty, wait another round
          break;
        }
      }
    }

    if (tx_finished && rx_finished)
    {
      regs->SER = 0;
      regs->SSIENR = 0;

      finished = true;
      state = 100;
    }
  }
}

#endif
