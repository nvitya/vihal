/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     hwpins_am.cpp
 *  brief:    AM Pin/Pad and GPIO configuration
 *  date:     2024-06-15
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"

#define MAX_PORT_NUMBER   2

typedef struct
{
  uint32_t   padcfg_reg;
  uint32_t   gpio;
//
} gpio_pad_table_item_t;

const gpio_pad_table_item_t  gpio_pad_table[] =
{
  { PAD_ADC0_AIN0        },
  { PAD_ADC0_AIN1        },
  { PAD_ADC0_AIN2        },
  { PAD_ADC0_AIN3        },
  { PAD_ADC0_AIN4        },
  { PAD_ADC0_AIN5        },
  { PAD_ADC0_AIN6        },
  { PAD_ADC0_AIN7        },
  { PAD_OSPI0_CLK        },
  { PAD_OSPI0_LBCLKO     },
  { PAD_OSPI0_DQS        },
  { PAD_OSPI0_D0         },
  { PAD_OSPI0_D1         },
  { PAD_OSPI0_D2         },
  { PAD_OSPI0_D3         },
  { PAD_OSPI0_D4         },
  { PAD_OSPI0_D5         },
  { PAD_OSPI0_D6         },
  { PAD_OSPI0_D7         },
  { PAD_OSPI0_CSN0       },
  { PAD_OSPI0_CSN1       },
  { PAD_OSPI0_CSN2       },
  { PAD_OSPI0_CSN3       },
  { PAD_GPMC0_AD0        },
  { PAD_GPMC0_AD1        },
  { PAD_GPMC0_AD2        },
  { PAD_GPMC0_AD3        },
  { PAD_GPMC0_AD4        },
  { PAD_GPMC0_AD5        },
  { PAD_GPMC0_AD6        },
  { PAD_GPMC0_AD7        },
  { PAD_GPMC0_AD8        },
  { PAD_GPMC0_AD9        },
  { PAD_GPMC0_AD10       },
  { PAD_GPMC0_AD11       },
  { PAD_GPMC0_AD12       },
  { PAD_GPMC0_AD13       },
  { PAD_GPMC0_AD14       },
  { PAD_GPMC0_AD15       },
  { PAD_GPMC0_CLK        },
  { PAD_GPMC0_ADVN_ALE   },
  { PAD_GPMC0_OEN_REN    },
  { PAD_GPMC0_WEN        },
  { PAD_GPMC0_BE0N_CLE   },
  { PAD_GPMC0_BE1N       },
  { PAD_GPMC0_WAIT0      },
  { PAD_GPMC0_WAIT1      },
  { PAD_GPMC0_WPN        },
  { PAD_GPMC0_DIR        },
  { PAD_GPMC0_CSN0       },
  { PAD_GPMC0_CSN1       },
  { PAD_GPMC0_CSN2       },
  { PAD_GPMC0_CSN3       },

  { PAD_PRG1_PRU0_GPO0   },
  { PAD_PRG1_PRU0_GPO1   },
  { PAD_PRG1_PRU0_GPO2   },
  { PAD_PRG1_PRU0_GPO3   },
  { PAD_PRG1_PRU0_GPO4   },
  { PAD_PRG1_PRU0_GPO5   },
  { PAD_PRG1_PRU0_GPO6   },
  { PAD_PRG1_PRU0_GPO7   },
  { PAD_PRG1_PRU0_GPO8   },
  { PAD_PRG1_PRU0_GPO9   },
  { PAD_PRG1_PRU0_GPO10  },
  { PAD_PRG1_PRU0_GPO11  },
  { PAD_PRG1_PRU0_GPO12  },
  { PAD_PRG1_PRU0_GPO13  },
  { PAD_PRG1_PRU0_GPO14  },
  { PAD_PRG1_PRU0_GPO15  },
  { PAD_PRG1_PRU0_GPO16  },
  { PAD_PRG1_PRU0_GPO17  },
  { PAD_PRG1_PRU0_GPO18  },
  { PAD_PRG1_PRU0_GPO19  },
  { PAD_PRG1_PRU1_GPO0   },
  { PAD_PRG1_PRU1_GPO1   },
  { PAD_PRG1_PRU1_GPO2   },
  { PAD_PRG1_PRU1_GPO3   },
  { PAD_PRG1_PRU1_GPO4   },
  { PAD_PRG1_PRU1_GPO5   },
  { PAD_PRG1_PRU1_GPO6   },
  { PAD_PRG1_PRU1_GPO7   },
  { PAD_PRG1_PRU1_GPO8   },
  { PAD_PRG1_PRU1_GPO9   },
  { PAD_PRG1_PRU1_GPO10  },
  { PAD_PRG1_PRU1_GPO11  },
  { PAD_PRG1_PRU1_GPO12  },
  { PAD_PRG1_PRU1_GPO13  },
  { PAD_PRG1_PRU1_GPO14  },
  { PAD_PRG1_PRU1_GPO15  },
  { PAD_PRG1_PRU1_GPO16  },
  { PAD_PRG1_PRU1_GPO17  },
  { PAD_PRG1_PRU1_GPO18  },
  { PAD_PRG1_PRU1_GPO19  },

  { PAD_PRG1_MDIO0_MDIO  },
  { PAD_PRG1_MDIO0_MDC   },
  { PAD_PRG0_PRU0_GPO0   },
  { PAD_PRG0_PRU0_GPO1   },
  { PAD_PRG0_PRU0_GPO2   },
  { PAD_PRG0_PRU0_GPO3   },
  { PAD_PRG0_PRU0_GPO4   },
  { PAD_PRG0_PRU0_GPO5   },
  { PAD_PRG0_PRU0_GPO6   },
  { PAD_PRG0_PRU0_GPO7   },
  { PAD_PRG0_PRU0_GPO8   },
  { PAD_PRG0_PRU0_GPO9   },
  { PAD_PRG0_PRU0_GPO10  },
  { PAD_PRG0_PRU0_GPO11  },
  { PAD_PRG0_PRU0_GPO12  },
  { PAD_PRG0_PRU0_GPO13  },
  { PAD_PRG0_PRU0_GPO14  },
  { PAD_PRG0_PRU0_GPO15  },
  { PAD_PRG0_PRU0_GPO16  },
  { PAD_PRG0_PRU0_GPO17  },
  { PAD_PRG0_PRU0_GPO18  },
  { PAD_PRG0_PRU0_GPO19  },
  { PAD_PRG0_PRU1_GPO0   },
  { PAD_PRG0_PRU1_GPO1   },
  { PAD_PRG0_PRU1_GPO2   },
  { PAD_PRG0_PRU1_GPO3   },
  { PAD_PRG0_PRU1_GPO4   },
  { PAD_PRG0_PRU1_GPO5   },
  { PAD_PRG0_PRU1_GPO6   },
  { PAD_PRG0_PRU1_GPO7   },
  { PAD_PRG0_PRU1_GPO8   },
  { PAD_PRG0_PRU1_GPO9   },
  { PAD_PRG0_PRU1_GPO10  },
  { PAD_PRG0_PRU1_GPO11  },
  { PAD_PRG0_PRU1_GPO12  },
  { PAD_PRG0_PRU1_GPO13  },
  { PAD_PRG0_PRU1_GPO14  },
  { PAD_PRG0_PRU1_GPO15  },
  { PAD_PRG0_PRU1_GPO16  },
  { PAD_PRG0_PRU1_GPO17  },
  { PAD_PRG0_PRU1_GPO18  },
  { PAD_PRG0_PRU1_GPO19  },

  { PAD_PRG0_MDIO0_MDIO   },
  { PAD_PRG0_MDIO0_MDC    },
  { PAD_SPI0_CS0          },
  { PAD_SPI0_CS1          },
  { PAD_SPI0_CLK          },
  { PAD_SPI0_D0           },
  { PAD_SPI0_D1           },
  { PAD_SPI1_CS0          },
  { PAD_SPI1_CS1          },
  { PAD_SPI1_CLK          },
  { PAD_SPI1_D0           },
  { PAD_SPI1_D1           },
  { PAD_UART0_RXD         },
  { PAD_UART0_TXD         },
  { PAD_UART0_CTSN        },
  { PAD_UART0_RTSN        },
  { PAD_UART1_RXD         },
  { PAD_UART1_TXD         },
  { PAD_UART1_CTSN        },
  { PAD_UART1_RTSN        },

  { PAD_MCAN0_TX          },
  { PAD_MCAN0_RX          },
  { PAD_MCAN1_TX          },
  { PAD_MCAN1_RX          },
  { PAD_I2C0_SCL          },
  { PAD_I2C0_SDA          },
  { PAD_I2C1_SCL          },
  { PAD_I2C1_SDA          },
  { PAD_ECAP0_IN_APWM_OUT },
  { PAD_EXT_REFCLK1       },
  { PAD_EXTINTN           },
  { PAD_MMC1_DAT3         },
  { PAD_MMC1_DAT2         },
  { PAD_MMC1_DAT1         },
  { PAD_MMC1_DAT0         },
  { PAD_MMC1_CLK          },
  { PAD_MMC1_LB           },
  { PAD_MMC1_CMD          },
  { PAD_MMC1_SDCD         },
  { PAD_MMC1_SDWP         },
  { PAD_RESET_REQZ        },
  { PAD_RESETSTATZ        },
  { PAD_USB0_DRVVBUS      },
  { PAD_PORZ_OUT          },

  { PAD_MCU_SPI1_CS0      },
  { PAD_MCU_I2C0_SCL      },
  { PAD_MCU_I2C0_SDA      },
  { PAD_MCU_I2C1_SCL      },
  { PAD_MCU_I2C1_SDA      },
  { PAD_MCU_SPI0_CLK      },
  { PAD_MCU_SPI0_CS0      },
  { PAD_MCU_SPI0_CS1      },
  { PAD_MCU_SPI0_D0       },
  { PAD_MCU_SPI0_D1       },
  { PAD_MCU_SPI1_CLK      },
  { PAD_MCU_SPI1_CS1      },
  { PAD_MCU_SPI1_D0       },
  { PAD_MCU_SPI1_D1       },
  { PAD_MCU_PORZ          },
  { PAD_MCU_RESETSTATZ    },
  { PAD_MCU_RESETZ        },
  { PAD_MCU_SAFETY_ERRORN },
  { PAD_MCU_UART0_CTSN    },
  { PAD_MCU_UART0_RTSN    },
  { PAD_MCU_UART0_RXD     },
  { PAD_MCU_UART0_TXD     },
  { PAD_MCU_UART1_CTSN    },
  { PAD_MCU_UART1_RTSN    },
  { PAD_MCU_UART1_RXD     },
  { PAD_MCU_UART1_TXD     },

  { 0, 0 } // close the table
};

GPIO_Unit_Regs * THwPinCtrl_am::GetGpioUnit(int aportnum)
{
  if (PORTNUM_MCU == aportnum)
  {
    return (GPIO_Unit_Regs *)(SDL_MCU_GPIO0_BASE);
  }
  else if (0 == aportnum)
  {
    return (GPIO_Unit_Regs *)(SDL_GPIO0_BASE);
  }
  else if (1 == aportnum)
  {
    return (GPIO_Unit_Regs *)(SDL_GPIO1_BASE);
  }
  else
  {
		return nullptr;
	}
}

GPIO_Bank_Regs * THwPinCtrl_am::GetGpioBankRegs(GPIO_Unit_Regs * aunit, int apinnum)
{
  return &(aunit->BANK_REGISTERS[apinnum >> 5]);
}

GPIO_Bank_Regs * THwPinCtrl_am::GetGpioBankRegs(int aportnum, int apinnum)
{
  GPIO_Unit_Regs * uregs = GetGpioUnit(aportnum);
  if (!uregs)
  {
    return nullptr;
  }
  return &(uregs->BANK_REGISTERS[apinnum >> 5]);
}

#define KICK0_UNLOCK_VAL                        (0x68EF3490U)
#define KICK1_UNLOCK_VAL                        (0xD172BC5AU)

#define PADCONFIG_LOCK0_KICK0_OFFSET   (0x1008)
#define PADCONFIG_LOCK1_KICK0_OFFSET   (0x5008)


void THwPinCtrl_am::UnlockPadConfig(volatile uint32_t * apadctlreg)
{
  uint32_t baseaddr = (uint32_t(apadctlreg) & 0xFFFF0000);

  volatile uint32_t * reg;

  // Lock 0
  reg = (volatile uint32_t *)(baseaddr + PADCONFIG_LOCK0_KICK0_OFFSET);
  if (0 == (*reg & 1))
  {
    *reg++ = KICK0_UNLOCK_VAL;   /* KICK 0 */
    *reg++ = KICK1_UNLOCK_VAL;   /* KICK 1 */
  }

  // Lock 1
  reg = (volatile uint32_t *)(baseaddr + PADCONFIG_LOCK1_KICK0_OFFSET);
  if (0 == (*reg & 1))
  {
    *reg++ = KICK0_UNLOCK_VAL;   /* KICK 0 */
    *reg++ = KICK1_UNLOCK_VAL;   /* KICK 1 */
  }
}

bool THwPinCtrl_am::PadSetup(uint32_t apadctl, uint32_t agpio, unsigned flags)
{
  volatile uint32_t * reg_padctl  = (volatile uint32_t *)(apadctl);

  UnlockPadConfig(reg_padctl);

  uint32_t padcfg = (0
    | (0  << 21)  // TX_DIS: 1 = disable output
    | (0  << 19)  // DRV_STR(2): drive strenght
    | (0  << 18)  // RX_ACTIVE: 1 = receiver enabled
    | (0  << 17)  // PULLTYPESEL: 0 = pulldown, 1 = pullup
    | (0  << 16)  // PULLUDEN: 0 = enable pullup/pulldown, 1 = disable pullup/pulldown
    | (0  << 14)  // ST_EN: 1 = enable Smitt trigger
    | (0  << 11)  // DEBOUNCE_SEL(2)
    | (0  <<  0)  // MUXMODE(4): af function select
  );

  if (flags & PINCFG_PULLUP)
  {
    padcfg |= (1 << 17);
  }

  if (flags & PINCFG_PULLDOWN)
  {
    // leave it as is
  }

  if (0 == (flags & (PINCFG_PULLDOWN | PINCFG_PULLUP)))
  {
    padcfg |= (1 << 16);  // disable pulldown
  }

  if (0 == (flags & PINCFG_ANALOGUE))
  {
    padcfg |= (1 << 14);  // activate the Schmitt trigger
  }

  padcfg |= (1 << 18); // activate RX (input)

  if (flags & PINCFG_AF_MASK)
  {
    padcfg |= ((flags >> PINCFG_AF_SHIFT) & 0xF);
  }
  else // GPIO
  {
    GPIO_Bank_Regs * regs = GetGpioBankRegs(agpio >> 8, agpio & 0xFF);
    if (regs)
    {
      uint32_t pinmask = (1 << (agpio & 0x1F));
      if (flags & PINCFG_OUTPUT)
      {
        if (flags & PINCFG_GPIO_INIT_1)
        {
          regs->SET_DATA = pinmask;
        }
        else
        {
          regs->CLR_DATA = pinmask;
        }
        regs->DIR &= ~pinmask;
      }
      else
      {
        regs->DIR |= pinmask;
        //padcfg |= (1 << 21); // disable output
      }

      padcfg |= 7; // AF7 = GPIO
    }
  }

  *reg_padctl = padcfg;

  return true;
}

bool THwPinCtrl_am::PinSetup(int aportnum, int apinnum, unsigned flags)
{
  const gpio_pad_table_item_t * pitem = &gpio_pad_table[0];
  uint32_t gpio_code = aportnum * 0x100 + apinnum;
  while (pitem->padcfg_reg | pitem->gpio)
  {
    if (pitem->gpio == gpio_code)
    {
      PadSetup(pitem->padcfg_reg, pitem->gpio, flags);
      return true;
    }
    ++pitem;
  }
  return false;
}

bool THwPinCtrl_am::GpioPortEnable(int aportnum)
{
  return true;
}

void THwPinCtrl_am::GpioSet(int aportnum, int apinnum, int value)
{
	GPIO_Bank_Regs * regs = GetGpioBankRegs(aportnum, apinnum);
	if (!regs)
	{
	  return;
	}

	uint32_t pinmask = (1 << apinnum);
  if (1 == value)
  {
    regs->SET_DATA = pinmask;
  }
  else if (value & 2) // toggle
  {
    if (regs->OUT_DATA & pinmask)
    {
      regs->CLR_DATA = pinmask;
    }
    else
    {
      regs->SET_DATA = pinmask;
    }
  }
  else
  {
    regs->CLR_DATA = pinmask;
  }
}

// GPIO Port

void TGpioPort_am::Assign(int aportnum)
{
  regs = hwpinctrl.GetGpioBankRegs(aportnum, 0);
}

void TGpioPort_am::Assign(int aportnum, int apinnum)
{
	regs = hwpinctrl.GetGpioBankRegs(aportnum, apinnum);
}

void TGpioPort_am::Set(unsigned value)
{
	regs->OUT_DATA = value;
}

// GPIO Pin

void TGpioPin_am::Assign(int aportnum, int apinnum, bool ainvert)
{
	portnum = aportnum;
  pinnum = apinnum;
  inverted = ainvert;
  bitmask = (1 << (pinnum & 0x1F));

	regs = hwpinctrl.GetGpioBankRegs(aportnum, apinnum);
	if (!regs)
	{
		return;
	}

	setbitvalue = bitmask;
	clrbitvalue = bitmask;

  getbitptr    = (unsigned *)&(regs->IN_DATA);
  getoutbitptr = (unsigned *)&(regs->OUT_DATA);
  getbitshift = pinnum;

  if (ainvert)
  {
    setbitptr = (unsigned *)&(regs->CLR_DATA);
    clrbitptr = (unsigned *)&(regs->SET_DATA);
  }
  else
  {
    setbitptr = (unsigned *)&(regs->SET_DATA);
    clrbitptr = (unsigned *)&(regs->CLR_DATA);
  }
}

void TGpioPin_am::Toggle()
{
  if (*getoutbitptr & bitmask)
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
		regs->DIR &= ~(bitmask); // output enable
	}
	else
	{
    regs->DIR |= (bitmask); // output disable
	}
}
