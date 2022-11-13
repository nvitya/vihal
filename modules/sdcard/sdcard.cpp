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
