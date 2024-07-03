/*
 *  file:     hwpins_am.cpp
 *  brief:    TI AM355x Pin/Pad and GPIO configuration
 *  date:     2024-06-29
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"
#include "hw_utils.h"

#include "stdio.h"
#include "stddef.h"

typedef struct
{
	uint16_t  confreg_offs;
	uint16_t  gpio_num;
//
} am_pad_item_t;

const am_pad_item_t  am_pads_by_gpio[] =
{
/* 0_00 */ { PAD_MDIO               },
/* 0_01 */ { PAD_MDC                },
/* 0_02 */ { PAD_SPI0_SCLK          },
/* 0_03 */ { PAD_SPI0_D0            },
/* 0_04 */ { PAD_SPI0_D1            },
/* 0_05 */ { PAD_SPI0_CS0           },
/* 0_06 */ { PAD_SPI0_CS1           },
/* 0_07 */ { PAD_ECAP0_IN_PWM0_OUT  },
/* 0_08 */ { PAD_LCD_DATA12         },
/* 0_09 */ { PAD_LCD_DATA13         },
/* 0_10 */ { PAD_LCD_DATA14         },
/* 0_11 */ { PAD_LCD_DATA15         },
/* 0_12 */ { PAD_UART1_CTSN         },
/* 0_13 */ { PAD_UART1_RTSN         },
/* 0_14 */ { PAD_UART1_RXD          },
/* 0_15 */ { PAD_UART1_TXD          },
/* 0_16 */ { PAD_MII1_TXD3          },
/* 0_17 */ { PAD_MII1_TXD2          },
/* 0_18 */ { PAD_USB0_DRVVBUS       },
/* 0_19 */ { PAD_XDMA_EVENT_INTR0   },
/* 0_20 */ { PAD_XDMA_EVENT_INTR1   },
/* 0_21 */ { PAD_MII1_TXD1          },
/* 0_22 */ { PAD_GPMC_AD8           },
/* 0_23 */ { PAD_GPMC_AD9           },
/* 0_24 */ { 0xFFFF, 0xFFFF         },
/* 0_25 */ { 0xFFFF, 0xFFFF         },
/* 0_26 */ { PAD_GPMC_AD10          },
/* 0_27 */ { PAD_GPMC_AD11          },
/* 0_28 */ { PAD_MII1_TXD0          },
/* 0_29 */ { PAD_RMII1_REF_CLK      },
/* 0_30 */ { PAD_GPMC_WAIT0         },
/* 0_31 */ { PAD_GPMC_WPN           },

//---------------------------------

/* 1_00 */ { PAD_GPMC_AD0           },
/* 1_01 */ { PAD_GPMC_AD1           },
/* 1_02 */ { PAD_GPMC_AD2           },
/* 1_03 */ { PAD_GPMC_AD3           },
/* 1_04 */ { PAD_GPMC_AD4           },
/* 1_05 */ { PAD_GPMC_AD5           },
/* 1_06 */ { PAD_GPMC_AD6           },
/* 1_07 */ { PAD_GPMC_AD7           },
/* 1_08 */ { PAD_UART0_CTSN         },
/* 1_09 */ { PAD_UART0_RTSN         },
/* 1_10 */ { PAD_UART0_RXD          },
/* 1_11 */ { PAD_UART0_TXD          },
/* 1_12 */ { PAD_GPMC_AD12          },
/* 1_13 */ { PAD_GPMC_AD13          },
/* 1_14 */ { PAD_GPMC_AD14          },
/* 1_15 */ { PAD_GPMC_AD15          },
/* 1_16 */ { PAD_GPMC_A0            },
/* 1_17 */ { PAD_GPMC_A1            },
/* 1_18 */ { PAD_GPMC_A2            },
/* 1_19 */ { PAD_GPMC_A3            },
/* 1_20 */ { PAD_GPMC_A4            },
/* 1_21 */ { PAD_GPMC_A5            },
/* 1_22 */ { PAD_GPMC_A6            },
/* 1_23 */ { PAD_GPMC_A7            },
/* 1_24 */ { PAD_GPMC_A8            },
/* 1_25 */ { PAD_GPMC_A9            },
/* 1_26 */ { PAD_GPMC_A10           },
/* 1_27 */ { PAD_GPMC_A11           },
/* 1_28 */ { PAD_GPMC_BEN1          },
/* 1_29 */ { PAD_GPMC_CSN0          },
/* 1_30 */ { PAD_GPMC_CSN1          },
/* 1_31 */ { PAD_GPMC_CSN2          },

//----------------------------------

/* 2_00 */ { PAD_GPMC_CSN3          },
/* 2_01 */ { PAD_GPMC_CLK           },
/* 2_02 */ { PAD_GPMC_ADVN_ALE      },
/* 2_03 */ { PAD_GPMC_OEN_REN       },
/* 2_04 */ { PAD_GPMC_WEN           },
/* 2_05 */ { PAD_GPMC_BEN0_CLE      },
/* 2_06 */ { PAD_LCD_DATA0          },
/* 2_07 */ { PAD_LCD_DATA1          },
/* 2_08 */ { PAD_LCD_DATA2          },
/* 2_09 */ { PAD_LCD_DATA3          },
/* 2_10 */ { PAD_LCD_DATA4          },
/* 2_11 */ { PAD_LCD_DATA5          },
/* 2_12 */ { PAD_LCD_DATA6          },
/* 2_13 */ { PAD_LCD_DATA7          },
/* 2_14 */ { PAD_LCD_DATA8          },
/* 2_15 */ { PAD_LCD_DATA9          },
/* 2_16 */ { PAD_LCD_DATA10         },
/* 2_17 */ { PAD_LCD_DATA11         },
/* 2_18 */ { PAD_MII1_RXD3          },
/* 2_19 */ { PAD_MII1_RXD2          },
/* 2_20 */ { PAD_MII1_RXD1          },
/* 2_21 */ { PAD_MII1_RXD0          },
/* 2_22 */ { PAD_LCD_VSYNC          },
/* 2_23 */ { PAD_LCD_HSYNC          },
/* 2_24 */ { PAD_LCD_PCLK           },
/* 2_25 */ { PAD_LCD_AC_BIAS_EN     },
/* 2_26 */ { PAD_MMC0_DAT3          },
/* 2_27 */ { PAD_MMC0_DAT2          },
/* 2_28 */ { PAD_MMC0_DAT1          },
/* 2_29 */ { PAD_MMC0_DAT0          },
/* 2_30 */ { PAD_MMC0_CLK           },
/* 2_31 */ { PAD_MMC0_CMD           },

//----------------------------------

/* 3_00 */ { PAD_MII1_COL           },
/* 3_01 */ { PAD_MII1_CRS           },
/* 3_02 */ { PAD_MII1_RX_ER         },
/* 3_03 */ { PAD_MII1_TX_EN         },
/* 3_04 */ { PAD_MII1_RX_DV         },
/* 3_05 */ { PAD_I2C0_SDA           },
/* 3_06 */ { PAD_I2C0_SCL           },
/* 3_07 */ { PAD_EMU0               },
/* 3_08 */ { PAD_EMU1               },
/* 3_09 */ { PAD_MII1_TX_CLK        },
/* 3_10 */ { PAD_MII1_RX_CLK        },
/* 3_11 */ { 0xFFFF, 0xFFFF         },
/* 3_12 */ { 0xFFFF, 0xFFFF         },
/* 3_13 */ { PAD_USB1_DRVVBUS       },
/* 3_14 */ { PAD_MCASP0_ACLKX       },
/* 3_15 */ { PAD_MCASP0_FSX         },
/* 3_16 */ { PAD_MCASP0_AXR0        },
/* 3_17 */ { PAD_MCASP0_AHCLKR      },
/* 3_18 */ { PAD_MCASP0_ACLKR       },
/* 3_19 */ { PAD_MCASP0_FSR         },
/* 3_20 */ { PAD_MCASP0_AXR1        },
/* 3_21 */ { PAD_MCASP0_AHCLKX      },
/* 3_22 */ { 0xFFFF, 0xFFFF         },
/* 3_23 */ { 0xFFFF, 0xFFFF         },
/* 3_24 */ { 0xFFFF, 0xFFFF         },
/* 3_25 */ { 0xFFFF, 0xFFFF         },
/* 3_26 */ { 0xFFFF, 0xFFFF         },
/* 3_27 */ { 0xFFFF, 0xFFFF         },
/* 3_28 */ { 0xFFFF, 0xFFFF         },
/* 3_29 */ { 0xFFFF, 0xFFFF         },
/* 3_30 */ { 0xFFFF, 0xFFFF         },
/* 3_31 */ { 0xFFFF, 0xFFFF         },
};


THwGpioRegs * THwPinCtrl_am::GetGpioRegs(int aportnum)
{
	if ((aportnum < 0) || (aportnum >= HW_GPIO_COUNT))
	{
		return nullptr;
	}

	THwGpioRegs * regs = mapped_gpio_regs[aportnum];
	if (!regs)
	{
		uint32_t baddr[HW_GPIO_COUNT] = {HW_GPIO0_BASE, HW_GPIO1_BASE, HW_GPIO2_BASE, HW_GPIO3_BASE};
		regs = (THwGpioRegs *)hw_memmap(baddr[aportnum], sizeof(THwGpioRegs));
		mapped_gpio_regs[aportnum] = regs;
	}

	return regs;
}

uint32_t * THwPinCtrl_am::GetConfReg(uint32_t aconfreg_offs)
{
	if (aconfreg_offs >= 0xFFF)
	{
		return nullptr;
	}

	if (!mm_cmodule)
	{
		mm_cmodule = (uint8_t *)hw_memmap(CONTROL_MODULE_BASE, 128 * 1024);
	}

	return (uint32_t *)&mm_cmodule[aconfreg_offs];
}

bool THwPinCtrl_am::PadSetup(uint32_t aconfreg_offs, uint32_t agpio, unsigned flags)
{
	uint32_t * confreg = GetConfReg(aconfreg_offs);
	if (!confreg)
	{
		return false;
	}

  uint32_t padcfg = (0
    | (0  <<  6)  // SLEWCTRL: 0 = Fast, 1 = Slow
    | (0  <<  5)  // RX_ACTIVE: 0 = receiver disabled, 1 = receiver enabled
    | (0  <<  4)  // PUTYPESEL: 0 = pulldown, 1 = pullup
    | (0  <<  3)  // PUDEN: 0 = enable pullup/pulldown, 1 = disable pullup/pulldown
    | (0  <<  0)  // MMODE(3): af function select
  );

  if (flags & PINCFG_PULLUP)
  {
    padcfg |= (1 << 4);
  }

  if (flags & PINCFG_PULLDOWN)
  {
    // leave it as is
  }

  if (0 == (flags & (PINCFG_PULLDOWN | PINCFG_PULLUP)))
  {
    padcfg |= (1 << 3);  // disable pulldown
  }

  padcfg |= (1 << 5); // activate RX (input)

  if (flags & PINCFG_SPEED_SLOW)
  {
    padcfg |= (1 << 6);
  }

  if (flags & PINCFG_AF_MASK)
  {
    padcfg |= ((flags >> PINCFG_AF_SHIFT) & 0x7);
  }
  else // GPIO
  {
  	THwGpioRegs * regs = GetGpioRegs(agpio >> 8);
    if (regs)
    {
      uint32_t pinmask = (1 << (agpio & 0x1F));
      if (flags & PINCFG_OUTPUT)
      {
        if (flags & PINCFG_GPIO_INIT_1)
        {
          regs->SETDATAOUT = pinmask;
        }
        else
        {
          regs->CLEARDATAOUT = pinmask;
        }
        regs->OE &= ~pinmask;
      }
      else
      {
        regs->OE |= pinmask;
      }

      padcfg |= 7; // AF7 = GPIO
    }
  }

  *confreg = padcfg;

  return true;
}

bool THwPinCtrl_am::PinSetup(int aportnum, int apinnum, unsigned flags)
{
	if ((aportnum < 0) || (aportnum >= HW_GPIO_COUNT))
	{
		return false;
	}

	const am_pad_item_t *  pitem = &am_pads_by_gpio[(aportnum << 5) + (apinnum & 0x1F)];
	if (pitem->gpio_num != ((aportnum << 8) + apinnum))
	{
		return false;
	}

	return PadSetup(pitem->confreg_offs, pitem->gpio_num, flags);
}

bool THwPinCtrl_am::GpioPortEnable(int aportnum)
{
  return true;
}

void THwPinCtrl_am::GpioSet(int aportnum, int apinnum, int value)
{
	THwGpioRegs * regs = GetGpioRegs(aportnum); // set the regs member
	if (!regs)
	{
		return;
	}

  if (1 == value)
  {
  	regs->SETDATAOUT = (1 << apinnum);
  }
  else if (value & 2) // toggle
  {
  	if (regs->DATAOUT & (1 << apinnum))
  	{
  		regs->CLEARDATAOUT = (1 << apinnum);
  	}
  	else
  	{
    	regs->SETDATAOUT = (1 << apinnum);
  	}
  }
  else
  {
  	regs->CLEARDATAOUT = (1 << apinnum);
  }
}


// GPIO Pin

void TGpioPin_am::Assign(int aportnum, int apinnum, bool ainvert)
{
	portnum = aportnum;
  pinnum = apinnum;
  inverted = ainvert;

	regs = hwpinctrl.GetGpioRegs(aportnum);
	if (!regs)
	{
		return;
	}

	if ((apinnum < 0) || (apinnum > 31))
	{
		regs = nullptr;
		return;
	}

  getbitshift = apinnum;
	setbitvalue = (1 << apinnum);
	clrbitvalue = setbitvalue;

  getbitptr = (unsigned *)&(regs->DATAIN);
  getoutbitptr = (unsigned *)&(regs->DATAOUT);

  if (ainvert)
  {
    setbitptr = (unsigned *)&(regs->CLEARDATAOUT);
    clrbitptr = (unsigned *)&(regs->SETDATAOUT);
  }
  else
  {
    setbitptr = (unsigned *)&(regs->SETDATAOUT);
    clrbitptr = (unsigned *)&(regs->CLEARDATAOUT);
  }
}

void TGpioPin_am::Toggle()
{
	// generic code without hw toggle support:
	if (Value())
	{
		Set0();
	}
	else
	{
		Set1();
	}
}

void TGpioPin_am::SwitchDirection(int adirection)
{
	if (adirection)
	{
		regs->OE &= ~setbitvalue;
	}
	else
	{
		regs->OE |= setbitvalue;
	}
}
