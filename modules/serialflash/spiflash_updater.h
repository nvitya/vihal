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
/* file:     spiflash_updater.h
 * brief:    Helper object to write memory contents into the SPI Flash memory.
 *           it compares the flash content with the memory so it tries to avoid unnecessary write and erase.
 *           It can operate with smaller temporary buffers, which can be allocated on the stack
 * created:  2021-10-26
 * authors:  nvitya
 */

#ifndef SPIFLASH_UPDATER_H_
#define SPIFLASH_UPDATER_H_

#include "hwspi.h"
#include "spiflash.h"

class TSpiFlashUpdater
{
public:
  TSpiFlash *  spiflash;
  uint8_t *    bufptr;
  unsigned     buflen;

  unsigned     sectorsize = 4096; // by default, it will be 64k if the flash does not support it

  bool         erased = true;

  unsigned     samecnt = 0;
  unsigned     erasecnt = 0;
  unsigned     writecnt = 0;

  TSpiFlashUpdater(TSpiFlash * aspiflash, uint8_t * abufptr, unsigned abuflen);

  bool FlashSectorDiffer(unsigned asectoraddr, unsigned sectorlen, uint8_t * memptr);  // address and length alignment of 4 bytes are required
  bool UpdateFlash(unsigned aflashaddr, uint8_t * asrc, unsigned alen);  // the flash address must begin on sector boundary !

};

#endif /* SPIFLASH_UPDATER_H_ */
