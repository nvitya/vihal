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

extern "C" void cold_entry();
extern unsigned __app_image_end;

#if defined(MCUSF_20)

extern unsigned __bl_stage2_src;

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

#elif defined(MCUSF_23)

extern uint32_t __stack;

const uint32_t rp235x_boot_blocks[] =
{
    0xffffded3,  // block begin magic header

    // IMAGE TYPE
    (0
      | (0x42   <<  0)  // TYPE(8): 0x42 = IMAGE_TYPE item
      | (1      <<  8)  // SIZE(8): 1 = just one word
      | (0x1021 << 16)  // FLAGS(16): 0x1021 = ARM Executable, Secure , 0x1011 = ARM Executable, non-secure
    ),

    // LOAD_MAP item
    (0
      | (0x06  <<  0)  // TYPE(8) LOAD_MAP item
      | (4     <<  8)  // SIZE(16): block size in words
      | (0x81u << 24)  // ITEMS(8): 1x items + Absolute SPI Flash Addressing,
    ),
    0x10000000 + SELF_FLASHING_SPI_ADDR,               // Storage start address
    SELF_FLASHING_RAM_ADDR,  // Runtime start address
    ((3 + unsigned(&__app_image_end))  & 0xFFFFFFFC),  // Runtime end address

    // ENTRY_POINT item
    (0
      | (0x44 <<  0)  // TYPE(8): 0x44 = ENTRY_POINT item
      | (3    <<  8)  // SIZE(8): block size in words
      | (0    << 16)  // PAD(16): padding
    ),
    uint32_t(cold_entry),    // Initial PC
    uint32_t(&__stack),      // Initial Stack

    // ITEM_LAST
    (0
      | (0xFF        <<  0)  // TYPE(8): ENTRY_POINT item
      | ((4 + 3 + 1) <<  8)  // SIZE(16): sum of the sizes of the previous items
      | (0           << 24)  // PAD(8): just padding
    ),

    0x00000000,  // block loop link to self = end of the block loop
    0xab123579,  // block end magic footer

    // just padding
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF
};

// do self flashing using the flash writer
bool spi_self_flashing(TSpiFlash * spiflash)
{
  uint8_t   localbuf[SELFFLASH_BUFSIZE] __attribute__((aligned(8)));

  // Using the flash writer to first compare the flash contents:
  TSpiFlashUpdater  flashupdater(spiflash, localbuf, sizeof(localbuf));
  TAppHeader        apph;

  // Application header is not strictly necessary

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

  // write the boot-blocks into the first SPI Flash sector

  TRACE("RP235x Boot Blocks...\r\n");
  if (!flashupdater.UpdateFlash(0x000000, (uint8_t *)(&rp235x_boot_blocks[0]), sizeof(rp235x_boot_blocks)))
  {
    TRACE("  ERROR!\r\n");
    return false;
  }
  if (flashupdater.writecnt)  { TRACE("  updated\r\n"); }  else { TRACE("  matched\r\n"); }

  return true;
}


#else
  #error "SPI Self-Flashing: Unhandled RP MCU family"
#endif

#endif  // ifdef SPI_SELF_FLASHING
