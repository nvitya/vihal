/*
 *  file:     am355x.h
 *  brief:    AM355x definitions
 *  date:     2024-06-29
 *  authors:  nvitya
*/

#ifndef AM355X_H_
#define AM355X_H_

#include "stdint.h"

//                             CONF    GPIO
#define PAD_GPMC_AD0           0x800,  (0x100 +  0)
#define PAD_GPMC_AD1           0x804,  (0x100 +  1)
#define PAD_GPMC_AD2           0x808,  (0x100 +  2)
#define PAD_GPMC_AD3           0x80C,  (0x100 +  3)
#define PAD_GPMC_AD4           0x810,  (0x100 +  4)
#define PAD_GPMC_AD5           0x814,  (0x100 +  5)
#define PAD_GPMC_AD6           0x818,  (0x100 +  6)
#define PAD_GPMC_AD7           0x81C,  (0x100 +  7)
#define PAD_GPMC_AD8           0x820,  (0x000 + 22)
#define PAD_GPMC_AD9           0x824,  (0x000 + 23)
#define PAD_GPMC_AD10          0x828,  (0x000 + 26)
#define PAD_GPMC_AD11          0x82C,  (0x000 + 27)
#define PAD_GPMC_AD12          0x830,  (0x100 + 12)
#define PAD_GPMC_AD13          0x834,  (0x100 + 13)
#define PAD_GPMC_AD14          0x838,  (0x100 + 14)
#define PAD_GPMC_AD15          0x83C,  (0x100 + 15)
#define PAD_GPMC_A0            0x840,  (0x100 + 16)
#define PAD_GPMC_A1            0x844,  (0x100 + 17)
#define PAD_GPMC_A2            0x848,  (0x100 + 18)
#define PAD_GPMC_A3            0x84C,  (0x100 + 19)
#define PAD_GPMC_A4            0x850,  (0x100 + 20)
#define PAD_GPMC_A5            0x854,  (0x100 + 21)
#define PAD_GPMC_A6            0x858,  (0x100 + 22)
#define PAD_GPMC_A7            0x85C,  (0x100 + 23)
#define PAD_GPMC_A8            0x860,  (0x100 + 24)
#define PAD_GPMC_A9            0x864,  (0x100 + 25)
#define PAD_GPMC_A10           0x868,  (0x100 + 26)
#define PAD_GPMC_A11           0x86C,  (0x100 + 27)
#define PAD_GPMC_WAIT0         0x870,  (0x000 + 30)
#define PAD_GPMC_WPN           0x874,  (0x000 + 31)
#define PAD_GPMC_BEN1          0x878,  (0x100 + 28)
#define PAD_GPMC_CSN0          0x87C,  (0x100 + 29)
#define PAD_GPMC_CSN1          0x880,  (0x100 + 30)
#define PAD_GPMC_CSN2          0x884,  (0x100 + 31)
#define PAD_GPMC_CSN3          0x888,  (0x200 +  0)
#define PAD_GPMC_CLK           0x88C,  (0x200 +  1)
#define PAD_GPMC_ADVN_ALE      0x890,  (0x200 +  2)
#define PAD_GPMC_OEN_REN       0x894,  (0x200 +  3)
#define PAD_GPMC_WEN           0x898,  (0x200 +  4)
#define PAD_GPMC_BEN0_CLE      0x89C,  (0x200 +  5)
#define PAD_LCD_DATA0          0x8A0,  (0x200 +  6)
#define PAD_LCD_DATA1          0x8A4,  (0x200 +  7)
#define PAD_LCD_DATA2          0x8A8,  (0x200 +  8)
#define PAD_LCD_DATA3          0x8AC,  (0x200 +  9)
#define PAD_LCD_DATA4          0x8B0,  (0x200 + 10)
#define PAD_LCD_DATA5          0x8B4,  (0x200 + 11)
#define PAD_LCD_DATA6          0x8B8,  (0x200 + 12)
#define PAD_LCD_DATA7          0x8BC,  (0x200 + 13)
#define PAD_LCD_DATA8          0x8C0,  (0x200 + 14)
#define PAD_LCD_DATA9          0x8C4,  (0x200 + 15)
#define PAD_LCD_DATA10         0x8C8,  (0x200 + 16)
#define PAD_LCD_DATA11         0x8CC,  (0x200 + 17)
#define PAD_LCD_DATA12         0x8D0,  (0x000 +  8)
#define PAD_LCD_DATA13         0x8D4,  (0x000 +  9)
#define PAD_LCD_DATA14         0x8D8,  (0x000 + 10)
#define PAD_LCD_DATA15         0x8DC,  (0x000 + 11)
#define PAD_LCD_VSYNC          0x8E0,  (0x200 + 22)
#define PAD_LCD_HSYNC          0x8E4,  (0x200 + 23)
#define PAD_LCD_PCLK           0x8E8,  (0x200 + 24)
#define PAD_LCD_AC_BIAS_EN     0x8EC,  (0x200 + 25)
#define PAD_MMC0_DAT3          0x8F0,  (0x200 + 26)
#define PAD_MMC0_DAT2          0x8F4,  (0x200 + 27)
#define PAD_MMC0_DAT1          0x8F8,  (0x200 + 28)
#define PAD_MMC0_DAT0          0x8FC,  (0x200 + 29)
#define PAD_MMC0_CLK           0x900,  (0x200 + 30)
#define PAD_MMC0_CMD           0x904,  (0x200 + 31)
#define PAD_MII1_COL           0x908,  (0x300 +  0)
#define PAD_MII1_CRS           0x90C,  (0x300 +  1)
#define PAD_MII1_RX_ER         0x910,  (0x300 +  2)
#define PAD_MII1_TX_EN         0x914,  (0x300 +  3)
#define PAD_MII1_RX_DV         0x918,  (0x300 +  4)
#define PAD_MII1_TXD3          0x91C,  (0x000 + 16)
#define PAD_MII1_TXD2          0x920,  (0x000 + 17)
#define PAD_MII1_TXD1          0x924,  (0x000 + 21)
#define PAD_MII1_TXD0          0x928,  (0x000 + 28)
#define PAD_MII1_TX_CLK        0x92C,  (0x300 +  9)
#define PAD_MII1_RX_CLK        0x930,  (0x300 + 10)
#define PAD_MII1_RXD3          0x934,  (0x200 + 18)
#define PAD_MII1_RXD2          0x938,  (0x200 + 19)
#define PAD_MII1_RXD1          0x93C,  (0x200 + 20)
#define PAD_MII1_RXD0          0x940,  (0x200 + 21)
#define PAD_RMII1_REF_CLK      0x944,  (0x000 + 29)
#define PAD_MDIO               0x948,  (0x000 +  0)
#define PAD_MDC                0x94C,  (0x000 +  1)
#define PAD_SPI0_SCLK          0x950,  (0x000 +  2)
#define PAD_SPI0_D0            0x954,  (0x000 +  3)
#define PAD_SPI0_D1            0x958,  (0x000 +  4)
#define PAD_SPI0_CS0           0x95C,  (0x000 +  5)
#define PAD_SPI0_CS1           0x960,  (0x000 +  6)
#define PAD_ECAP0_IN_PWM0_OUT  0x964,  (0x000 +  7)
#define PAD_UART0_CTSN         0x968,  (0x100 +  8)
#define PAD_UART0_RTSN         0x96C,  (0x100 +  9)
#define PAD_UART0_RXD          0x970,  (0x100 + 10)
#define PAD_UART0_TXD          0x974,  (0x100 + 11)
#define PAD_UART1_CTSN         0x978,  (0x000 + 12)
#define PAD_UART1_RTSN         0x97C,  (0x000 + 13)
#define PAD_UART1_RXD          0x980,  (0x000 + 14)
#define PAD_UART1_TXD          0x984,  (0x000 + 15)
#define PAD_I2C0_SDA           0x988,  (0x300 +  5)
#define PAD_I2C0_SCL           0x98C,  (0x300 +  6)
#define PAD_MCASP0_ACLKX       0x990,  (0x300 + 14)
#define PAD_MCASP0_FSX         0x994,  (0x300 + 15)
#define PAD_MCASP0_AXR0        0x998,  (0x300 + 16)
#define PAD_MCASP0_AHCLKR      0x99C,  (0x300 + 17)
#define PAD_MCASP0_ACLKR       0x9A0,  (0x300 + 18)
#define PAD_MCASP0_FSR         0x9A4,  (0x300 + 19)
#define PAD_MCASP0_AXR1        0x9A8,  (0x300 + 20)
#define PAD_MCASP0_AHCLKX      0x9AC,  (0x300 + 21)
#define PAD_XDMA_EVENT_INTR0   0x9B0,  (0x000 + 19)
#define PAD_XDMA_EVENT_INTR1   0x9B4,  (0x000 + 20)
#define PAD_WARMRSTN           0x9B8,  0xFFF
#define PAD_NNMI               0x9C0,  0xFFF
#define PAD_TMS                0x9D0,  0xFFF
#define PAD_TDI                0x9D4,  0xFFF
#define PAD_TDO                0x9D8,  0xFFF
#define PAD_TCK                0x9DC,  0xFFF
#define PAD_TRSTN              0x9E0,  0xFFF
#define PAD_EMU0               0x9E4,  (0x300 +  7)
#define PAD_EMU1               0x9E8,  (0x300 +  8)
#define PAD_RTC_PWRONRSTN      0x9F8,  0xFFF
#define PAD_PMIC_POWER_EN      0x9FC,  0xFFF
#define PAD_EXT_WAKEUP         0xA00,  0xFFF
#define PAD_USB0_DRVVBUS       0xA1C,  (0x000 + 18)
#define PAD_USB1_DRVVBUS       0xA34,  (0x300 + 13)

//----------------------------------------------------------------------------------

#define HW_GPIO_COUNT         4
#define HW_GPIO0_BASE         0x44E07000
#define HW_GPIO1_BASE         0x4804C000
#define HW_GPIO2_BASE         0x481AC000
#define HW_GPIO3_BASE         0x481AE000


#define HW_ECAP0_BASE         0x48300100
#define HW_ECAP1_BASE         0x48302100
#define HW_ECAP2_BASE         0x48304100

#define HW_MCSPI0_BASE        0x48030000
#define HW_MCSPI1_BASE        0x481A0000

#define HW_CM_PER_BASE        0x44E00000
#define CM_PER_REG_OFFS_MCSPI0  0x4C
#define CM_PER_REG_OFFS_MCSPI1  0x50

#define CONTROL_MODULE_BASE   0x44E10000

// clocks

#define CLOCKCNT_SPEED           1000000
#define HWUART_BASE_CLOCK       48000000

#endif /* AM355X_H_ */
