/*
 * vn_pinmux.h
 *
 *  Created on: Jun 16, 2024
 *      Author: vitya
 */

#ifndef AM24_VN_PINMUX_H_
#define AM24_VN_PINMUX_H_

// Main Domain

#define PAD_GPIO_NA   0xFFF

//                                  PADCTL       GPIO
#define PAD_ADC0_AIN0               0x00F42B0,   PAD_GPIO_NA
#define PAD_ADC0_AIN1               0x00F42B4,   PAD_GPIO_NA
#define PAD_ADC0_AIN2               0x00F42B8,   PAD_GPIO_NA
#define PAD_ADC0_AIN3               0x00F42BC,   PAD_GPIO_NA
#define PAD_ADC0_AIN4               0x00F42C0,   PAD_GPIO_NA
#define PAD_ADC0_AIN5               0x00F42C4,   PAD_GPIO_NA
#define PAD_ADC0_AIN6               0x00F42C8,   PAD_GPIO_NA
#define PAD_ADC0_AIN7               0x00F42CC,   PAD_GPIO_NA

#define PAD_OSPI0_CLK               0x00F4000,   (0x000 +  0)
#define PAD_OSPI0_LBCLKO            0x00F4004,   (0x000 +  1)
#define PAD_OSPI0_DQS               0x00F4008,   (0x000 +  2)
#define PAD_OSPI0_D0                0x00F400C,   (0x000 +  3)
#define PAD_OSPI0_D1                0x00F4010,   (0x000 +  4)
#define PAD_OSPI0_D2                0x00F4014,   (0x000 +  5)
#define PAD_OSPI0_D3                0x00F4018,   (0x000 +  6)
#define PAD_OSPI0_D4                0x00F401C,   (0x000 +  7)
#define PAD_OSPI0_D5                0x00F4020,   (0x000 +  8)
#define PAD_OSPI0_D6                0x00F4024,   (0x000 +  9)
#define PAD_OSPI0_D7                0x00F4028,   (0x000 + 10)
#define PAD_OSPI0_CSN0              0x00F402C,   (0x000 + 11)
#define PAD_OSPI0_CSN1              0x00F4030,   (0x000 + 12)
#define PAD_OSPI0_CSN2              0x00F4034,   (0x000 + 13)
#define PAD_OSPI0_CSN3              0x00F4038,   (0x000 + 14)

#define PAD_GPMC0_AD0               0x00F403C,   (0x000 + 15)
#define PAD_GPMC0_AD1               0x00F4040,   (0x000 + 16)
#define PAD_GPMC0_AD2               0x00F4044,   (0x000 + 17)
#define PAD_GPMC0_AD3               0x00F4048,   (0x000 + 18)
#define PAD_GPMC0_AD4               0x00F404C,   (0x000 + 82)  // special GPIO num
#define PAD_GPMC0_AD5               0x00F4050,   (0x000 + 83)  // special GPIO num
#define PAD_GPMC0_AD6               0x00F4054,   (0x000 + 21)
#define PAD_GPMC0_AD7               0x00F4058,   (0x000 + 22)
#define PAD_GPMC0_AD8               0x00F405C,   (0x000 + 23)
#define PAD_GPMC0_AD9               0x00F4060,   (0x000 + 24)
#define PAD_GPMC0_AD10              0x00F4064,   (0x000 + 25)
#define PAD_GPMC0_AD11              0x00F4068,   (0x000 + 26)
#define PAD_GPMC0_AD12              0x00F406C,   (0x000 + 27)
#define PAD_GPMC0_AD13              0x00F4070,   (0x000 + 28)
#define PAD_GPMC0_AD14              0x00F4074,   (0x000 + 29)
#define PAD_GPMC0_AD15              0x00F4078,   (0x000 + 30)
#define PAD_GPMC0_CLK               0x00F407C,   (0x000 + 31)

#define PAD_GPMC0_ADVN_ALE          0x00F4084,   (0x000 + 32)
#define PAD_GPMC0_OEN_REN           0x00F4088,   (0x000 + 33)
#define PAD_GPMC0_WEN               0x00F408C,   (0x000 + 34)
#define PAD_GPMC0_BE0N_CLE          0x00F4090,   (0x000 + 35)
#define PAD_GPMC0_BE1N              0x00F4094,   (0x000 + 36)
#define PAD_GPMC0_WAIT0             0x00F4098,   (0x000 + 37)
#define PAD_GPMC0_WAIT1             0x00F409C,   (0x000 + 38)
#define PAD_GPMC0_WPN               0x00F40A0,   (0x000 + 39)
#define PAD_GPMC0_DIR               0x00F40A4,   (0x000 + 40)
#define PAD_GPMC0_CSN0              0x00F40A8,   (0x000 + 41)
#define PAD_GPMC0_CSN1              0x00F40AC,   (0x000 + 42)
#define PAD_GPMC0_CSN2              0x00F40B0,   (0x000 + 43)
#define PAD_GPMC0_CSN3              0x00F40B4,   (0x000 + 44)

#define PAD_PRG1_PRU0_GPO0          0x00F40B8,   (0x000 + 45)
#define PAD_PRG1_PRU0_GPO1          0x00F40BC,   (0x000 + 46)
#define PAD_PRG1_PRU0_GPO2          0x00F40C0,   (0x000 + 47)
#define PAD_PRG1_PRU0_GPO3          0x00F40C4,   (0x000 + 48)
#define PAD_PRG1_PRU0_GPO4          0x00F40C8,   (0x000 + 49)
#define PAD_PRG1_PRU0_GPO5          0x00F40CC,   (0x000 + 50)
#define PAD_PRG1_PRU0_GPO6          0x00F40D0,   (0x000 + 51)
#define PAD_PRG1_PRU0_GPO7          0x00F40D4,   (0x000 + 52)
#define PAD_PRG1_PRU0_GPO8          0x00F40D8,   (0x000 + 53)
#define PAD_PRG1_PRU0_GPO9          0x00F40DC,   (0x000 + 54)
#define PAD_PRG1_PRU0_GPO10         0x00F40E0,   (0x000 + 55)
#define PAD_PRG1_PRU0_GPO11         0x00F40E4,   (0x000 + 56)
#define PAD_PRG1_PRU0_GPO12         0x00F40E8,   (0x000 + 57)
#define PAD_PRG1_PRU0_GPO13         0x00F40EC,   (0x000 + 58)
#define PAD_PRG1_PRU0_GPO14         0x00F40F0,   (0x000 + 59)
#define PAD_PRG1_PRU0_GPO15         0x00F40F4,   (0x000 + 60)
#define PAD_PRG1_PRU0_GPO16         0x00F40F8,   (0x000 + 61)
#define PAD_PRG1_PRU0_GPO17         0x00F40FC,   (0x000 + 62)
#define PAD_PRG1_PRU0_GPO18         0x00F4100,   (0x000 + 63)
#define PAD_PRG1_PRU0_GPO19         0x00F4104,   (0x000 + 64)

#define PAD_PRG1_PRU1_GPO0          0x00F4108,   (0x000 + 65)
#define PAD_PRG1_PRU1_GPO1          0x00F410C,   (0x000 + 66)
#define PAD_PRG1_PRU1_GPO2          0x00F4110,   (0x000 + 67)
#define PAD_PRG1_PRU1_GPO3          0x00F4114,   (0x000 + 68)
#define PAD_PRG1_PRU1_GPO4          0x00F4118,   (0x000 + 69)
#define PAD_PRG1_PRU1_GPO5          0x00F411C,   (0x000 + 70)
#define PAD_PRG1_PRU1_GPO6          0x00F4120,   (0x000 + 71)
#define PAD_PRG1_PRU1_GPO7          0x00F4124,   (0x000 + 72)
#define PAD_PRG1_PRU1_GPO8          0x00F4128,   (0x000 + 73)
#define PAD_PRG1_PRU1_GPO9          0x00F412C,   (0x000 + 74)
#define PAD_PRG1_PRU1_GPO10         0x00F4130,   (0x000 + 75)
#define PAD_PRG1_PRU1_GPO11         0x00F4134,   (0x000 + 76)
#define PAD_PRG1_PRU1_GPO12         0x00F4138,   (0x000 + 77)
#define PAD_PRG1_PRU1_GPO13         0x00F413C,   (0x000 + 78)
#define PAD_PRG1_PRU1_GPO14         0x00F4140,   (0x000 + 79)
#define PAD_PRG1_PRU1_GPO15         0x00F4144,   (0x000 + 80)
#define PAD_PRG1_PRU1_GPO16         0x00F4148,   (0x000 + 81)
#define PAD_PRG1_PRU1_GPO17         0x00F414C,   (0x000 + 19)  // special GPIO num
#define PAD_PRG1_PRU1_GPO18         0x00F4150,   (0x000 + 20)  // special GPIO num
#define PAD_PRG1_PRU1_GPO19         0x00F4154,   (0x000 + 84)

#define PAD_PRG1_MDIO0_MDIO         0x00F4158,   (0x000 + 85)
#define PAD_PRG1_MDIO0_MDC          0x00F415C,   (0x000 + 86)

#define PAD_PRG0_PRU0_GPO0          0x00F4160,   (0x100 +  0)
#define PAD_PRG0_PRU0_GPO1          0x00F4164,   (0x100 +  1)
#define PAD_PRG0_PRU0_GPO2          0x00F4168,   (0x100 +  2)
#define PAD_PRG0_PRU0_GPO3          0x00F416C,   (0x100 +  3)
#define PAD_PRG0_PRU0_GPO4          0x00F4170,   (0x100 +  4)
#define PAD_PRG0_PRU0_GPO5          0x00F4174,   (0x100 +  5)
#define PAD_PRG0_PRU0_GPO6          0x00F4178,   (0x100 +  6)
#define PAD_PRG0_PRU0_GPO7          0x00F417C,   (0x100 +  7)
#define PAD_PRG0_PRU0_GPO8          0x00F4180,   (0x100 +  8)
#define PAD_PRG0_PRU0_GPO9          0x00F4184,   (0x100 +  9)
#define PAD_PRG0_PRU0_GPO10         0x00F4188,   (0x100 + 10)
#define PAD_PRG0_PRU0_GPO11         0x00F418C,   (0x100 + 11)
#define PAD_PRG0_PRU0_GPO12         0x00F4190,   (0x100 + 12)
#define PAD_PRG0_PRU0_GPO13         0x00F4194,   (0x100 + 13)
#define PAD_PRG0_PRU0_GPO14         0x00F4198,   (0x100 + 14)
#define PAD_PRG0_PRU0_GPO15         0x00F419C,   (0x100 + 15)
#define PAD_PRG0_PRU0_GPO16         0x00F41A0,   (0x100 + 16)
#define PAD_PRG0_PRU0_GPO17         0x00F41A4,   (0x100 + 17)
#define PAD_PRG0_PRU0_GPO18         0x00F41A8,   (0x100 + 18)
#define PAD_PRG0_PRU0_GPO19         0x00F41AC,   (0x100 + 19)

#define PAD_PRG0_PRU1_GPO0          0x00F41B0,   (0x100 + 20)
#define PAD_PRG0_PRU1_GPO1          0x00F41B4,   (0x100 + 21)
#define PAD_PRG0_PRU1_GPO2          0x00F41B8,   (0x100 + 22)
#define PAD_PRG0_PRU1_GPO3          0x00F41BC,   (0x100 + 23)
#define PAD_PRG0_PRU1_GPO4          0x00F41C0,   (0x100 + 24)
#define PAD_PRG0_PRU1_GPO5          0x00F41C4,   (0x100 + 25)
#define PAD_PRG0_PRU1_GPO6          0x00F41C8,   (0x100 + 26)
#define PAD_PRG0_PRU1_GPO7          0x00F41CC,   (0x100 + 27)
#define PAD_PRG0_PRU1_GPO8          0x00F41D0,   (0x100 + 28)
#define PAD_PRG0_PRU1_GPO9          0x00F41D4,   (0x100 + 29)
#define PAD_PRG0_PRU1_GPO10         0x00F41D8,   (0x100 + 30)
#define PAD_PRG0_PRU1_GPO11         0x00F41DC,   (0x100 + 31)
#define PAD_PRG0_PRU1_GPO12         0x00F41E0,   (0x100 + 32)
#define PAD_PRG0_PRU1_GPO13         0x00F41E4,   (0x100 + 33)
#define PAD_PRG0_PRU1_GPO14         0x00F41E8,   (0x100 + 34)
#define PAD_PRG0_PRU1_GPO15         0x00F41EC,   (0x100 + 35)
#define PAD_PRG0_PRU1_GPO16         0x00F41F0,   (0x100 + 36)
#define PAD_PRG0_PRU1_GPO17         0x00F41F4,   (0x100 + 37)
#define PAD_PRG0_PRU1_GPO18         0x00F41F8,   (0x100 + 38)
#define PAD_PRG0_PRU1_GPO19         0x00F41FC,   (0x100 + 39)

#define PAD_PRG0_MDIO0_MDIO         0x00F4200,   (0x100 + 40)
#define PAD_PRG0_MDIO0_MDC          0x00F4204,   (0x100 + 41)
#define PAD_SPI0_CS0                0x00F4208,   (0x100 + 42)
#define PAD_SPI0_CS1                0x00F420C,   (0x100 + 43)
#define PAD_SPI0_CLK                0x00F4210,   (0x100 + 44)
#define PAD_SPI0_D0                 0x00F4214,   (0x100 + 45)
#define PAD_SPI0_D1                 0x00F4218,   (0x100 + 46)
#define PAD_SPI1_CS0                0x00F421C,   (0x100 + 47)
#define PAD_SPI1_CS1                0x00F4220,   (0x100 + 48)
#define PAD_SPI1_CLK                0x00F4224,   (0x100 + 49)
#define PAD_SPI1_D0                 0x00F4228,   (0x100 + 50)
#define PAD_SPI1_D1                 0x00F422C,   (0x100 + 51)
#define PAD_UART0_RXD               0x00F4230,   (0x100 + 52)
#define PAD_UART0_TXD               0x00F4234,   (0x100 + 53)
#define PAD_UART0_CTSN              0x00F4238,   (0x100 + 54)
#define PAD_UART0_RTSN              0x00F423C,   (0x100 + 55)
#define PAD_UART1_RXD               0x00F4240,   (0x100 + 56)
#define PAD_UART1_TXD               0x00F4244,   (0x100 + 57)
#define PAD_UART1_CTSN              0x00F4248,   (0x100 + 58)
#define PAD_UART1_RTSN              0x00F424C,   (0x100 + 59)

#define PAD_MCAN0_TX                0x00F4250,   (0x100 + 60)
#define PAD_MCAN0_RX                0x00F4254,   (0x100 + 61)
#define PAD_MCAN1_TX                0x00F4258,   (0x100 + 62)
#define PAD_MCAN1_RX                0x00F425C,   (0x100 + 63)
#define PAD_I2C0_SCL                0x00F4260,   (0x100 + 64)
#define PAD_I2C0_SDA                0x00F4264,   (0x100 + 65)
#define PAD_I2C1_SCL                0x00F4268,   (0x100 + 66)
#define PAD_I2C1_SDA                0x00F426C,   (0x100 + 67)
#define PAD_ECAP0_IN_APWM_OUT       0x00F4270,   (0x100 + 68)
#define PAD_EXT_REFCLK1             0x00F4274,   (0x100 + 69)
#define PAD_EXTINTN                 0x00F4278,   (0x100 + 70)
#define PAD_MMC1_DAT3               0x00F427C,   (0x100 + 71)
#define PAD_MMC1_DAT2               0x00F4280,   (0x100 + 72)
#define PAD_MMC1_DAT1               0x00F4284,   (0x100 + 73)
#define PAD_MMC1_DAT0               0x00F4288,   (0x100 + 74)
#define PAD_MMC1_CLK                0x00F428C,   (0x100 + 75)
#define PAD_MMC1_LB                 0x00F4290,   PAD_GPIO_NA
#define PAD_MMC1_CMD                0x00F4294,   (0x100 + 76)
#define PAD_MMC1_SDCD               0x00F4298,   (0x100 + 77)
#define PAD_MMC1_SDWP               0x00F429C,   (0x100 + 78)

#define PAD_RESET_REQZ              0x00F42A0,   PAD_GPIO_NA
#define PAD_RESETSTATZ              0x00F42A4,   PAD_GPIO_NA
#define PAD_USB0_DRVVBUS            0x00F42A8,   (0x100 + 79)
#define PAD_PORZ_OUT                0x00F42AC,   PAD_GPIO_NA


// MCU Domain

#define PAD_MCU_GPIO  0xC00

#define PAD_MCU_SPI1_CS0            0x4084014,   (0xC00 +  5)
#define PAD_MCU_I2C0_SCL            0x4084048,   (0xC00 + 18)
#define PAD_MCU_I2C0_SDA            0x408404C,   (0xC00 + 19)
#define PAD_MCU_I2C1_SCL            0x4084050,   (0xC00 + 20)
#define PAD_MCU_I2C1_SDA            0x4084054,   (0xC00 + 21)
#define PAD_MCU_SPI0_CLK            0x4084008,   (0xC00 + 11)
#define PAD_MCU_SPI0_CS0            0x4084000,   (0xC00 + 13)
#define PAD_MCU_SPI0_CS1            0x4084004,   (0xC00 + 12)
#define PAD_MCU_SPI0_D0             0x408400C,   (0xC00 + 10)
#define PAD_MCU_SPI0_D1             0x4084010,   (0xC00 +  4)
#define PAD_MCU_SPI1_CLK            0x408401C,   (0xC00 +  7)
#define PAD_MCU_SPI1_CS1            0x4084018,   (0xC00 +  6)
#define PAD_MCU_SPI1_D0             0x4084020,   (0xC00 +  8)
#define PAD_MCU_SPI1_D1             0x4084024,   (0xC00 +  9)
#define PAD_MCU_PORZ                0x408405C,   PAD_GPIO_NA
#define PAD_MCU_RESETSTATZ          0x4084060,   (0xC00 + 22)
#define PAD_MCU_RESETZ              0x4084058,   PAD_GPIO_NA
#define PAD_MCU_SAFETY_ERRORN       0x4084064,   PAD_GPIO_NA
#define PAD_MCU_UART0_CTSN          0x4084030,   (0xC00 +  1)
#define PAD_MCU_UART0_RTSN          0x4084034,   (0xC00 +  0)
#define PAD_MCU_UART0_RXD           0x4084028,   (0xC00 +  3)
#define PAD_MCU_UART0_TXD           0x408402C,   (0xC00 +  2)
#define PAD_MCU_UART1_CTSN          0x4084040,   (0xC00 + 16)
#define PAD_MCU_UART1_RTSN          0x4084044,   (0xC00 + 17)
#define PAD_MCU_UART1_RXD           0x4084038,   (0xC00 + 14)
#define PAD_MCU_UART1_TXD           0x408403C,   (0xC00 + 15)



#endif /* AM24_VN_PINMUX_H_ */
