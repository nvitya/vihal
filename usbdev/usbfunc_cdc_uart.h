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
/*
 *  file:     usbif_cdc_uart.h
 *  brief:    USB to Serial USB Interface definition
 *  version:  1.00
 *  date:     2020-08-01
 *  authors:  nvitya
 *  note:
 *    it can be used multiple times
*/

#ifndef SRC_USBIF_CDC_UART_H_
#define SRC_USBIF_CDC_UART_H_

#include "usbdevice.h"
#include "usbif_cdc.h"
#include "hwuart.h"
#include "hwdma.h"


class TUifCdcUartData;

class TUifCdcUartControl : public TUsbInterface
{
private:
	typedef TUsbInterface super;

public:
	THwUart *         uart = nullptr;
	TUifCdcUartData *	dataif = nullptr;

	bool              initialized = false;

	TCdcLineCoding    linecoding;

	TUsbEndpoint      ep_manage;
	uint8_t           databuf[64] __attribute__((aligned(4)));

	// Serial (UART) --> USB
	uint8_t           serial_rxbuf[128] __attribute__((aligned(4)));  // use as circular buffer
	uint8_t           serial_rxidx = 0;


	// USB --> Serial (UART)
	uint8_t           serial_txbuf[2][64] __attribute__((aligned(4)));
	uint8_t           serial_txbufidx = 0;
	uint8_t           serial_txlen = 0;

	TUsbCdcDescCallUnionFunc  desc_cdc_union =
  {
    .length = 5,
    .descriptor_type = 0x24,  // 0x24 = CS_INTERFACE
    .descriptor_subtype = 6,  // 6 = union func
    .master_interface = 0,    // will be set automatically
    .slave_interface = 1      // will be set automatically
  };

	TUsbCdcDescCallManagement  desc_cdc_callmgmt =
  {
    .length = 5,
    .descriptor_type = 0x24,  // 0x24 = CS_INTERFACE
    .descriptor_subtype = 6,  // 6 = union func
    .capabilities = 0,        // 0 = no call management
    .data_interface = 0       // will be set automatically
  };

public: // interface specific
	bool              InitCdcUart(TUifCdcUartData * adataif, THwUart * auart);
	void              Run(); // must be called periodically

	bool              SerialAddBytes(uint8_t * adata, unsigned adatalen);
	void              SerialSendBytes();

protected:
	bool              uart_running = false;
	THwDmaTransfer    dmaxfer_tx;
	THwDmaTransfer    dmaxfer_rx;

	void              StopUart();
	void              StartUart();

public: // mandatory virtual functions
	virtual bool      InitInterface();
	virtual void      OnConfigured();
	virtual bool      HandleTransferEvent(TUsbEndpoint * aep, bool htod);
	virtual bool      HandleSetupRequest(TUsbSetupRequest * psrq);
	virtual bool      HandleSetupData(TUsbSetupRequest * psrq, void * adata, unsigned adatalen);

};

class TUifCdcUartData : public TUsbInterface
{
private:
	typedef TUsbInterface super;

public:
	TUifCdcUartControl *  control = nullptr;

	bool                  ready_to_send = true;

	TUsbEndpoint          ep_input;
	TUsbEndpoint          ep_output;

	uint8_t               usb_txbuf[2][128] __attribute__((aligned(4)));
	uint8_t               usb_txbufidx = 0;
	uint8_t               usb_txlen = 0;  // non null value signalizes pending USB RX data to be sent to the serial

	uint8_t           		usb_rxbuf[64+4] __attribute__((aligned(4)));  // the data from the usb (+4 for debugging)
	uint8_t               usb_rxlen = 0;

public:
	bool                  AddTxByte(uint8_t abyte);
	bool                  SendTxBytes();
	void                  Reset();
	void                  TrySendUsbDataToSerial();

public: // mandatory virtual functions
	virtual bool    InitInterface();
	virtual void    OnConfigured();
	virtual bool    HandleTransferEvent(TUsbEndpoint * aep, bool htod);
};

class TUsbFuncCdcUart : public TUsbFunction
{
public:
  THwUart *            uart = nullptr;

  TUifCdcUartControl   uif_control;
  TUifCdcUartData      uif_data;

  void                 AssignUart(THwUart * auart);

  virtual bool         InitFunction();
  virtual void         Run();
};

#endif /* SRC_USBIF_CDC_UART_H_ */
