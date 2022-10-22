/*
 * net_ip4.h
 *
 *  Created on: Mar 19, 2022
 *      Author: vitya
 */

#ifndef NETWORK_NET_IP4_H_
#define NETWORK_NET_IP4_H_

#include "stdint.h"
#include "network.h"
#include "netadapter.h"
#include "hweth.h"

typedef struct
{
  union
  {
    uint8_t   u8[4];
    uint32_t  u32;
  };

  inline void Set(int a0, int a1, int a2, int a3)
  {
    u8[0] = a0;
    u8[1] = a1;
    u8[2] = a2;
    u8[3] = a3;
  }
//
} TIp4Addr, * PIp4Addr;

typedef struct
{
  uint8_t   hl_v;      /**< Header length and version */
  uint8_t   tos;       /**< Type of service */
  uint16_t  len;       /**< Total length */
  uint16_t  id;        /**< Identification */
  uint16_t  fl_offs;   /**< Flags + Fragment offset field */
  uint8_t   ttl;       /**< Time to live */
  uint8_t   protocol;  /**< Protocol */
  uint16_t  csum;      /**< Checksum */
  uint8_t   srcaddr[4];  /**< Source IP address */
  uint8_t   dstaddr[4];  /**< Destination IP address */
//
} TIp4Header, * PIp4Header;


typedef struct
{
  uint16_t  sport;   // source port
  uint16_t  dport;   // destination port
  uint16_t  len;     // length
  uint16_t  csum;    // checksum
//
} TUdp4Header, * PUdp4Header;

typedef struct TArp4TableItem
{
  TIp4Addr  ipaddr;
  uint8_t   macaddr[6];
  uint8_t   _pad[2];
  uint32_t  timestamp_ms;

  TArp4TableItem *  prev;
  TArp4TableItem *  next;
//
} TArp4TableItem, * PArp4TableItem;

class TIp4Handler;

class TArp4Table
{
public:
  TArp4TableItem *    firstitem = nullptr;
  TArp4TableItem *    lastitem  = nullptr;
  TArp4TableItem *    freelist  = nullptr;

  TNetAdapter *       adapter = nullptr;
  TIp4Handler *       phandler = nullptr;

  uint8_t             max_items = 8;
  uint8_t             max_tries = 5;  // after so many tries will be given up
  uint32_t            response_timeout_ms = 5000;


  TPacketMem *        firstjob = nullptr;
  TPacketMem *        lastjob = nullptr;

  void                Init(TIp4Handler * ahandler);
  void                Update(TIp4Addr * aipaddr, uint8_t * amacaddr);
  TArp4TableItem *    FindByIp(PIp4Addr aipaddr);
  TArp4TableItem *    FindByMac(uint8_t * amacaddr);

  bool                SendWithArp(TPacketMem * pmem, PIp4Addr paddr);
  bool                ProcessArpResponse(TPacketMem * pmem);

  void                Run(); // handle resolution state machine

protected:
  int                 phase = 0;
  int                 trycnt = 0;
  uint32_t            start_ms = 0;

  TArp4TableItem *    CreateNewItem(uint8_t * amacaddr);

  TPacketMem *        syspkt = nullptr;

  void                FinishJob(bool asend);
};

class TUdp4Socket
{
public:
  TIp4Addr          destaddr;
  uint16_t          destport = 0;

  TIp4Addr          srcaddr;
  uint16_t          srcport = 0;

  uint16_t          listenport = 0;

  TIp4Handler *     phandler = nullptr;

  TPacketMem *      rxpkt_first = nullptr;
  TPacketMem *      rxpkt_last  = nullptr;

  TPacketMem *      txpkt_first = nullptr;  // waiting for address resolution
  TPacketMem *      txpkt_last  = nullptr;

  TUdp4Socket *     nextsocket = nullptr;

  void Init(TIp4Handler * ahandler, uint16_t alistenport);

  int Send(void * adataptr, unsigned adatalen);
  int Receive(void * adataptr, unsigned adatalen);

  void AddRxPacket(TPacketMem * pmem);
};

class TIp4Handler : public TProtocolHandler
{
public:
  TIp4Addr            ipaddress;
  TIp4Addr            netmask;
  TIp4Addr            gwaddress;

  TUdp4Socket *       udp_first = nullptr;
  TUdp4Socket *       udp_last  = nullptr;

  TArp4Table          arptable; // this is a small object

  TPacketMem *        syspkt = nullptr; // is this necessary?

  TPacketMem *        rxpkt = nullptr;
  TEthernetHeader *   rxeh  = nullptr;
  TIp4Header *        rxiph = nullptr;


  void                Init(TNetAdapter * aadapter);
  virtual void        Run();

  virtual bool        HandleRxPacket(TPacketMem * pmem);  // return true, if the packet is handled

  bool                SendWithRouting(TPacketMem * pmem);
  bool                LocalAddress(TIp4Addr * aaddr);

public:
  void                AddUdpSocket(TUdp4Socket * audp);

protected:
  bool                HandleArp();
  bool                HandleIcmp();
  bool                HandleUdp();
};

uint16_t calc_ip4_header_checksum(TIp4Header * piph);
uint16_t calc_udp4_checksum(TIp4Header * piph, uint16_t datalen);

inline void  mac_address_copy(uint8_t * pdst, uint8_t * psrc)
{
  *(uint32_t *)pdst = *(uint32_t *)psrc;
  *(uint16_t *)(pdst + 4) = *(uint16_t *)(psrc + 4);
}

#endif /* NETWORK_NET_IP4_H_ */
