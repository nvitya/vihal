/* -----------------------------------------------------------------------------
 * This file is a part of the NVCM Tests project: https://github.com/nvitya/nvcmtests
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
 *  file:     ledstripe.cpp
 *  brief:    APA 102 led stripe handler
 *  version:  1.00
 *  date:     2018-12-16
 *  authors:  nvitya
 *  note:
 *    Required resources: SPI + DMA
*/

#include "string.h"
#include <ledstripe.h>

void TLedStripe_base::Init(THwSpi * aspi, uint32_t aledcount, uint8_t * admabuf, unsigned abufsize)
{
  spi = aspi;
  ledcount = aledcount;
  dmabuf8 = (uint8_t *)admabuf;
  dmabuf32 = (uint32_t *)admabuf;
  bufsize = abufsize;

  spisendlen = 1;

  updatecount = 0;
  prev_updatecount = 0;
}

void TLedStripe_base::Update()
{
  ++updatecount;
  Run();
}

void TLedStripe_base::Run()
{
  if (!spi->finished)
  {
    spi->Run();
    return;
  }

  if (updatecount == prev_updatecount)
  {
    return;
  }

  // start the SPI transaction
  spi->StartTransfer(0, 0, 0, spisendlen, dmabuf8, nullptr);
  prev_updatecount = updatecount;
}

bool TLedStripe_base::UpdateFinished()
{
  return (spi->finished && (updatecount == prev_updatecount));
}

// --------------------------------

void TLedStripe_apa102::Init(THwSpi * aspi, uint32_t aledcount, uint8_t * admabuf, unsigned abufsize)
{
  super::Init(aspi, aledcount, admabuf, abufsize);

  spisendlen = ledcount * 4 + 2 * 4;  // one start and one stop frame
  if (spisendlen > bufsize)
  {
    spisendlen = bufsize;
    ledcount = ((bufsize - 8) >> 2);
  }

  dmabuf32[0] = 0;  // start word

  // init with dark
  uint32_t n;
  for (n = 0; n < ledcount; ++n)
  {
    dmabuf32[n + 1] = 0;
  }

  dmabuf32[ledcount + 2] = 0xFFFFFFFF;  // stop frame
}


void TLedStripe_ws2813::Init(THwSpi * aspi, uint32_t aledcount, uint8_t * admabuf, unsigned abufsize)
{
  super::Init(aspi, aledcount, admabuf, abufsize);

  if (spi->speed != 2400000)
  {
    spi->speed = 2400000;
    spi->Init(spi->devnum); // should keep DMA settings
  }

  spisendlen = ledcount * 9 + 2 * resetbytes;
  if (spisendlen > bufsize)
  {
    spisendlen = bufsize;
    ledcount = ((bufsize - 2 * resetbytes) / 9);
  }

  memset(dmabuf8, 0, spisendlen);
}

void TLedStripe_ws2813::SetRGB(uint16_t aled, uint8_t r, uint8_t g, uint8_t b)
{
  uint32_t grbcode = ((g << 16) | (r << 8) | b);
  uint32_t cbmask = (1 << 23);
  uint32_t bitidx = 0;
  uint8_t * pdata = &dmabuf8[resetbytes + 9 * aled];
  uint8_t  bv = 0;
  uint8_t  bm = 0x80;
  while (cbmask)
  {
    // the first bit is always 1
    bv |= bm;
    bm >>= 1;
    if (!bm)
    {
      *pdata++ = bv;
      bm = 0x80;
      bv = 0;
    }

    // the second bit is the real data
    if (grbcode & cbmask)
    {
      bv |= bm;
    }
    bm >>= 1;
    if (!bm)
    {
      *pdata++ = bv;
      bm = 0x80;
      bv = 0;
    }

    // the third bit is always zero
    bm >>= 1;
    if (!bm)
    {
      *pdata++ = bv;
      bm = 0x80;
      bv = 0;
    }

    cbmask >>= 1;
  }
}
