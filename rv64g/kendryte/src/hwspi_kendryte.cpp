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
 *  file:     hwspi_kendryte.cpp
 *  brief:    Kendryte SPI (master only)
 *  created:  2022-07-24
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwspi_kendryte.h"

#include "kendryte_utils.h"

bool THwSpi_kendryte::Init(int adevnum)
{
	devnum = adevnum;
  unsigned resetmask;

	initialized = false;

  basespeed = kendryte_bus_speed(0);

	SYSCTL->CLK_EN_CENT |= SYSCTL_CLK_EN_CENT_APB0;

  if (0 == devnum)
  {
    regs = SPI0;
    SYSCTL->CLK_EN_PERI |= SYSCTL_CLK_EN_PERI_SPI0;
    resetmask = SYSCTL_PERI_RESET_SPI0;
  }
  else if (1 == devnum)
  {
    regs = SPI1;
    SYSCTL->CLK_EN_PERI |= SYSCTL_CLK_EN_PERI_SPI1;
    resetmask = SYSCTL_PERI_RESET_SPI1;
  }
  else if (3 == devnum)
  {
    regs = SPI3;
    SYSCTL->CLK_EN_PERI |= SYSCTL_CLK_EN_PERI_SPI3;
    resetmask = SYSCTL_PERI_RESET_SPI3;
    basespeed = SystemCoreClock;  // different bus !
  }

  if (!regs)
  {
    return false;
  }


  uint8_t dfs_offset, frf_offset, work_mode_offset;
  uint8_t tmod_offset;

  if (3 == devnum)
  {
    dfs_offset = 0;
    frf_offset = 22;
    work_mode_offset = 8;
    tmod_offset = 10;
  }
  else
  {
    dfs_offset = 16;
    frf_offset = 21;
    work_mode_offset = 6;
    tmod_offset = 8;
  }

  regs->IMR = 0x00;
  regs->DMACR = 0x00;
  regs->DMATDLR = 0x10;
  regs->DMARDLR = 0x00;
  regs->SER = 0x00;
  regs->SSIENR = 0x00;

  uint32_t frame_format = 0;  // 0 = standard, 1 = dual, 2 = quad, 3 = octal
  uint32_t work_mode = 0;
  uint32_t data_bit_length = 8;
  uint32_t tmod = 0; // 0 = send and receive, 1 = send only, 2 = receive only, 3 = EEPROM Mode
  regs->CTRLR0 = (0
    | (work_mode << work_mode_offset)
    | (frame_format << frf_offset)
    | ((data_bit_length - 1) << dfs_offset)
    | (tmod << tmod_offset)
  );

  regs->SPI_CTRLR0 = 0;
  regs->ENDIAN = 0;  // 0 = little endian, 1 = big endian


	unsigned baudr = basespeed / speed;
	if (baudr < 2)  baudr = 2;
  regs->BAUDR = baudr;

	initialized = true;

	return true;
}

bool THwSpi_kendryte::TrySendData(uint8_t adata)
{
	return false;
}

bool THwSpi_kendryte::TryRecvData(uint8_t * dstptr)
{
	return false;
}

bool THwSpi_kendryte::SendFinished()
{
  if ((regs->SR & 0x05) == 0x04)  // from Kendryte SDK
  //if (0 == regs->TXFLR)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void THwSpi_kendryte::Run()
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

