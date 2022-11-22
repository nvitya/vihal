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
// file:     hwdac_stm32.cpp
// brief:    STM32 DAC Implementation
// created:  2022-11-20
// authors:  nvitya
// notes:    uses TIM6 for timing

#include <hwdac.h>

#if defined(DAC) || defined(DAC1)

#if defined(DAC_MCR_MODE1)
  #define DAC_V2 1
  #define DAC_V1 0
#else
  #define DAC_V2 0
  #define DAC_V1 1
#endif

#include "stm32_utils.h"

#if !defined(RCC_APB1ENR_DAC1EN) && defined(RCC_APB1ENR_DACEN)
  #define RCC_APB1ENR_DAC1EN  RCC_APB1ENR_DACEN
#endif

bool THwDacChannel_stm32::Init(int adacnum, int achnum)
{
  initialized = false;

  devnum = -1;
  regs = nullptr;

  if ((achnum < 1) || (achnum > 2))
  {
    return false;
  }

  if (1 == adacnum)
  {
    #if defined(DAC1)
        regs = DAC1;
        #if defined(RCC_APB1ENR_DAC1EN)
            RCC->APB1ENR |= RCC_APB1ENR_DAC1EN;
        #elif defined(RCC_APB1LENR_DAC12EN)
            RCC->APB1LENR |= RCC_APB1LENR_DAC12EN;
        #else
            RCC->AHB2ENR |= RCC_AHB2ENR_DAC1EN;
        #endif
    #else
        regs = DAC;
        RCC->AHB2ENR |= RCC_AHB2ENR_DACEN;
    #endif
  }
#if defined(DAC2)
  else if (2 == adacnum)
  {
    regs = DAC2;
    RCC->AHB2ENR |= RCC_AHB2ENR_DAC2EN;
  }
#endif
  else
  {
    return false;
  }

  timer = TIM6; // uses TIM6!
  #ifdef RCC_APB1ENR1_TIM6EN
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
  #elif defined(RCC_APB1LENR_TIM6EN)
    RCC->APB1LENR |= RCC_APB1LENR_TIM6EN;
  #else
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
  #endif

  SetFrequency(frequency);

  devnum = adacnum;
  chnum = achnum;
  chidx = (achnum - 1);
  chshift = (chidx << 4);  // *16

  if (1 == chidx)
  {
    outreg = &regs->DHR12R2;
  }
  else
  {
    outreg = &regs->DHR12R1;
  }

  uint32_t crreg = regs->CR;

  #if DAC_V2  // newer DAC


    uint32_t dactrigger;
    #if defined(MCUSF_H7)

      dactrigger = 5;  // 5 = TIM6

    #else

      // G4

      uint32_t mcrreg = regs->MCR;
      mcrreg &= ~(0x0307 << chshift);
      mcrreg &= ~(3 << 14);
      mcrreg |=  (2 << 14);  // selet high frequency interface mode
      regs->MCR = mcrreg;

      dactrigger = 7;
    #endif

    uint32_t cr16 = (0
      | (0  << 14)  // CEN: calibration enable
      | (0  << 13)  // DMAUDRIE:
      | (1  << 12)  // DMAEN:
      | (0  <<  8)  // MAMP(4):
      | (0  <<  6)  // WAVE(2):
      | (dactrigger  <<  2)  // TSEL(4): trigger selection, 7 = TIM6
      | (1  <<  1)  // TEN: trigger enable
      | (1  <<  0)  // EN: channnel enable
    );

  #else

    uint32_t cr16 = (0
      | (0  << 13)  // DMAUDRIE:
      | (1  << 12)  // DMAEN:
      | (0  <<  8)  // MAMP(4):
      | (0  <<  6)  // WAVE(2):
      | (0  <<  3)  // TSEL(3): trigger selection, 0 = TIM6
      | (1  <<  2)  // TEN: trigger enable
      | (0  <<  1)  // BOFF: 1 = output buffer disable
      | (1  <<  0)  // EN: channnel enable
    );

  #endif


  crreg &= ~(0xFFFF << chshift);
  crreg |= (cr16 << chshift);
  regs->CR = crreg;

  dmach.Prepare(true, (void *)outreg, 0);
  dmach.per_flow_controller = 0;
  dmaxfer.bytewidth = 2;
  dmaxfer.flags = DMATR_CIRCULAR;
  #if DAC_V2 && !defined(MCUSF_H7)
    // PER32 (2->4 byte conversion) required for G4 !
    dmaxfer.flags |= DMATR_PER32;
  #endif

  initialized = true;
  return true;
}

void THwDacChannel_stm32::SetFrequency(uint32_t afrequency)
{
  frequency = afrequency;

  uint32_t timer_base_speed = stm32_bus_speed(1);
  if ((timer_base_speed < SystemCoreClock) && (timer_base_speed < 150000000))
  {
    timer_base_speed = (timer_base_speed << 1);  // the timer clock speed is twice of the APB speed
  }

  timer->ARR = (timer_base_speed / frequency) - 1;
  timer->CR2 = (2 << 4);  // generate trigger at every update
  timer->CR1 = 1; // enable timer
}

void THwDacChannel_stm32::RepeatPattern(uint16_t * asrc, uint32_t alen)
{
  if (!dmach.initialized)
  {
    return;
  }

  dmaxfer.srcaddr = asrc;
  dmaxfer.count = alen;

#if 1

  //crreg &= ~((1 << 0) << chshift); // disable
  //regs->CR = crreg;

  dmach.StartTransfer(&dmaxfer);

  //uint32_t crreg = regs->CR;
  //crreg |= ((1 << 12) << chshift); // enable DMA
  //regs->CR = crreg;


#else

  uint32_t crreg = regs->CR;
  crreg &= ~((1 << 0) << chshift); // disable
  regs->CR = crreg;
  crreg |= ((1 << 12) << chshift); // enable DMA
  regs->CR = crreg;

  dmach.StartTransfer(&dmaxfer);

  crreg |= ((1 << 0) << chshift); // enable DAC
  regs->CR = crreg;
#endif
}

void THwDacChannel_stm32::StopPattern()
{
  uint32_t crreg = regs->CR;
  crreg &= ~( ((1 << 0) | (1 << 12)) << chshift ) ; // disable + disable DMA
  regs->CR = crreg;
  crreg |= ((1 << 0) << chshift); // enable DAC
  regs->CR = crreg;
}

#endif
