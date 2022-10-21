/* -----------------------------------------------------------------------------
 * This file is a part of the NVCM project: https://github.com/nvitya/nvcm
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
 *  file:     hweth.h
 *  brief:    Ethernet vendor-independent definitions
 *  version:  1.00
 *  date:     2018-05-30
 *  authors:  nvitya
*/

#ifndef HWETH_H_PRE_
#define HWETH_H_PRE_

#include "hwpins.h"

#define HWETH_MAX_PACKET_SIZE   1536

// Generic PHY registers
#define HWETH_PHY_BCR_REG        0x0	/*!< Basic Control Register */
#define HWETH_PHY_BSR_REG        0x1	/*!< Basic Status Reg */
#define HWETH_PHY_PHYID1_REG     0x2	/*!< PHY ID 1 Reg  */
#define HWETH_PHY_PHYID2_REG     0x3	/*!< PHY ID 2 Reg */

// BCR bit definitions
#define HWETH_PHY_BCR_RESET          (1 << 15)	/*!< 1= S/W Reset */
#define HWETH_PHY_BCR_LOOPBACK       (1 << 14)	/*!< 1=loopback Enabled */
#define HWETH_PHY_BCR_100M_SPEED_SELECT   (1 << 13)	/*!< 1=Select 100MBps */
#define HWETH_PHY_BCR_AUTONEG        (1 << 12)	/*!< 1=Enable auto-negotiation */
#define HWETH_PHY_BCR_POWER_DOWN     (1 << 11)	/*!< 1=Power down PHY */
#define HWETH_PHY_BCR_ISOLATE        (1 << 10)	/*!< 1=Isolate PHY */
#define HWETH_PHY_BCR_RESTART_AUTONEG (1 << 9)	/*!< 1=Restart auto-negoatiation */
#define HWETH_PHY_BCR_DUPLEX_MODE    (1 << 8)	/*!< 1=Full duplex mode */

// BSR bit definitions
#define HWETH_PHY_BSR_AUTONEG_COMP   (1 << 5)	/*!< Auto-negotation complete */
#define HWETH_PHY_BSR_RMT_FAULT      (1 << 4)	/*!< Fault */
#define HWETH_PHY_BSR_AUTONEG_ABILITY (1 << 3)	/*!< Auto-negotation supported */
#define HWETH_PHY_BSR_LINK_STATUS    (1 << 2)	/*!< 1=Link active */
#define HWETH_PHY_BSR_JABBER_DETECT  (1 << 1)	/*!< Jabber detect */
#define HWETH_PHY_BSR_EXTEND_CAPAB   (1 << 0)	/*!< Supports extended capabilities */

#define HWETH_PHY_SPEEDINFO_REG       0x1F
#define HWETH_PHY_SPEEDINFO_100M    0x0008
#define HWETH_PHY_SPEEDINFO_FULLDX  0x0010

#define HWETH_PHY_SPEEDINFO_MASK    (HWETH_PHY_SPEEDINFO_100M | HWETH_PHY_SPEEDINFO_FULLDX)

#define HWETH_PMEM_HEAD_SIZE        32

#define PMEMFLAG_KEEP             0x01  // do not release the Rx Packet
#define PMEMFLAG_SYS              0x02  // special system Tx Packet, do not add back to the TX packet pool

typedef struct TPacketMem
{
  uint8_t       idx;
  uint8_t       flags;
  uint8_t       status; // for tx 1 = sending, 0 = sending finished / aborted
  uint8_t       _reserved;
  uint16_t      datalen;
  uint16_t      max_datalen;
  uint64_t      timestamp_ns;
  // this part can be freely used by the application / network stack
  TPacketMem *  next;
  uint8_t       extra[12];

  uint8_t       data[HWETH_MAX_PACKET_SIZE];
//
} TPacketMem, * PPacketMem;

class THwEth_pre
{
public: // settings

	uint8_t       phy_address = 0;

	uint8_t       mac_address[6] = {0x03, 0x01, 0x01, 0x01, 0x01, 0x01};

	bool          irq_on_rx = false;
	bool          irq_on_tx = false;

	bool          hw_ip_checksum = true; // IP checksums are generated by the HW
	bool          promiscuous_mode = false;

	uint8_t       refclock_mhz = 50;
	bool          external_ref_clock = false;

	bool          loopback = false;

public:

	uint32_t      rx_desc_count = 0;
	uint32_t      tx_desc_count = 0;

public:
	bool          initialized = false;
	bool          link_up = false;
	uint32_t      phy_config = 0;
	bool          mii_running = false;

	uint32_t      recv_count = 0;
	uint32_t      recv_error_count = 0;

	uint8_t *     descmem = nullptr;
	uint32_t      descmemsize = 0;

	virtual ~THwEth_pre() { }

};

#endif // ndef HWETH_H_PRE_

#ifndef HWETH_PRE_ONLY

//-----------------------------------------------------------------------------

#ifndef HWETH_H_
#define HWETH_H_

#include "mcu_impl.h"

#if !defined(HWETH_IMPL)

class THwEth_noimpl : public THwEth_pre
{
public: // mandatory
	bool               InitMac(void * prxdesclist, uint32_t rxcnt, void * ptxdesclist, uint32_t txcnt);

	void               Start()  { }
	void               Stop()   { }

	void               SetMacAddress(uint8_t * amacaddr) { }
	void               SetSpeed(bool speed100)   { }
	void               SetDuplex(bool full)      { }

	bool               TryRecv(TPacketMem * * pmem) { return false; }
	void               ReleaseRxBuf(TPacketMem * pmem) { }
	void               AssignRxBuf(uint32_t idx, TPacketMem * pmem, uint32_t datalen) { }

	bool               TrySend(uint32_t * pidx, void * pdata, uint32_t datalen) { return false; }
  bool               SendFinished(uint32_t idx) { return false; }
	uint64_t           GetTimeStamp(uint32_t idx) { return 0; }

	void               StartMiiWrite(uint8_t reg, uint16_t data) { }
	void               StartMiiRead(uint8_t reg) { }
	bool               IsMiiBusy() { return false; }
	inline uint16_t    MiiData() { return 0; }

	void               NsTimeStart() { }
	uint64_t           NsTimeRead()  { return 0; }
	void               NsTimeSetCorrection(float acorr) { }
};

#define HWETH_IMPL   THwEth_noimpl

#endif // ndef HWETH_IMPL

//-----------------------------------------------------------------------------

class THwEth : public HWETH_IMPL
{
public:
	bool               Init(void * prxdesclist, uint32_t rxcnt, void * ptxdesclist, uint32_t txcnt);

	bool               PhyInit();

	bool               MiiWaitBusy(int amaxms);

	bool               MiiWrite(uint8_t reg, uint16_t data); // blocking mii write (for setup only)
	bool               MiiRead(uint8_t reg, uint16_t * data); // blocking mii read (for setup only)
	bool 			         PhyWaitReset();

	void               PhyStatusPoll(void); // this must be called regularly !

protected:
	uint16_t           phy_speedinfo;
	uint16_t           phy_speedinfo_prev;
	uint16_t           phy_bsr_value;

	uint8_t            phy_poll_state = 0;
	void               AdjustSpeed(uint16_t aphy_speedinfo);

};

#endif // ndef HWETH_H_ */

#else
  #undef HWETH_PRE_ONLY
#endif
