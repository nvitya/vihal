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
// file:     hwuscounter_atsam.cpp
// brief:    ATSAM implementation of the us (microseconds) counter
// created:  2024-04-06
// authors:  nvitya
// notes:    uses the 32-bit TIM2 or TIM5 timers

#include "platform.h"

#if defined(TC3)

#include "hwuscounter.h"
#include "hwuscounter_atsam.h"

#include "atsam_utils.h"

bool THwUsCounter_atsam::Init()
{
  // the device and block numbering is a huge mess at the ATSAM !
  // Every channel has its own clock enable id which is continuously numbered
  // and it is called also TCx but the meaning is different

  if (3 == timerdev)
  {
    commonregs = (Tc *)TC3;
    atsam_enable_peripheral(ID_TC9);
    atsam_enable_peripheral(ID_TC10);
    atsam_enable_peripheral(ID_TC11);
  }
#if defined(TC2)
  else if (2 == timerdev)
  {
    commonregs = (Tc *)TC2;
    atsam_enable_peripheral(ID_TC6);
    atsam_enable_peripheral(ID_TC7);
    atsam_enable_peripheral(ID_TC8);
  }
#endif
#if defined(TC1)
  else if (1 == timerdev)
  {
    commonregs = (Tc *)TC1;
    atsam_enable_peripheral(ID_TC3);
    atsam_enable_peripheral(ID_TC4);
    atsam_enable_peripheral(ID_TC5);
  }
#endif
  else
  {
    commonregs = (Tc *)TC0;
    atsam_enable_peripheral(ID_TC0);
    atsam_enable_peripheral(ID_TC1);
    atsam_enable_peripheral(ID_TC2);
  }

  uint32_t periphclock = atsam_peripheral_clock();

#if 0
  // Enable PCK6:
  PMC->PMC_PCK[6] = (0
    | (4  <<  0) // CSS(3): 1 = MAIN_CLK, 4 = MCK
    | (0  <<  4) // PRES(8): prescaler
  );
  PMC->PMC_SCER |= (1 << 14); // enables the PCK6
#endif

  c0regs = &commonregs->TC_CHANNEL[0];
  c1regs = &commonregs->TC_CHANNEL[1];
  c2regs = &commonregs->TC_CHANNEL[2];

  // chaining the clocks:
  //  TCLK0(PCLK6)--> [CH0] >--TIOA0--> [CH1] >--TIOA1--> [CH2]
  commonregs->TC_BMR = (0
    | (0  <<  0)  // TC0XC0S(2): 0 = TCLK0 (= MCLK)
    | (2  <<  2)  // TC1XC1S(2): 2 = TIOA0
    | (3  <<  4)  // TC2XC2S(2): 3 = TIOA1
    // the rest of the fields can be kept zero
  );

  c0regs->TC_CCR = TC_CCR_CLKDIS;
  c0regs->TC_IDR = 0xFFFFFFFF;
  c1regs->TC_CCR = TC_CCR_CLKDIS;
  c1regs->TC_IDR = 0xFFFFFFFF;
  c2regs->TC_CCR = TC_CCR_CLKDIS;
  c2regs->TC_IDR = 0xFFFFFFFF;

  if (c0regs->TC_SR) { }  /*  Clear status register */
  if (c1regs->TC_SR) { }  /*  Clear status register */
  if (c2regs->TC_SR) { }  /*  Clear status register */

  // CH0: prescaler function: generates 1 us clock to CH1

  c0regs->TC_CMR = (0 | TC_CMR_CPCTRG
    | (0  <<  0)  // TCCLKS(3): (ignored here by the TC_EMR!) 0 = PCK6, 1 = MCK/8, 5 = XC0, 6 = XC1, 7 = XC2
    | (0  <<  3)  // CLKI: 0 = increment on rising edge, 1 = increment on falling edge
    | (0  <<  4)  // BURST(2): 0 = none
    | (0  <<  6)  // CPCSTOP: 0 = do not stop on RC
    | (0  <<  7)  // CPCDIS:  0 = do not disable on RC
    | (0  <<  8)  // EEVTEDG(2)
    | (0  << 10)  // EEVT
    | (0  << 12)  // ENETRG: 0 = external trigger has no effect
    | (2  << 13)  // WAVESEL(2): 0 = UP, 1 = UPDOWN, 2 = UP_RC, 3 = UDDOWN_RC
    | (1  << 15)  // WAVE: 0 = capture mode, 1 = wave generation mode
    | (2  << 16)  // ACPA(2): RA Copare Effect on TIOA: 0 = none, 1 = set, 2 = clear, 3 = toggle
    | (1  << 18)  // ACPC(2): RC Copare Effect on TIOA: 0 = none, 1 = set, 2 = clear, 3 = toggle
    | (0  << 20)  // AEEVT(2): External Event Effect on TIOA
    | (0  << 22)  // ASWTRG(2): Software Trigger Effect on TIOA
    | (0  << 24)  // BCPB(2): RB Copare Effect on TIOB: 0 = none, 1 = set, 2 = clear, 3 = toggle
    | (0  << 26)  // BCPC(2): RC Copare Effect on TIOB: 0 = none, 1 = set, 2 = clear, 3 = toggle
    | (0  << 28)  // BEEVT(2): External Event Effect on TIOB
    | (0  << 30)  // BSWTRG(2): Software Trigger Effect on TIOB
  );

  uint32_t rcclks = (periphclock / 1000000);
  c0regs->TC_RC = rcclks;// - 1;
  c0regs->TC_RA = (rcclks >> 1);// - 1;  // generate 50% duty on TIOA
  c0regs->TC_EMR = (1 << 8); // ignore TCCLKS and use the peripheral clock (=SystemCoreClock / 2)

  c1regs->TC_CMR = (0
    | (6  <<  0)  // TCCLKS(3): 0 = PCK6, 5 = XC0, 6 = XC1, 7 = XC2
    | (0  <<  3)  // CLKI: 0 = increment on rising edge, 1 = increment on falling edge
    | (0  <<  4)  // BURST(2): 0 = none
    | (0  <<  6)  // CPCSTOP: 0 = do not stop on RC
    | (0  <<  7)  // CPCDIS:  0 = do not disable on RC
    | (0  <<  8)  // EEVTEDG(2)
    | (0  << 10)  // EEVT
    | (0  << 12)  // ENETRG: 0 = external trigger has no effect
    | (0  << 13)  // WAVESEL(2): 0 = UP, 1 = UPDOWN, 2 = UP_RC, 3 = UDDOWN_RC
    | (1  << 15)  // WAVE: 0 = capture mode, 1 = wave generation mode
    | (2  << 16)  // ACPA(2): RA Copare Effect on TIOA: 0 = none, 1 = set, 2 = clear, 3 = toggle
    | (1  << 18)  // ACPC(2): RC Copare Effect on TIOA: 0 = none, 1 = set, 2 = clear, 3 = toggle
    | (0  << 20)  // AEEVT(2): External Event Effect on TIOA
    | (0  << 22)  // ASWTRG(2): Software Trigger Effect on TIOA
    | (0  << 24)  // BCPB(2): RB Copare Effect on TIOB: 0 = none, 1 = set, 2 = clear, 3 = toggle
    | (0  << 26)  // BCPC(2): RC Copare Effect on TIOB: 0 = none, 1 = set, 2 = clear, 3 = toggle
    | (0  << 28)  // BEEVT(2): External Event Effect on TIOB
    | (0  << 30)  // BSWTRG(2): Software Trigger Effect on TIOB
  );
  c1regs->TC_RA = 0x8000;  // generate 50% duty on TIOA
  c1regs->TC_RC = 0xFFFF;

  c2regs->TC_CMR = (0
    | (7  <<  0)  // TCCLKS(3): 0 = PCK6, 5 = XC0, 6 = XC1, 7 = XC2
    | (0  <<  3)  // CLKI: 0 = increment on rising edge, 1 = increment on falling edge
    | (0  <<  4)  // BURST(2): 0 = none
    | (0  <<  6)  // CPCSTOP: 0 = do not stop on RC
    | (0  <<  7)  // CPCDIS:  0 = do not disable on RC
    | (0  <<  8)  // EEVTEDG(2)
    | (0  << 10)  // EEVT
    | (0  << 12)  // ENETRG: 0 = external trigger has no effect
    | (0  << 13)  // WAVESEL(2): 0 = UP, 1 = UPDOWN, 2 = UP_RC, 3 = UDDOWN_RC
    | (1  << 15)  // WAVE: 0 = capture mode, 1 = wave generation mode
    | (2  << 16)  // ACPA(2): RA Copare Effect on TIOA: 0 = none, 1 = set, 2 = clear, 3 = toggle
    | (1  << 18)  // ACPC(2): RC Copare Effect on TIOA: 0 = none, 1 = set, 2 = clear, 3 = toggle
    | (0  << 20)  // AEEVT(2): External Event Effect on TIOA
    | (0  << 22)  // ASWTRG(2): Software Trigger Effect on TIOA
    | (0  << 24)  // BCPB(2): RB Copare Effect on TIOB: 0 = none, 1 = set, 2 = clear, 3 = toggle
    | (0  << 26)  // BCPC(2): RC Copare Effect on TIOB: 0 = none, 1 = set, 2 = clear, 3 = toggle
    | (0  << 28)  // BEEVT(2): External Event Effect on TIOB
    | (0  << 30)  // BSWTRG(2): Software Trigger Effect on TIOB
  );
  c2regs->TC_RA = 0x8000;  // generate 50% duty on TIOA
  c2regs->TC_RC = 0xFFFF;

  c2regs->TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG; // enable the clock
  c1regs->TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG; // enable the clock
  c0regs->TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;

  cnt64_high = 0;
  cnt32_prev = 0;

  initialized = true;
  return true;
}

uint32_t THwUsCounter_atsam::Get32()
{
  uint32_t uslow1;
  uint32_t ushigh1;
  uint32_t ushigh2;
  do
  {
    ushigh1 = c2regs->TC_CV;
    uslow1  = c1regs->TC_CV;
    ushigh2 = c2regs->TC_CV;
  }
  while (ushigh1 != ushigh2);

  return ((ushigh2 << 16) | uslow1);
}

uint64_t THwUsCounter_atsam::Get64()
{
  uint32_t cnt32_cur = Get32();
  if (cnt32_cur < cnt32_prev)
  {
    // the 32-bit version overflowed (after 66 minutes)
    cnt64_high += 0x100000000ull;
  }
  cnt32_prev = cnt32_cur;
  return cnt64_high + cnt32_cur;
}

#endif
