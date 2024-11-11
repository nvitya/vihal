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
// file:     hwdac.h
// brief:    DAC vendor-independent definitions
// created:  2022-11-20
// authors:  nvitya

#ifndef HWDAC_PRE_H_
#define HWDAC_PRE_H_

#include "hwdma.h"

class THwDacChannel_pre
{
public:
  bool      initialized = false;
  int       devnum = -1;
  int       chnum = -1;
  unsigned  frequency = 1000000;

  THwDmaChannel   dmach;
  THwDmaTransfer  dmaxfer;
};


#endif // ndef HWDAC_PRE_H_

#ifndef HWDAC_PRE_ONLY

//-----------------------------------------------------------------------------

#ifndef HWDAC_H_
#define HWDAC_H_

#include "mcu_impl.h"

#ifndef HWDACCHANNEL_IMPL

class THwDacChannel_noimpl : public THwDacChannel_pre
{
public: // mandatory

  bool         Init(int adacnum, int achnum) { return false; }
  void         SetFrequency(uint32_t afrequency) { }
  inline void      SetTo(uint16_t avalue)  { }
  inline uint16_t  Value()                 { return 0; }
  void         RepeatPattern(uint16_t * asrc, uint32_t alen) { }
  void         StopPattern() { }
};

#define HWDACCHANNEL_IMPL   THwDacChannel_noimpl

#endif // ndef HWDACCHANNEL_IMPL

class THwDacChannel : public HWDACCHANNEL_IMPL
{
public:
};

#endif // ndef HWDAC_H_

#else
  #undef HWDAC_PRE_ONLY
#endif

