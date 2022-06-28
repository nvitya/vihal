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
 *  file:     hwclkctrl_esp.cpp
 *  brief:    ESP MCU Clock / speed setup
 *  version:  1.00
 *  date:     2022-01-29
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwclk.h"

//#include "soc/dport_access.h"
#include "soc/dport_reg.h"
#include "soc/efuse_reg.h"
#include "esp32/regi2c_ctrl.h"

#define RTC_CNTL_DBIAS_HP_VOLT         (RTC_CNTL_DBIAS_1V25 - (REG_GET_FIELD(EFUSE_BLK0_RDATA5_REG, EFUSE_RD_VOL_LEVEL_HP_INV)))
#define DIG_DBIAS_80M_160M  RTC_CNTL_DBIAS_1V10
#define DIG_DBIAS_240M      RTC_CNTL_DBIAS_HP_VOLT
#define DIG_DBIAS_XTAL      RTC_CNTL_DBIAS_1V10
#define DIG_DBIAS_2M        RTC_CNTL_DBIAS_1V00

/* BBPLL configuration values */
#define BBPLL_ENDIV5_VAL_320M       0x43
#define BBPLL_BBADC_DSMP_VAL_320M   0x84
#define BBPLL_ENDIV5_VAL_480M       0xc3
#define BBPLL_BBADC_DSMP_VAL_480M   0x74
#define BBPLL_IR_CAL_DELAY_VAL      0x18
#define BBPLL_IR_CAL_EXT_CAP_VAL    0x20
#define BBPLL_OC_ENB_FCAL_VAL       0x9a
#define BBPLL_OC_ENB_VCON_VAL       0x00
#define BBPLL_BBADC_CAL_7_0_VAL     0x00

void hwclk_start_ext_osc(unsigned aextspeed)
{
}

void hwclk_prepare_hispeed(unsigned acpuspeed)
{
  // nothing to do, no internal flash memory or voltage scaling
}

void rtc_clk_bbpll_configure(unsigned xtal_freq, unsigned pll_freq)
{
    uint8_t div_ref;
    uint8_t div7_0;
    uint8_t div10_8;
    uint8_t lref;
    uint8_t dcur;
    uint8_t bw;

    if (pll_freq == 320000000)
    {
        /* Raise the voltage, if needed */
        REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_80M_160M);
        /* Configure 320M PLL */
        switch (xtal_freq)
        {
            case 40000000:
                div_ref = 0;
                div7_0 = 32;
                div10_8 = 0;
                lref = 0;
                dcur = 6;
                bw = 3;
                break;
            case 26000000:
                div_ref = 12;
                div7_0 = 224;
                div10_8 = 4;
                lref = 1;
                dcur = 0;
                bw = 1;
                break;
            case 24000000:
                div_ref = 11;
                div7_0 = 224;
                div10_8 = 4;
                lref = 1;
                dcur = 0;
                bw = 1;
                break;
            default:
                div_ref = 12;
                div7_0 = 224;
                div10_8 = 4;
                lref = 0;
                dcur = 0;
                bw = 0;
                break;
        }
        REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_ENDIV5, BBPLL_ENDIV5_VAL_320M);
        REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_BBADC_DSMP, BBPLL_BBADC_DSMP_VAL_320M);
    }
    else  // 480 M
    {
        /* Raise the voltage */
        REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_240M);
        for (unsigned n = 0; n < 10000; ++n)
        {
        }
        //esp_rom_delay_us(DELAY_PLL_DBIAS_RAISE);
        /* Configure 480M PLL */
        switch (xtal_freq) {
            case 40000000:
                div_ref = 0;
                div7_0 = 28;
                div10_8 = 0;
                lref = 0;
                dcur = 6;
                bw = 3;
                break;
            case 26000000:
                div_ref = 12;
                div7_0 = 144;
                div10_8 = 4;
                lref = 1;
                dcur = 0;
                bw = 1;
                break;
            case 24000000:
                div_ref = 11;
                div7_0 = 144;
                div10_8 = 4;
                lref = 1;
                dcur = 0;
                bw = 1;
                break;
            default:
                div_ref = 12;
                div7_0 = 224;
                div10_8 = 4;
                lref = 0;
                dcur = 0;
                bw = 0;
                break;
        }
        REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_ENDIV5, BBPLL_ENDIV5_VAL_480M);
        REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_BBADC_DSMP, BBPLL_BBADC_DSMP_VAL_480M);
    }

    uint8_t i2c_bbpll_lref  = (lref << 7) | (div10_8 << 4) | (div_ref);
    uint8_t i2c_bbpll_div_7_0 = div7_0;
    uint8_t i2c_bbpll_dcur = (bw << 6) | dcur;
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_LREF, i2c_bbpll_lref);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_DIV_7_0, i2c_bbpll_div_7_0);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_DCUR, i2c_bbpll_dcur);

    //uint32_t delay_pll_en = (rtc_clk_slow_freq_get() == RTC_SLOW_FREQ_RTC) ?
    //        DELAY_PLL_ENABLE_WITH_150K : DELAY_PLL_ENABLE_WITH_32K;
    //esp_rom_delay_us(delay_pll_en);

    for (unsigned n = 0; n < 10000; ++n)
    {
    }

    //s_cur_pll_freq = pll_freq;
}

bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz)
{
  // configure the crystal first
  REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL, RTC_CNTL_SOC_CLK_SEL_XTL);
  SystemCoreClock = external_clock_hz;  // this is the defalt speed 40 MHz

  if (target_speed_hz >= 240000000)
  {
    rtc_clk_bbpll_configure(external_clock_hz, 480000000);

    //REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, RTC_CNTL_DBIAS_1V20); //RTC_CNTL_DBIAS_HP_VOLT);

    DPORT_REG_WRITE(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL_240);
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL, RTC_CNTL_SOC_CLK_SEL_PLL);

    //WRITE_PERI_REG(RTC_APB_FREQ_REG, clk_val_to_reg_val(apb_freq >> 12));

    SystemCoreClock = 240000000;
  }
  else if (target_speed_hz >= 160000000)
  {
    rtc_clk_bbpll_configure(external_clock_hz, 320000000);

    //REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_80M_160M);

    DPORT_REG_WRITE(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL_160);
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL, RTC_CNTL_SOC_CLK_SEL_PLL);
    SystemCoreClock = 160000000;
  }
  else if (target_speed_hz >= 80000000)
  {
    rtc_clk_bbpll_configure(external_clock_hz, 320000000);

    //REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, RTC_CNTL_DBIAS_1V10);

    DPORT_REG_WRITE(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL_80);
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL, RTC_CNTL_SOC_CLK_SEL_PLL);
    SystemCoreClock = 80000000;
  }
  else // use the crystal frequency
  {
    rtc_clk_bbpll_configure(external_clock_hz, 320000000);

    //REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, RTC_CNTL_DBIAS_1V10);

    // keep the crystal freq
  }

  return true;
}

