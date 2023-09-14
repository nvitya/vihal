/*
 *  file:     hweth_imxrt.cpp
 *  brief:    VIHAL compatible IMXRT Ethernet driver
 *  date:     2023-09-14
 *  authors:  nvitya
*/

#include <stdio.h>
#include "string.h"
#include <stdarg.h>

#include "platform.h"

#include "hweth.h"
#include "traces.h"
#include "clockcnt.h"
#include "imxrt_utils.h"

#define ETH_CHECK_CALL_ERRORS  1

bool THwEth_imxrt::InitMac(void * prxdesclist, uint32_t rxcnt, void * ptxdesclist, uint32_t txcnt)
{
	uint32_t tmp;
	uint32_t n;

	regs = ENET;

  // setup reference clock

  if (external_ref_clock)
  {
#if defined(MCUSF_1020)
    // the PLL_ENET is already running
#else
  	// The PLL_ENET must be enabled for timestamp generation
    CCM_ANALOG->PLL_ENET = 0
  		| (0            <<  0) // DIV_SELECT(2): 0 = 25 MHz, 1 = 50 MHz, 2 = 100 MHz, 3 = 125 MHz
  		| (0            << 12) // POWERDOWN
  		| (0            << 13) // ENABLE: Enable the PLL providing the ENET reference clock
  		| (0            << 14) // BYPASS_CLK_SRC(2)
  		| (0            << 16) // BYPASS
  		| (1            << 21) // ENET_25M_REF_EN: Enable the PLL providing ENET 25 MHz reference clock, required for timestamping!
  	;
#endif

  	IOMUXC_GPR->GPR1 |=  (1 << 13);  // ENET1_CLK_SEL: 0 = internal, 1 = external
  	IOMUXC_GPR->GPR1 &= ~(1 << 17);  // ENET1_TX_CLK_DIR: 0 = input, 1 = output
  }
  else
  {
  	uint8_t loopdivider;
  	if (25 == refclock_mhz)
  	{
  		loopdivider = 0; // 25 MHz, required for MII
  	}
  	else if (125 == refclock_mhz)
  	{
  		loopdivider = 3;
  	}
  	else
  	{
  		loopdivider = 1; // 50 MHz, required for RMII
  	}

#if defined(MCUSF_1020)

  	// update the ethernet reference clock
    tmp = CCM_ANALOG->PLL_ENET;
    tmp &= ~(3);
    tmp |= loopdivider;
    CCM_ANALOG->PLL_ENET = tmp;

#else
  	// init ethernet clock
    CCM_ANALOG->PLL_ENET = 0
  		| (loopdivider  <<  0) // DIV_SELECT(2): 0 = 25 MHz, 1 = 50 MHz, 2 = 100 MHz, 3 = 125 MHz
  		| (0            << 12) // POWERDOWN
  		| (1            << 13) // ENABLE: Enable the PLL providing the ENET reference clock
  		| (0            << 14) // BYPASS_CLK_SRC(2)
  		| (0            << 16) // BYPASS
  		| (1            << 21) // ENET_25M_REF_EN: Enable the PLL providing ENET 25 MHz reference clock, required for timestamping!
  	;

    // Wait for stable
    while ((CCM_ANALOG->PLL_ENET & CCM_ANALOG_PLL_ENET_LOCK_MASK) == 0)
    {
    }
#endif

  	IOMUXC_GPR->GPR1 &= ~(1 << 13);  // ENET1_CLK_SEL: 0 = internal, 1 = external
  	IOMUXC_GPR->GPR1 |=  (1 << 17);  // ENET1_TX_CLK_DIR: 0 = input, 1 = output
  }

	imxrt_set_clock_gate(1, 10, 3);  // turn on ENET clock

	// reset
  regs->ECR = ENET_ECR_RESET_MASK;

	// setup MII (MSI) Interface

	uint32_t baseclk = SystemCoreClock / 4 / 2;
	uint32_t mii_speed = baseclk / 2500000;  // MII speed fix 2.5 MHz

	regs->MSCR = 0
	  | ((mii_speed - 1) <<  1)  // MII_SPEED(6):
	  | (0  <<  7)  // DIS_PRE: 1 = disable preamble
	  | (3  <<  8)  // HOLD_TIME(3):
	;

	regs->ECR = 0
		| (1  <<  8)  // DBSWP: 0 = do not swap descriptor bytes, 1 = swap descriptor bytes !!!!
		| (0  <<  6)  // DBGEN: 0 = continue to run in debug mode
		| (1  <<  4)  // EN1588: 0 = legacy (small) descriptors, 1 = 32 byte descriptords with time stamping
		| (0  <<  3)  // SLEEP: 0 = normal mode
		| (0  <<  2)  // MAGICEN: 0 = magic packet detection disabled
		| (0  <<  1)  // ETHEREN: do not enable yet
		| (0  <<  0)  // RESET: clears the ETHEREN flag
	;

	regs->MRBR = HWETH_MAX_PACKET_SIZE;
	regs->FTRL = HWETH_MAX_PACKET_SIZE;

	regs->RCR = 0
		| (0    << 31)  // GRS: Graceful Receive Stopped (RO)
		| (0    << 30)  // NLC: Payload Length Check Disable
		| (HWETH_MAX_PACKET_SIZE << 16)  // MAX_FL(14): Maximum Frame Length
		| (0    << 15)  // CFEN: MAC Control Frame Enable, 1 = special control frames are discarded
		| (1    << 14)  // CRCFWD: 1 = The CRC field is stripped from the frame
		| (0    << 13)  // PAUFWD: 0 = Pause Frames are terminated, 1 = forwarded
		| (0    << 12)  // PADEN
		| (0    <<  9)  // RMII_10T: 0 = 100 MBit/s RMII, 1 = 10 MBit/s RMII
		| (1    <<  8)  // RMII_MODE: 0 = MII mode, 1 = RMII mode
		| (0    <<  5)  // FCE: Flow Control Enable
		| (0    <<  4)  // BC_REJ: Broadcast Frame Reject
		| ((promiscuous_mode ? 1 : 0)  <<  3)  // PROM: Promiscous Mode
		| (1    <<  2)  // MII_MODE: must be set to 1
		| (0    <<  1)  // DRT: Disable Receive On Transmit, set 1 for half duplex mode
		| ((loopback ? 1 : 0) <<  0)  // LOOP: Internal Loopback, 1 = loopback
	;

	regs->TCR = 0
		| (0 <<  9)  // CRCFWD: 0 = MAC calculates the CRC, 1 = App calculates the CRC
		| (0 <<  8)  // ADDINS: 0 = source MAC address is not modified, 1 = programmed MAC address inserted
		| (0 <<  5)  // ADDSEL(3): MAC Address select for ADDINS, only 0 supported here
		| (0 <<  4)  // RFC_PAUSE: Receive Frame Control Pause (RO)
		| (0 <<  3)  // TFC_PAUSE: Transmit Frame Control Pause, 1 = stop transmission after this frame
		| (1 <<  2)  // FDEN: 1 = enable full duplex mode
		| (0 <<  0)  // GTS: Graceful Transmit Stop, 1 = stops transmission
	;

#if 1
	regs->TACC = 0
		| ((hw_ip_checksum ? 1 : 0) <<  4)  // PROCHK: 1 = insert IP content checksum for known protocols
		                                    //         the checksum field must be 0
		| ((hw_ip_checksum ? 1 : 0) <<  3)  // IPCHK: 1 = insert IP header checksum (the checksum field must be 0!)
		| (0 <<  0)  // SHIFT16: 1 = the frame has two additional byte at the beginning for better alignment
	;

	// todo: discard frames with PHY or CRC error ???

	regs->RACC = 0
		| (0 <<  7)  // SHIFT16: 1 = insert two additional bytes at the beginning for better alignment
		| (0 <<  6)  // LINEDIS: Enable Discard Of Frames With MAC Layer Errors, 1 = discard frames with PHY or CRC errors
		| ((hw_ip_checksum ? 1 : 0) <<  2)  // PRODIS: 1 = discard known protocol IP frames with wrong checksum
		| ((hw_ip_checksum ? 1 : 0) <<  1)  // IPDIS: 1 = discard IP frames with wrong IP header checksum
		| (0 <<  0)  // PADREM: Padding Removal, 0 = padding is not removed
	;

#else
	regs->TACC = 0;
	regs->RACC = 0;
#endif

	// set store and forward mode
  regs->TFWR = ENET_TFWR_STRFWD_MASK;
  regs->RSFL = 0;

  regs->TXIC = 0; // disable tx interrupt coalescing
  regs->RXIC = 0; // disable rx interrupt coalescing

  regs->EIMR = 0
  	| (0  << 30)  // BABR: Babbling Receive Error (rx frame too long)
  	| (0  << 29)  // BABT: Babbling Transmit Error (tx frame too long)
  	| (0  << 28)  // GRA: Graceful Stop Complete
  	| ((irq_on_tx ? 1 : 0)  << 27)  // TXF: Transmit Frame Interrupt
  	| (0  << 26)  // TXB: Transmit Buffer Interrup
  	| ((irq_on_rx ? 1 : 0)  << 25)  // RXF: Receive Frame Interrupt
  	| (0  << 24)  // RXB: Receive Buffer Interrupt
  	| (0  << 23)  // MII: MII Interrupt
  	| (0  << 22)  // EBERR: Ethernet Bus Error
  	| (0  << 21)  // LC: Late Collision
  	| (0  << 20)  // RL: Collision Retry Limit
  	| (0  << 19)  // UN: Transmit FIFO Underrun
  	| (0  << 18)  // PRL: Payload Receive Error
  	| (0  << 17)  // WAKEUP: Node Wakeup Request Indication
  	| (0  << 16)  // TS_AVAIL: Transmit Timestamp Available
  	| (0  << 15)  // TS_TIMER: Timestamp Timer
  ;

  regs->EIR = 0x7FFF8000; // clear all interrupt flags

  SetMacAddress(&mac_address[0]);

	NsTimeStart();

	// initialize descriptor lists

	rx_desc_list = (HW_ETH_DMA_DESC *)prxdesclist;
	rx_desc_count = rxcnt;
	tx_desc_list = (HW_ETH_DMA_DESC *)ptxdesclist;
	tx_desc_count = txcnt;

	// Initialize Tx Descriptors list: Chain Mode
	InitDescList(true, tx_desc_count, tx_desc_list);
	regs->TDSR = (uint32_t)&tx_desc_list[0];

	// Initialize Rx Descriptors list: Chain Mode
	InitDescList(false, rx_desc_count, rx_desc_list);
	regs->RDSR = (uint32_t)&rx_desc_list[0];

  regs->MRBR = 1522 & 0xFFC0; // maximum receive seize

	actual_tx_idx = 0;
	actual_rx_desc = &rx_desc_list[0];

	regs->ECR |= ENET_ECR_ETHEREN_MASK; // enable RX/TX
	regs->RDAR = (1 << 24);  // start receive

	return true;
}

void THwEth_imxrt::InitDescList(bool istx, int bufnum, HW_ETH_DMA_DESC * pdesc_list)
{
	// the RX descriptors will be adjusted later at buffer allocation (AssingRxBuf)

	int i;
	HW_ETH_DMA_DESC *  pdesc = pdesc_list;

#if ETH_CHECK_CALL_ERRORS // alignment check
	if (unsigned(pdesc_list) & 0x3F)
	{
		TRACE("ETH Warning: wrong aligned descriptor list!\r\n")
	}
#endif

	memset(pdesc_list, 0, bufnum * sizeof(HW_ETH_DMA_DESC)); // reset to all zero first

	for (i = 0; i < bufnum; ++i)
	{
		pdesc->BUFFER = nullptr;
		pdesc->LENGTH = 0;
		pdesc->CONTROL = 0;
		pdesc->BDU = 0;

		if (istx)
		{
			pdesc->CONTROL |= (1  <<  10); // Transmit CRC automatically
		}

		if (i == bufnum - 1)
		{
			pdesc->CONTROL |= (1  <<  13); // WRAP flag: last descriptor in the chain
		}

		++pdesc;
	}
}

void THwEth_imxrt::AssignRxBuf(uint32_t idx, TPacketMem * pmem, uint32_t datalen)
{
	if (idx >= rx_desc_count)  return;

#if ETH_CHECK_CALL_ERRORS // size check
	if (datalen < HWETH_MAX_PACKET_SIZE)
	{
		TRACE("ETH AssignRxBuf: datalen is too small!\r\n")
	}
#endif

	HW_ETH_DMA_DESC *  pdesc = &rx_desc_list[idx];

  pmem->idx = idx;

	pdesc->LENGTH = 0; // written by the MAC on receive
	pdesc->BUFFER = (uint8_t *)(&pmem->data[0]);  // skip the header
	pdesc->CONTROL |= (1 << 15); // set the EMPTY bit
	pdesc->WORD_0A = (1 << 7); // enable RX Interrupt
}

void THwEth_imxrt::Start(void)
{
	// todo: control sleep mode

  regs->EIR = 0x7FFF8000; // clear all interrupt flags
	regs->RDAR = (1 << 24);  // start receive
}

void THwEth_imxrt::Stop(void)
{
	// todo: control sleep mode

	//regs->ECR &= ~ENET_ECR_ETHEREN_MASK; // disable RX/TX
}

void THwEth_imxrt::SetMacAddress(uint8_t * amacaddr)
{
	if (amacaddr != &mac_address[0])
	{
		memcpy(&mac_address, amacaddr, 6);
	}

	regs->PALR = ((uint32_t) amacaddr[0] << 24) | ((uint32_t) amacaddr[1] << 16)
					                  | ((uint32_t) amacaddr[2] <<  8) | ((uint32_t) amacaddr[3]);

	regs->PAUR = ((uint32_t) amacaddr[5] << 24) | ((uint32_t) amacaddr[4] << 16);
}

void THwEth_imxrt::SetSpeed(bool speed100)
{
	if (speed100)
	{
		regs->RCR &= ~(1 << 9); // RMII_10T: 0 = 100 MBit/s RMII, 1 = 10 MBit/s RMII
	}
	else
	{
		regs->RCR |=  (1 << 9); // RMII_10T: 0 = 100 MBit/s RMII, 1 = 10 MBit/s RMII
	}
}

void THwEth_imxrt::SetDuplex(bool full)
{
#if 0
	bool wasenabled = ((regs->ECR & ENET_ECR_ETHEREN_MASK) != 0);
	if (wasenabled)
	{
		regs->ECR &= ~ENET_ECR_ETHEREN_MASK;
	}
#endif

	if (full)
	{
		regs->RCR &= ~(1 << 1); // DRT: Disable Receive On Transmit, set 1 for half duplex mode
		regs->TCR |=  (1 << 2); // FDEN: 1 = enable full duplex mode
	}
	else
	{
		regs->RCR |=  (1 << 1); // DRT: Disable Receive On Transmit, set 1 for half duplex mode
		regs->TCR &= ~(1 << 2); // FDEN: 1 = enable full duplex mode
	}

#if 0
	if (wasenabled)
	{
		regs->ECR |= ENET_ECR_ETHEREN_MASK;
	}
#endif
}

// we set the RO1 user flag to signal that the frame was already allocated for app processing
// but not yet released

bool THwEth_imxrt::TryRecv(TPacketMem * * ppmem) // uint32_t * pidx, void * * ppdata, uint32_t * pdatalen)
{
	HW_ETH_DMA_DESC * pdesc = rx_desc_list;

	// TODO: optimize through checking only the next descriptor

	int i = 0;
	while (i < rx_desc_count)
	{
		if ((pdesc->CONTROL & (3 << 14)) == 0) // check empty (15) + user flag (14)
		{
			// use this descriptor

			regs->EIR = (1 << 25); // clear Receive Frame Interrupt flag
			pdesc->CONTROL |= (1 << 14); // signalize in processing

      TPacketMem * pmem = (TPacketMem *)(pdesc->BUFFER - HWETH_PMEM_HEAD_SIZE);
      pmem->idx = i;  // this should not be necessary
      pmem->datalen = (pdesc->LENGTH & 0x1FFF);
      *ppmem = pmem;
      return true;
		}

		++pdesc;
		++i;
	}

	return false;
}

void THwEth_imxrt::ReleaseRxBuf(TPacketMem * pmem)
{
	HW_ETH_DMA_DESC *  pdesc = &rx_desc_list[pmem->idx];
	pdesc->BDU = 0;
	uint16_t tmp = pdesc->CONTROL;
	tmp &= ~(1 << 14); // clear user flag
	tmp |=  (1 << 15); // set empty flag
	pdesc->CONTROL = tmp;
	regs->RDAR = (1 << 24);  // signalize that at least one empty descriptor is available
}

bool THwEth_imxrt::TrySend(uint32_t * pidx, void * pdata, uint32_t datalen)
{
	// we need to find the next inactive decriptor, otherwise the sending won't happen

	HW_ETH_DMA_DESC * pdesc = &tx_desc_list[actual_tx_idx];
	if ((pdesc->CONTROL & (1 << 15)) == 0) // this buffer is ready to transmission
	{
		pdesc->BUFFER = (uint8_t *)pdata;
		pdesc->LENGTH = datalen;
		pdesc->BDU = 0;
		pdesc->WORD_0A = 0
			| ((irq_on_tx ? 1 : 0)      << 14) // INT: generate interrupt
			| (0                        << 13) // TS: generate timestamp frame
			| ((hw_ip_checksum ? 1 : 0) << 12) // PINS: Insert protocol specific checksum
			| ((hw_ip_checksum ? 1 : 0) << 11) // IINS: Insert IP header checksum
		;
		pdesc->CONTROL |= (1 << 15) | (1 << 11); // signal that this descriptor has to be sent + last flag

		regs->TDAR = (1 << 24); // signal that at least one TX descriptor is active

		*pidx = actual_tx_idx;
		++actual_tx_idx;
		if (actual_tx_idx >= tx_desc_count)  actual_tx_idx = 0;
		return true;
	}
	else
	{
		return false;
	}
}

void THwEth_imxrt::StartMiiWrite(uint8_t reg, uint16_t data)
{
	regs->EIR = ENET_EIR_MII_MASK;

	uint32_t tmp = 0
		| (1           << 30) // ST(2): Start Of Frame Delimiter, 1 = clause 22 operation
		| (1           << 28) // OP(2): Operation Code, 2 = read, 1 = write
		| (phy_address << 23) // PA(5): PHY Address
		| (reg         << 18) // RA(5): Register Address
		| (2           << 16) // TA(2): Turn Around
		| (data        <<  0) // DATA(16)
	;
	regs->MMFR = tmp;
	mii_running = true;
}

void THwEth_imxrt::StartMiiRead(uint8_t reg)
{
	regs->EIR = ENET_EIR_MII_MASK;

	uint32_t tmp = 0
		| (1           << 30) // ST(2): Start Of Frame Delimiter, 1 = clause 22 operation
		| (2           << 28) // OP(2): Operation Code, 2 = read, 1 = write
		| (phy_address << 23) // PA(5): PHY Address
		| (reg         << 18) // RA(5): Register Address
		| (2           << 16) // TA(2): Turn Around
		| (0           <<  0) // DATA(16)
	;
	regs->MMFR = tmp;
	mii_running = true;
}

bool THwEth_imxrt::IsMiiBusy()
{
	if (mii_running)
	{
		if ((regs->EIR & ENET_EIR_MII_MASK) == 0)
		{
			return true;
		}
		else
		{
			mii_running = false;
			return false;
		}
	}
	else
	{
		return false;
	}
}

void THwEth_imxrt::NsTimeStart()
{
	// the timer runs with 25 MHz, so the increment is 40 for nanosecond counting
	nstime_inc_base = 40;

  regs->ATCR = (1 << 9);   // restart the timer
  regs->ATINC = nstime_inc_base | (nstime_inc_base << 8); // for 25 MHz fix clock
  regs->ATPER = 0; // we use the whole 32 bits, not wrapping by 1 second
  //regs->ATCOR = 1000000000;
  //regs->ATCR = 1 | (1 << 5) | (1 << 9);   // enable timer
  regs->ATCR = 1; // | (1 << 4));   // enable timer

  nstimer_high = 0;
  nstimer_low_prev = 0;

	NsTimeSetCorrection(0.0);
}

uint64_t THwEth_imxrt::NsTimeRead()
{
	unsigned pm = __get_PRIMASK();  // save interrupt disable status
	__disable_irq();

	regs->ATCR = (1 << 11); // start the capture
	// there is a delay until the value appears in the AVTR register
	// this is the fastest solution, 2 x register read time:
  if (regs->ATCR) { }
  if (regs->ATCR) { }  // (one is definetily not enough)

  uint32_t ns_low = regs->ATVR;
  if (ns_low < nstimer_low_prev)
  {
  	++nstimer_high;
  }

  nstimer_low_prev = ns_low;

  uint64_t result = (uint64_t(nstimer_high) << 32) | ns_low;

 	__set_PRIMASK(pm); // restore interrupt disable status
	return result;
}

uint64_t THwEth_imxrt::GetTimeStamp(uint32_t idx) // must be called within 2 s to get the right upper 32 bit
{
	HW_ETH_DMA_DESC * pdesc = rx_desc_list;
	pdesc += idx;

	uint64_t curts = NsTimeRead();
	uint32_t curts_low = (curts & 0xFFFFFFFF);
	uint32_t curts_high = (curts >> 32);

	if (pdesc->TIMESTAMP > curts_low)
	{
		return (uint64_t(curts_high - 1) << 32) | pdesc->TIMESTAMP;
	}
	else
	{
		return (uint64_t(curts_high) << 32) | pdesc->TIMESTAMP;
	}
}

void THwEth_imxrt::NsTimeSetCorrection(float acorr)
{
	if ((acorr > -0.01) && (acorr < 0.01))
	{
		nstime_correction = acorr;
	}

	uint32_t corrinc;
	float corr_counts;

	if (nstime_correction > 0.00000002)
	{
		corrinc = nstime_inc_base + 1;
		corr_counts = 1.0 / (nstime_correction * (float)nstime_inc_base);
	}
	else if (nstime_correction < -0.00000002)
	{
		corrinc = nstime_inc_base - 1;
		corr_counts = 1.0 / (-nstime_correction * (float)nstime_inc_base);
	}
	else
	{
		// ignore, too small correction
		corrinc = nstime_inc_base;
		corr_counts = 0;
	}

  regs->ATINC = nstime_inc_base | (corrinc << 8);
  // the lowest bit can not be 1, this is not mentioned in the IMXRT documentation !
  regs->ATCOR = (unsigned(corr_counts + 0.5) + 1) & 0x7FFFFFFE;
}
