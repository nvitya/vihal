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
 *  file:     hwi2c_rp.cpp
 *  brief:    R2040 I2C Master Driver
 *  created:  2022-06-10
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"
#include "hwi2c.h"
#include "rp_utils.h"


bool THwI2c_rp::Init(int adevnum)
{
	unsigned tmp;
	unsigned perid;

	initialized = false;

	devnum = adevnum;
	regs = nullptr;

  uint32_t reset_mask;
	if (0 == devnum)
	{
		regs = i2c0_hw;
		reset_mask = RESETS_RESET_I2C0_BITS;
	}
	else if (1 == devnum)
	{
		regs = i2c1_hw;
    reset_mask = RESETS_RESET_I2C1_BITS;
	}

	if (!regs)
	{
		return false;
	}

  rp_reset_control(reset_mask, true);  // force reset
  rp_reset_control(reset_mask, false); // remove reset

  regs->enable = 0; // disable

  uint32_t creg = (0
    | (0  << 10)  // STOP_DEF_IF_MASTER_ACTIVE
    | (1  <<  9)  // RX_FIFO_FULL_HLD_CTRL: 1 = hold bus when RX_FIFO full
    | (0  <<  8)  // TX_EMPTY_CTRL: 1 = controlled TX empty interrupt
    | (0  <<  7)  // STOP_DET_IF_ADDRESSED
    | (1  <<  6)  // IC_SLAVE_DISABLED: 1 = master mode
    | (1  <<  5)  // IC_RESTART_EN: 1 = master restart enabled
    | (0  <<  4)  // IC_10BITADDR_MASTER: 0 = 7-bit master addressing mode
    | (0  <<  3)  // IC_10BITADDR_SLAVE: 0 = 7-bit slave addressing mode
    | (0  <<  1)  // SPEED(2): 1 = standard (100 kBit/s max), 2 = fast mode (400 kBit/s)
    | (1  <<  0)  // MASTER_MODE: 1 = master mode
  );
  if (speed > 100000)
  {
    creg |= (2 << 1); // SPEED: 2 = fast
  }
  else
  {
    creg |= (1 << 1); // SPEED: 1 = standard
  }
  regs->con = creg;

  unsigned halfclockdiv = ((SystemCoreClock / speed) >> 1);

  regs->ss_scl_lcnt = halfclockdiv;
  regs->ss_scl_hcnt = halfclockdiv;

  if (speed > 100000)
  {
    // these registers are writeable only SPEED = 2 (fast)
    regs->fs_scl_lcnt = halfclockdiv;
    regs->fs_scl_hcnt = halfclockdiv;
  }

  // enable it only for transfers

  initialized = true;

	return true;
}

void THwI2c_rp::DmaAssign(bool istx, THwDmaChannel * admach)
{
  if (istx)
  {
    txdma = admach;
    if (admach)
    {
      admach->Prepare(istx, (void *)&regs->data_cmd, 0);
    }
  }
  else
  {
    rxdma = admach;
    if (admach)
    {
      admach->Prepare(istx, (void *)&regs->data_cmd, 0);
    }
  }
}

void THwI2c_rp::RunTransaction()
{
  if (0 == trastate)
  {
    istx = (0 != curtra->iswrite);
    dataptr = (uint8_t *)curtra->dataptr;
    datalen = curtra->datalen;
    remainingbytes = datalen;

    if (regs->clr_tx_abrt) { } // clear abort flags

    // set the slave target address
    regs->tar = curtra->address;

    // prepare the unit with blocked TX for preprogramming the FIFO
    regs->enable = (I2C_IC_ENABLE_TX_CMD_BLOCK_BITS | I2C_IC_ENABLE_ENABLE_BITS);

    unsigned extracnt = ((curtra->extra >> 24) & 3);
    if (extracnt)
    {
      if (extracnt > 2)
      {
        regs->data_cmd = ((curtra->extra >> 16) & 0xFF);
        regs->data_cmd = ((curtra->extra >>  8) & 0xFF);
      }
      else if (extracnt > 1)
      {
        regs->data_cmd = ((curtra->extra >> 8) & 0xFF);
      }
      regs->data_cmd = (curtra->extra & 0xFF);
    }

    if (istx)
    {
      dmaused = (txdma && (datalen > 1));
      if (dmaused)
      {
        xfer.srcaddr = dataptr;
        xfer.bytewidth = 1;
        xfer.count = remainingbytes - 1;
        xfer.flags = 0; // peripheral transfer with defaults

        dataptr += xfer.count;
        remainingbytes = 1;

        txdma->StartTransfer(&xfer);
      }
      else
      {
        if (datalen <= 1)
        {
          regs->data_cmd = (*dataptr | HWI2C_CMD_STOP);
          --remainingbytes;
        }
      }

      rxcmd_dmaused = false;
      rxcmd_remaining = 0;
    }
    else
    {
      rxcmd_remaining = remainingbytes;
      if (extracnt)
      {
        // restart required !
        regs->data_cmd = (HWI2C_CMD_RESTART | HWI2C_CMD_RXDATA);
        --rxcmd_remaining;
      }

      rxcmd_dmaused = (txdma && rxdma && (rxcmd_remaining > 1));
      if (rxcmd_dmaused)
      {
        cmdxfer.srcaddr = &rxcmd_data;
        cmdxfer.bytewidth = 2;
        cmdxfer.count = rxcmd_remaining - 1;
        cmdxfer.flags = DMATR_NO_SRC_INC;  // keep sending the RXDATA CMD

        rxcmd_remaining = 1; // keep the last one for stop

        txdma->StartTransfer(&cmdxfer);
      }

      dmaused = (rxcmd_dmaused && (remainingbytes > 1));
      if (dmaused)
      {
        xfer.dstaddr = dataptr;
        xfer.bytewidth = 1;
        xfer.count = remainingbytes - 1;
        xfer.flags = 0; // peripheral transfer with defaults

        dataptr += xfer.count;
        remainingbytes = 1;

        rxdma->StartTransfer(&xfer);
      }
    }

    // start processing the TX FIFO:
    regs->enable &= ~I2C_IC_ENABLE_TX_CMD_BLOCK_BITS;

    trastate = 1;
  }

  if (trastate <= 2)  // error handling
  {
    uint32_t abrt = regs->tx_abrt_source;
    if (abrt &
          (
                I2C_IC_TX_ABRT_SOURCE_ABRT_USER_ABRT_BITS
              | I2C_IC_TX_ABRT_SOURCE_ARB_LOST_BITS
              | I2C_IC_TX_ABRT_SOURCE_ABRT_MASTER_DIS_BITS
              | I2C_IC_TX_ABRT_SOURCE_ABRT_10B_RD_NORSTRT_BITS
              | I2C_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK_BITS
              | I2C_IC_TX_ABRT_SOURCE_ABRT_10B_RD_NORSTRT_BITS
              | I2C_IC_TX_ABRT_SOURCE_ABRT_HS_NORSTRT_BITS | I2C_IC_TX_ABRT_SOURCE_ABRT_SBYTE_NORSTRT_BITS
              | I2C_IC_TX_ABRT_SOURCE_ABRT_SBYTE_ACKDET_BITS
              | I2C_IC_TX_ABRT_SOURCE_ABRT_HS_ACKDET_BITS
              | I2C_IC_TX_ABRT_SOURCE_ABRT_GCALL_READ_BITS
              | I2C_IC_TX_ABRT_SOURCE_ABRT_GCALL_NOACK_BITS
              | I2C_IC_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK_BITS
              | I2C_IC_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK_BITS
              | I2C_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK_BITS
          )
        ) // any error ?
    {
      if (abrt & I2C_IC_TX_ABRT_SOURCE_ABRT_SLV_ARBLOST_BITS)
      {
        curtra->error = ERR_I2C_ARBLOST;
      }
      else if (abrt & (I2C_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK_BITS | I2C_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK_BITS))
      {
        curtra->error = ERR_I2C_ACK;
      }
      else
      {
        curtra->error = ERR_I2C_BUS;
      }

      if (curtra->error)
      {
        trastate = 90;
        regs->enable = 0;
        curtra->completed = true;
        return;
      }
    }
  }

  if (1 == trastate) // send and receive main data
  {
    if (istx)
    {
      if (dmaused && txdma->Active())
      {
        return;
      }

      while (remainingbytes > 0)
      {
        if (0 == (regs->status & I2C_IC_STATUS_TFNF_BITS))
        {
          return; // TX fifo is full
        }

        uint32_t data = *dataptr++;
        if (1 == remainingbytes)
        {
           data |= HWI2C_CMD_STOP;
        }
        regs->data_cmd = data;
        --remainingbytes;
      }
    }
    else
    {
      if (dmaused && (txdma->Active() || rxdma->Active()))
      {
        return;
      }

      while (rxcmd_remaining > 0)
      {
        if (0 == (regs->status & I2C_IC_STATUS_TFNF_BITS))
        {
          break; // TX fifo is full
        }

        if (1 == rxcmd_remaining)
        {
          regs->data_cmd = (HWI2C_CMD_RXDATA | HWI2C_CMD_STOP);
        }
        else
        {
          regs->data_cmd = HWI2C_CMD_RXDATA;
        }
        --rxcmd_remaining;
      }

      while (remainingbytes > 0)
      {
        if (0 == (regs->status & I2C_IC_STATUS_RFNE_BITS))
        {
          break; // RX fifo is empty
        }

        *dataptr++ = regs->data_cmd;
        --remainingbytes;
      }

      if (remainingbytes > 0)
      {
        return;
      }
    }

    trastate = 2; // closing
  }

  if (2 == trastate)  // wait until the last byte is sent and the master goes to idle
  {
    if (0 == (regs->status & I2C_IC_STATUS_TFE_BITS))
    {
      return; // TX is not finished yet
    }

    // wait until stop detected
    if (regs->status & I2C_IC_STATUS_ACTIVITY_BITS)
    {
      return;
    }

    trastate = 50;
  }

  // finished, disable
  regs->enable = 0;
  curtra->completed = true; // transaction finished.
}

