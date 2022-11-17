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
// file:     sdcard.cpp
// brief:    Base (abstract) SDCARD class
// created:  2022-22-12
// authors:  nvitya

#include <sdcard.h>
#include "hwerrors.h"

bool TSdCard::StartReadBlocks(uint32_t astartblock, void * adataptr, uint32_t ablockcount)
{
  if (!initialized)
  {
    errorcode = HWERR_NOTINIT;
    completed = true;
    return false;
  }

  if (!completed)
  {
    errorcode = HWERR_BUSY;  // this might be overwriten later
    return false;
  }

  dataptr = (uint8_t *)adataptr;
  blockcount = ablockcount;
  remainingblocks = ablockcount;
  startblock = astartblock;
  curblock = astartblock;

  errorcode = 0;
  completed = false;

  trstate = SDCARD_TRSTATE_START_READ; // read blocks

  Run();

  return (errorcode == 0);
}

bool TSdCard::StartWriteBlocks(uint32_t astartblock, void * adataptr, uint32_t ablockcount)
{
  if (!initialized)
  {
    errorcode = HWERR_NOTINIT;
    completed = true;
    return false;
  }

  if (!completed)
  {
    errorcode = HWERR_BUSY;  // this might be overwriten later
    return false;
  }

  dataptr = (uint8_t *)adataptr;
  blockcount = ablockcount;
  remainingblocks = ablockcount;
  startblock = astartblock;
  curblock = astartblock;

  errorcode = 0;
  completed = false;

  trstate = SDCARD_TRSTATE_START_WRITE; // write blocks

  Run();

  return (errorcode == 0);
}


void TSdCard::WaitForComplete()
{
  while (!completed)
  {
    Run();
  }
}

uint32_t TSdCard::GetRegBits(void * adata, uint32_t startpos, uint8_t bitlen)
{
  uint32_t * d32p = (uint32_t *)adata;
  uint32_t widx = (startpos >> 5);
  d32p += widx;
  uint8_t bitidx = (startpos & 31);

  uint32_t result = (*d32p >> bitidx);

  if (bitidx + bitlen > 32)
  {
    ++d32p;
    result |= (*d32p << (32 - bitidx));
  }

  if (bitlen < 32)
  {
    result &= ((1 << bitlen) - 1);
  }

  return result;
}

//! SD/MMC transfer rate unit codes (10K) list
static const uint32_t sd_trans_units[8] = { 10, 100, 1000, 10000, 0, 0, 0, 0 };
//! SD transfer multiplier factor codes (1/10) list
static const uint32_t sd_trans_values[16] = { 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };

void TSdCard::ProcessCsd()
{
  /* CSD bits:

       126(2): Version
       122(4): Spec. version
        96(8): Tran speed
  V1:
        47(3): V1 C size mult.
       62(12): V1 C size
        80(4): V1 Read BL Len
  v2:
       48(22): V2 C. Size
  */
    csd_ver = GetRegBits(&reg_csd[0], 126, 2);

    uint32_t transunit = sd_trans_units[GetRegBits(&reg_csd[0], 96, 3)];
    uint32_t transval  = sd_trans_values[GetRegBits(&reg_csd[0], 99, 4)];
    csd_max_speed = 1000 * transunit * transval;

    // decode card size

    /*
     * Get card capacity.
     * ----------------------------------------------------
     * For normal SD/MMC card:
     * memory capacity = BLOCKNR * BLOCK_LEN
     * Where
     * BLOCKNR = (C_SIZE+1) * MULT
     * MULT = 2 ^ (C_SIZE_MULT+2)       (C_SIZE_MULT < 8)
     * BLOCK_LEN = 2 ^ READ_BL_LEN      (READ_BL_LEN < 12)
     * ----------------------------------------------------
     * For high capacity SD card:
     * memory capacity = (C_SIZE+1) * 512K byte
     */

    if (csd_ver >= 1)
    {
      card_megabytes = ((GetRegBits(&reg_csd[0], 48, 22) + 1) >> 1);
      high_capacity = true;
    }
    else
    {
      uint32_t c_size = GetRegBits(&reg_csd[0], 62, 12);
      uint32_t c_size_mult = GetRegBits(&reg_csd[0], 47, 3);
      uint32_t block_len = (1 << GetRegBits(&reg_csd[0], 80, 4));

      uint32_t blocknr = (c_size + 1) * (1 << (c_size_mult + 2));

      card_megabytes = ((blocknr * block_len) >> 20);
      high_capacity = false;
    }

    //TRACE("SD card max speed = %u MHz, size = %u MBytes\r\n", csd_max_speed / 1000000, card_megabytes);
}
