/*
 *  file:     hweth_imxrt.h
 *  brief:    VIHAL compatible IMXRT Ethernet driver
 *  date:     2023-09-14
 *  authors:  nvitya
*/

#ifndef HWETH_IMXRT_H_
#define HWETH_IMXRT_H_

#include "platform.h"

#define HWETH_PRE_ONLY
#include "hweth.h"

#define HW_ETH_REGS  ENET_Type

// extra fields are defined by hex offet to comply to the reference manual

typedef struct
{
    uint16_t    LENGTH;
    uint16_t    CONTROL;  // Buffer descriptor control and status
    uint8_t *   BUFFER;   // Data buffer pointer.
    uint16_t    WORD_08;  // word at offset 0x08: extended status and control
    uint16_t    WORD_0A;
    uint16_t    WORD_0C;
    uint16_t    WORD_0E;
    uint16_t    WORD_10;
    uint16_t    BDU;
    uint32_t    TIMESTAMP; // (at 0x14)
    uint16_t    WORD_18;   // not used
    uint16_t    WORD_1A;   // not used
    uint16_t    WORD_1C;   // not used
    uint16_t    WORD_1E;   // not used
//
} __attribute__((packed)) HW_ETH_DMA_DESC;

static_assert(sizeof(HW_ETH_DMA_DESC) == 32, "Wrong ETH DMA structure size!");

class THwEth_imxrt : public THwEth_pre
{
public:
	void               Start();
	void               Stop();

public:
	void               SetMacAddress(uint8_t * amacaddr);
	void               SetSpeed(bool speed100);
	void               SetDuplex(bool full);

public:
	void               StartMiiWrite(uint8_t reg, uint16_t data);
	void               StartMiiRead(uint8_t reg);
	inline uint16_t    MiiData() { return (regs->MMFR & 0xFFFF); }
	bool               IsMiiBusy();

public:
	HW_ETH_REGS *      regs = nullptr;

	HW_ETH_DMA_DESC *  rx_desc_list = nullptr;
	HW_ETH_DMA_DESC *  tx_desc_list = nullptr;

	bool               InitMac(void * prxdesclist, uint32_t rxcnt, void * ptxdesclist, uint32_t txcnt);
	void               AssignRxBuf(uint32_t idx, TPacketMem * pmem, uint32_t datalen);

  bool               TryRecv(TPacketMem * * pmem);
  void               ReleaseRxBuf(TPacketMem * pmem);
	bool               TrySend(uint32_t * pidx, void * pdata, uint32_t datalen);
  bool               SendFinished(uint32_t idx);

public: // ns timer
	uint32_t           nstimer_high = 0;
	uint32_t           nstimer_low_prev = 0;

	uint32_t           nstime_inc_base = 40;
	float              nstime_correction = 0.0;

  uint64_t           GetTimeStamp(uint32_t idx); // must be called within 2 s to get the right upper 32 bit
	void               NsTimeStart();
	uint64_t           NsTimeRead();
	void               NsTimeSetCorrection(float acorr);

protected:
	HW_ETH_DMA_DESC *  actual_rx_desc;
	uint32_t           actual_tx_idx;

	void               InitDescList(bool istx, int bufnum, HW_ETH_DMA_DESC * pdesc_list);

};

#define HWETH_IMPL THwEth_imxrt

#endif // def HWETH_IMXRT_H_
