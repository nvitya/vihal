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
 *  file:     self_flashing_vrv100.cpp
 *  brief:    VRV100 (FPGA) SPI Self Flashing
 *  date:     2021-10-11
 *  authors:  nvitya
*/

#include "platform.h"

#if defined(SPI_SELF_FLASHING) && SPI_SELF_FLASHING

#include "hwspi.h"
#include "spiflash.h"
#include "spiflash_updater.h"
#include "clockcnt.h"
#include "traces.h"

#define SELFFLASH_BUFSIZE  256

extern const bootblock_header_t application_header;

extern unsigned __app_image_end;

// do self flashing using the flash writer
bool spi_self_flashing(TSpiFlash * spiflash)
{
  uint8_t   localbuf[SELFFLASH_BUFSIZE] __attribute__((aligned(8)));
	unsigned  len = unsigned(&__app_image_end) - unsigned(&application_header);

  bootrom_info_t * bootrom_info = (bootrom_info_t *)BOOTROM_INFO_ADDRESS;

  if (bootrom_info->signature != BOOTROM_INFO_SIGNATURE)
  {
    TRACE("BootRom signature error: %08X = %08X\r\n", BOOTROM_INFO_ADDRESS, bootrom_info->signature);
    return false;
  }

	bootblock_header_t * phead = (bootblock_header_t *)&application_header;
	phead->compid = bootrom_info->compid;

	// Using the flash writer to first compare the flash contents:
	TSpiFlashUpdater  flashupdater(spiflash, localbuf, sizeof(localbuf));

  TRACE("Self-Flashing:\r\n");
  TRACE("  mem = %08X -> flash = %08X, len = %u ...\r\n", unsigned(&application_header), bootrom_info->bootblock_staddr, len);

  len = ((len + 7) & 0xFFFFFFF8); // length must be also dividible with 8 !

	unsigned  t0, t1;
	t0 = CLOCKCNT;

	if (!flashupdater.UpdateFlash(bootrom_info->bootblock_staddr, (uint8_t *)&application_header, len))
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

#endif
