/*
 * net_ip4.cpp
 *
 *  Created on: Mar 19, 2022
 *      Author: vitya
 */

#include "string.h"
#include "platform.h"
#include "net_ip4.h"
#include "traces.h"

uint16_t calc_ip4_header_checksum(TIp4Header * piph)
{
  uint32_t sum = 0;
  uint16_t * pd16 = (uint16_t *)(piph);
  uint16_t * pd16_end = (uint16_t *)(piph + 1);

  while (pd16 < pd16_end)
  {
    sum += __REV16(*pd16++);
  }

  sum -= __REV16(piph->csum);  // remove the csum

  //  Fold 32-bit sum to 16 bits
  while (sum >> 16)
  {
    sum = (sum & 0xffff) + (sum >> 16);
  }

  return __REV16(~sum);
}

uint16_t calc_udp4_checksum(TIp4Header * piph, uint16_t datalen)
{
  uint32_t n;
  uint32_t sum = 0;
  TUdp4Header * pudp = PUdp4Header(piph + 1); // the UDP header comes after the IP header
  uint16_t * pd16;
  uint16_t * pd16_end;  // using this needs fewer registers

  // add the two IP addresses first
  pd16 = (uint16_t *)&piph->srcaddr;
  pd16_end = pd16 + 4;
  while (pd16 < pd16_end)
  {
    sum += __REV16(*pd16++);
  }

  sum += piph->protocol; // add the protocol as well (8-bit only)
  sum += __REV16(pudp->len); // add the UDP length

  // add the UDP header parts exlusive the checksum
  pd16 = (uint16_t *)pudp;
  pd16_end = pd16 + 3;
  while (pd16 < pd16_end)
  {
    sum += __REV16(*pd16++);
  }

  // and then the data
  pd16 = (uint16_t *)(pudp + 1);
  pd16_end = pd16 + (datalen >> 1);
  while (pd16 < pd16_end)
  {
    sum += __REV16(*pd16++);
  }

  if (datalen & 1)  // one byte remained
  {
    sum += *(uint8_t *)pd16;
  }

  //  Fold 32-bit sum to 16 bits
  while (sum >> 16)
  {
    sum = (sum & 0xffff) + (sum >> 16);
  }

  return __REV16(~sum);
}

//--------------------------------------------------------------

void TArp4Table::Init(TIp4Handler * ahandler)
{
  phandler = ahandler;
  adapter  = phandler->adapter;

  firstitem = nullptr;
  lastitem = nullptr;
  freelist = nullptr;

  syspkt = adapter->CreateSysTxPacket(128); // actually 64 would be enough

  // allocate the arp table
  TArp4TableItem *  items =
      (TArp4TableItem *) phandler->adapter->AllocateNetMem(sizeof(TArp4TableItem) * max_items);

  // fill the free list
  for (int i = max_items - 1; i >= 0; --i)  // make it reverse to be linear in the memory (cache optimization)
  {
    TArp4TableItem *  item = &items[i];
    item->next = freelist;
    item->prev = nullptr; // not used here
    freelist = item;
  }
}

TArp4TableItem * TArp4Table::CreateNewItem(uint8_t * amacaddr)
{
  TArp4TableItem *  item;

  if (freelist)
  {
    item = freelist;
    freelist = freelist->next;
  }
  else
  {
    // take the last (oldest) item
    item = lastitem;

    lastitem = lastitem->prev;
    if (lastitem)
    {
      lastitem->next = nullptr;
    }
    else
    {
      firstitem = nullptr; // (impossible case)
    }
  }

  mac_address_copy(&item->macaddr[0], &amacaddr[0]);

  item->prev = nullptr;
  item->next = nullptr;
  return item;
}

void TArp4Table::Update(TIp4Addr * aipaddr, uint8_t * amacaddr)
{
  TArp4TableItem *  item = FindByMac(amacaddr);

  if (item)
  {
    // just move to forward

    // unchain
    if (item->prev)   item->prev->next = item->next;
    else              firstitem = item->next;

    if (item->next)   item->next->prev = item->prev;
    else              lastitem = item->prev;
  }
  else
  {
    item = CreateNewItem(amacaddr);
  }

  item->ipaddr = *aipaddr;

  // add as first

  item->next = firstitem;
  item->prev = nullptr;
  if (firstitem)
  {
    firstitem->prev = item;
  }
  firstitem = item;
  if (!lastitem)
  {
    lastitem = item;
  }
}

TArp4TableItem * TArp4Table::FindByIp(PIp4Addr paddr)
{
  TArp4TableItem *  item = firstitem;
  while (item)
  {
    if (item->ipaddr.u32 == paddr->u32)
    {
      return item;
    }
    item = item->next;
  }
  return nullptr;
}

TArp4TableItem * TArp4Table::FindByMac(uint8_t * amacaddr)
{
  TArp4TableItem *  item = firstitem;
  while (item)
  {
    if (    (*(uint32_t *)&item->macaddr[0] == *(uint32_t *)&amacaddr[0])
         && (*(uint16_t *)&item->macaddr[4] == *(uint16_t *)&amacaddr[4]) )
    {
      return item;
    }
    item = item->next;
  }
  return nullptr;
}

bool TArp4Table::SendWithArp(TPacketMem * pmem, PIp4Addr paddr)
{
  TArp4TableItem * arpitem = FindByIp(paddr);
  if (arpitem)
  {
    // update the destination MAC (this is the first field)
    mac_address_copy(&pmem->data[0], &arpitem->macaddr[0]);

    return adapter->SendTxPacket(pmem);
  }

  // add to the jobs
  pmem->next = nullptr;
  *(PIp4Addr)&pmem->extra[0] = *paddr; // copy the required address to the extra field

  if (lastjob)
  {
    lastjob->next = pmem;
  }
  else
  {
    firstjob = pmem;
  }

  lastjob = pmem;

  Run();

  return true;
}

bool TArp4Table::ProcessArpResponse(TPacketMem * pmem)
{
  PEthernetHeader rxeh = PEthernetHeader(&pmem->data[0]);
  PArpHeader      parp = PArpHeader(rxeh + 1);

  TRACE("ARP response.\r\n");

  Update(PIp4Addr(&parp->spa[0]), &parp->sha[0]);
  Run();  // someone probably waits for this

  return true;
}

void TArp4Table::Run()
{
  TPacketMem * pmem = firstjob;
  if (!pmem)
  {
    return;
  }

  if (0 == phase)  // prepare the request
  {
    TRACE("%u Start ARP %d.%d.%d.%d\r\n", adapter->mscounter, pmem->extra[0], pmem->extra[1], pmem->extra[2], pmem->extra[3]);

    PEthernetHeader txeh = PEthernetHeader(&syspkt->data[0]);
    PArpHeader      parp = PArpHeader(txeh + 1);

    int n;

    // setup the mac addresses
    for (n = 0; n < 6; ++n)
    {
      // broadcast:
      txeh->dest_mac[n] = 0xFF; // broadcast
      txeh->src_mac[n] = adapter->peth->mac_address[n]; // our mac

      parp->sha[n] = adapter->peth->mac_address[n]; // our mac
      parp->tha[n] = 0;
    }

    // setup the IP addresses
    for (n = 0; n < 4; ++n)
    {
      parp->spa[n] = phandler->ipaddress.u8[n];
      parp->tpa[n] = pmem->extra[n]; // the required address is copied to here
    }

    txeh->ethertype = 0x0608; // ARP, byte swapped

    parp->htype = 0x0100;
    parp->ptype = 0x0008;
    parp->hlen = 6;
    parp->plen = 4;
    parp->oper = 0x0100; // request, byte swapped

    syspkt->datalen = sizeof(TEthernetHeader) + sizeof(TArpHeader);

    adapter->SendTxPacket(syspkt);
    start_ms = adapter->mscounter;

    trycnt = 1; // reset the try count
    phase = 1;  // wait until it is sent
  }
  else if (1 == phase) // wait until the packet is sent
  {
    if (0 == syspkt->status)
    {
      phase = 5; // wait for the resolution (with the ARP response)
    }
    else if (adapter->mscounter - start_ms > response_timeout_ms)
    {
      // something is very wrong!
      TRACE("Timeout sending ARP request!\r\n");
      phase = 9; // re-sending
    }
  }
  else if (5 == phase) // check if the ip is resolved
  {
    PIp4Addr paddr = PIp4Addr(&pmem->extra[0]);
    TArp4TableItem * arpitem = FindByIp(paddr);
    if (arpitem)
    {
      TRACE("%u ARP %d.%d.%d.%d resolved, continue sending...\r\n", adapter->mscounter, pmem->extra[0], pmem->extra[1], pmem->extra[2], pmem->extra[3]);

      // the address is resolved!, we can finish this job!
      // update the destination MAC (this is the first field
      mac_address_copy(&pmem->data[0], &arpitem->macaddr[0]);

      FinishJob(true); // finish the job sending the packet

      phase = 0;  // go to the next job
    }
    else if (adapter->mscounter - start_ms > response_timeout_ms) // repeat on timeout
    {
      phase = 9; // repeat the request
    }
  }
  else if (9 == phase)
  {
    if (trycnt >= max_tries)
    {
      FinishJob(false); // give up, free the packet

      phase = 0;  // go to the next
    }
    else // try again
    {
      TRACE("%u re-trying ARP...\r\n", adapter->mscounter);

      ++trycnt;
      adapter->SendTxPacket(syspkt);
      start_ms = adapter->mscounter;
      phase = 1; // wait until it is sent
    }
  }
}

void TArp4Table::FinishJob(bool asend)
{
  TPacketMem * pmem = firstjob;
  if (!pmem)
  {
    return;
  }

  // warning: unchain required first
  firstjob = firstjob->next;
  if (!firstjob)  lastjob = nullptr;

  if (asend)
  {
    adapter->SendTxPacket(pmem);
  }
  else
  {
    adapter->ReleaseTxPacket(pmem);
  }
}


//--------------------------------------------------------------

void TUdp4Socket::Init(TIp4Handler * ahandler, uint16_t alistenport)
{
  phandler = ahandler;
  listenport = alistenport;

  phandler->AddUdpSocket(this);
}

int TUdp4Socket::Send(void * adataptr, unsigned adatalen)
{
  TPacketMem * pmem;

  if (adatalen > sizeof(pmem->data) - (sizeof(TIp4Header) + sizeof(TUdp4Header) + sizeof(TEthernetHeader)))
  {
    return -1;
  }

  pmem = phandler->adapter->AllocateTxPacket();
  if (!pmem)
  {
    return 0;  // no free packet !
  }

  // assemble the UDP packet
  PEthernetHeader eh    = PEthernetHeader(&pmem->data[0]);
  PIp4Header      iph   = PIp4Header(eh + 1);
  PUdp4Header     udph  = PUdp4Header(iph + 1);
  uint8_t *       pdata = (uint8_t *)(udph + 1);

  eh->ethertype = 0x0008; // ether type: 0x0800 = IPV4 (byte swapped)

  *PIp4Addr(&iph->srcaddr[0]) = phandler->ipaddress;
  *PIp4Addr(&iph->dstaddr[0]) = destaddr;

  memcpy(pdata, adataptr, adatalen);

  iph->hl_v = 0x45;
  iph->tos = 0;
  iph->len = __REV16(adatalen + sizeof(TIp4Header) + sizeof(TUdp4Header));
  iph->id = 0x0000;
  iph->fl_offs = __REV16(0x4000);
  iph->ttl = 64;
  iph->protocol = 17;
  iph->csum = calc_ip4_header_checksum(iph);

  udph->sport = __REV16(listenport);
  udph->dport = __REV16(destport);
  udph->len   = __REV16(adatalen + sizeof(TUdp4Header));
  udph->csum  = calc_udp4_checksum(iph, adatalen);

  pmem->datalen = adatalen + sizeof(TIp4Header) + sizeof(TUdp4Header) + sizeof(TEthernetHeader);

  if (!phandler->SendWithRouting(pmem))
  {
    return -1;
  }

  return adatalen;
}

int TUdp4Socket::Receive(void * adataptr, unsigned adatalen)
{
  int err = 0;

  TPacketMem * pmem = rxpkt_first;
  if (!pmem)
  {
    return 0;
  }

  PEthernetHeader eh    = PEthernetHeader(&pmem->data[0]);
  PIp4Header      iph   = PIp4Header(eh + 1);
  PUdp4Header     udph  = PUdp4Header(iph + 1);
  uint8_t *       pdata = (uint8_t *)(udph + 1);

  uint16_t dlen = __REV16(udph->len) - sizeof(TUdp4Header);
  if (dlen > adatalen)
  {
    err = -1;
  }
  else
  {
    err = dlen;
    memcpy(adataptr, pdata, dlen); // copy the data
  }

  srcaddr = *PIp4Addr(&iph->srcaddr[0]);
  srcport = __REV16(udph->sport);

  // unchain the pmem first
  rxpkt_first = rxpkt_first->next;
  if (!rxpkt_first)  rxpkt_last = nullptr;

  // release the packet !
  phandler->adapter->ReleaseRxPacket(pmem);

  return err;
}

void TUdp4Socket::AddRxPacket(TPacketMem * pmem)
{
  pmem->flags = PMEMFLAG_KEEP; // do not release this packet until it is processed !
  pmem->next = nullptr;
  if (rxpkt_last)
  {
    rxpkt_last->next = pmem;
  }
  else
  {
    rxpkt_first = pmem;
  }
  rxpkt_last = pmem;
}


//--------------------------------------------------------------

void TIp4Handler::Init(TNetAdapter * aadapter)
{
  adapter = aadapter;

  udp_first = nullptr;
  udp_last  = nullptr;

  arptable.Init(this);

  syspkt = adapter->AllocateTxPacket();  // reserve one TX packet for system purposes

  adapter->AddHandler(this);
}

void TIp4Handler::AddUdpSocket(TUdp4Socket * audp)
{
  if (udp_last)
  {
    udp_last->nextsocket = audp;
  }
  else
  {
    udp_last = audp;
    udp_first = audp;
  }

  audp->nextsocket = nullptr;
}


void TIp4Handler::Run()
{
  arptable.Run();
}

bool TIp4Handler::HandleRxPacket(TPacketMem * pmem)  // return true, if the packet is handled
{
  rxpkt = pmem;
  rxeh = PEthernetHeader(&pmem->data[0]);

  uint16_t etype = __REV16(rxeh->ethertype);

  if (0x0806 == etype) // ARP ?
  {
    return HandleArp();
  }
  else if (0x800 == etype) // IP
  {
    rxiph = PIp4Header(rxeh + 1);

    if (1 == rxiph->protocol) // ICMP ?
    {
      return HandleIcmp();
    }
    else if (17 == rxiph->protocol) // UDP ?
    {
      return HandleUdp();
    }
  }

  return false;
}

bool TIp4Handler::HandleArp()
{
  uint32_t  n;
  TPacketMem * pmem;

  PArpHeader parp = PArpHeader(rxeh + 1);

  if (0x0100 == parp->oper) // ARP request ?
  {
    if (*(uint32_t *)&(parp->tpa) == ipaddress.u32)  // someone want to know my IP
    {
      TRACE("ARP request from %u.%u.%u.%u\r\n", parp->spa[0], parp->spa[1], parp->spa[2], parp->spa[3] );

      // it is worth to put it into the ARP table
      arptable.Update(PIp4Addr(&parp->spa[0]), &parp->sha[0]);

      // prepare the answer

      pmem = adapter->AllocateTxPacket();
      if (!pmem)
      {
        return false;
      }

      pmem->datalen = sizeof(TEthernetHeader) + sizeof(TArpHeader);
      memcpy(&pmem->data[0], &rxpkt->data[0], pmem->datalen);

      PEthernetHeader txeh = PEthernetHeader(&pmem->data[0]);
      parp = PArpHeader(txeh + 1);

      parp->oper = 0x0200; // ARP Reply (byte swapped)

      // fill the ETH addresses
      for (n = 0; n < 6; ++n)
      {
        txeh->dest_mac[n] = txeh->src_mac[n];
        parp->tha[n] = parp->sha[n];
        txeh->src_mac[n] = adapter->peth->mac_address[n];
        parp->sha[n] = adapter->peth->mac_address[n];
      }

      // fill the IP addresses
      for (n = 0; n < 4; ++n)
      {
        parp->tpa[n] = parp->spa[n];
        parp->spa[n] = ipaddress.u8[n];
      }

      // send the packet
      adapter->SendTxPacket(pmem);  // the tx packet will be released automatically
    }
    // else: ignore it
  }
  else if (0x0200 == parp->oper)  // ARP response for me
  {
    arptable.ProcessArpResponse(rxpkt);
  }

  return true; // signalize handled
}

bool TIp4Handler::HandleIcmp()
{
  uint32_t  n;
  TPacketMem * pmem;

  PIcmpHeader rxich = PIcmpHeader(rxiph + 1);
  if (8 == rxich->type) // echo request ?
  {
    //TRACE("Echo request detected.\r\n");

    // prepare the answer

    pmem = adapter->AllocateTxPacket();
    if (!pmem)
    {
      return false;
    }

    pmem->datalen = rxpkt->datalen;
    memcpy(&pmem->data[0], &rxpkt->data[0], pmem->datalen);

    PEthernetHeader txeh = PEthernetHeader(&pmem->data[0]);
    PIp4Header  txiph = PIp4Header(txeh + 1);
    PIcmpHeader txich = PIcmpHeader(txiph + 1);

    // fill the ETH addresses
    for (n = 0; n < 6; ++n)
    {
      txeh->dest_mac[n] = txeh->src_mac[n];
      txeh->src_mac[n] = adapter->peth->mac_address[n];
    }

    // fill the IP addresses
    for (n = 0; n < 4; ++n)
    {
      txiph->dstaddr[n] = txiph->srcaddr[n];
      txiph->srcaddr[n] = ipaddress.u8[n];
    }

    txich->type = 0; // ICMP ECHO reply
    txich->code = 0;
    txich->cksum = 0;

    // the ICMP message can contain arbitrary length data, we need to calculate its size first
    uint16_t icmp_len = __REV16(txiph->len) - sizeof(TIp4Header);
    txich->cksum = __REV16(calc_icmp_checksum(txich, icmp_len));

    // send the packet
    adapter->SendTxPacket(pmem);  // the tx packet will be released automatically
  }

  return true;
}

bool TIp4Handler::HandleUdp()
{
  // rxeh, rxiph is already set
  TUdp4Header * udph = (TUdp4Header *)(rxiph + 1);

  uint16_t dport = __REV16(udph->dport);

  // search for listeners
  TUdp4Socket * udp = udp_first;
  while (udp)
  {
    if (udp->listenport == dport)
    {
      // got it
      udp->AddRxPacket(rxpkt);
      return true;  // returning true here does not releases the Rx packet here
    }
    udp = udp->nextsocket;
  }

  return true;
}

bool TIp4Handler::LocalAddress(TIp4Addr * aaddr)
{
  if ((aaddr->u32 ^ ipaddress.u32) & netmask.u32)
  {
    return false;
  }
  return true;
}

bool TIp4Handler::SendWithRouting(TPacketMem * pmem)
{
  PEthernetHeader txeh = PEthernetHeader(&pmem->data[0]);

  // insert my mac address
  mac_address_copy(&txeh->src_mac[0], &adapter->peth->mac_address[0]);

  PIp4Header  txiph  = PIp4Header(&pmem->data[sizeof(TEthernetHeader)]);
  PIp4Addr    pdstip = PIp4Addr(&txiph->dstaddr[0]);

  // 1. is it a local network packet or it must be sent to the gateway?
  if (LocalAddress(pdstip))
  {
    return arptable.SendWithArp(pmem, pdstip);
  }
  else  // send to the gateway
  {
    return arptable.SendWithArp(pmem, &gwaddress);
  }

  return false;
}
