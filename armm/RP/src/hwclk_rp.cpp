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
 *  file:     hwclkctrl_rp.cpp
 *  brief:    RP MCU Clock / speed setup
 *  version:  1.00
 *  date:     2021-12-19
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwclk.h"
#include "rp_utils.h"

void hwclk_start_ext_osc(unsigned aextspeed)
{
  xosc_hw->status = (1 << 24); // clear badwrite
  xosc_hw->ctrl = 0x00FABAA0;  // enable with Freq range 1-15 MHz
  while (0 == (xosc_hw->status & (1u << 31)))
  {
    // wait until it is stable
  }
}

void hwclk_prepare_hispeed(unsigned acpuspeed)
{
  // nothing to do, no internal flash memory or voltage scaling
}

void hwclk_setup_pll(pll_hw_t * pll_hw, unsigned basespeed, unsigned target_speed_hz)
{
  uint32_t tmp;

  // PLL CONSTRAINTS:
  //   Reference clock frequency min=5MHz, max=800MHz
  //   Feedback divider min=16, max=320
  //   VCO frequency min=400MHz, max=1600MHz

  // VCO freq      = (input_freq / input_div) * input_mul
  // pll_out_freq  = (input_freq / input_div1) * input_mul / (pll_postdiv1 * pll_postdiv2)

  uint32_t vco_div  = 12;
  uint32_t vco_freq = target_speed_hz * vco_div; // target speed * 12 is almost universal

  if (vco_freq < 400000000) // speeds smaller than 400 MHz
  {
    vco_div = 24;
    vco_freq = target_speed_hz * vco_div;
  }

  uint32_t pll_postdiv1 = vco_freq / target_speed_hz;
  uint32_t pll_postdiv2 = 1;
  while ((pll_postdiv1 > 7) && (pll_postdiv2 < 4))
  {
    ++pll_postdiv2;
    pll_postdiv1 = (vco_freq / (target_speed_hz * pll_postdiv2));
  }

  // handle input

  uint32_t input_div = 1;
  uint32_t input_mul = vco_freq / basespeed;
  while (input_mul < 16)
  {
    ++input_div;
    input_mul = input_div * (vco_freq / basespeed);
  }

  pll_hw->prim = ((pll_postdiv1 << 16) | (pll_postdiv2 << 12));
  pll_hw->fbdiv_int = input_mul;

  tmp = pll_hw->cs;
  tmp &= ~(0x3F | (1 << 8)); // clear refdiv and bypass
  tmp |= input_div;
  pll_hw->cs = tmp;

  // tur on the power...
  tmp = pll_hw->pwr;

  // first turn all off
  tmp |= (0
    | (1 << 5) // VCOPD
    | (1 << 3) // POSTDIVPD
    | (1 << 2) // DSMPD
    | (1 << 0) // PD
  );

  pll_hw->pwr = tmp;

  for (unsigned n = 0; n < 100; ++n)
  {
    __NOP();
  }

  tmp &= ~((1 << 5) | (1 << 0));  // enable PLL and VCO
  pll_hw->pwr = tmp;

  while (!(pll_hw->cs & PLL_CS_LOCK_BITS))
  {
    __NOP();
  }

  tmp &= ~(1 << 3);  // enable post divider
  pll_hw->pwr = tmp;
}

bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
  SystemCoreClock = MCU_INTERNAL_RC_SPEED;

  uint32_t tmp;

  // swithc the reference clock to ring osc
  tmp = clocks_hw->clk[clk_ref].ctrl;
  tmp &= ~(3);
  clocks_hw->clk[clk_ref].ctrl = tmp;

  // switch back to ref_clk (driven by the ring oscillator)
  clock_hw_t * pclk_sys = &clocks_hw->clk[clk_sys];
  pclk_sys->ctrl &= ~(1u); // select the clk_ref
  while (0 == (pclk_sys->selected & 1))
  {
    __NOP();
  }

  // now it is safe to modify the PLLs

  if (!external_clock_hz)
  {
    // no external crystal, the PLL does not supports it !
    SystemCoreClock = MCU_INTERNAL_RC_SPEED;
    return true;
  }

  hwclk_start_ext_osc(external_clock_hz);

  tmp = clocks_hw->clk[clk_ref].ctrl;
  tmp &= ~(3);
  tmp |= 2; // select XOSC for the reference clock
  clocks_hw->clk[clk_ref].ctrl = tmp;

  unsigned basespeed = (external_clock_hz & HWCLK_EXTCLK_MASK);

  hwclk_prepare_hispeed(target_speed_hz);

  rp_reset_control(RESETS_RESET_PLL_SYS_BITS, true);
  rp_reset_control(RESETS_RESET_PLL_SYS_BITS, false);

  hwclk_setup_pll(pll_sys_hw, basespeed, target_speed_hz);

  // select the pll for the system clock
  tmp = pclk_sys->ctrl;
  tmp &= ~((7 << 5) | (1 << 0)); // select the PLL, keep with reference
  pclk_sys->ctrl = tmp;
  for (unsigned n = 0; n < 10; ++n)
  {
    __NOP();
  }
  tmp |= 1; // swith to PLL
  pclk_sys->ctrl = tmp;

  // now we running hi-speed...

  // set the peripheral clocks
  tmp = clocks_hw->clk[clk_peri].ctrl;
  tmp &= ~(3 << 5);  // 0 = clk_sys
  tmp |= (1 << 11);  // enable
  clocks_hw->clk[clk_peri].ctrl = tmp;

  // setup the USB PLL and set to the USB clock source
  clock_hw_t * pclk_usb = &clocks_hw->clk[clk_usb];

  pclk_usb->ctrl &= ~(1 << 11); // disable the generator
  while (0 == (pclk_usb->selected & 1))
  {
    __NOP();
  }

  rp_reset_control(RESETS_RESET_PLL_USB_BITS, true);
  rp_reset_control(RESETS_RESET_PLL_USB_BITS, false);
  hwclk_setup_pll(pll_usb_hw, basespeed, 48000000);

  pclk_usb->div = (1 << 8);  // set the divisor to 1

  tmp = pclk_usb->ctrl;
  tmp &= ~((1 << 20) | (3 << 16) | (7 << 5)); // clear the bitfields (NUDGE, PHASE, AUXSRC)
  tmp |= (0 << 5);               // AUXSRC(3): 0 = USB PLL
  pclk_usb->ctrl = tmp;
  for (unsigned n = 0; n < 10; ++n)
  {
    __NOP();
  }
  tmp |= (1 << 11); // turn on the generator
  pclk_usb->ctrl = tmp;

  // inform the system of the clock speeds

  rp_watchdog_tick_mul = target_speed_hz / basespeed;
  SystemCoreClock = target_speed_hz;

  return true;
}

