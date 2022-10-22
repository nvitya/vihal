/*
 * netadapter.h
 *
 *  Created on: Mar 20, 2022
 *      Author: vitya
 */

#ifndef NETWORK_NETADAPTER_H_
#define NETWORK_NETADAPTER_H_

#include "stdint.h"
#include "network.h"
#include "hweth.h"

class TNetAdapter;

class TProtocolHandler
{
public:
  TNetAdapter *       adapter = nullptr;

  TProtocolHandler *  next = nullptr;

  virtual             ~TProtocolHandler() { } // never ment to be destructed, but the GCC requires this

public: // virtual functions
  virtual bool        HandleRxPacket(TPacketMem * apkt) { return false; }  // return true, if the packet is handled
  virtual void        Run() { }
};

// TNetAdapter could be the child of the THwEth too

class TNetAdapter
{
public: // settings

  uint8_t             max_rx_packets = 8;
  uint8_t             max_tx_packets = 8;

public:
  bool                initialized = false;
  THwEth *            peth = nullptr;

  TProtocolHandler *  firsthandler = nullptr;
  TPacketMem *        first_sending_pkt = nullptr;
  TPacketMem *        last_sending_pkt = nullptr;

  bool                Init(THwEth * aeth, void * anetmem, unsigned anetmemsize);

  void                Run(); // must be called regularly

  bool                SendTxPacket(TPacketMem * apmem);  // the packet will be automatically released

  TPacketMem *        AllocateTxPacket();
  void                ReleaseTxPacket(TPacketMem * apmem);
  void                ReleaseRxPacket(TPacketMem * apmem);

  uint8_t *           AllocateNetMem(unsigned asize);
  TPacketMem *        CreateSysTxPacket(unsigned asize);  // allocates from the NetMem a usually smaller packet

  void                AddHandler(TProtocolHandler * ahandler);

  bool                IsLinkUp() { return peth->link_up; }

  uint32_t            mscounter = 0; // millisecond timer for various timing

  unsigned            NetMemSize() { return netmem_size; }
  unsigned            NetMemFree() { return netmem_size - netmem_allocated; }

protected: // internal memory management

  unsigned            last_mscounter_clocks = 0;
  unsigned            clocks_per_ms = 0;

  TPacketMem *        first_free_tx_pmem = nullptr;

  uint8_t *           rx_desc_mem = nullptr;
  uint8_t *           tx_desc_mem = nullptr;

  uint8_t *           rx_pmem = nullptr;
  uint8_t *           tx_pmem = nullptr;

  uint8_t *           netmem = nullptr;
  unsigned            netmem_size = 0;
  unsigned            netmem_allocated = 0;

};


#endif /* NETWORK_NETADAPTER_H_ */
