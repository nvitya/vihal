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
// file:     hwdac_stm32.h
// brief:    STM32 DAC Implementation
// created:  2022-11-20
// authors:  nvitya

#ifndef HWDAC_STM32_H_
#define HWDAC_STM32_H_

#include "platform.h"

#if defined(DAC) || defined(DAC1)

#define HWDAC_PRE_ONLY
#include "hwdac.h"

class THwDacChannel_stm32 : public THwDacChannel_pre
{
protected:

  unsigned  chidx = 0; // zero based index
  uint32_t  chshift = 0;

  TIM_TypeDef *        timer = nullptr;  // Uses TIM6

public:

  DAC_TypeDef *        regs = nullptr;
  volatile uint32_t *  outreg = nullptr;

  bool                 Init(int adacnum, int achnum);

  inline void          SetTo(uint16_t avalue)  { *outreg = avalue; }
  inline uint16_t      Value()                 { return *outreg;   }

  void                 SetFrequency(uint32_t afrequency);

  void                 RepeatPattern(uint16_t * asrc, uint32_t alen);
  void                 StopPattern();

};

#define HWDACCHANNEL_IMPL THwDacChannel_stm32

#endif

#endif // ndef HWDAC_STM32_H_

