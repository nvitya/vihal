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
/* file:     spiflash_updater.cpp
 * brief:    Helper object to write memory contents into the SPI Flash memory.
 *           it compares the flash content with the memory so it tries to avoid unnecessary write and erase.
 *           It can operate with smaller temporary buffers, which can be allocated on the stack
 * created:  2021-10-26
 * authors:  nvitya
 */

#include "platform.h"
#include "spiflash_updater.h"

TSpiFlashUpdater::TSpiFlashUpdater(TSpiFlash * aspiflash, uint8_t * abufptr, unsigned abuflen)
{
  spiflash = aspiflash;
  bufptr = abufptr;
  buflen = abuflen;

  if (spiflash && !spiflash->has4kerase)
  {
    sectorsize = 65536;
  }
}

bool TSpiFlashUpdater::FlashSectorDiffer(unsigned asectoraddr, unsigned sectorlen, uint8_t * memptr)  // address and length alignment of 4 bytes are required
{
  erased = true;

  bool      differ = false;
  unsigned  remaining = sectorlen;
  unsigned  faddr = asectoraddr;

  while (remaining > 0)
  {
    unsigned chunksize = remaining;
    if (chunksize > buflen)  chunksize = buflen;

    spiflash->StartReadMem(faddr, bufptr, chunksize);
    spiflash->WaitForComplete();

    // compare memory

    uint32_t * mdp32  = (uint32_t *)(memptr);
    uint32_t * fdp32  = (uint32_t *)&(bufptr[0]);
    uint32_t * endptr = (uint32_t *)&(bufptr[chunksize]);

    while (fdp32 < endptr)
    {
      if (*fdp32 != 0xFFFFFFFF)
      {
        erased = false;
      }

      if (*fdp32 != *mdp32)
      {
        differ = true; // do not break for complete the erased check!
      }

      ++fdp32;
      ++mdp32;
    }

    faddr += chunksize;
    memptr += chunksize;
    remaining -= chunksize;
  }

  return differ;
}

bool TSpiFlashUpdater::UpdateFlash(unsigned aflashaddr, uint8_t * asrc, unsigned alen)  // the flash address must begin on sector boundary !
{
  if (!spiflash->initialized)
  {
    return false;
  }

  erasecnt = 0;
  writecnt = 0;
  samecnt = 0;

  unsigned faddr = aflashaddr;
  uint8_t * srcptr = asrc;
  unsigned remaining = alen;

  while (remaining > 0)
  {
    unsigned chunksize = remaining;
    if (chunksize > sectorsize)  chunksize = sectorsize;

    if (FlashSectorDiffer(faddr, chunksize, srcptr))
    {
      if (!erased)
      {
        spiflash->StartEraseMem(faddr, chunksize);
        spiflash->WaitForComplete();
        ++erasecnt;
      }
      spiflash->StartWriteMem(faddr, srcptr, chunksize);
      spiflash->WaitForComplete();
      ++writecnt;
    }
    else
    {
      ++samecnt;
    }

    faddr += chunksize;
    srcptr += chunksize;
    remaining -= chunksize;
  }

  return true;
}
