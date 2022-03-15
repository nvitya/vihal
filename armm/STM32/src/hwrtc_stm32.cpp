/*
 * hwrtc_stm32.cpp
 *
 *  Created on: 12.03.2022
 *      Author: Bergi
 */

#ifndef HWRTC_STM32_CPP_
#define HWRTC_STM32_CPP_

#include "hwrtc.h"

#if defined(MCUSF_WB)

bool THwRtc_stm32::init()
{
  regs = RTC;

  // enable backup domain access, after reset this access is disabled
  //to avoid wrong writes to backup domain during startup
  PWR->CR1 |= PWR_CR1_DBP;
  // wait for backup domain access
  while(!(PWR->CR1 & PWR_CR1_DBP))
  {

  }

  uint32_t pheriphclock;

  uint32_t tmp = RCC->BDCR & RCC_BDCR_RTCSEL;
  if(tmp == 1 << RCC_BDCR_RTCSEL_Pos)
  {
    // LSE is clock source
    pheriphclock = 32768;
  }
  else if(tmp == 2 << RCC_BDCR_RTCSEL_Pos)
  {
    // LSI is clock source
    pheriphclock = 32000;
  }
  else
  {
    // error clock source not supported
    return false;
  }

  // enable clocks for RTC
  RCC->BDCR |= RCC_BDCR_RTCEN;
  RCC->APB1ENR1 |= RCC_APB1ENR1_RTCAPBEN;

  unlockRegs();

  // switch to init mode
  if(!(regs->ISR & RTC_ISR_INITF))
  {
    regs->ISR = RTC_ISR_INIT;
    while(!(regs->ISR & RTC_ISR_INITF))
    {

    }
  }

  // disable wake up timer
  if(~(regs->ISR & RTC_ISR_WUTWF))
  {
    regs->CR &= ~RTC_CR_WUTE;
    while(!(regs->ISR & RTC_ISR_WUTWF))
    {

    }
  }

  // set wake up timer divider to 16
  // disable output
  // switch 24h format
  tmp = regs->CR;
  tmp &= ~(RTC_CR_WUCKSEL | RTC_CR_FMT | RTC_CR_OSEL | RTC_CR_POL | RTC_CR_BYPSHAD);
  tmp &= 0 << RTC_CR_WUCKSEL_Pos;
  regs->CR = tmp;

  // set async divider to same value as wake up timer (div 16)
  // set sync divider to generate 1Hz for RTC
  syncDiv = pheriphclock/16;
  wakeUpMaxMsec = 0x0000FFFF * 1000UL / (uint32_t)syncDiv;
  regs->PRER = (15 << RTC_PRER_PREDIV_A_Pos) | ((pheriphclock/16 - 1) << RTC_PRER_PREDIV_S_Pos);

  // exit init mode
  regs->ISR &= ~RTC_ISR_INIT;

  lockRegs();

  return true;
}

bool THwRtc_stm32::setTime(time_t atime, time_t &rtime)
{
  uint32_t ssr = (uint32_t)syncDiv - 1UL - (uint32_t)atime.msec * (uint32_t)syncDiv / 1000UL;

  uint32_t tr = ((atime.sec%10) << RTC_TR_SU_Pos) | ((atime.sec/10) << RTC_TR_ST_Pos);
  tr |= ((atime.min%10) << RTC_TR_MNU_Pos) | ((atime.min/10) << RTC_TR_MNT_Pos);
  tr |= ((atime.hour%10) << RTC_TR_HU_Pos) | ((atime.hour/10) << RTC_TR_HT_Pos);

  uint32_t dr = ((atime.day%10) << RTC_DR_DU_Pos) | ((atime.day/10) << RTC_DR_DT_Pos);
  dr |= ((atime.month%10) << RTC_DR_MU_Pos) | ((atime.month/10) << RTC_DR_MT_Pos);
  dr |= ((atime.year%10) << RTC_DR_YU_Pos) | ((atime.year/10) << RTC_DR_YT_Pos);

  unlockRegs();

  // switch to init mode
  if(!(regs->ISR & RTC_ISR_INITF))
  {
    regs->ISR = RTC_ISR_INIT;
    while(!(regs->ISR & RTC_ISR_INITF))
    {

    }
  }

  uint32_t rSsr = regs->SSR;
  uint32_t rTr = regs->TR;
  uint32_t rDr = regs->DR;

  regs->TR = tr;
  regs->DR = dr;

  // exit init mode
  regs->ISR &= ~(RTC_ISR_INIT | RTC_ISR_RSF);
  while(regs->ISR & RTC_ISR_INITF)
  {

  }
  regs->SHIFTR = (1U << 31) | ssr;

  lockRegs();

  lastSsr = ssr;
  lastTr = tr;
  lastDr = dr;

  convSsr2Ms(rSsr, rtime.msec);
  convTr2Int(rTr, rtime.sec, rtime.min, rtime.hour);
  convDr2Int(rDr, rtime.day, rtime.month, rtime.year);

  return true;
}

bool THwRtc_stm32::getTime(time_t &atime)
{
  uint32_t ssr, tr, dr;
  readCalReg(ssr, tr,  dr);
  convSsr2Ms(ssr, atime.msec);
  convTr2Int(tr, atime.sec, atime.min, atime.hour);
  convDr2Int(dr, atime.day, atime.month, atime.year);

  return true;
}

bool THwRtc_stm32::setWakeupTimer(uint16_t aMSecTime)
{
  if(aMSecTime > wakeUpMaxMsec || 1 > aMSecTime)
  {
    return false;
  }

  unlockRegs();

  // disable wake up timer
  regs->CR &= ~RTC_CR_WUTE;
  while(!(regs->ISR & RTC_ISR_WUTWF))
  {

  }


  regs->WUTR = (uint32_t)aMSecTime * (uint32_t)syncDiv / 1000UL - 1UL;

  regs->CR |= RTC_CR_WUTE;

  lockRegs();

  return true;
}

bool THwRtc_stm32::stopWakeupTimer()
{
  unlockRegs();

  // disable wake up timer
  regs->CR &= ~RTC_CR_WUTE;
  while(!(regs->ISR & RTC_ISR_WUTWF))
  {

  }

  lockRegs();

  return true;
}

bool THwRtc_stm32::enableWakeupIRQ()
{
  EXTI->RTSR1 |= EXTI_RTSR1_RT19;   // enable rising trigger
  EXTI->IMR1 |= EXTI_IMR1_IM19;     // enable exti 19 (RTC wake up)

  unlockRegs();

  regs->ISR &= ~RTC_ISR_WUTF;      // clear update flag
  EXTI->PR1 = EXTI_PR1_PIF19;     // clear exti flag
  NVIC_ClearPendingIRQ(RTC_WKUP_IRQn);  // clear pendig interrupt
  NVIC_EnableIRQ(RTC_WKUP_IRQn);

  regs->CR |= RTC_CR_WUTIE;        // enable interrupt

  lockRegs();

  return true;
}

bool THwRtc_stm32::disableWakeupIRQ()
{
  unlockRegs();

  regs->CR &= ~RTC_CR_WUTIE;        // disable interrupt

  regs->ISR &= ~RTC_ISR_WUTF;      // clear update flag
  EXTI->PR1 = EXTI_PR1_PIF19;     // clear exti flag
  NVIC_ClearPendingIRQ(RTC_WKUP_IRQn);  // clear pendig interrupt
  NVIC_DisableIRQ(RTC_WKUP_IRQn);

  lockRegs();

  return true;
}

void THwRtc_stm32::clearWakeupIRQ()
{
  regs->ISR &= ~RTC_ISR_WUTF;
  EXTI->PR1 = EXTI_PR1_PIF19;
  NVIC_ClearPendingIRQ(RTC_WKUP_IRQn);
}

inline void THwRtc_stm32::lockRegs()
{
  regs->WPR = 0xFFU;
}

inline void THwRtc_stm32::unlockRegs()
{
  regs->WPR = 0xCAU;
  regs->WPR = 0x53U;
}

inline void THwRtc_stm32::readCalReg(uint32_t &aSsr, uint32_t &aTr, uint32_t &aDr)
{
  if(regs->ISR & RTC_ISR_RSF)
  {
    // shadow register update was done
    aSsr = regs->SSR;
    aTr = regs->TR;
    aDr = regs->DR;

    regs->ISR &= ~RTC_ISR_RSF;

    lastSsr = aSsr;
    lastTr = aTr;
    lastDr = aDr;

  }
  else
  {
    // no shadow register update occurred since last read
    aSsr = lastSsr;
    aTr = lastTr;
    aDr = lastDr;
  }
}

inline void THwRtc_stm32::convSsr2Ms(uint32_t &aSsr, uint16_t &aMs)
{
  if(aSsr >= syncDiv)
  {
    aSsr = aSsr - syncDiv;
  }
  aMs = 999UL - (1000UL * aSsr)/(uint32_t)syncDiv;
}

void THwRtc_stm32::convTr2Int(uint32_t &aTr, uint8_t &aSec, uint8_t &aMin, uint8_t &aHour)
{
  aSec = ((aTr & RTC_TR_SU) >> RTC_TR_SU_Pos) + 10*((aTr & RTC_TR_ST) >> RTC_TR_ST_Pos);
  aMin = ((aTr & RTC_TR_MNU) >> RTC_TR_MNU_Pos) + 10*((aTr & RTC_TR_MNT) >> RTC_TR_MNT_Pos);
  aHour = ((aTr & RTC_TR_HU) >> RTC_TR_HU_Pos) + 10*((aTr & RTC_TR_HT) >> RTC_TR_HT_Pos);
}

void THwRtc_stm32::convDr2Int(uint32_t &aDr, uint8_t &aDay, uint8_t &aMon, uint8_t &aYear)
{
  aDay = ((aDr & RTC_DR_DU) >> RTC_DR_DU_Pos) + 10*((aDr & RTC_DR_DT) >> RTC_DR_DT_Pos);
  aMon = ((aDr & RTC_DR_MU) >> RTC_DR_MU_Pos) + 10*((aDr & RTC_DR_MT) >> RTC_DR_MT_Pos);
  aYear = ((aDr & RTC_DR_YU) >> RTC_DR_YU_Pos) + 10*((aDr & RTC_DR_YT) >> RTC_DR_YT_Pos);
}

#endif // defined(MCUSF_WB)

#endif /* HWRTC_STM32_CPP_ */
