/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
/*
 *  file:     hwclk_imxrt.cpp
 *  brief:    IMXRT clock speed implementation
 *  date:     2018-11-23
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwclk.h"

void hwclk_start_ext_osc(unsigned aextspeed)
{
  // the Boot rom initializes the external crystal
}

void hwclk_prepare_hispeed(unsigned acpuspeed)
{
	unsigned tmp;

	// no flash = no wait states...

   bool overdrive;
#if defined(MCUSF_1020)
   overdrive = (acpuspeed > 400000000);
#elif defined(MCUSF_1050) || defined(MCUSF_1040)
   overdrive = (acpuspeed > 528000000);
#else
  #error "define MCU overdrive conditions"
#endif

	// Adjust the core voltage at the DCDC->REG3
	//   0 = 0.8V, every step means 0.025V
	tmp = DCDC->REG3;
	tmp &= ~DCDC_REG3_TRG_MASK;
	if (overdrive)
	{
		tmp |= DCDC_REG3_TRG(18);  // 18 = 1.25V
	}
	else
	{
		tmp |= DCDC_REG3_TRG(14);  // 14 = 1.15V
	}
	DCDC->REG3 = tmp;
}

bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
	unsigned tmp;

	// 1. change to a safe (slow) clock

  //CLOCK_SetMux(kCLOCK_PeriphClk2Mux, 0x1); /* Set PERIPH_CLK2 MUX to OSC */
	tmp = CCM->CBCMR;
	tmp &= ~(3 << CCM_CBCMR_PERIPH_CLK2_SEL_SHIFT);
	tmp |=  (1 << CCM_CBCMR_PERIPH_CLK2_SEL_SHIFT); // 1 = OSC selected for PERIPH_CLK2
	CCM->CBCMR = tmp;

  //CLOCK_SetMux(kCLOCK_PeriphMux, 0x1);     /* Set PERIPH_CLK MUX to PERIPH_CLK2 */
	tmp = CCM->CBCDR;
	tmp &=  ~(1 << CCM_CBCDR_PERIPH_CLK_SEL_SHIFT);
	tmp |=   (1 << CCM_CBCDR_PERIPH_CLK_SEL_SHIFT); // 1 = derive clock from periph_clk2_clk_divided
	CCM->CBCDR = tmp;
  // Wait until CCM internal handshake finish.
  while (CCM->CDHIPR & (1U << CCM_CDHIPR_PERIPH_CLK_SEL_BUSY_SHIFT))
  {
  }

  SystemCoreClock = external_clock_hz; // set the global variable for the fall error happens

  hwclk_prepare_hispeed(target_speed_hz);

  //-----------------------------------------------------------------------------
  // set clock dividers

  //CLOCK_SetDiv(kCLOCK_ArmDiv, 0x1); /* Set ARM PODF to 1, divide by 2 */
  tmp = CCM->CACRR;
  tmp &= ~(7 << CCM_CACRR_ARM_PODF_SHIFT);
  tmp |=  (0 << CCM_CACRR_ARM_PODF_SHIFT);  // ARM PODF: 0 = do not divide
  CCM->CACRR = tmp;
  while (CCM->CDHIPR & (1U << CCM_CDHIPR_ARM_PODF_BUSY_SHIFT))
  {
  }

  //CLOCK_SetDiv(kCLOCK_AhbDiv, 0x0); /* Set AHB PODF to 0, divide by 1 */
  tmp = CCM->CBCDR;
  tmp &= ~(7 << CCM_CBCDR_AHB_PODF_SHIFT);
  tmp |=  (0 << CCM_CBCDR_AHB_PODF_SHIFT);  // AHB PODF: 0 = divide it by 1
  CCM->CBCDR = tmp;
  while (CCM->CDHIPR & (1U << CCM_CDHIPR_AHB_PODF_BUSY_SHIFT))
  {
  }

  //CLOCK_SetDiv(kCLOCK_IpgDiv, 0x3); /* Set IPG PODF to 3, divide by 4 */
  tmp = CCM->CBCDR;
  tmp &= ~(3 << CCM_CBCDR_IPG_PODF_SHIFT);
  tmp |=  (3 << CCM_CBCDR_IPG_PODF_SHIFT);  // IPG PODF: 3 = divide it by 4
  CCM->CBCDR = tmp;

  // Configure PLLs / dividers

  // 4. Set USB1 PLL (PLL3) to 480 MHz

  //CLOCK_InitUsb1Pll(&usb1PllConfig); /* Configure USB1 PLL to 480M */
  CCM_ANALOG->PLL_USB1 = CCM_ANALOG_PLL_USB1_ENABLE_MASK      |
                         CCM_ANALOG_PLL_USB1_POWER_MASK       |
                         CCM_ANALOG_PLL_USB1_EN_USB_CLKS_MASK |
                         CCM_ANALOG_PLL_USB1_DIV_SELECT(0);
  while ((CCM_ANALOG->PLL_USB1 & CCM_ANALOG_PLL_USB1_LOCK_MASK) == 0)
  {
  }

  // prepare the 720 MHz clock at the USB_PLL (480) fractional divider 0
  // the 720 MHz base clock is required by the LPSPI!
  CCM_ANALOG->PFD_480 = 0
    | (12 <<  0)   // PFD0_FRAC(6): 12 = 720 MHz
    | (16 <<  8)   // PFD1_FRAC(6): 16 = 540 MHz
    | (17 << 16)   // PFD2_FRAC(6): 17 = 508 MHz
    | (18 << 24)   // PFD3_FRAC(6): 18 = 480 MHz - dedicated to CPU clock source
  ;

  // Set the System PLL (PLL2) to 528 MHz

  //CLOCK_InitSysPll(&sysPllConfig); /* Configure SYS PLL to 528M */
  CCM_ANALOG->PLL_SYS = CCM_ANALOG_PLL_SYS_ENABLE_MASK | CCM_ANALOG_PLL_SYS_DIV_SELECT(1);
  while ((CCM_ANALOG->PLL_SYS & CCM_ANALOG_PLL_SYS_LOCK_MASK) == 0)
  {
  }

  // set its fractional dividers
  // resulting frequency f = 528 * 18 / PFDx_FRAC

  CCM_ANALOG->PFD_528 = 0
    | (27 <<  0)   // PFD0_FRAC(6): 27 = 352 MHz
    | (16 <<  8)   // PFD1_FRAC(6): 16 = 594 MHz
    | (24 << 16)   // PFD2_FRAC(6): 24 = 396 MHz - used by the SD-CARD
    | (24 << 24)   // PFD3_FRAC(6): 24 = 396 MHz - dedicated to CPU clock source
  ;

#if defined(MCUSF_1020)

  // 2. Configure the Ethernet PLL (PLL6) to generate the 500 MHz (fixed) clock

  // configure with bypass first
  CCM_ANALOG->PLL_ENET = 0
  	| (1 <<  0)    // DIV_SELECT(2): Ethernet Reference Clock, 0 = 25 MHz, 1 = 50 MHz, 2 = 100 MHz, 3 = 125 MHz
  	| (0 << 12)    // POWERDOWN: 1 = power down
  	| (1 << 13)    // ENABLE: enable the ethernet clock output
  	| (0 << 14)    // BYPASS_CLK_SRC(2): 0 = 24 MHz ref clock
  	| (1 << 16)    // BYPASS:
  	| (1 << 21)    // ENET_25M_REF_EN: Enable the PLL providing ENET 25 MHz reference clock
  	| (1 << 22)    // ENET_500M_REF_EN: Enable the PLL providing ENET 500 MHz reference clock
  ;

  /* Wait for stable */
  while ((CCM_ANALOG->PLL_ENET & CCM_ANALOG_PLL_ENET_LOCK_MASK) == 0)
  {
  }

  /* Disable Bypass */
  CCM_ANALOG->PLL_ENET &= ~CCM_ANALOG_PLL_ENET_BYPASS_MASK;

  // 5. Setup the audio PLL (PLL4)

  //TODO: setup the audio pll to 786 MHz

  //--------------------------------------------------------------------------------
  // Set MCU clock source

  uint32_t clksrc;
  if (500000000 == target_speed_hz)
  {
  	clksrc = 3; // use the Ethernet PLL (PLL6)
  }
  else if (480000000 == target_speed_hz)
  {
  	clksrc = 1; // PLL3 PFD3
  }
  else if (396000000 == target_speed_hz)
  {
  	clksrc = 2; // PLL2 PFD3
  }
  else
  {
  	//TODO: adjust the fractional dividers
  	return false;
  }

	tmp = CCM->CBCMR;
	tmp &= ~(3 << CCM_CBCMR_PRE_PERIPH_CLK_SEL_SHIFT);
	tmp |=  (clksrc << CCM_CBCMR_PRE_PERIPH_CLK_SEL_SHIFT);
	CCM->CBCMR = tmp;


#elif defined(MCUSF_1050) || defined(MCUSF_1040)

  //CLOCK_InitArmPll(&armPllConfig); /* Configure ARM PLL to 1200M */
  unsigned loopdivider;
  loopdivider = 4 * (target_speed_hz / external_clock_hz);
  CCM_ANALOG->PLL_ARM = CCM_ANALOG_PLL_ARM_ENABLE_MASK | CCM_ANALOG_PLL_ARM_DIV_SELECT(loopdivider);
  while ((CCM_ANALOG->PLL_ARM & CCM_ANALOG_PLL_ARM_LOCK_MASK) == 0)
  {
  }

  // set the clock

  if (528000000 == target_speed_hz)
  {
    // use SYS PLL (PLL2)
    tmp = CCM->CBCMR;
    tmp &= ~(3 << CCM_CBCMR_PRE_PERIPH_CLK_SEL_SHIFT);
    tmp |=  (0 << CCM_CBCMR_PRE_PERIPH_CLK_SEL_SHIFT); // 3 = Set PRE_PERIPH_CLK to PLL2, 528 MHz
    CCM->CBCMR = tmp;
  }
  else
  {
    //CLOCK_SetMux(kCLOCK_PrePeriphMux, 0x3); /* Set PRE_PERIPH_CLK to PLL1, 1200M */
    tmp = CCM->CBCMR;
    tmp &= ~(3 << CCM_CBCMR_PRE_PERIPH_CLK_SEL_SHIFT);
    tmp |=  (3 << CCM_CBCMR_PRE_PERIPH_CLK_SEL_SHIFT); // 3 = Set PRE_PERIPH_CLK to PLL1, 1200M
    CCM->CBCMR = tmp;
  }

#else

  #error "Clock setup is not implemented for this MCU subfamily"

#endif

  // set other clock domains

  tmp = CCM->CSCMR1;
  tmp &= ~(CCM_CSCMR1_PERCLK_PODF_MASK | CCM_CSCMR1_PERCLK_CLK_SEL_MASK
           | CCM_CSCMR1_USDHC1_CLK_SEL_MASK | CCM_CSCMR1_USDHC2_CLK_SEL_MASK);
  // PERCLK_CLK_ROOT: max 75 MHz, used by the GPT timers, set to 24 MHz
  tmp |=  CCM_CSCMR1_PERCLK_CLK_SEL(1);  // select the 24 MHz oscillator clock
  tmp |=  CCM_CSCMR1_PERCLK_PODF(0);     // do not divide
  // SD Card clocks
  tmp |=  CCM_CSCMR1_USDHC1_CLK_SEL(0);  // select PFD2 = 198 MHz
  tmp |=  CCM_CSCMR1_USDHC2_CLK_SEL(0);  // select PFD2 = 198 MHz
  CCM->CSCMR1 = tmp;

  // SEMC_CLK_ROOT: max 166 MHz
  tmp = CCM->CBCDR;
  tmp &= ~CCM_CBCDR_SEMC_CLK_SEL_MASK;
  tmp |=  CCM_CBCDR_SEMC_CLK_SEL(0);
  tmp &= ~CCM_CBCDR_SEMC_PODF_MASK;
  tmp |=  CCM_CBCDR_SEMC_PODF(3);       // select main clock / 4 for SEMC = 125 MHz
  CCM->CBCDR = tmp;

  // USDHC1 + USDHC2: max 198 MHz
  tmp = CCM->CSCDR1;
  tmp &= ~(CCM_CSCDR1_USDHC1_PODF_MASK | CCM_CSCDR1_USDHC2_PODF_MASK);
  tmp |=  CCM_CSCDR1_USDHC1_PODF(1);   // divide by two
  tmp |=  CCM_CSCDR1_USDHC2_PODF(1);   // divide by two
  CCM->CSCDR1 = tmp;

  //CLOCK_SetMux(kCLOCK_PeriphMux, 0x0);    /* Set PERIPH_CLK MUX to PRE_PERIPH_CLK */
  tmp = CCM->CBCDR;
  tmp &=  ~(1 << CCM_CBCDR_PERIPH_CLK_SEL_SHIFT);
  tmp |=   (0 << CCM_CBCDR_PERIPH_CLK_SEL_SHIFT); // 0 = derive clock from pre_periph_clk_sel
  CCM->CBCDR = tmp;
  // Wait until CCM internal handshake finish.
  while (CCM->CDHIPR & (1U << CCM_CDHIPR_PERIPH_CLK_SEL_BUSY_SHIFT))
  {
  }

  // set the GPT, CCM clock to the IPG_CLOCK to allow higher resolutin IRQ timing
  tmp = CCM->CSCMR1;
  tmp &= ~CCM_CSCMR1_PERCLK_CLK_SEL_MASK;  // 0 = IPG, 1 = OSC (24M)
  CCM->CSCMR1 = tmp;

  SystemCoreClock = target_speed_hz;
  return true;
}
