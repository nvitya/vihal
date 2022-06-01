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

#ifdef SPI_SELF_FLASHING

#include "string.h"

#include "platform.h"

#include "hwspi.h"
#include "spiflash.h"
#include "spiflash_updater.h"
#include "clockcnt.h"
#include "traces.h"

#include "app_header.h"

#define SELFFLASH_BUFSIZE  256

extern unsigned __bl_stage2_src;
extern unsigned __app_image_end;

extern "C" void cold_entry();

// crc32_small is implemented in the "rp_st2_crc.S", taken from the pico-bootrom repostitory:
extern "C" uint32_t crc32_small(const uint8_t * buf, unsigned int len, uint32_t seed);

// do self flashing using the flash writer
bool spi_self_flashing(TSpiFlash * spiflash)
{
  uint8_t   localbuf[SELFFLASH_BUFSIZE] __attribute__((aligned(8)));

  // Using the flash writer to first compare the flash contents:
  TSpiFlashUpdater  flashupdater(spiflash, localbuf, sizeof(localbuf));
  TAppHeader        apph;

  apph.signature = APP_HEADER_SIGNATURE;
  apph.addr_load = SELF_FLASHING_RAM_ADDR;
  apph.length = ((3 + unsigned(&__app_image_end) - unsigned(SELF_FLASHING_RAM_ADDR)) & 0x0FFFFFFC);
  apph.addr_entry = unsigned(cold_entry);

  TRACE("Self-Flashing Main Code:\r\n");
  TRACE("  mem = %08X -> flash = %08X, len = %u ...\r\n", unsigned(SELF_FLASHING_RAM_ADDR), SELF_FLASHING_SPI_ADDR, apph.length);

  unsigned  t0, t1;
  t0 = CLOCKCNT;
  if (!flashupdater.UpdateFlash(SELF_FLASHING_SPI_ADDR, (uint8_t *)(SELF_FLASHING_RAM_ADDR), apph.length))
  {
    TRACE("  ERROR!\r\n");
    return false;
  }
  t1 = CLOCKCNT;
  unsigned clocksperus = SystemCoreClock / 1000000;
  unsigned ssize_k = flashupdater.sectorsize >> 10;
  TRACE("  %u * %uk updated, %u * %uk matched, took %u us\r\n",
      flashupdater.writecnt, ssize_k, flashupdater.samecnt, ssize_k, (t1 - t0) / clocksperus);

  uint8_t *   st2_source = (uint8_t *)&__bl_stage2_src;
  uint32_t *  st2_crc = (uint32_t *)(st2_source + 252);

  //TRACE("Sencond stage addr=%08X, len=%u\r\n", , &__bl_stage2_len);

  // now we are changing the image inserting the CRC at the last position !
  *st2_crc = crc32_small(st2_source, 252, 0xFFFFFFFF);

  // to debug it can be copied to the original place
  //memcpy((void *)0x20041000, st2_source, 256);

  TRACE("Stage2 Loader...\r\n");
  if (!flashupdater.UpdateFlash(0x000000, st2_source, 256))
  {
    TRACE("  ERROR!\r\n");
    return false;
  }
  if (flashupdater.writecnt)  { TRACE("  updated\r\n"); }  else { TRACE("  matched\r\n"); }

	return true;
}

#endif  // ifdef SPI_SELF_FLASHING
