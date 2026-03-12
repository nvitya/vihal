/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2023 Viktor Nagy, nvitya
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
 *  file:     hwclkl_lpc.cpp
 *  brief:    LPC MCU Clock / speed setup
 *  created:  2026-03-12
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwclk.h"
//#include "msp_utils.h"

#ifdef MCUSF_17XX

inline bool hwclk_ext_osc_ready()
{
  return ((LPC_SC->SCS & SCS_OSCSTAT) != 0);
}

void hwclk_start_ext_osc(unsigned aextspeed)
{
	unsigned extspeed = (aextspeed & HWCLK_EXTCLK_MASK);

  // Select main oscillator range and enable clock from main oscillator
	uint32_t range;
	if (extspeed < 15000000)
	{
		range = SCS_OSCRANGE_1MHZ_15MHZ;
	}
	else
	{
		range = SCS_OSCRANGE_15MHZ_25MHZ;
	}

  LPC_SC->SCS = range | SCS_OSCEN_DISABLE;
  LPC_SC->SCS = range | SCS_OSCEN_ENABLE;

	while (!hwclk_ext_osc_ready())
	{
	  // wait
	}
}

bool hwclk_start_inths_osc(unsigned aextspeed)
{
	return true;
}

void hwclk_prepare_hispeed(unsigned acpuspeed)
{
	unsigned fws;
	if      (acpuspeed <= 20000000) 	fws = 0;
	else if (acpuspeed <= 40000000)   fws = 1;
	else if (acpuspeed <= 60000000)   fws = 2;
	else if (acpuspeed <= 80000000)   fws = 3;
	else                              fws = 4;

  LPC_SC->FLASHCFG = (LPC_SC->FLASHCFG & ~(0xF << 12)) | (fws << 12);
}

bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
	unsigned tmp;

  SystemCoreClock = MCU_INTERNAL_RC_SPEED;

  // Select RC oscillator (default oscillator)
  LPC_SC->CLKSRCSEL = CLKSRCSEL_CLKSRC_RC_OSC;

  // Disconnect PLL
  tmp = LPC_SC->PLL0CON & ~PLL0CON_PLLC0_MASK;
  LPC_SC->PLL0CON = tmp;
  LPC_SC->PLL0FEED = 0xAA;
  LPC_SC->PLL0FEED = 0x55;

  // Disable PLL
  LPC_SC->PLL0CON = tmp & ~PLL0CON_PLLE0_MASK;
  LPC_SC->PLL0FEED = 0xAA;
  LPC_SC->PLL0FEED = 0x55;

  unsigned pllref;
  unsigned basespeed;
  if (!external_clock_hz)
  {
    // use the internal 4 MHz oscillator
    basespeed = MCU_INTERNAL_RC_SPEED;
    pllref = CLKSRCSEL_CLKSRC_RC_OSC;
  }
  else
  {
    hwclk_start_ext_osc(external_clock_hz);
    basespeed = (external_clock_hz & HWCLK_EXTCLK_MASK);
    pllref = CLKSRCSEL_CLKSRC_MAIN_OSC;
	}

  LPC_SC->CLKSRCSEL = CLKSRCSEL_CLKSRC_MAIN_OSC;  // Select the main oscillator for the PLL0 source


	unsigned mul = 2;   // increased with +2 (as it has internally a fix x2 path)
	unsigned prediv = 1;
	unsigned postdiv = 1;

	unsigned vcospeed;  // The VCO frequency must be within 275 and 550 MHz

	while (1)
	{
	  vcospeed = (basespeed / prediv) * mul;
	  if (vcospeed < 275000000)  // minimum 275 MHz
	  {
	    mul += 2;
	  }
	  else if (vcospeed > 550000000) // maximum 550 MHz
	  {
	  	if (prediv >= 32)
	  	{
	  		break;
	  	}
	    ++prediv;
	    mul = 2;
	  }
	  else
	  {
	  	postdiv = vcospeed / target_speed_hz;
	    if (target_speed_hz * postdiv == vcospeed)  // integer divisible ?
	    {
	      break; // found it
	    }
	    else
	    {
	      mul += 2;  // continue searching
	    }
	  }
	}

	LPC_SC->PLL0CFG = (0
	  | ((prediv - 1)      << 16)  // NSEL0(5): pre-division - 1
		| (((mul >> 1) - 1)  <<  0)  // MSEL(15): multiplier / 2 - 1
	);
	LPC_SC->PLL0FEED = 0xAA;
	LPC_SC->PLL0FEED = 0x55;

  // Enable PLL
  LPC_SC->PLL0CON = PLL0CON_PLLE0_ENABLE | PLL0CON_PLLC0_DISCONNECT;
  LPC_SC->PLL0FEED = 0xAA;
  LPC_SC->PLL0FEED = 0x55;

  // Wait for PLL enabled
  while( ( LPC_SC->PLL0STAT & PLL0STAT_PLLE0_STAT_ENABLED ) != PLL0STAT_PLLE0_STAT_ENABLED );

  // Change the CPU Clock Divider setting for the operation with PLL0
  LPC_SC->CCLKCFG = (postdiv - 1);

  // Wait for PLL locked
  while( ( LPC_SC->PLL0STAT & PLL0STAT_PLOCK0_LOCKED ) != PLL0STAT_PLOCK0_LOCKED );

  // Setup the peripheral clock dividers
  LPC_SC->PCLKSEL0 = PCLKSEL0_PCLK_WDT_CCLK |
                     PCLKSEL0_PCLK_TIMER0_CCLK |
                     PCLKSEL0_PCLK_TIMER1_CCLK |
                     PCLKSEL0_PCLK_UART0_CCLK |
                     PCLKSEL0_PCLK_UART1_CCLK |
                     PCLKSEL0_PCLK_PWM1_CCLK |
                     PCLKSEL0_PCLK_I2C0_CCLK |
                     PCLKSEL0_PCLK_SPI_CCLK |
                     PCLKSEL0_PCLK_SSP1_CCLK |
                     PCLKSEL0_PCLK_DAC_CCLK |
                     PCLKSEL0_PCLK_ADC_CCLK |
                     PCLKSEL0_PCLK_CAN1_CCLK_4 |
                     PCLKSEL0_PCLK_CAN2_CCLK_4 |
                     PCLKSEL0_PCLK_ACF_CCLK;

  LPC_SC->PCLKSEL1 = PCLKSEL1_PCLK_QEI_CCLK |
                     PCLKSEL1_PCLK_GPIOINT_CCLK |
                     PCLKSEL1_PCLK_PCB_CCLK |
                     PCLKSEL1_PCLK_I2C1_CCLK |
                     PCLKSEL1_PCLK_SSP0_CCLK |
                     PCLKSEL1_PCLK_TIMER2_CCLK |
                     PCLKSEL1_PCLK_TIMER3_CCLK |
                     PCLKSEL1_PCLK_UART2_CCLK |
                     PCLKSEL1_PCLK_UART3_CCLK |
                     PCLKSEL1_PCLK_I2C2_CCLK |
                     PCLKSEL1_PCLK_I2S_CCLK |
                     PCLKSEL1_PCLK_RIT_CCLK |
                     PCLKSEL1_PCLK_SYSCON_CCLK |
                     PCLKSEL1_PCLK_MC_CCLK;

  // Connect the PLL0 so the CPU and the peripherals start using the PLL0
  LPC_SC->PLL0CON = PLL0CON_PLLE0_ENABLE | PLL0CON_PLLC0_CONNECT;
  LPC_SC->PLL0FEED = 0xAA;
  LPC_SC->PLL0FEED = 0x55;

  // Wait for PLL connected
  while( ( LPC_SC->PLL0STAT & PLL0STAT_PLLC0_STAT_CONNECTED ) != PLL0STAT_PLLC0_STAT_CONNECTED );

  SystemCoreClock = target_speed_hz;
  return true;
}

#else
  #error "Unhandled LPC family"
#endif
