/*
 * hwrtc_stm32.h
 *
 *  Created on: 12.03.2022
 *      Author: Bergi
 */

#ifndef HWRTC_STM32_H_
#define HWRTC_STM32_H_

#define HWRTC_PRE_ONLY
#include "hwrtc.h"

class THwRtc_stm32 : public THwRtc_pre
{
public:
  RTC_TypeDef* regs;

  uint16_t syncDiv;
  uint16_t wakeUpMaxMsec;

public:
  bool init();
  bool setTime(time_t atime, time_t &rtime); // returns the last time value before set time to new value
  bool getTime(time_t &atime);

  // set periodic wake up timer to aMSecTime and enable timer
  bool setWakeupTimer(uint16_t aMSecTime);

  // disable timer
  bool stopWakeupTimer();

  // enables interrupt for wake up event
  bool enableWakeupIRQ();

  // disable interrupt for wake up event
  bool disableWakeupIRQ();

  void clearWakeupIRQ();

private:
  uint32_t lastSsr;
  uint32_t lastTr;
  uint32_t lastDr;

  // inlined helper functions
  void lockRegs();
  void unlockRegs();
  void readCalReg(uint32_t &aSsr, uint32_t &aTr, uint32_t &aDr);
  void convSsr2Ms(uint32_t &aSsr, uint16_t &aMs);
  void convTr2Int(uint32_t &aTr, uint8_t &aSec, uint8_t &aMin, uint8_t &aHour);
  void convDr2Int(uint32_t &aDr, uint8_t &aDay, uint8_t &aMon, uint8_t &aYear);
};

#define HWRTC_IMPL THwRtc_stm32

#endif /* HWRTC_STM32_H_ */
