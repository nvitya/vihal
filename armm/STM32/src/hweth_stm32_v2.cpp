/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2018 Viktor Nagy, nvitya
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
 *  file:     hweth_stm32_v2.cpp
 *  brief:    STM32 H7 Ethernet MAC
 *  created:  2023-11-26
 *  authors:  nvitya
*/

#include "platform.h"

#if defined(MCUSF_H7)

#include "string.h"
#include "stm32_utils.h"
#include "hweth_stm32_v2.h"

bool THwEth_stm32_v2::InitMac(void * prxdesclist, uint32_t rxcnt, void * ptxdesclist, uint32_t txcnt)
{
  uint32_t n;
  uint32_t tmp;

  // Select RMII Mode, must be done before enabling the hw
  RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
  if (RCC->APB4ENR) { } // read back for delay
  SYSCFG->PMCR |= SYSCFG_PMCR_EPIS_SEL_2;  // select RMII mode
  if (SYSCFG->PMCR) { }

  // enable clocks
  RCC->AHB1ENR |= RCC_AHB1ENR_ETH1MACEN;
  if (RCC->AHB1ENR) { } // read back for delay
  RCC->AHB1ENR |= RCC_AHB1ENR_ETH1TXEN;
  if (RCC->AHB1ENR) { } // read back for delay
  RCC->AHB1ENR |= RCC_AHB1ENR_ETH1RXEN;
  if (RCC->AHB1ENR) { } // read back for delay

  regs = ETH;

  // software reset
  regs->DMAMR |= ETH_DMAMR_SWR; // software reset
  while (regs->DMAMR & ETH_DMAMR_SWR)
  {
    __NOP();
  }

  // start the nanosecond timer and enable timestamping
  regs->MACTSCR = 0x0003; // enable time in fine mode
  regs->MACSSIR = 0;
  // start from 0
  regs->MACSTNR = 0;
  regs->MACSTSUR = 0;
  regs->MACTSCR = 0x0007;

  // the SystemCoreClock must be higher than 100 MHz !!!

  float mhz = SystemCoreClock / 1000000; // this is round
  regs->MACTSAR = (100. / mhz) * (float)0x100000000;
  regs->MACTSCR |= 0x0020;  // use the new ADDEND register
  regs->MACSSIR = 10;       // increment by 10 if the addend accumulator overflows
  regs->MACTSCR = 0x0103;   // start fine mode, timestamp all frames

  SetupMii(CalcMdcClock(), phy_address);

  // setup 1 us tick
  regs->MAC1USTCR = (stm32_bus_speed(0) / 1000000) - 1;

  // MAC Configuration

  /*------------------------ MACCR Configuration --------------------*/

  tmp = 0
    | (0 <<  0)  // RE: receive enable
    | (0 <<  1)  // TE: transmit enable
    | (0 <<  2)  // PRELEN(2): ?
    | (0 <<  4)  // DC: Deferral check
    | (0 <<  5)  // BL(2): back-off limit
    | (0 <<  9)  // DR: Retry disable
    | (0 <<  9)  // DCRS: Disable Carrier Sense During Transmission
    | (0 << 10)  // DO: Receive Own Disable
    | (0 << 11)  // ECRSFD: Enable Carrier Sense Before Transmission in Full-Duplex Mode
    | (0 << 12)  // LM: Loopback Mode
    | (1 << 13)  // DM: Duplex Mode
    | (1 << 14)  // FES: Fast Ethernet, 1 = 100 MBit/s
    | (0 << 16)  // JE: Jumbo Packet Enable
    | (0 << 17)  // JD: Jabber Disable
    | (0 << 19)  // WD: Watchdog Disable
    | (0 << 20)  // ACS: Automatic PAD+CRC stripping
    | (0 << 21)  // CST: CRC stripping for Type packets
    | (0 << 22)  // S2KP: IEEE 802.3as Support for 2K Packets
    | (0 << 23)  // GPSLCE: Giant Packet Size Limit Control Enable
    | (0 << 24)  // IFG(3): Inter-Frame Gap, 3 = 72 Bit times
    | (0 << 27)  // IPC: Checksum Offload = generate frame checksum by hardware
    | (3 << 28)  // SARC(3): Source Address Insertion/Replace Control: 3 = replace the source address with my MAC
    | (0 << 31)  // ARPEN: ARP Offload Enable
  ;
  if (loopback)        tmp |= (1 << 12);
  if (hw_ip_checksum)  tmp |= (1 << 27);
  regs->MACCR = tmp;

  tmp = (0
    | (0 << 25)  // EIPG(5): Extended Inter-Packet Gap
    | (0 << 24)  // EIPGEN: Extended Inter-Packet Gap Enable
    | (0 << 18)  // USP: Unicast Slow Protocol Packet Detect
    | (0 << 17)  // SPEN: Slow Protocol Detection Enable
    | (0 << 16)  // DCRCC: Disable CRC Checking for Received Packets
    | (0 <<  0)  // GPSL(14): Giant Packet Size Limit
  );
  regs->MACECR = tmp;

  // MAC Packet Filter / MACFFR

  tmp = 0
    | (0 <<  0)  // PR: Promiscous Mode
    | (0 <<  1)  // HUC: Hash Unicast
    | (0 <<  2)  // HMC: Hash Multicast
    | (0 <<  3)  // DAIF: DA Inverse Filtering
    | (0 <<  4)  // PM: Pass All Multicast
    | (0 <<  5)  // DBF: Disable Broadcast Frames
    | (2 <<  6)  // PCF(2): Pass Control Frames
    | (0 <<  8)  // SAIF: SA Inverse Filtering
    | (0 <<  9)  // SAF: Source Address Filter Enable
    | (0 << 10)  // HPF: Hash or Perfect Filter
    | (0 << 16)  // VTFE: VLAN Tag Filter Enable
    | (0 << 20)  // IPFE: Layer 3 and Layer 4 Filter Enable
    | (0 << 21)  // DNTU: Drop Non-TCP/UDP over IP Packets
    | (0 << 31)  // RA: Receive All
  ;
  if (promiscuous_mode)  tmp |= ((1 << 0) | (1u << 31));
  regs->MACPFR = tmp;

  regs->MACWTR = 0; // disable watchdog

  // Tx Flow Control
  tmp = (0
    | (0  << 16)  // PT(16): Pause Time
    | (0  <<  7)  // DZPQ: Disable Zero-Quanta Pause
    | (0  <<  4)  // PLT(3): Pause Low Threshold
    | (0  <<  1)  // TFE: Transmit Flow Control Enable
    | (0  <<  0)  // FCB_BPA: Flow Control Busy or Backpressure Activate
  );
  regs->MACTFCR = tmp;

  // Rx Flow Control
  tmp = (0
    | (0  <<  1)  // UP: Unicast Pause Packet Detect
    | (0  <<  0)  // RFE: Receive Flow Control Enable
  );
  regs->MACRFCR = tmp;

  // Tx Queue Operating Mode
  tmp = (0
    | (7  << 16)  // TQS(3): Transmit queue size
    | (0  <<  4)  // TTC(3): Transmit Threshold Control
    | (2  <<  2)  // TXQEN(0): Transmit Queue Enable
    | (1  <<  1)  // TSF: Transmit Store and Forward
    | (1  <<  0)  // FTQ: Flush Transmit Queue
  );
  regs->MTLTQOMR = tmp;


  tmp = (0
    | (7  << 20)  // RQS(3): Receive Queue Size
    | (0  << 14)  // RFD(3): Threshold for Deactivating Flow Control
    | (0  <<  8)  // RFA(3): Threshold for Activating Flow Control
    | (0  <<  7)  // EHFC: Enable Hardware Flow Control
    | (1  <<  6)  // DIS_TCP_EF: Disable Dropping of TCP/IP Checksum Error Packets
    | (1  <<  5)  // RSF: Receive Queue Store and Forward
    | (0  <<  4)  // FEP: Forward Error Packets
    | (0  <<  3)  // FUP: Forward Undersized Good Packets
    | (0  <<  0)  // RTC(2): Receive Queue Threshold Control
  );
  regs->MTLRQOMR = tmp;

  // DMA Configuration
  regs->DMAMR = 0; // keep the defaults

  regs->DMASBMR = (0
    | (1 << 12)  // AAL: Address-Aligned Beats
    | (1 <<  0)  // FB: Fixed Burst Length
  );

  regs->DMACCR = (0
    | (2     << 18)  // DSL(3): Descriptor Skip Length: 2 = skip 2*32-bits between the descriptors
    | (0     << 16)  // PBLX8: 8xPBL mode
    | (0x218 <<  0)  // MSS(14): Maximum Segment Size
  );

  regs->DMACTCR = (0
    | (32 << 16)  // TXPBL(6): Transmit Programmable Burst Length
    | (0  << 12)  // TSE: TCP Segmentation Enabled
    | (0  <<  4)  // OSF: Operate on Second Packet
    | (0  <<  0)  // ST: Start or Stop Transmission Command
  );

  regs->DMACRCR = (0
    | (0  << 31)  // RPF: DMA Rx Channel Packet Flush
    | (32 <<  0)  // RXPBL(6): Receive Programmable Burst Length
    | (HWETH_MAX_PACKET_SIZE <<  1)  // RBSZ(14): Receive Buffer size
    | (0  <<  0)  // SR
  );

  regs->MACIER = 0;
  if (regs->MACISR) { }  // clears some interrupts

  // Save MAC address
  SetMacAddress(&mac_address[0]);

  // initialize with 100M + Full Duplex, but later this will be overridden from the Speed Info of the PHY
  SetSpeed(true);
  SetDuplex(true);

  // initialize descriptor lists

  rx_desc_list = (HW_ETH_DMA_DESC *)prxdesclist;
  rx_desc_count = rxcnt;
  tx_desc_list = (HW_ETH_DMA_DESC *)ptxdesclist;
  tx_desc_count = txcnt;

#if 0
  // Initialize Tx Descriptors list: Chain Mode
  InitDescList(true, tx_desc_count, tx_desc_list, nullptr);
  regs->DMA_TRANS_DES_ADDR = (uint32_t)&tx_desc_list[0];

  // Initialize Rx Descriptors list: Chain Mode
  InitDescList(false, rx_desc_count, rx_desc_list, nullptr);
  regs->DMA_REC_DES_ADDR = (uint32_t)&rx_desc_list[0];
#endif

  actual_rx_desc = &rx_desc_list[0];

  return true;
}

void THwEth_stm32_v2::InitDescList(bool istx, int bufnum, HW_ETH_DMA_DESC * pdesc_list, uint8_t * pbuffer)
{
#if 0
  int i;
  HW_ETH_DMA_DESC *  pdesc = pdesc_list;

  memset(pdesc_list, 0, bufnum * sizeof(HW_ETH_DMA_DESC));

  for (i = 0; i < bufnum; ++i)
  {
    if (istx)
    {
      // different register usage!
      pdesc->DES0 = HWETH_DMADES_TCH;
      if (hw_ip_checksum)
      {
        pdesc->DES0 |= (3 << 22);  // setup HW IP Checksum calculation
      }
      pdesc->DES1 = 0;
    }
    else
    {
      pdesc->DES0 = 0;    // do not enable it yet because there is no buffer assignment
      pdesc->DES1 = HWETH_DMADES_RCH | 0;   // interrupt enabled
    }

    pdesc->B1ADD = 0; // do not assign data yet
    pdesc->B2ADD = (uint32_t)(pdesc + 1);

    if (i == bufnum - 1)
    {
      // last descriptor
      pdesc->B2ADD = (uint32_t)pdesc_list;  // link back

      // signal descriptor ring, it seems, that this is not really necessary, but does not hurt
      if (istx)
      {
        pdesc->DES0 |= HWETH_DMADES_TER;
      }
      else
      {
        pdesc->DES1 |= HWETH_DMADES_RER;
      }
    }

    ++pdesc;
    pbuffer += HWETH_MAX_PACKET_SIZE;
  }
#endif
}

void THwEth_stm32_v2::AssignRxBuf(uint32_t idx, TPacketMem * pmem, uint32_t datalen)
{
#if 0
  if (idx >= rx_desc_count)  return;

  int i;
  HW_ETH_DMA_DESC *  pdesc = &rx_desc_list[idx];

  pmem->idx = idx;

  pdesc->B1ADD = (uint32_t)(&pmem->data[0]);
  pdesc->DES1 |= datalen;
  pdesc->DES0 = HWETH_DMADES_OWN;  // enable receive on this decriptor
#endif
}

bool THwEth_stm32_v2::TryRecv(TPacketMem * * ppmem)
{
#if 0
  if (!(regs->MAC_CONFIG & HWETH_MAC_CFG_RE))
  {
    return false;
  }

  __DSB();

  HW_ETH_DMA_DESC * pdesc = (HW_ETH_DMA_DESC *)regs->DMA_CURHOST_REC_DES;

  while (1)
  {
    uint32_t stat = actual_rx_desc->DES0;
    if (stat & HWETH_DMADES_OWN)
    {
      // nothing was received.
      if (actual_rx_desc != pdesc)
      {
        // some error, correct it
        actual_rx_desc = (HW_ETH_DMA_DESC *)actual_rx_desc->B2ADD;
        continue;
      }
      return false;
    }

    // check for errors
    if (stat & (1 << 15))  // Error Summary Bit
    {
      ++recv_error_count;
    }
    else if ((stat & (3 << 8)) == (3 << 8))  // First + Last Descriptor bit
    {
      // this is ok
      ++recv_count;
      HW_ETH_DMA_DESC * result = actual_rx_desc;
      actual_rx_desc = (HW_ETH_DMA_DESC *)actual_rx_desc->B2ADD;

      // resulting

      TPacketMem * pmem = (TPacketMem *)(result->B1ADD - HWETH_PMEM_HEAD_SIZE);

      pmem->idx = (result - rx_desc_list); // / sizeof(HW_ETH_DMA_DESC);
      pmem->datalen = ((result->DES0 >> 16) & 0x1FFF);
      *ppmem = pmem;
      return true;
    }

    // free this, and go to the next.
    actual_rx_desc->DES0 = HWETH_DMADES_OWN;
    actual_rx_desc = (HW_ETH_DMA_DESC *)actual_rx_desc->B2ADD;

    // restart the dma controller if it was out of secriptors.

    __DSB();
    regs->DMA_REC_POLL_DEMAND = 1;
  }
#else
  return false;
#endif
}

void THwEth_stm32_v2::ReleaseRxBuf(TPacketMem * pmem)
{
#if 0
  HW_ETH_DMA_DESC *  pdesc = &rx_desc_list[pmem->idx];
  pdesc->DES0 = HWETH_DMADES_OWN;
  __DSB();
  regs->DMA_REC_POLL_DEMAND = 1;  // for the case when we were out of descriptors
#endif
}

bool THwEth_stm32_v2::TrySend(uint32_t * pidx, void * pdata, uint32_t datalen)
{
#if 0
  // it is important to know the current descriptor otherwise the transfer won't be started
  // if this descriptor is owned by the CPU the sending is stopped and checks only this entry for the OWN bit change

  HW_ETH_DMA_DESC * pdesc = (HW_ETH_DMA_DESC *)regs->DMA_CURHOST_TRANS_DES;

  int i = 0;
  while (i < tx_desc_count)
  {
    if ((pdesc->DES0 & HWETH_DMADES_OWN) == 0)
    {
      //TRACE("TX using desc %p\r\n", pdesc);

      // use this descriptor
      pdesc->B1ADD  = (uint32_t) pdata;
      pdesc->DES1   = datalen & 0x0FFF;
      pdesc->DES0 |= (HWETH_DMADES_OWN | (3 << 28));  // set First + Last descriptor as well

      // Tell DMA to poll descriptors to start transfer
      __DSB(); // required on Cortex-M7

      regs->DMA_TRANS_POLL_DEMAND = 1;

      *pidx = (unsigned(pdesc) - unsigned(&tx_desc_list[0])) / sizeof(HW_ETH_DMA_DESC);
      //TRACE("  idx = %i\r\n", *pidx);
      return true;
    }

    pdesc = (HW_ETH_DMA_DESC *)pdesc->B2ADD;
    ++i;
  }
#endif
  // no free descriptors

  return false;
}

bool THwEth_stm32_v2::SendFinished(uint32_t idx)
{
#if 0
  if (idx >= tx_desc_count)
  {
    return false;
  }

  HW_ETH_DMA_DESC * pdesc = &tx_desc_list[idx];

  return ((pdesc->DES0 & HWETH_DMADES_OWN) == 0);
#else
  return false;
#endif
}

void THwEth_stm32_v2::Start(void)
{
#if 0
  // Clear all MAC interrupts
  regs->DMA_STAT = HWETH_DMA_ST_ALL;

  // Enable MAC interrupts
  regs->DMA_INT_EN = HWETH_DMA_IE_RIE | HWETH_DMA_IE_NIE; // enable only receive interrupt (+Normal interrupt enable)

  // Enable packet reception
  regs->MAC_CONFIG |= HWETH_MAC_CFG_RE | HWETH_MAC_CFG_TE;

  // Setup DMA to flush receive FIFOs at 32 bytes, service TX FIFOs at 64 bytes
  regs->DMA_OP_MODE |= HWETH_DMA_OM_ST | HWETH_DMA_OM_SR;

  // Start receive polling
  __DSB();
  regs->DMA_REC_POLL_DEMAND = 1;
#endif
}

void THwEth_stm32_v2::Stop(void)
{
#if 0
  regs->DMA_INT_EN = 0; // enable only receive interrupt (+Normal interrupt enable)

  regs->DMA_OP_MODE &= ~(HWETH_DMA_OM_ST | HWETH_DMA_OM_SR); // stop transmit and receive
  regs->MAC_CONFIG &= ~(HWETH_MAC_CFG_RE | HWETH_MAC_CFG_TE);
#endif
}

uint32_t THwEth_stm32_v2::CalcMdcClock(void)
{
  uint32_t val = stm32_bus_speed(0) / 1000000;

  if (val <  20)  return 0; // invalid
  if (val <  35)  return ETH_MACMDIOAR_CR_DIV16;
  if (val <  60)  return ETH_MACMDIOAR_CR_DIV26;
  if (val < 100)  return ETH_MACMDIOAR_CR_DIV42;
  if (val < 150)  return ETH_MACMDIOAR_CR_DIV62;
  if (val < 250)  return ETH_MACMDIOAR_CR_DIV102;
  if (val < 300)  return ETH_MACMDIOAR_CR_DIV124;

  return 0;
}

void THwEth_stm32_v2::SetMacAddress(uint8_t * amacaddr)
{
  if (amacaddr != &mac_address[0])
  {
    memcpy(&mac_address, amacaddr, 6);
  }

  if (!regs) // might be called before Init();
  {
    return;
  }

  regs->MACA0LR = ((uint32_t) amacaddr[3] << 24) | ((uint32_t) amacaddr[2] << 16)
                | ((uint32_t) amacaddr[1] << 8) | ((uint32_t) amacaddr[0]);

  regs->MACA0HR = ((uint32_t) amacaddr[5] << 8) | ((uint32_t) amacaddr[4]);
}

void THwEth_stm32_v2::SetSpeed(bool speed100)
{
  if (speed100)
  {
    regs->MACCR |= ETH_MACCR_FES;
  }
  else
  {
    regs->MACCR &= ~ETH_MACCR_FES;
  }
}

void THwEth_stm32_v2::SetDuplex(bool full)
{
  if (full)
  {
    regs->MACCR |= ETH_MACCR_DM;
  }
  else
  {
    regs->MACCR &= ~ETH_MACCR_DM;
  }
}


void THwEth_stm32_v2::SetupMii(uint32_t div, uint8_t addr)
{
  // Save clock divider and PHY address for the MACMDIOAR register
  phy_config = (addr << ETH_MACMDIOAR_CR_Pos) | (div << ETH_MACMDIOAR_CR_Pos);
}

bool THwEth_stm32_v2::IsMiiBusy()
{
  return (regs->MACMDIOAR & ETH_MACMDIOAR_MB) ? true : false;
}

void THwEth_stm32_v2::StartMiiWrite(uint8_t reg, uint16_t data)
{
  regs->MACMDIODR = (uint32_t) data;
  regs->MACMDIOAR = phy_config | (reg << ETH_MACMDIOAR_RDA_Pos) | ETH_MACMDIOAR_MOC_WR | ETH_MACMDIOAR_MB;
}

void THwEth_stm32_v2::StartMiiRead(uint8_t reg)
{
  regs->MACMDIOAR = phy_config | (reg << ETH_MACMDIOAR_RDA_Pos) | ETH_MACMDIOAR_MOC_RD | ETH_MACMDIOAR_MB;
}

void THwEth_stm32_v2::NsTimeStart()
{
}

uint64_t THwEth_stm32_v2::NsTimeRead()
{
  unsigned pm = __get_PRIMASK();  // save interrupt disable status
  __disable_irq();

  uint64_t result = 0;

  __set_PRIMASK(pm); // restore interrupt disable status
  return result;
}

uint64_t THwEth_stm32_v2::GetTimeStamp(uint32_t idx) // must be called within 2 s to get the right upper 32 bit
{
  return 0;
}

#endif // defined(MCUSF_H7)
