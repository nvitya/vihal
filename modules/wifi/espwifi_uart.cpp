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
 *  file:     espwifi_uart.cpp
 *  brief:    UART (AT) WiFi Driver for Espressif Modules
 *  date:     2022-11-27
 *  authors:  nvitya
 *  notes:
 *    I had problems with ESP8266 Firmwares less than 2.2.0.0.
 *    The firmware I've found working perfectly on the ESP-01S and WROOM-02 was this:
 *      https://github.com/jandrassy/UnoWiFiDevEdSerial1/wiki/files/ESP8266-1MB-tx1rx3-AT_V2.2.zip
*/

#include "string.h"
#include "hwpins.h"
#include "espwifi_uart.h"
#include "clockcnt.h"
#include "mp_printf.h"
#include "traces.h"

// the TEspWifiUart::InitHw() are moved to the board_pins.cpp

bool TEspWifiUart::Init(void * anetmem, unsigned anetmemsize)
{
  unsigned n;

  initialized = false;

  netmem = (uint8_t *)anetmem;
  netmem_size = anetmemsize;
  netmem_allocated = 0;

  first_free_pmem = nullptr;
  while (NetMemFree() >= sizeof(TEspPmem))
  {
    TEspPmem * pmem = (TEspPmem *)AllocateNetMem(sizeof(TEspPmem));
    if (!pmem)
    {
      break;
    }

    pmem->datalen = 0;
    pmem->max_datalen = sizeof(pmem->data);

    pmem->next = first_free_pmem;
    first_free_pmem = pmem;
  }

  for (n = 0; n < ESPWIFI_MAX_SOCKETS; ++n)
  {
    sockets[n] = nullptr;
  }

  if (!InitHw()) // must be provided externally
  {
    return false;
  }

  if (pin_en.Assigned())
  {
    pin_en.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_1);
  }

  if (pin_rst.Assigned())
  {
    pin_rst.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);
  }

  uart.DmaAssign(true,  &dma_tx);
  uart.DmaAssign(false, &dma_rx);

  // start the DMA receive with circular DMA buffer
  rxdmapos = 0;
  ipd_rx_remaining = 0;
  dmaxfer_rx.bytewidth = 1;
  dmaxfer_rx.count = sizeof(rxdmabuf);
  dmaxfer_rx.dstaddr = &rxdmabuf[0];
  dmaxfer_rx.flags = DMATR_CIRCULAR;
  uart.DmaStartRecv(&dmaxfer_rx);

  rxmsglen = 0;
  txlen = 0;
  state = 0;
  txstate = 0;
  cmd_running = false;
  initstate = 1;  // start initialization
  prev_state_time = CLOCKCNT;

  ResetConnection();

  us_clocks = SystemCoreClock / 1000000;
  ms_clocks = SystemCoreClock / 1000;
  cmd_timeout_clocks = ms_clocks * 5000;

  initialized = true;
  return true;
}

void TEspWifiUart::RunInit()
{
  unsigned t = CLOCKCNT;
  unsigned elapsedclocks = t - prev_state_time;

  if (cmd_error && !cmd_ignore_error && (initstate < 100))
  {
    initstate = 100;
  }

  if (1 == initstate)
  {
    TRACE("ESP-AT: starting initialization...\r\n");

    uart.SetBaudRate(initial_uart_speed);  // switch back to the initial speed mode

    // start the initialization with reset
    if (pin_en.Assigned())
    {
      pin_en.Set1();
    }

    if (pin_rst.Assigned())
    {
      pin_rst.Set0();
      prev_state_time = t;
      ++initstate;
    }
  }
  else if (2 == initstate) // hold the reset low
  {
    if (elapsedclocks > 10 * ms_clocks)
    {
      if (pin_rst.Assigned())  pin_rst.Set1();

      rxmsglen = 0;

      ready_received = false;
      ResetConnection();

      prev_state_time = t;
      ++initstate;
    }
  }
  else if (3 == initstate)  // wait for the ready message
  {
    if (ready_received)
    {
      StartCommand("AT+CWQAP");  // do not connect to the stored access point
      cmd_ignore_error = true;
      initstate = 4;
    }
    else if (elapsedclocks > 5000 * ms_clocks)  // 5 s timeout
    {
      initstate = 1; // back to the reset
      prev_state_time = t;
    }
  }
  else if (4 == initstate)
  {
    if (disable_command_echo)
    {
      StartCommand("ATE0");  // disable command echo
      ExpectCmdResponse(0, "ATE0");
    }
    else
    {
      StartCommand("ATE1");  // enable command echo
      ExpectCmdResponse(1, "ATE0");
    }
    ++initstate;
  }
  else if (5 == initstate)
  {
    StartCommand("AT+CWMODE=1"); // set station mode
    ++initstate;
  }
  else if (6 == initstate)
  {
    StartCommand("AT+GMR"); // print version info
    initstate = 10;
  }

  // switch to high-speed mode

  else if (10 == initstate)
  {
    TRACE("ESP-AT: switch to %u bit/s\r\n", uart_speed);

    StartCommand("AT+UART_CUR=%u,8,1,0,0", uart_speed);
    ++initstate;
  }
  else if (11 == initstate) // wait until the TX message is sent
  {
    uart.SetBaudRate(uart_speed);

    TRACE("ESP-AT: high speed uart configured.\r\n");
    initstate = 20;
  }

  // prepare station

  else if (20 == initstate)
  {
    // configure the IP address
    StartCommand("AT+CIPSTA=\"%u.%u.%u.%u\",\"%u.%u.%u.%u\",\"%u.%u.%u.%u\"",
        ipaddress.u8[0], ipaddress.u8[1], ipaddress.u8[2], ipaddress.u8[3],
        gwaddress.u8[0], gwaddress.u8[1], gwaddress.u8[2], gwaddress.u8[3],
        netmask.u8[0], netmask.u8[1], netmask.u8[2], netmask.u8[3]
    );
    ++initstate;
  }
  else if (21 == initstate) // process setting static IP
  {
    StartCommand("AT+CWJAP=\"%s\",\"%s\"", ssid, password);
    ++initstate;
  }
  else if (22 == initstate) // joining the AP
  {
    if (wifi_got_ip)
    {
      StartCommand("AT+CIPMUX=1");
      ++initstate;
    }
  }
  else if (23 == initstate) // show the remote IP address in the +IPD data
  {
    StartCommand("AT+CIPDINFO=1");

    initstate = 50; // add sockets
  }

  // prepare the UDP sockets

  else if (50 == initstate)
  {
    for (unsigned n; n < ESPWIFI_MAX_SOCKETS; ++n)
    {
      if (sockets[n])  sockets[n]->Reset();
    }
    initsocknum = 0;
    ++initstate;
  }
  else if (51 == initstate)
  {
    while ((initsocknum < ESPWIFI_MAX_SOCKETS) && (!sockets[initsocknum] || sockets[initsocknum]->initialized))
    {
      ++initsocknum;
    }

    if (initsocknum < ESPWIFI_MAX_SOCKETS)
    {
      TEspAtUdpSocket * psock = sockets[initsocknum];

      StartCommand("AT+CIPSTART=%i,\"UDP\",\"%u.%u.%u.%u\",%u,%u,2",
          psock->socketnum,
          0,0,0,0,  // destination IP address
          psock->listenport,  // remote (destination) port, can not be 0 ?
          psock->listenport   // local port
          // mode = 2 is the last parameter: allows to specify destination for every send
      );

      ExpectCmdResponse(0, "%i,CONNECT", psock->socketnum);

      ++initstate;
    }
    else
    {
      // no more sockets to initialize
      initstate = 200; // finish initialization
    }
  }
  else if (52 == initstate) // process UDP socket initialization result
  {
    if (cmd_responses & 1)
    {
      TRACE("ESP-AT: UDP socket connected.\r\n");
      sockets[initsocknum]->connected = true;
    }

    // initialize the next UDP socket
    ++initsocknum;
    initstate = 51;
  }


  //---------------------------------------------------------------------------

  else if (100 == initstate)
  {
    TRACE("ESP-AT: Initialization error.\r\n");
    ++initstate;
  }
  else if (101 == initstate)
  {
    // stay here
  }

  else if (200 == initstate)
  {
    TRACE("ESP-AT: Init finished.\r\n");
    initstate = 0;
  }

}

unsigned TEspWifiUart::AddTx(void * asrc, unsigned len) // returns the amount actually written
{
  unsigned available = TxAvailable();
  if (0 == available)
  {
    return 0;
  }

  if (len > available)  len = available;

  uint8_t * srcp = (uint8_t *)asrc;
  uint8_t * dstp = &txbuf[txlen];
  uint8_t * endp = dstp + len;
  while (dstp < endp)
  {
    *dstp++ = *srcp++;
  }

  txlen += len;

  return len;
}

void TEspWifiUart::StartCommand(const char *fmt, ...)
{
  va_list arglist;
  va_start(arglist, fmt);

  // allocate format buffer on the stack:
  char fmtbuf[FMT_BUFFER_SIZE];

  char * pch = &fmtbuf[0];
  *pch = 0;

  int len = mp_vsnprintf(pch, FMT_BUFFER_SIZE, fmt, arglist);

  if (len > 0)
  {
#if 0
    if (0 == memcmp("AT+CWJAP", &fmtbuf[0], 8)) // do not print the WIFI secrets !
    {
      TRACE("ESP-AT -> \"AT+CWJAP=\"***\",\"***\"\"\r\n");
    }
    else
    {
      TRACE("ESP-AT -> \"%s\"\r\n", &fmtbuf[0]);
    }
#endif

    AddTx(&fmtbuf[0], len);
    AddTx((void *)"\r\n", 2);
  }

  va_end(arglist);

  for (int i = 0; i < ESPWIFI_MAX_EXPCMDR; ++i)
  {
    expected_cmd_response[i][0] = 0;
  }
  cmd_responses = 0;

  cmd_ignore_error = false;
  cmd_running = true;
  cmd_echo_received = false;
  cmd_start_time = CLOCKCNT;
}

void TEspWifiUart::AddTxMessage(const char * fmt, ...)
{
  va_list arglist;
  va_start(arglist, fmt);

  // allocate format buffer on the stack:
  char fmtbuf[FMT_BUFFER_SIZE];

  char * pch = &fmtbuf[0];
  *pch = 0;

  int len = mp_vsnprintf(pch, FMT_BUFFER_SIZE, fmt, arglist);

  if (len > 0)
  {
    AddTx(&fmtbuf[0], len);
  }

  va_end(arglist);
}

void TEspWifiUart::ResetConnection()
{
  wifi_connected = false;
  wifi_got_ip = false;
}

void TEspWifiUart::ExpectCmdResponse(uint8_t aidx, const char * fmt, ...)
{
  if (aidx >= ESPWIFI_MAX_EXPCMDR)  return;

  va_list arglist;
  va_start(arglist, fmt);

  char * pch = &expected_cmd_response[aidx][0];
  *pch = 0;

  int len = mp_vsnprintf(pch, FMT_BUFFER_SIZE, fmt, arglist);

  va_end(arglist);
}

uint8_t * TEspWifiUart::AllocateNetMem(unsigned asize)
{
  if (asize > NetMemFree())
  {
    TRACE("AllocateNetMem FAILED!\r\n");
    TRACE_FLUSH();

    //__BKPT();  // serious error, change the memory sizes, the adapter configuration

    return nullptr;
  }

  uint8_t * result = &netmem[netmem_allocated];
  netmem_allocated += asize;

  return result;
}

TEspPmem * TEspWifiUart::AllocatePmem()
{
  if (first_free_pmem)
  {
    TEspPmem * result = first_free_pmem;
    first_free_pmem = first_free_pmem->next;
    return result;
  }
  else
  {
    return nullptr;
  }
}

void TEspWifiUart::ReleasePmem(TEspPmem * apmem)
{
  apmem->next = first_free_pmem;
  first_free_pmem = apmem;
}


void TEspWifiUart::StartSendTxBuffer()
{
  if (txlen && !dma_tx.Active())
  {
    // setup the TX DMA and flip the buffer

    dmaxfer_tx.flags = 0;
    dmaxfer_tx.bytewidth = 1;
    dmaxfer_tx.count = txlen;
    dmaxfer_tx.srcaddr = &txbuf[0];

    uart.DmaStartSend(&dmaxfer_tx);

    txlen = 0;
  }
}

void TEspWifiUart::RunRx()
{
  // RX processing
  uint16_t newrxdmapos = sizeof(rxdmabuf) - dma_rx.Remaining();
  if (newrxdmapos >= sizeof(rxdmabuf))
  {
    // fix for Remaining() == 0 (sometimes the linked list operation is processed only later)
    newrxdmapos = 0;
  }

  while (rxdmapos != newrxdmapos)
  {
    uint8_t b = rxdmabuf[rxdmapos];

    if (ipd_rx_remaining > 0)
    {
      if (rxmsglen < sizeof(rxmsgbuf))
      {
        rxmsgbuf[rxmsglen] = b;
        ++rxmsglen;
      }

      --ipd_rx_remaining;
      if (0 == ipd_rx_remaining)
      {
        if (ipd_data_start + ipd_data_len > sizeof(rxmsgbuf))
        {
          // the data does not fit into our buffer, just ignore it.
        }
        else // process the IPD data
        {
          ProcessIpdData();
        }
        rxmsglen = 0;
      }
    }
    else
    {
      if (10 == b)
      {
        rxmsgbuf[rxmsglen] = 0; // zero terminate the message

        if (rxmsglen > 0)
        {
          ProcessRxMessage();
          rxmsglen = 0;
        }
      }
      else if (13 == b) // ignore, usually comes before 10
      {

      }
      else
      {
        if (rxmsglen < sizeof(rxmsgbuf))
        {
          rxmsgbuf[rxmsglen] = b;
          ++rxmsglen;

          if (':' == b)  // special case for +IPD messages
          {
            if (0 == memcmp("+IPD", &rxmsgbuf[0], 4))
            {
              ProcessIpdStart(); // sets the ipd_rx_remaining
            }
          }
        }
        else
        {
          // TODO: figure out something better
          rxmsglen = 0;  // reset on overflow
        }
      }
    }

    ++rxdmapos;
    if (rxdmapos >= sizeof(rxdmabuf))  rxdmapos = 0;
  }
}

void TEspWifiUart::RunTx()
{
  TEspPmem * pmem = sending_first;
  if (!pmem)
  {
    return;
  }

  if (0 == txstate)
  {
    if (!cmd_running)
    {
      StartCommand("AT+CIPSEND=%u,%u,\"%u.%u.%u.%u\",%u",
          pmem->psocket->socketnum,
          pmem->datalen,
          pmem->ip_addr.u8[0], pmem->ip_addr.u8[1], pmem->ip_addr.u8[2], pmem->ip_addr.u8[3],
          pmem->ip_port
      );
      send_start_time = CLOCKCNT;
      ++txstate;
    }
  }
  else if (1 == txstate) // waiting for command result
  {
    if (cmd_running)
    {
      return;
    }

    if (rxmsglen < 1)
    {
      if (CLOCKCNT - send_start_time > 1000 * ms_clocks)
      {
        TRACE("ESP-AT: send timeout\r\n");
        ++data_send_error_count;
        RemoveSendingPacket(pmem);
        txstate = 0;
      }

      return;
    }

    if (rxmsgbuf[0] != '>')
    {
      ++data_send_error_count;
      RemoveSendingPacket(pmem);
      txstate = 0;
      return;
    }

    rxmsglen = 0; // clear the '>' from the buffer, otherwise it will be processed

    //TRACE("ESP-AT: send data prompt detected.\r\n")

    AddTx(&pmem->data[0], pmem->datalen);
    send_start_time = CLOCKCNT;
    txstate = 10;
  }
  else if (10 == txstate)
  {
    // Let the RX state machine to process the messages

    if (CLOCKCNT - send_start_time > 2000 * ms_clocks)
    {
      TRACE("ESP-AT: send timeout\r\n");
      ++data_send_error_count;
      RemoveSendingPacket(pmem);
      txstate = 0;
      return;
    }
  }
  else if (11 == txstate) // send finished
  {
    //TRACE("ESP-AT: send finished.\r\n");
    RemoveSendingPacket(pmem);
    txstate = 0;
  }
}

void TEspWifiUart::Run()
{
  RunRx();

  if (cmd_running)
  {
    if (CLOCKCNT - cmd_start_time > cmd_timeout_clocks)
    {
      cmd_error = true;
      cmd_running = false;
      TRACE("ESP-AT: cmd timeout!\r\n");
    }
  }

  if (initstate)
  {
    if (!cmd_running)
    {
      RunInit();
    }
  }
  else
  {
    RunTx();
  }

  StartSendTxBuffer();  // Sending buffered tx messages
}

void TEspWifiUart::AddUdpSocket(TEspAtUdpSocket * audp)
{
  unsigned n = 0;
  while (n < ESPWIFI_MAX_SOCKETS)
  {
    if ((sockets[n] == audp) || (sockets[n] == nullptr))
    {
      audp->socketnum = n;
      sockets[n] = audp;
      return;
    }
    ++n;
  }
}

void TEspWifiUart::ProcessRxMessage()
{
  sp.Init((char *)&rxmsgbuf[0], rxmsglen);

  if (!ready_received)  // check for ready
  {
    if (sp.CheckSymbol("ready"))
    {
      TRACE("ESP-AT: ready detected.\r\n");
      ready_received = true;
      return;
    }

    return; // at the beginning there comes garbage stop here
  }

  unsigned n;

  if (cmd_running)
  {
    if (sp.CheckSymbol("OK"))
    {
      cmd_running = false;
      cmd_error = false;
      return;
    }

    if (sp.CheckSymbol("ERROR"))
    {
      cmd_running = false;
      cmd_error = true;
      return;
    }

    if (!cmd_echo_received && sp.CheckSymbol("AT+"))
    {
      cmd_echo_received = true;
      return;
    }

    // processing command additional data
    // like "n,CONNECT" for UDP socket connections

    for (n = 0; n < ESPWIFI_MAX_EXPCMDR; ++n)
    {
      if ((expected_cmd_response[n][0] != 0) && sp.CheckSymbol(&expected_cmd_response[n][0]))
      {
        cmd_responses |= (1 << n);
        return;
      }
    }
  }

  if (10 == txstate) // expecting send result ?
  {
    if (sp.CheckSymbol("SEND OK"))
    {
      txstate = 11; // go on the send finished
    }
    else if (sp.CheckSymbol("ERROR"))
    {
      TRACE("ESP-AT: Error sending data\r\n");
      ++data_send_error_count;
      txstate = 11;
    }
    else if (sp.CheckSymbol("Recv "))  // "Recv 123 bytes"
    {
      // just ignore this info message
    }

    return;
  }
  else if (sp.CheckSymbol("WIFI CONNECTED"))
  {
    TRACE("ESP-AT: Wifi Connected.\r\n");
    wifi_connected = true;
    return;
  }
  else if (sp.CheckSymbol("WIFI GOT IP"))
  {
    TRACE("ESP-AT: Wifi got IP.\r\n");
    wifi_got_ip = true;
    return;
  }
  else  // some other asynchronous messages
  {
    TRACE("ESP-AT: Unprocessed msg: \"%s\"\r\n", &rxmsgbuf[0]);
  }
}

void TEspWifiUart::ProcessIpdStart()
{
  ipd_data_len = 0;
  ipd_rx_remaining = 0;

  sp.Init((char *)&rxmsgbuf[0], rxmsglen);

  if (!sp.CheckSymbol("+IPD,"))
  {
    return;
  }

  //  "+IPD,0,10,"192.168.0.99",58145:0123456789"

  if (!sp.ReadDecimalNumbers())
  {
    ++invalid_ipd_count;  // invalid socket id
    return;
  }

  int i = sp.PrevToInt();
  if ((i < 0) || (i >= ESPWIFI_MAX_SOCKETS) || !sockets[i])
  {
    ++invalid_ipd_count;  // invalid socket
    return;
  }

  ipd_sock_num = i;

  sp.CheckSymbol(","); // skip comma

  if (!sp.ReadDecimalNumbers())
  {
    ++invalid_ipd_count; // invalid data length
    return;
  }
  ipd_data_len = sp.PrevToInt();

  // UDP source IP address
  if (!sp.CheckSymbol(","))
  {
    ++invalid_ipd_count; // IP address quote is missing, mandatory for the UDP
    return;
  }
  sp.CheckSymbol("\""); // some FW use quotes some not
  if (!ParseIpAddr()) // result goes to the sp_ipaddr
  {
    ++invalid_ipd_count; // invalid IP address
    return;
  }
  ipd_ip_addr = sp_ipaddr;

  sp.CheckSymbol("\""); // some FW use quotes some not
  sp.CheckSymbol(","); // skip closing quote and comma

  // source Port
  if (!sp.ReadDecimalNumbers())
  {
    ++invalid_ipd_count;  // invalid string
    return;
  }
  ipd_port = sp.PrevToInt();

  if (!sp.CheckSymbol(":")) // finally the data marker
  {
    ++invalid_ipd_count;  // data marker is missing
    return;
  }

  ipd_data_start = (uint8_t *)sp.readptr - &rxmsgbuf[0]; // here comes the data
  ipd_rx_remaining = ipd_data_len;
}

void TEspWifiUart::ProcessIpdData()
{
  TEspAtUdpSocket * psock = sockets[ipd_sock_num];
  TEspPmem * pmem = AllocatePmem();
  if (!pmem)
  {
    TRACE("ESP-AT: no more pme to store RX data!\r\n");
    return;
  }

  if (ipd_data_len > pmem->max_datalen)
  {
    TRACE("ESP-AT: data chunk does not fit into a pmem buffer!\r\n");
    ReleasePmem(pmem);
    return;
  }

  pmem->datalen = ipd_data_len;
  pmem->ip_addr = ipd_ip_addr;  // acquired in ParseIpAddr()
  pmem->ip_port = ipd_port;
  memcpy(&pmem->data[0], &rxmsgbuf[ipd_data_start], ipd_data_len);
  psock->AddRxPacket(pmem);

  //TRACE("ESP-AT: %i bytes received for socket %i\r\n", ipd_data_len, psock->socketnum);
  //TRACE("ESP-AT:   \"%s\"\r\n", &rxmsgbuf[0]);
}

bool TEspWifiUart::ParseIpAddr()
{
  for (int n = 0; n < 4; ++n)
  {
    sp.SkipSpaces();
    if (n > 0)
    {
      if (!sp.CheckSymbol("."))
      {
        TRACE("Invalid IP\r\n");
        return false;
      }
      sp.SkipSpaces();
    }

    if (!sp.ReadDecimalNumbers())
    {
      TRACE("Invalid IP\r\n");
      return false;
    }
    sp_ipaddr.u8[n] = sp.PrevToInt();
  }

  return true;
}

void TEspWifiUart::AddSendingPacket(TEspPmem * apmem)
{
  if (sending_first)
  {
    // check if already added !
    TEspPmem * pmem = sending_first;
    while (pmem)
    {
      if (pmem == apmem)
      {
        return; // already added
      }
      pmem = pmem->next;
    }

    apmem->next = nullptr;
    sending_last->next = apmem;
    sending_last = apmem;
  }
  else
  {
    apmem->next = nullptr;
    sending_first = apmem;
    sending_last = apmem;
  }
}

void TEspWifiUart::RemoveSendingPacket(TEspPmem * apmem)
{
  TEspPmem * pmem_prev = nullptr;
  TEspPmem * pmem = sending_first;
  while (pmem)
  {
    if (pmem == apmem)
    {
      if (pmem_prev)
      {
        pmem_prev->next = pmem->next;
      }

      if (pmem == sending_last)
      {
        sending_last = pmem_prev;
      }

      if (pmem == sending_first)
      {
        sending_first = pmem->next;
      }

      ReleasePmem(pmem);
      return;
    }

    pmem_prev = pmem;
    pmem = pmem->next;
  }
}

//-----------------------------------------------------------------------------

void TEspAtUdpSocket::Init(TEspWifiUart * awifim, uint16_t alistenport)
{
  pwifim = awifim;
  listenport = alistenport;

  pwifim->AddUdpSocket(this);
}

void TEspAtUdpSocket::Reset()
{
  initialized = false;
  connected = false;
}

int TEspAtUdpSocket::Send(void * adataptr, unsigned adatalen)
{
  TEspPmem * pmem;

  pmem = pwifim->AllocatePmem();
  if (!pmem)
  {
    return 0;  // no free packet !
  }

  if (pmem->max_datalen < adatalen)
  {
    pwifim->ReleasePmem(pmem);
    return -1;
  }

  pmem->ip_addr = destaddr;
  pmem->ip_port = destport;
  pmem->datalen = adatalen;

  memcpy(&pmem->data[0], adataptr, adatalen);

  // add to the sending packet chain
  pmem->next = nullptr;
  if (txpkt_last)
  {
    txpkt_last->next = pmem;
  }
  else
  {
    txpkt_first = pmem;
  }
  txpkt_last = pmem;

  pmem->psocket = this;
  pwifim->AddSendingPacket(pmem);

  return adatalen;
}

int TEspAtUdpSocket::Receive(void * adataptr, unsigned adatalen)
{
  int err = 0;

  TEspPmem * pmem = rxpkt_first;
  if (!pmem)
  {
    return 0;
  }

  if (pmem->datalen > adatalen)
  {
    err = -1;
  }
  else
  {
    err = pmem->datalen;
    memcpy(adataptr, &pmem->data[0], pmem->datalen); // copy the data
  }

  srcaddr = pmem->ip_addr;
  srcport = pmem->ip_port;

  // unchain the pmem first
  rxpkt_first = rxpkt_first->next;
  if (!rxpkt_first)  rxpkt_last = nullptr;

  // release the packet !
  pwifim->ReleasePmem(pmem);

  return err;
}

void TEspAtUdpSocket::AddRxPacket(TEspPmem *pmem)
{
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

