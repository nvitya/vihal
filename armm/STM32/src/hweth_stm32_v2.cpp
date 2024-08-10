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

#if defined(MCUSF_H7) || defined(MCUSF_H7RS)

#include "string.h"
#include "stm32_utils.h"
#include "hweth_stm32_v2.h"

bool THwEth_stm32_v2::InitMac(void * prxdesclist, uint32_t rxcnt, void * ptxdesclist, uint32_t txcnt)
{
  uint32_t n;
  uint32_t tmp;

  #if defined(SYSCFG_PMCR_EPIS_SEL_2)
    // Select RMII Mode, must be done before enabling the hw
    RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
    if (RCC->APB4ENR) { } // read back for delay
    SYSCFG->PMCR |= SYSCFG_PMCR_EPIS_SEL_2;  // select RMII mode
    if (SYSCFG->PMCR) { }
  #else
    #warning "implement for H7RS"
  #endif

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

#if 0
  regs->MACTSCR = 0x0103;   // start fine mode, timestamp all frames
#else
  regs->MACTSCR = 0x0003;   // disable timestamping for now (requires additional descriptors) !!!!!!!!!!!!!!!!!!!!!!!
#endif

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
    | (0 <<  8)  // DR: Retry disable
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
    | (32 << 16)  // RXPBL(6): Receive Programmable Burst Length
    | (HWETH_MAX_PACKET_SIZE <<  1)  // RBSZ(14): Receive Buffer size
    | (0  <<  0)  // SR: Start or stop receive
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

  InitDescList(true,  tx_desc_count, tx_desc_list, nullptr);
  InitDescList(false, rx_desc_count, rx_desc_list, nullptr);

  return true;
}

void THwEth_stm32_v2::InitDescList(bool istx, int bufnum, HW_ETH_DMA_DESC * pdesc_list, uint8_t * pbuffer)
{
  int i;
  HW_ETH_DMA_DESC *  pdesc = pdesc_list;

  memset(pdesc_list, 0, bufnum * sizeof(HW_ETH_DMA_DESC));

  if (istx)
  {
    cur_tx_desc = pdesc_list;
    tx_desc_list_end = pdesc_list + bufnum;

    regs->DMACTDRLR = bufnum - 1;          // Set Transmit Descriptor Ring Length
    regs->DMACTDLAR = (uint32_t)&pdesc_list[0]; // Set Transmit Descriptor List Address
    regs->DMACTDTPR = 0; // Set Transmit Descriptor Tail pointer
  }
  else
  {
    cur_rx_desc = pdesc_list;
    rx_desc_list_end = pdesc_list + bufnum;

    regs->DMACRDRLR = bufnum - 1;  // Set Receive Descriptor Ring Length
    regs->DMACRDLAR = (uint32_t)&pdesc_list[0];  // Set Receive Descriptor List Address
    regs->DMACRDTPR = 0;  // Set Receive Descriptor Tail pointer Address
  }
}

void THwEth_stm32_v2::AssignRxBuf(uint32_t idx, TPacketMem * pmem, uint32_t datalen)
{
  if (idx >= rx_desc_count)  return;

  int i;
  HW_ETH_DMA_DESC *  pdesc = &rx_desc_list[idx];

  rxdesc_irq_flag = (irq_on_rx ? HWETH_DMADES_IOC : 0);

  pmem->idx = idx;

  pdesc->DESC0 = (uint32_t)(&pmem->data[0]);
  pdesc->DESC1 = 0;
  pdesc->DESC2 = 0;
  pdesc->DESC3 = (HWETH_DMADES_OWN | rxdesc_irq_flag | HWETH_DMADES_BUF1V);  // enable receive on this decriptor
  pdesc->BackupAddr0 = (uint32_t)(&pmem->data[0]);
}

bool THwEth_stm32_v2::TryRecv(TPacketMem * * ppmem)
{
  if (0 == (regs->MACCR & 1)) // receive enabled ?
  {
    return false;
  }

  while (1)
  {
    __DSB();

    uint32_t desc3 = cur_rx_desc->DESC3;
    if (desc3 & HWETH_DMADES_OWN)
    {
      return false;
    }

    // check for errors
    if (desc3 & (1 << 15))  // Error Summary Bit
    {
      ++recv_error_count;
    }
    else if (((desc3 >> 28) & 3) == 3)  // First + Last Descriptor bit
    {
      // this is ok
      ++recv_count;

      HW_ETH_DMA_DESC * result = cur_rx_desc;
      ++cur_rx_desc;
      if (cur_rx_desc >= rx_desc_list_end)  cur_rx_desc = rx_desc_list;

      // resulting

      TPacketMem * pmem = (TPacketMem *)(result->BackupAddr0 - HWETH_PMEM_HEAD_SIZE);

      pmem->idx = (result - rx_desc_list); // the compiler already dividies with sizeof(HW_ETH_DMA_DESC);
      pmem->datalen = (desc3 & 0x1FFF);
      *ppmem = pmem;
      return true;
    }

    // free this, and go to the next.

    cur_rx_desc->DESC0 = cur_rx_desc->BackupAddr0;
    cur_rx_desc->DESC1 = 0;
    cur_rx_desc->DESC2 = 0;
    cur_rx_desc->DESC3 = (HWETH_DMADES_OWN | rxdesc_irq_flag | HWETH_DMADES_BUF1V);  // enable receive on this decriptor

    __DSB();

    RestartStoppedRx(cur_rx_desc - rx_desc_list);

    // go to the next rx descriptor
    ++cur_rx_desc;
    if (cur_rx_desc >= rx_desc_list_end)  cur_rx_desc = rx_desc_list;
  }
}

void THwEth_stm32_v2::ReleaseRxBuf(TPacketMem * pmem)
{
  HW_ETH_DMA_DESC *  pdesc = &rx_desc_list[pmem->idx];
  pdesc->DESC0 = pdesc->BackupAddr0;
  pdesc->DESC1 = 0;
  pdesc->DESC2 = 0;
  pdesc->DESC3 = HWETH_DMADES_OWN | rxdesc_irq_flag | HWETH_DMADES_BUF1V;  // enable receive on this decriptor
  __DSB();

  RestartStoppedRx(pmem->idx);
}

bool THwEth_stm32_v2::TrySend(uint32_t * pidx, void * pdata, uint32_t datalen)
{
  if (0 == (regs->MACCR & 2)) // transmit enabled ?
  {
    return false;
  }

  __DSB();

  HW_ETH_DMA_DESC * pdesc = cur_tx_desc;

  if (pdesc->DESC3 & HWETH_DMADES_OWN)
  {
    return false; // tx descriptor not sended
  }

  pdesc->DESC0 = (uint32_t) pdata;
  pdesc->DESC1 = 0;
  pdesc->DESC2 = (0
    | ((irq_on_tx ? 1 : 0) << 31)
    | (0  <<  14)  // VTIR(2)
    | (datalen & 0xFFF)
  );
  pdesc->DESC3 = (0
    | (1  << 31)  // OWN: 1 = owned by the DMA
    | (0  << 30)  // CTX
    | (1  << 29)  // FD: 1 = first descriptor
    | (1  << 28)  // LD: 1 = last descriptor
    | (0  << 26)  // CPC(2): 0 = insert CRC and padding (if necessary)
    | (0  << 23)  // SAIC(3): 0 = do not include source address, 2 = replace the source address
    | (0  << 19)  // THL(4):
    | (0  << 18)  // TSE
    | (0  << 16)  // CIC/TPL(2)
    | ((datalen & 0x1FFF)  <<  0)  // TPL(16): total packet length
  );

  __DSB(); // required on Cortex-M7, especially if CPU caches are active
  //__DMB();

  // Tell DMA to poll descriptors to start transfer

  uint32_t idx = pdesc - tx_desc_list;
  *pidx = idx;

  RestartStoppedTx(idx);
  //regs->DMACTDTPR = idx;  // activates the sending

  // go to the next TX desc
  ++pdesc;
  if (pdesc >= tx_desc_list_end)  pdesc = tx_desc_list;
  cur_tx_desc = pdesc;

  return true;
}

bool THwEth_stm32_v2::SendFinished(uint32_t idx)
{
  if (idx >= tx_desc_count)
  {
    return false;
  }

  HW_ETH_DMA_DESC * pdesc = &tx_desc_list[idx];

  return ((pdesc->DESC3 & HWETH_DMADES_OWN) == 0);
}

void THwEth_stm32_v2::Start(void)
{
  // Clear all MAC interrupts
  //regs->DMA_STAT = HWETH_DMA_ST_ALL;

  // Enable MAC interrupts
  //regs->DMA_INT_EN = HWETH_DMA_IE_RIE | HWETH_DMA_IE_NIE; // enable only receive interrupt (+Normal interrupt enable)

  regs->MACCR |= (ETH_MACCR_TE | ETH_MACCR_RE); // enable transmit and receive

  __DSB();

  regs->DMACTCR |= ETH_DMACTCR_ST; // start transmission
  regs->DMACRCR |= ETH_DMACRCR_SR; // start receive

  // Start receive polling
  regs->DMACSR |= (ETH_DMACSR_TPS | ETH_DMACSR_RPS);  // Clear Tx and Rx process stopped flags

  //regs->DMACRDTPR = 0;  // Set Receive Descriptor Tail pointer Address
}

void THwEth_stm32_v2::Stop(void)
{
  //regs->DMA_INT_EN = 0; // enable only receive interrupt (+Normal interrupt enable)

  regs->DMACTCR &= ~1; // stop transmit
  regs->DMACRCR &= ~1; // stop receive

  regs->MACCR &= ~3; // disable transmit and receive
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

  return ETH_MACMDIOAR_CR_DIV124;
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

  // the high mac-address must be written first, writing the low triggers the update
  regs->MACA0HR = ((uint32_t) amacaddr[5] << 8) | ((uint32_t) amacaddr[4]) | (1u << 31);
  regs->MACA0LR = ((uint32_t) amacaddr[3] << 24) | ((uint32_t) amacaddr[2] << 16)
                | ((uint32_t) amacaddr[1] << 8) | ((uint32_t) amacaddr[0]);
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


void THwEth_stm32_v2::SetupMii(uint32_t divmask, uint8_t addr)
{
  // Save clock divider and PHY address for the MACMDIOAR register
  phy_config = (addr << ETH_MACMDIOAR_CR_Pos) | divmask;
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
