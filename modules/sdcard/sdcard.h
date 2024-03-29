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
// file:     sdcard.h
// brief:    Base (abstract) SDCARD class
// created:  2022-22-12
// authors:  nvitya

#ifndef SRC_SDCARD_H_
#define SRC_SDCARD_H_

#include "platform.h"
#include "hwerrors.h"

#define SDCARD_TRSTATE_START_READ     1
#define SDCARD_TRSTATE_START_WRITE   11

class TSdCard
{
public:
  uint32_t      clockspeed  = 20000000; //  20 MHz by default
  uint32_t      forced_clockspeed = 0;  //  0 = no forced high-speed
  uint32_t      initial_speed = 400000; // 400 kHz for the first initialization

  bool          initialized = false;
  bool          card_initialized = false;
  bool          card_present = false;
  bool          card_v2 = false;
  bool          high_capacity = false;

  uint32_t      csd_max_speed = 0;
  uint32_t      card_megabytes = 0;

  int           initstate = 0;

  bool          completed = true;  // transaction status
  int           errorcode = 0;

  uint32_t      rca = 0;      // relative card address, required for point-to-point communication
  uint32_t      reg_ocr = 0;  // Card Operating Condition + status register
  uint8_t       csd_ver = 0;
  uint8_t       reg_cid[16]  __attribute__((aligned(4)));  // Card Identification Register
  uint8_t       reg_csd[16]  __attribute__((aligned(4)));  // Card Specific Data Register
  uint8_t       reg_scr[8]   __attribute__((aligned(4)));  // SD Configuration Register

  virtual       ~TSdCard() { }
  virtual void  Run() { }

  bool          StartReadBlocks(uint32_t astartblock, void * adataptr, uint32_t ablockcount);
  bool          StartWriteBlocks(uint32_t astartblock, void * adataptr, uint32_t ablockcount);
  void          WaitForComplete();

protected:
  int           trstate = 0; // transfer state machine control, 1 = start read,

  bool          iswrite = false;
  uint32_t      startblock = 0;
  uint32_t      curblock = 0;
  uint32_t      blockcount = 0;
  int           remainingblocks = 0;
  uint8_t *     dataptr = nullptr;

  uint32_t      GetRegBits(void * adata, uint32_t startpos, uint8_t bitlen);
  void          ProcessCsd();

};

#endif /* SRC_SDCARD_H_ */
