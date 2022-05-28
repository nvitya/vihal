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
 *  file:     hwusbctrl_rp.h
 *  brief:    RP2040 USB Devcie Controller
 *  version:  1.00
 *  date:     2022-05-19
 *  authors:  nvitya
*/

#ifndef HWUSBCTRL_RP_H_
#define HWUSBCTRL_RP_H_

#include "platform.h"

#define HWUSBCTRL_PRE_ONLY
#include "hwusbctrl.h"

//#define USB_MAX_ENDPOINTS       8

#define USB_RX_BUFFER_SIZE    256
#define USB_TX_BUFFER_SIZE    256

class THwUsbEndpoint_rp : public THwUsbEndpoint_pre
{
public:
	volatile uint32_t *  epctrl  = nullptr;
	volatile uint32_t *  bufctrl_dtoh  = nullptr;
	volatile uint32_t *  bufctrl_htod = nullptr;

	uint8_t *            rxmem = nullptr;
	uint8_t *            txmem = nullptr;

	uint32_t             next_data_pid = 0;

	bool ConfigureHwEp();
  int  ReadRecvData(void * buf, uint32_t buflen);
	int  StartSendData(void * buf, unsigned len);

	void SendAck();
  void Stall();
  void Nak();

  bool IsSetupRequest();

  void EnableRecv();
  void DisableRecv();
  void StopSend();
  void FinishSend();
};

class THwUsbCtrl_rp : public THwUsbCtrl_pre
{
public:
  usb_hw_t *            regs       = nullptr;
  usb_hw_t *            regs_set   = nullptr;  // alias for set bits
  usb_hw_t *            regs_clear = nullptr;  // alias for clear bits

  usb_device_dpram_t *  dpram = nullptr;

	uint32_t              irq_mask = 0;

	uint32_t              ep_mem_alloc = 0;

	bool                  setup_request = false;

	bool InitHw();

	void HandleIrq();

	void DisableIrq();
	void EnableIrq();
	void SetDeviceAddress(uint8_t aaddr);
	virtual void SetPullUp(bool aenable);

	void ResetEndpoints();
};

#define HWUSBENDPOINT_IMPL   THwUsbEndpoint_rp
#define HWUSBCTRL_IMPL       THwUsbCtrl_rp

#endif // def HWUSBCTRL_RP_H_
