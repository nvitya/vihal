/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     self_flashing_rp.cpp
 *  brief:    RP2040 SPI Self Flashing
 *  date:     2022-06-01
 *  authors:  nvitya
*/

#include "platform.h"

#if defined(SPI_SELF_FLASHING) && SPI_SELF_FLASHING

#include "string.h"

#include "hwspi.h"
#include "spiflash.h"
#include "spiflash_updater.h"
#include "clockcnt.h"
#include "traces.h"

#include "app_header.h"

#define SELFFLASH_BUFSIZE  256

extern unsigned __app_image_end;
extern const vihal_esp_image_header_t   application_header;

extern "C" void _cold_entry();

uint8_t esp_img_checksum(uint32_t * dataptr, uint32_t datalen)
{
  uint8_t     result = 0xEF;
  uint32_t *  endp = dataptr + (datalen >> 2);
  while (dataptr < endp)
  {
    uint32_t d = *dataptr++;
    result ^= ((d ^ (d >> 8) ^ (d >> 16) ^ (d >> 24)) & 0xFF);
  }
  return result;
}

// do self flashing using the flash writer
bool spi_self_flashing(TSpiFlash * spiflash)
{
  uint8_t   localbuf[SELFFLASH_BUFSIZE] __attribute__((aligned(8)));

  // Using the flash writer to first compare the flash contents:
  TSpiFlashUpdater  flashupdater(spiflash, localbuf, sizeof(localbuf));

  // prepare the footer with checksum

  vihal_esp_image_header_t * phead = (vihal_esp_image_header_t *)&application_header;
  uint8_t *  pimgbody = (uint8_t *)(phead + 1);
  uint8_t *  pimgstart = (uint8_t *)phead;
  uint8_t *  pimgbodyend = (uint8_t *)&__app_image_end;  // must be 4-byte aligned !
  uint32_t   bodylen = pimgbodyend - pimgbody;

  // pad with zeroes:
  while ((uintptr_t(pimgbodyend) & 15) < 12)
  {
    *(uint32_t *)pimgbodyend = 0;
    pimgbodyend += 4;
  }
  *(uint32_t *)pimgbodyend = (esp_img_checksum((uint32_t *)pimgbody, bodylen) << 24);
  pimgbodyend += 4;

  uint32_t imglen = pimgbodyend - pimgstart;

  TRACE("Self-Flashing:\r\n");
  TRACE("  mem = %08X -> flash = %08X, len = %u ...\r\n", unsigned(pimgstart), 0, imglen);

  unsigned  t0, t1;
  t0 = CLOCKCNT;

  if (!flashupdater.UpdateFlash(0, pimgstart, imglen))
  {
    TRACE("  ERROR!\r\n");
    return false;
  }

  t1 = CLOCKCNT;
  unsigned clocksperus = SystemCoreClock / 1000000;

  unsigned ssize_k = flashupdater.sectorsize >> 10;

  TRACE("  %u * %uk updated, %u * %uk matched, took %u us\r\n",
      flashupdater.writecnt, ssize_k, flashupdater.samecnt, ssize_k, (t1 - t0) / clocksperus);

	return true;
}

#endif  // ifdef SPI_SELF_FLASHING
