/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL TEST project: https://github.com/nvitya/vihaltests
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
 *  file:     espwifi_uart.h
 *  brief:    UART (AT) WiFi Driver for Espressif Modules
 *  date:     2022-11-27
 *  authors:  nvitya
*/

#ifndef SRC_ESPWIFI_UART_H_
#define SRC_ESPWIFI_UART_H_

#include "stdint.h"
#include "platform.h"
#include "hwuart.h"
#include "hwdma.h"
#include "hwpins.h"
#include "net_ip4.h"
#include "strparse.h"

#define UARTCOMM_RXBUF_SIZE       512  // circular DMA buffer
#define UARTCOMM_TXBUF_SIZE      1600  // one tx response must fit into it
#define UARTCOMM_MAX_RX_MSG_LEN  1600  // maximal length of a parsed message

#define ESPWIFI_MAX_SOCKETS   5
#define ESPWIFI_MAX_EXPCMDR   4

class TEspAtUdpSocket;

#ifndef ESPWIFI_MAX_PACKET_SIZE
  #define ESPWIFI_MAX_PACKET_SIZE  1516
#endif

#define ESPWIFI_PMEM_HEAD_SIZE     20

typedef struct TEspPmem  // 1.5 kByte
{
  uint16_t           datalen;
  uint16_t           max_datalen;
  uint16_t           ip_port;
  uint16_t           _reserved;
  TIp4Addr           ip_addr;
  TEspAtUdpSocket *  psocket;
  TEspPmem *         next;

  uint8_t            data[ESPWIFI_MAX_PACKET_SIZE];
//
} TEspPmem, * PEspPmem;


class TEspWifiUart
{
  friend class TEspAtUdpSocket;

protected:
  uint8_t             state = 0;
  uint8_t             txstate = 0;
  uint8_t             initstate = 0;
  uint8_t             initsocknum = 0;
  unsigned            ipd_rx_remaining = 0;
  unsigned            prev_state_time = 0;
  unsigned            cmd_start_time = 0;
  unsigned            send_start_time = 0;
  unsigned            us_clocks = 0;
  unsigned            ms_clocks = 0;
  unsigned            cmd_timeout_clocks = 0;
  bool                cmd_error = 0;
  bool                cmd_ignore_error = false;

  TStrParseObj        sp;

  uint16_t            curlinestart = 0;
  uint16_t            curlinelen = 0;
  uint16_t            rxmsglen = 0;
  uint16_t            rxdmapos = 0;

  uint16_t            txlen = 0;

  THwDmaTransfer      dmaxfer_tx;
  THwDmaTransfer      dmaxfer_rx;

public:  // required resources

  THwUart             uart;
  THwDmaChannel       dma_rx;
  THwDmaChannel       dma_tx;
  TGpioPin            pin_rst;
  TGpioPin            pin_en;

public:  // settings

  TIp4Addr            ipaddress;
  TIp4Addr            netmask;
  TIp4Addr            gwaddress;
  TIp4Addr            dns;
  TIp4Addr            dns2;

  const char *        ssid = "SSID";
  const char *        password = "PASSWORD";

  unsigned            initial_uart_speed = 115200;
  unsigned            uart_speed = 2000000;  // 2 MBit/s can be easily realized on the most targets
                                             // maximum 3 Mbit/s supported on the ESP8266

  bool                disable_command_echo = true; // disable command echo

public:
  bool                initialized = false;
  unsigned            invalid_ipd_count = 0;  // number of invalid +IPD (data) messages
  unsigned            data_send_error_count = 0;
  TEspAtUdpSocket *   sockets[ESPWIFI_MAX_SOCKETS] = {0};

  bool                Init(void * anetmem, unsigned anetmemsize);
  void                Run();  // processes Rx and Tx

  void                AddUdpSocket(TEspAtUdpSocket * audp);

  bool                IsLinkUp() { return (0 == initstate); }

  void                StartCommand(const char * fmt, ...);

  unsigned            NetMemSize() { return netmem_size; }
  unsigned            NetMemFree() { return netmem_size - netmem_allocated; }

protected:

  uint8_t *           netmem = nullptr;
  unsigned            netmem_size = 0;
  unsigned            netmem_allocated = 0;
  TEspPmem *          first_free_pmem = nullptr;

  char                expected_cmd_response[ESPWIFI_MAX_EXPCMDR][32];
  uint8_t             cmd_responses = 0;

  bool                cmd_running = false;
  bool                cmd_echo_received = false;
  bool                ready_received = false;
  bool                wifi_got_ip = false;
  bool                wifi_connected = false;

  int                 ipd_sock_num = 0;
  TIp4Addr            ipd_ip_addr;
  uint16_t            ipd_port = 0;
  uint16_t            ipd_data_len = 0;
  uint16_t            ipd_data_start = 0;

  TIp4Addr            sp_ipaddr;

  TEspPmem *          sending_first = nullptr;
  TEspPmem *          sending_last = nullptr;

  bool                InitHw();  // board specific implementation

  void                ResetConnection();

  void                RunRx();
  void                RunTx();
  void                RunInit(); // initialization state-machine
  void                ProcessRxMessage();
  void                ProcessIpdStart();
  void                ProcessIpdData();

  uint8_t *           AllocateNetMem(unsigned asize);
  TEspPmem *          AllocatePmem();
  void                ReleasePmem(TEspPmem * apmem);

  void                ExpectCmdResponse(uint8_t aidx, const char * fmt, ...);
  bool                ParseIpAddr();

  unsigned            AddTx(void * asrc, unsigned len); // returns the amount actually written
  void                AddTxMessage(const char * fmt, ...);
  void                StartSendTxBuffer();
  inline unsigned     TxAvailable() { return sizeof(txbuf) - txlen; }

  void                AddSendingPacket(TEspPmem * apmem);
  void                RemoveSendingPacket(TEspPmem * apmem);

protected: // these big buffers must come to the last

  uint8_t             rxmsgbuf[UARTCOMM_MAX_RX_MSG_LEN];  // parsed message buffer
  uint8_t             rxdmabuf[UARTCOMM_RXBUF_SIZE];  // circular buffer, might contain more messages
  uint8_t             txbuf[UARTCOMM_TXBUF_SIZE];

};

class TEspAtUdpSocket
{
public:

  int               socketnum = -1;
  bool              initialized = false;
  bool              connected = false;

public:
  TIp4Addr          destaddr;
  uint16_t          destport = 0;

  TIp4Addr          srcaddr;
  uint16_t          srcport = 0;

  uint16_t          listenport = 0;

  uint16_t          idcounter = 0;

  TEspWifiUart *    pwifim = nullptr;

  TEspPmem *        rxpkt_first = nullptr;
  TEspPmem *        rxpkt_last  = nullptr;
  TEspPmem *        txpkt_first = nullptr;
  TEspPmem *        txpkt_last  = nullptr;

  TEspAtUdpSocket * sending_next = nullptr;

  void              Init(TEspWifiUart * awifim, uint16_t alistenport);

  int               Send(void * adataptr, unsigned adatalen);
  int               Receive(void * adataptr, unsigned adatalen);

  void              Reset();

  void              AddRxPacket(TEspPmem * pmem);
};

#endif /* SRC_ESPWIFI_UART_H_ */
