/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2024 Viktor Nagy, nvitya
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
 *  file:     hweth_nv_apb_eth.cpp
 *  brief:    VIHAL driver for the NV APB Ethernet: https://github.com/nvitya/nv_apb_eth
 *  created:  2024-02-11
 *  authors:  nvitya
*/


#include <stdio.h>
#include "string.h"
#include <stdarg.h>

#include "platform.h"

#if defined(NV_APB_ETH_BASE_ADDR)

#include "hweth.h"
#include "traces.h"
#include "clockcnt.h"

bool THwEth_nv_apb_eth::InitMac(void * prxdesclist, uint32_t rxcnt, void * ptxdesclist, uint32_t txcnt)
{
	uint32_t  n;
	uint32_t  tmp;

	regs = (TNvApbEthRegs *)apb_address;
	if (!regs)
	{
	  return false;
	}

	// check the fix ID
	tmp = regs->VERSION;
	if (((tmp >> 24) & 0xFF) != 0xE0)
	{
	  TRACE("NV_APB_ETH: invalid version: 0x%08X\r\n", tmp);
	  return false;
	}

	// Stop RX+TX if it was running before

	regs->RXCTRL = 0;
	regs->TXCTRL = 0;

  SetMacAddress(&mac_address[0]);

  // initialize the descriptors, they are used differently here
  tx_desc_list = (HW_ETH_DMA_DESC *)ptxdesclist;
  tx_desc_count = txcnt;
  rx_desc_list = (HW_ETH_DMA_DESC *)prxdesclist;
  rx_desc_count = txcnt;
  InitDescList(true,  tx_desc_count, tx_desc_list);
  InitDescList(false, rx_desc_count, rx_desc_list);
  rx_idx = 0;
  tx_idx = 0;

	// start the RX

  uint32_t rxctrl = (0
    | (rx_slot_words <<  0)  // RX_SLOT_SIZE(12) in 32-bit units
    | (0  << 29)  // IGNORE_ERR
    | (0  << 30)  // PROMISC
    | (0  << 31)  // RXEN
  );
  if (promiscuous_mode)  rxctrl |= (1 << 30);
  if (ignore_rx_errors)  rxctrl |= (1 << 29);

  regs->RXCTRL = rxctrl;

  // start the TX

  uint32_t txctrl = (0
    | (tx_slot_words  <<  0)  // TX_SLOT_SIZE(12) in 32-bit units
    | (0  << 30)  // MANUAL_CRC
    | (0  << 31)  // TXEN
  );
  if (manual_crc) txctrl |= (1 << 30);

  regs->TXCTRL = txctrl;


	return true;
}

void THwEth_nv_apb_eth::InitDescList(bool istx, int bufnum, HW_ETH_DMA_DESC * pdesc_list)
{
	int i;
	HW_ETH_DMA_DESC *  pdesc = pdesc_list;

	memset(pdesc_list, 0, bufnum * sizeof(HW_ETH_DMA_DESC));
}

void THwEth_nv_apb_eth::AssignRxBuf(uint32_t idx, TPacketMem * pmem, uint32_t datalen)
{
	if (idx >= rx_desc_count)  return;

	int i;
	HW_ETH_DMA_DESC *  pdesc = &rx_desc_list[idx];

  pmem->idx = idx;

	pdesc->LEN = datalen;
	pdesc->ADDR = (uint32_t *)&pmem->data[0];
}

bool THwEth_nv_apb_eth::TryRecv(TPacketMem * * ppmem)
{
  uint32_t  rxget = regs->RXGET;
  if (0 == (rxget & NV_APB_ETH_RXGET_FILLED))
  {
    return false;
  }

  // search a pmem to copy the data there

	HW_ETH_DMA_DESC * pdesc = rx_desc_list;

	int i = 0;
	while (i < rx_desc_count)
	{
		if (0 == pdesc->LEN) // is this RX pmem free?
		{
      TPacketMem * pmem = (TPacketMem *)((uint8_t *)(pdesc->ADDR) - HWETH_PMEM_HEAD_SIZE);

			pmem->idx = i;

			// copy the data here
      uint32_t *  pdst = (uint32_t *)&pmem->data[0];
      uint32_t    slotaddr = ((rxget >> 0) & 0xFFF);
      uint32_t    fbytes = ((rxget >> 12) & 0xFFF);
      uint32_t    fwords = ((fbytes + 3) >> 2);
		  uint32_t *  psrc = &regs->RXMEM[slotaddr];
		  uint32_t *  psrc_end = psrc + fwords;
		  while (psrc < psrc_end)
		  {
		    *pdst++ = *psrc++;
		  }

      pmem->datalen = fbytes;
      *ppmem = pmem;

      // release this RX slot
      regs->RXGET = 0;
			return true;
		}

		++pdesc;
		++i;
	}

	return false;
}

void THwEth_nv_apb_eth::ReleaseRxBuf(TPacketMem * pmem)
{
	HW_ETH_DMA_DESC *  pdesc = &rx_desc_list[pmem->idx];
	pdesc->LEN = 0;
}

bool THwEth_nv_apb_eth::TrySend(uint32_t * pidx, void * pdata, uint32_t datalen)
{
  uint32_t  txput = regs->TXPUT;
  if (txput & NV_APB_ETH_TXPUT_FILLED)
  {
    return false;  // no free slot yet
  }

  if (datalen > (tx_slot_words << 2)) // too much bytes ?
  {
    return false;
  }

  uint32_t    slotaddr = ((txput >> 0) & 0xFFF);
  uint32_t    fbytes = datalen;
  uint32_t *  psrc = (uint32_t *)pdata;
  uint32_t *  psrc_end = psrc + (datalen >> 2);
  uint32_t *  pdst = &regs->TXMEM[slotaddr];
  while (psrc < psrc_end) // copy the words
  {
    *pdst++ = *psrc++;
  }

  uint32_t tailbytes = (datalen & 3);
  if (3 == tailbytes)
  {
    *pdst++ = (*psrc & 0x00FFFFFF);
  }
  else if (2 == tailbytes)
  {
    *pdst++ = (*psrc & 0x0000FFFF);
  }
  else if (1 == tailbytes)
  {
    *pdst++ = (*psrc & 0x000000FF);
  }

  // add padding zeroes to 60 bytes to reach the minimum packet size of 64 bytes (with CRC)
  int padwords = (15 - int(datalen >> 2));
  if (padwords > 0)
  {
    fbytes += (padwords << 2);
    uint32_t * pdst_end = pdst + padwords;
    while (pdst < pdst_end)
    {
      *pdst++ = 0;
    }
  }

  regs->TXPUT = (0
    | (0      <<  0)  // SLOT_ADDR(12) (ignored by writes)
    | (fbytes << 12)  // FBYTES(12)
    | (1      << 31)  // FILLED
  );

  return true;
}

bool THwEth_nv_apb_eth::SendFinished(uint32_t idx)
{
  // this is not perfect, report true if there is an empty slot available

  return (0 == (regs->TXPUT & NV_APB_ETH_TXPUT_FILLED));
}

void THwEth_nv_apb_eth::Start(void)
{
  regs->TXCTRL |= NV_APB_ETH_TXCTRL_EN;
  regs->RXCTRL |= NV_APB_ETH_RXCTRL_EN;
}

void THwEth_nv_apb_eth::Stop(void)
{
  regs->TXCTRL &= ~NV_APB_ETH_TXCTRL_EN;
  regs->RXCTRL &= ~NV_APB_ETH_RXCTRL_EN;
}

void THwEth_nv_apb_eth::SetMacAddress(uint8_t * amacaddr)
{
	if (amacaddr != &mac_address[0])
	{
		memcpy(&mac_address, amacaddr, 6);
	}

  if (!regs)
  {
    return;
  }

	regs->MACADDR1 = *(uint32_t *)(&mac_address[0]);
	regs->MACADDR2 = *(uint16_t *)(&mac_address[4]);
}

void THwEth_nv_apb_eth::SetSpeed(bool speed100)
{
}

void THwEth_nv_apb_eth::SetDuplex(bool full)
{
}

void THwEth_nv_apb_eth::StartMiiWrite(uint8_t reg, uint16_t data)
{
  uint32_t cmd = (0
    | (data <<  0)  // WRDATA(16)
    | (reg  << 16)  // REGADDR(5)
    | (phy_address << 21)  // PHYADDR(5)
    | (1    << 30)  // WRITE_NREAD: 0 = read, 1 = write
    | (1    << 31)  // START
  );
  regs->MDIO_CMD = cmd; // start
}

void THwEth_nv_apb_eth::StartMiiRead(uint8_t reg)
{
  uint32_t cmd = (0
    | (0    <<  0)  // WRDATA(16)
    | (reg  << 16)  // REGADDR(5)
    | (phy_address << 21)  // PHYADDR(5)
    | (0    << 30)  // WRITE_NREAD: 0 = read, 1 = write
    | (1    << 31)  // START
  );
  regs->MDIO_CMD = cmd; // start
}

bool THwEth_nv_apb_eth::IsMiiBusy()
{
	return ((regs->MDIO_STATUS & NV_APB_ETH_MDIO_BUSY) ? false : true);
}

void THwEth_nv_apb_eth::NsTimeStart()
{
}

uint64_t THwEth_nv_apb_eth::NsTimeRead()
{
	return 0;
}

uint64_t THwEth_nv_apb_eth::GetTimeStamp(uint32_t idx)
{
	return 0;
}


#endif
