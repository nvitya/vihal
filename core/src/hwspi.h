/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
// file:     hwspi.h
// brief:    SPI Master vendor-independent definitions
// created:  2018-02-10
// authors:  nvitya

#ifndef HWSPI_H_PRE_
#define HWSPI_H_PRE_

#include "hwpins.h"
#include "hwdma.h"

#define HWSPI_MAX_XFER_BLOCK  4       // minimum 4 blocks required (cmd, addr, extra, data)

// Command byte count
#define SPITR_CMD         0x00000008  // send command with the default size
#define SPITR_CMD0        0x00000000  // do not send command (default)
#define SPITR_CMD1        0x00000001
#define SPITR_CMD2        0x00000002
#define SPITR_CMD3        0x00000003
#define SPITR_CMD4        0x00000004
#define SPITR_CMD_MASK    0x0000000F

// Addres byte count
#define SPITR_ADDR        0x00000080  // send address with the default size
#define SPITR_ADDR0       0x00000000  // do not send address (default)
#define SPITR_ADDR1       0x00000010
#define SPITR_ADDR2       0x00000020
#define SPITR_ADDR3       0x00000030
#define SPITR_ADDR4       0x00000040
#define SPITR_ADDR_MASK   0x000000F0

// Extra data after the address (dummy/mode bits)
#define SPITR_EXTRA       0x00000800  // send extra data with the default size
#define SPITR_EXTRA0      0x00000000  // do not send extra data (default)
#define SPITR_EXTRA1      0x00000100
#define SPITR_EXTRA2      0x00000200
#define SPITR_EXTRA3      0x00000300
#define SPITR_EXTRA4      0x00000400
#define SPITR_EXTRA_MASK  0x00000F00

typedef struct TSpiXferBlock
{
	uint32_t         len;
	uint8_t *        src;
	uint8_t *        dst;
//
} TSpiTransfer;

class THwSpi_pre
{
public:
	int      				 devnum = -1;
	bool             initialized = false;

	unsigned         speed = 1000000;  // default speed = 1MHz
	unsigned char    databits = 8; // frame length
	bool             lsb_first = false;
	bool             idleclk_high = false;
	bool             datasample_late = false;
	bool             inter_frame_pulse = false; // CS pulse between frames
	uint8_t          cs_number = 0; // if multiple CS lines are supported

	uint8_t          fifo_size = 1;

	TGpioPin *			 manualcspin = nullptr;

	THwDmaChannel *  txdma = nullptr;
	THwDmaChannel *  rxdma = nullptr;

	uint8_t          state = 0;  // state machine

public: // transaction handling

	bool             finished = false;
	uint8_t          blockcnt = 0;

	uint8_t          default_cmd_len = 1;
	uint8_t          default_addr_len = 3;
	uint8_t          default_extra_len = 1;

	uint32_t         data_cmd = 0;
	uint32_t         data_addr = 0;
	uint32_t         data_extra = 0;
	uint32_t         data_void = 0;  // ignored write target
	uint32_t         data_zero = 0;  // ignored read source

	TSpiXferBlock    xferblock[HWSPI_MAX_XFER_BLOCK];

public:
  void  PrepareTransfer(uint32_t acmd, uint32_t aaddr, uint32_t aflags,
  	                    uint32_t alen, uint8_t * asrc, uint8_t * adst);

};

#endif // ndef HWSPI_H_PRE_

#ifndef HWSPI_PRE_ONLY

//-----------------------------------------------------------------------------

#ifndef HWSPI_H_
#define HWSPI_H_

#include "mcu_impl.h"

#ifndef HWSPI_IMPL

#ifdef SKIP_UNIMPLEMENTED_WARNING
  #undef SKIP_UNIMPLEMENTED_WARNING
#else
  #warning "HWSPI is not implemented!"
#endif

class THwSpi_noimpl : public THwSpi_pre
{
public: // mandatory
	bool Init(int adevnum)        { return false; }

	bool TrySendData(unsigned short adata)     { return false; }
	bool TryRecvData(unsigned short * dstptr)  { return false; }
	bool SendFinished()                        { return true; }

	void Run() { } // runs the prepared transaction

	void DmaAssign(bool istx, THwDmaChannel * admach)  { }

	bool DmaStartSend(THwDmaTransfer * axfer)  { return false; }
	bool DmaStartRecv(THwDmaTransfer * axfer)  { return false; }
	bool DmaSendCompleted()  { return true; }
	bool DmaRecvCompleted()  { return false; }
};

#define HWSPI_IMPL   THwSpi_noimpl

#endif // ndef HWSPI_IMPL

//-----------------------------------------------------------------------------

class THwSpi : public HWSPI_IMPL
{
public:
	void            BeginTransaction(); // puts the chip select low
	void            EndTransaction(); // puts the chip select high

	//int             RunTransfer(TSpiTransfer * axfer);

	inline void     SendData(unsigned short adata) { while (!TrySendData(adata)) {} }; // wait until it successfully started, but does not wait to finish !
	void            WaitSendFinish();

  inline void     StartTransfer(uint32_t acmd, uint32_t aaddr, uint32_t aflags,
  		                          uint32_t alen, uint8_t * asrc, uint8_t * adst)
  {
  	PrepareTransfer(acmd, aaddr, aflags, alen, asrc, adst);
  	Run();
  }

};

#endif // ndef HWSPI_H_ */

#else
  #undef HWSPI_PRE_ONLY
#endif
