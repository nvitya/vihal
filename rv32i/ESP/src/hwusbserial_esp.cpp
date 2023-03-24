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
// file:     hwusbserial_esp.cpp
// brief:    Espressif internal USB Serial (CDC-ACM) character driver
// created:  2023-03-24
// authors:  nvitya

#include "mp_printf.h"
#include "hwuart.h"  // for FMT_BUFFER_SIZE
#include <hwusbserial_esp.h>

bool THwUsbSerialEsp::Init()
{
  initialized = false;

  regs = USB_SERIAL_JTAG;

  // no special setup is required, it should work from the beginning

  initialized = true;
  return true;
}

bool THwUsbSerialEsp::TrySendChar(char ach)
{
  if (regs->EP1_CONF & (1 << 1)) // are some free space for the data?
  {
    regs->EP1 = ach;
    return true;
  }
  return false;
}

bool THwUsbSerialEsp::TryRecvChar(char * ach)
{
  if (regs->EP1_CONF & (1 << 2))
  {
    *ach = regs->EP1;
    return true;
  }
  return false;
}

bool THwUsbSerialEsp::SendFinished()
{
  return (0 != (regs->EP1_CONF & (1 << 1)));
}

void THwUsbSerialEsp::printf_va(const char * fmt, va_list arglist)
{
  // allocate format buffer on the stack:
  char fmtbuf[FMT_BUFFER_SIZE];

  char * pch = &fmtbuf[0];
  *pch = 0;

  mp_vsnprintf(pch, FMT_BUFFER_SIZE, fmt, arglist);

  while (*pch != 0)
  {
    SendChar(*pch);
    ++pch;
  }

  if (auto_flush_printf)
  {
    Flush(); // to send the buffered characters to the host, also sends a zero packet
  }
}

void THwUsbSerialEsp::printf(const char* fmt, ...)
{
  va_list arglist;
  va_start(arglist, fmt);

  printf_va(fmt, arglist);

  va_end(arglist);
}

void THwUsbSerialEsp::Flush()
{
  while (0 == regs->EP1_CONF & (1 << 1)) // are some free space for the data?
  {
    // wait
  }

  regs->EP1_CONF = 1;
}
