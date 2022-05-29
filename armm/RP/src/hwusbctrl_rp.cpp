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
 *  file:     hwusbctrl_rp.cpp
 *  brief:    RP2040 USB Devcie Controller
 *  version:  1.00
 *  date:     2022-05-19
 *  authors:  nvitya
*/

#include "platform.h"

#include "string.h"
#include <stdio.h>
#include <stdarg.h>
#include <hwusbctrl.h>
#include "traces.h"
#include "clockcnt.h"

#include "rp_utils.h"

bool THwUsbEndpoint_rp::ConfigureHwEp()
{
	uint  tmp;
	uint  epc;
  uint  htod_len;
  uint  dtoh_len;

	if (!usbctrl)
	{
		return false;
	}

	usb_device_dpram_t *  dpram = usbctrl->dpram;

	if (dir_htod)  // "OUT"
	{
    htod_len = maxlen;
    dtoh_len = 0;
    if (index > 0)
    {
      epctrl = &dpram->ep_ctrl[index - 1].out;
    }
    else
    {
      epctrl = nullptr;
    }
	}
	else // dtoh = "IN"
	{
    htod_len = 0;
    dtoh_len = maxlen;
    if (index > 0)
    {
      epctrl = &dpram->ep_ctrl[index - 1].in;
    }
    else
    {
      epctrl = nullptr;
    }
	}

  bufctrl_dtoh = &dpram->ep_buf_ctrl[index].in;
  bufctrl_htod = &dpram->ep_buf_ctrl[index].out;

  epc = (0
    | (1  << 31)  // ENABLE
    | (0  << 30)  // DOUBLE_BUFFERED: 0 = single buffered, 1 = double buffered
    | (1  << 29)  // INT_PER_BUFFER: 1 = interrupt for every transfer
    | (0  << 28)  // INT_PER DOUBLE_BUFFER:
    | (0  << 26)  // EP_TYPE(2): 0 = control, 1 = ISO, 2 = bulk, 3 = interrupt
    | (0  << 17)  // INT_ON_STALL
    | (0  << 16)  // INT_ON_NAK
    | (0  <<  0)  // BASE_ADDR(16)
  );

  if ((attr & HWUSB_EP_TYPE_MASK) == HWUSB_EP_TYPE_INTERRUPT)
  {
    epc |= (3 << 26);
  }
  else if ((attr & HWUSB_EP_TYPE_MASK) == HWUSB_EP_TYPE_BULK)
  {
    epc |= (2 << 26);
  }
  else if ((attr & HWUSB_EP_TYPE_MASK) == HWUSB_EP_TYPE_ISO)
  {
    epc |= (1 << 26);
  }
  else // else control by default
  {
    htod_len = maxlen;
    dtoh_len = maxlen;
    epc |= (0 << 26);
  }

  uint32_t baddr = 0;

	// buffer allocation
	if (0 == index)
	{
	  rxmem = &dpram->ep0_buf_a[0];
	  txmem = &dpram->ep0_buf_a[0];  // same buffer !
	}
	else
	{
	  if (htod_len)
	  {
	    baddr = usbctrl->ep_mem_alloc;
	    rxmem = &dpram->epx_data[baddr];
	    usbctrl->ep_mem_alloc += 64;
	  }

    if (dtoh_len)
    {
      baddr = usbctrl->ep_mem_alloc;
      txmem = &dpram->epx_data[baddr];
      usbctrl->ep_mem_alloc += 64;
    }
	}

  epc |= baddr + offsetof(usb_device_dpram_t, epx_data);

  if (epctrl)
  {
    *epctrl = epc;
  }

  *bufctrl_dtoh = 0;
  *bufctrl_htod = 0;
  next_data_pid = 0;

	return true;
}

int THwUsbEndpoint_rp::ReadRecvData(void * buf, uint32_t buflen)
{
  if (usbctrl->setup_request)
  {
    // different buffer location
    memcpy(buf, (void *)&usbctrl->dpram->setup_packet[0], 8);
    next_data_pid = USB_BUF_CTRL_DATA1_PID; // set data pid to 1
    return 8;
  }

  uint32_t bufcr = *bufctrl_htod;
  uint32_t cnt = (bufcr & USB_BUF_CTRL_LEN_MASK);

	if (cnt)
	{
		if (buflen < cnt)
		{
			return USBERR_BUFFER_TOO_SMALL;
		}

		// optimized copy

		uint32_t cnt32 = ((cnt + 3) >> 2);
		uint32_t * psrc32 = (uint32_t *)rxmem;
		uint32_t * pdst32 = (uint32_t *)buf;
		uint32_t * pend32 = pdst32 + cnt32;
		while (pdst32 < pend32)
		{
			*pdst32++ = *psrc32++;
		}
	}

  next_data_pid ^= USB_BUF_CTRL_DATA1_PID; // flip next data pid

	return cnt;
}

// the answer of the device descriptor

int THwUsbEndpoint_rp::StartSendData(void * buf, unsigned len)
{
  int sendlen = len;
  if (sendlen > maxlen)  sendlen = maxlen;

  uint32_t bufcr = (0
    | (1  << 15)  // BUF0_FULL
    | (0  << 14)  // BUF0_LAST
    | (0  << 13)  // BUF0_DATA_PID: must be toggled
    | (0  << 12)  // BUF_SEL
    | (0  << 11)  // STALL
    | (1  << 10)  // BUF0_AVAILABLE
    | (sendlen <<  0)  // BUF0_LEN(10)
  );

	// optimized copy

	uint32_t cnt32 = ((sendlen + 3) >> 2);
	uint32_t * psrc32 = (uint32_t *)buf;
	uint32_t * pdst32 = (uint32_t *)txmem;
	uint32_t * pend32 = pdst32 + cnt32;
	while (pdst32 < pend32)
	{
		*pdst32++ = *psrc32++;
	}

  bufcr |= next_data_pid;

  *bufctrl_dtoh = bufcr;

  next_data_pid ^= USB_BUF_CTRL_DATA1_PID; // flip next data pid

	return sendlen;
}

void THwUsbEndpoint_rp::SendAck()
{
  uint32_t bufcr = (0
    | (1  << 15)  // BUF0_FULL
    | (0  << 14)  // BUF0_LAST
    | (0  << 13)  // BUF0_DATA_PID: must be toggled
    | (0  << 12)  // BUF_SEL
    | (0  << 11)  // STALL
    | (1  << 10)  // BUF0_AVAILABLE
    | (0  <<  0)  // BUF0_LEN(10)
  );

  bufcr |= next_data_pid;

  *bufctrl_dtoh = bufcr;

  next_data_pid ^= USB_BUF_CTRL_DATA1_PID; // flip next data pid
}

bool THwUsbEndpoint_rp::IsSetupRequest()
{
  return usbctrl->setup_request;
}

void THwUsbEndpoint_rp::Nak()
{
}

void THwUsbEndpoint_rp::EnableRecv()
{
  uint32_t bufcr = (0
    | (0  << 15)  // BUF0_FULL
    | (0  << 14)  // BUF0_LAST
    | (0  << 13)  // BUF0_DATA_PID: must be toggled
    | (0  << 12)  // BUF_SEL
    | (0  << 11)  // STALL
    | (1  << 10)  // BUF0_AVAILABLE
    | (maxlen <<  0)  // BUF0_LEN(10)
  );

  bufcr |= next_data_pid;

  *bufctrl_htod = bufcr;
}

void THwUsbEndpoint_rp::DisableRecv()
{
  uint32_t bufcr = *bufctrl_htod;
  if (bufcr & USB_BUF_CTRL_AVAIL)
  {
    bufcr &= ~USB_BUF_CTRL_AVAIL;
    *bufctrl_htod = bufcr;
  }
}

void THwUsbEndpoint_rp::StopSend()
{
	//udp_ep_csreg_bit_clear(csreg, UDP_CSR_TXCOMP | UDP_CSR_TXPKTRDY);
}

void THwUsbEndpoint_rp::FinishSend()
{
	//regs->EPINTFLAG.bit.TRCPT1 = 1;
}

void THwUsbEndpoint_rp::Stall()
{
  if (iscontrol)
  {
    *bufctrl_dtoh |= USB_BUF_CTRL_STALL;
    *bufctrl_htod |= USB_BUF_CTRL_STALL;
    // the stall is double gated for the EP0 with a special register:
    usbctrl->regs->ep_stall_arm = 3; // set the stall bits here too, they are cleared on the next SETUP request
  }
  else if (!dir_htod)
	{
	  *bufctrl_dtoh |= USB_BUF_CTRL_STALL;
	}
	else
	{
    *bufctrl_htod |= USB_BUF_CTRL_STALL;
	}
}

/************************************************************************************************************
 * THwUsbCtrl_rp
 ************************************************************************************************************/

bool THwUsbCtrl_rp::InitHw()
{
  unsigned n;

	regs       = usb_hw;
	regs_set   = hw_set_alias(usb_hw);
	regs_clear = hw_clear_alias(usb_hw);

	dpram      = usb_dpram;

	rp_reset_control(RESETS_RESET_USBCTRL_BITS, true);
	rp_reset_control(RESETS_RESET_USBCTRL_BITS, false);
	delay_us(10);


  // Mux the controller to the onboard usb phy
  regs->muxing = USB_USB_MUXING_TO_PHY_BITS | USB_USB_MUXING_SOFTCON_BITS;

  // Force VBUS detect so the device thinks it is plugged into a host
  regs->pwr = USB_USB_PWR_VBUS_DETECT_BITS | USB_USB_PWR_VBUS_DETECT_OVERRIDE_EN_BITS;

  // Enable the USB controller in device mode.
  regs->main_ctrl = USB_MAIN_CTRL_CONTROLLER_EN_BITS;

  // Enable an interrupt per EP0 transaction
  regs->sie_ctrl = USB_SIE_CTRL_EP0_INT_1BUF_BITS; // <2>

  // Enable interrupts for when a buffer is done, when the bus is reset,
  // and when a setup packet is received
  regs->inte = USB_INTS_BUFF_STATUS_BITS |
                 USB_INTS_BUS_RESET_BITS |
                 USB_INTS_SETUP_REQ_BITS;


	ResetEndpoints();


	return true;
}

void THwUsbCtrl_rp::ResetEndpoints()
{
  memset(dpram, 0, 1024);  // clear only the first 1k, this is more than enough

	ep_mem_alloc = 0;
}

void THwUsbCtrl_rp::SetPullUp(bool aenable)
{
	if (aenable)
	{
	  regs_set->sie_ctrl = USB_SIE_CTRL_PULLUP_EN_BITS;
	}
	else
	{
    regs_clear->sie_ctrl = USB_SIE_CTRL_PULLUP_EN_BITS;
	}
}

void THwUsbCtrl_rp::DisableIrq()
{
	//{  /* regs->CNTR &= ~irq_mask; */ }
}

void THwUsbCtrl_rp::EnableIrq()
{
	//{  /* regs->CNTR |=  irq_mask; */ }
}

void THwUsbCtrl_rp::SetDeviceAddress(uint8_t aaddr)
{
  usb_hw->dev_addr_ctrl = aaddr;
}

//uint32_t usb_dbg_time_ref = 0;

void THwUsbCtrl_rp::HandleIrq()
{
	uint32_t ints = regs->ints;
	if (ints & USB_INTS_SETUP_REQ_BITS)
	{
	  //TRACE("%u ", timer_hw->timelr - usb_dbg_time_ref);
    TRACE("USB SETUP IRQ\r\n");
    regs_clear->sie_status = USB_SIE_STATUS_SETUP_REC_BITS;
    setup_request = true;
    if (!HandleEpTransferEvent(0, true))
    {
      // todo: handle error
    }
    setup_request = false;
	}

	if (ints & USB_INTS_BUFF_STATUS_BITS)
	{
	  uint buffm = regs->buf_status;
    //TRACE("%u ", timer_hw->timelr - usb_dbg_time_ref);
	  //TRACE("USB BUFF IRQ: %08X\r\n", buffm);
	  // no __CLZ() is available here, so bit looping
    uint bit = 1;
    for (uint i = 0; buffm && i < USB_NUM_ENDPOINTS * 2; i++)
    {
      if (buffm & bit)
      {
        // clear this in advance
        regs_clear->buf_status = bit;

        // IN transfer for even i, OUT transfer for odd i
        uint epid = (i >> 1);
        if (i & 1)
        {
          // OUT transfer if odd
          if (!HandleEpTransferEvent(epid, true))
          {
            // todo: handle error
          }
        }
        else
        {
          // IN transfer
          if (!HandleEpTransferEvent(epid, false))
          {
            // todo: handle error
          }
        }

        buffm ^= bit;
      }
      bit <<= 1;
    }

	}

	if (ints & USB_INTS_BUS_RESET_BITS)
	{
	  //if (0 == usb_dbg_time_ref)  usb_dbg_time_ref = timer_hw->timelr;
    //TRACE("%u ", timer_hw->timelr - usb_dbg_time_ref);
    TRACE("USB RESET\r\n");

    regs_clear->sie_status = USB_SIE_STATUS_BUS_RESET_BITS;

    // disable address, configured state
    SetDeviceAddress(0);

    HandleReset();
	}
}

