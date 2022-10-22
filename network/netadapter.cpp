/*
 * netadapter.cpp
 *
 *  Created on: Mar 20, 2022
 *      Author: vitya
 */


#include "netadapter.h"
#include "clockcnt.h"
#include "traces.h"

bool TNetAdapter::Init(THwEth * aeth, void * anetmem, unsigned anetmemsize)
{
  peth = aeth;
  netmem = (uint8_t *)anetmem;
  netmem_size = anetmemsize;

  initialized = false;
  firsthandler = nullptr;
  first_sending_pkt = nullptr;
  last_sending_pkt = nullptr;

  mscounter = 0;
  last_mscounter_clocks = CLOCKCNT;
  clocks_per_ms = SystemCoreClock / 1000;

  // memory allocation
  netmem_allocated = 0;
  // RX and TX descriptors
  rx_desc_mem = AllocateNetMem(sizeof(HW_ETH_DMA_DESC) * max_rx_packets);
  tx_desc_mem = AllocateNetMem(sizeof(HW_ETH_DMA_DESC) * max_tx_packets);
  if (!rx_desc_mem || !tx_desc_mem)
  {
    TRACE("NetAdapter: Error allocating RX/TX descriptors!\r\n");
    TRACE_FLUSH();
    return false;
  }

  rx_pmem = AllocateNetMem(sizeof(TPacketMem) * max_rx_packets);
  if (!rx_pmem)
  {
    TRACE("NetAdapter: Error allocating RX packet buffers!\r\n");
    TRACE_FLUSH();
    return false;
  }

  tx_pmem = AllocateNetMem(sizeof(TPacketMem) * max_tx_packets);
  if (!tx_pmem)
  {
    TRACE("NetAdapter: Error allocating TX packet buffers!\r\n");
    TRACE_FLUSH();
    return false;
  }

  peth->promiscuous_mode = false;
  peth->hw_ip_checksum = false; //true;

  if (!peth->Init(rx_desc_mem, max_rx_packets, tx_desc_mem, max_tx_packets))
  {
    TRACE("NetAdapter: ETH INIT FAILED!\r\n");
    TRACE_FLUSH();
    return false;
  }

  // Assign RX packet buffers, required for the receive

  for (unsigned n = 0; n < max_rx_packets; ++n)
  {
    peth->AssignRxBuf(n, (TPacketMem *)&rx_pmem[sizeof(TPacketMem) * n], HWETH_MAX_PACKET_SIZE);
  }

  // Initialize TX packet allocation
  TPacketMem *  pmem = (TPacketMem *)tx_pmem;
  TPacketMem *  prevpmem = pmem;
  first_free_tx_pmem = pmem;
  for (unsigned n = 1; n < max_tx_packets; ++n)
  {
    pmem->flags = 0;
    pmem->max_datalen = sizeof(pmem->data); // these are max sized packets

    ++pmem;
    prevpmem->next = pmem;
    pmem->next = nullptr;
    prevpmem = pmem;
  }

  // start the network interface

  peth->Start();

  initialized = true;
  return true;
}

void TNetAdapter::AddHandler(TProtocolHandler * ahandler)
{
  ahandler->next = nullptr;
  if (!firsthandler)
  {
    firsthandler = ahandler;
  }
  else
  {
    TProtocolHandler * ph = firsthandler;
    while (ph->next)
    {
      ph = ph->next;
    }
    ph->next = ahandler;
  }
}

uint8_t * TNetAdapter::AllocateNetMem(unsigned asize)
{
  if (asize > NetMemFree())
  {
    TRACE("AllocateNetMem FAILED!\r\n");
    TRACE_FLUSH();

    __BKPT();  // serious error, change the memory sizes, the adapter configuration

    return nullptr;
  }

  uint8_t * result = &netmem[netmem_allocated];
  netmem_allocated += asize;

  return result;
}

TPacketMem * TNetAdapter::CreateSysTxPacket(unsigned asize)  // allocates from the NetMem a usually smaller packet
{
  TPacketMem * result = (TPacketMem *)AllocateNetMem(asize + HWETH_PMEM_HEAD_SIZE);
  if (result)
  {
    result->max_datalen = asize;
    result->flags = PMEMFLAG_SYS;
  }

  return result;
}

void TNetAdapter::Run()
{
  TPacketMem *        pmem;
  TProtocolHandler *  ph;

  // update the mscounter
  unsigned elapsed_clk = CLOCKCNT - last_mscounter_clocks;
  unsigned elapsed_ms = elapsed_clk / clocks_per_ms;
  mscounter += elapsed_ms;
  last_mscounter_clocks += elapsed_ms * clocks_per_ms;

  if (peth)
  {
    peth->PhyStatusPoll(); // must be called regularly
  }

  // free Sended Tx Packets
  // TODO: check the whole chain ?
  while (first_sending_pkt && peth->SendFinished(first_sending_pkt->idx))
  {
    pmem = first_sending_pkt;
    first_sending_pkt = first_sending_pkt->next; // unchain first before free !

    //TRACE("Releasing TX packet %u\r\n", pmem->idx);
    ReleaseTxPacket(pmem);
  }
  if (!first_sending_pkt)
  {
    last_sending_pkt = nullptr;
  }

  // check for Rx Packets

  if (peth->TryRecv(&pmem))
  {
    pmem->flags = 0;

    ph = firsthandler;
    while (ph)
    {
      if (ph->HandleRxPacket(pmem))
      {
        break;
      }
      ph = ph->next;
    }

    if (!ph)
    {
      // the packet was not handled
    }

    if (0 == (pmem->flags & PMEMFLAG_KEEP))  // release the packet when not explicitly told to keep it
    {
      peth->ReleaseRxBuf(pmem);
    }
  }

  // Run Idle parts

  ph = firsthandler;
  while (ph)
  {
    ph->Run();
    ph = ph->next;
  }
}

TPacketMem * TNetAdapter::AllocateTxPacket()
{
  if (first_free_tx_pmem)
  {
    TPacketMem * result = first_free_tx_pmem;
    first_free_tx_pmem = first_free_tx_pmem->next;
    return result;
  }
  else
  {
    return nullptr;
  }
}

void TNetAdapter::ReleaseTxPacket(TPacketMem * apmem)
{
  apmem->status = 0;

  if (apmem->flags & PMEMFLAG_SYS)
  {
    return; // do not touch the system packets
  }

  apmem->next = first_free_tx_pmem;
  first_free_tx_pmem = apmem;
}

void TNetAdapter::ReleaseRxPacket(TPacketMem * apmem)
{
  peth->ReleaseRxBuf(apmem);
}

bool TNetAdapter::SendTxPacket(TPacketMem * apmem)  // the packet will be automatically released
{
  // send the packet on the Ethernet

  uint32_t idx;
  if (!peth->TrySend(&idx, &apmem->data[0], apmem->datalen))
  {
    ReleaseTxPacket(apmem);
    return false;
  }

  // add to the sending chain
  apmem->idx = idx;
  apmem->status = 1; // sending active
  apmem->next = nullptr;

  if (last_sending_pkt)
  {
    last_sending_pkt->next = apmem;
    last_sending_pkt = apmem;
  }
  else
  {
    first_sending_pkt = apmem;
    last_sending_pkt = apmem;
  }

  return true;
}
