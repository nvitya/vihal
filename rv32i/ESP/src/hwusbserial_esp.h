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
// file:     hwusbserial_esp.h
// brief:    Espressif internal USB Serial (CDC-ACM) character driver
// created:  2023-03-24
// authors:  nvitya

#ifndef SRC_HWUSBSERIAL_ESP_H_
#define SRC_HWUSBSERIAL_ESP_H_

#include "platform.h"
#include "stdarg.h"

class THwUsbSerialEsp
{
public:
  bool    initialized = false;
  bool    auto_flush_printf = true;

  esp_usb_serial_jtag_t *  regs = nullptr;

  virtual ~THwUsbSerialEsp() { }

  bool    Init();

  bool    TrySendChar(char ach);
  bool    TryRecvChar(char * ach);
  bool    SendFinished();
  void    Flush();  // the PC does not receives anything until this is called

  void    SendChar(char ach) { while (!TrySendChar(ach)) {} }

  void    printf(const char * fmt, ...);
  void    printf_va(const char * fmt, va_list arglist);


};

#endif /* SRC_HWUSBSERIAL_ESP_H_ */
