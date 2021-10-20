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
/*
 *  file:     gd32vf103.h
 *  brief:    Monolitic defininiton for the GD32VF103 MCU with structures
 *  version:  1.00
 *  date:     2021-10-02
 *  authors:  nvitya
 *  notes:    the original manufacturer definition are strongly modified, extended
 *            for easier usage
*/
/*!
    \file    gd32vf103.h
    \brief   general definitions for GD32VF103
    \version 2019-06-05, V1.0.0, firmware for GD32VF103
*/
/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/


#ifndef GD32VF103_H
#define GD32VF103_H

#ifdef cplusplus
 extern "C" {
#endif 

 /* IO definitions (access restrictions to peripheral registers) */
 /**

     <strong>IO Type Qualifiers</strong> are used
     \li to specify the access to peripheral variables.
     \li for automatic generation of peripheral register debug information.
 */
 #ifdef __cplusplus
   #define   __I     volatile             /*!< Defines 'read only' permissions                 */
 #else
   #define   __I     volatile const       /*!< Defines 'read only' permissions                 */
 #endif
 #define     __O     volatile             /*!< Defines 'write only' permissions                */
 #define     __IO    volatile             /*!< Defines 'read / write' permissions              */

/* define value of internal 8MHz RC oscillator (IRC8M) in Hz */
#if !defined  (IRC8M_VALUE) 
#define IRC8M_VALUE  ((uint32_t)8000000)
#endif /* internal 8MHz RC oscillator value */

/* define value of internal 40KHz RC oscillator(IRC40K) in Hz */
#if !defined  (IRC40K_VALUE) 
#define IRC40K_VALUE  ((uint32_t)40000)
#endif /* internal 40KHz RC oscillator value */

/* define value of low speed crystal oscillator (LXTAL)in Hz */
#if !defined  (LXTAL_VALUE) 
#define LXTAL_VALUE  ((uint32_t)32768)
#endif /* low speed crystal oscillator value */

/* define interrupt number */
typedef enum IRQn
{

	CLIC_INT_RESERVED        	 = 0,      			/*!< RISC-V reserved		*/
	CLIC_INT_SFT         		 = 3,				/*!< Software interrupt		*/
	CLIC_INT_TMR         		 = 7,				/*!< CPU Timer interrupt	*/
	CLIC_INT_BWEI        		 = 17,				/*!< Bus Error interrupt	*/
	CLIC_INT_PMOVI       		 = 18,				/*!< Performance Monitor	*/

    /* interruput numbers */
    WWDGT_IRQn                   = 19,      /*!< window watchDog timer interrupt                          */
    LVD_IRQn                     = 20,      /*!< LVD through EXTI line detect interrupt                   */
    TAMPER_IRQn                  = 21,      /*!< tamper through EXTI line detect                          */
    RTC_IRQn                     = 22,      /*!< RTC alarm interrupt                                      */
    FMC_IRQn                     = 23,      /*!< FMC interrupt                                            */
    RCU_CTC_IRQn                 = 24,      /*!< RCU and CTC interrupt                                    */
    EXTI0_IRQn                   = 25,      /*!< EXTI line 0 interrupts                                   */
    EXTI1_IRQn                   = 26,      /*!< EXTI line 1 interrupts                                   */
    EXTI2_IRQn                   = 27,      /*!< EXTI line 2 interrupts                                   */
    EXTI3_IRQn                   = 28,      /*!< EXTI line 3 interrupts                                   */
    EXTI4_IRQn                   = 29,     /*!< EXTI line 4 interrupts                                   */
    DMA0_Channel0_IRQn           = 30,     /*!< DMA0 channel0 interrupt                                  */
    DMA0_Channel1_IRQn           = 31,     /*!< DMA0 channel1 interrupt                                  */
    DMA0_Channel2_IRQn           = 32,     /*!< DMA0 channel2 interrupt                                  */
    DMA0_Channel3_IRQn           = 33,     /*!< DMA0 channel3 interrupt                                  */
    DMA0_Channel4_IRQn           = 34,     /*!< DMA0 channel4 interrupt                                  */
    DMA0_Channel5_IRQn           = 35,     /*!< DMA0 channel5 interrupt                                  */
    DMA0_Channel6_IRQn           = 36,     /*!< DMA0 channel6 interrupt                                  */
    ADC0_1_IRQn                  = 37,     /*!< ADC0 and ADC1 interrupt                                  */
    CAN0_TX_IRQn                 = 38,     /*!< CAN0 TX interrupts                                       */
    CAN0_RX0_IRQn                = 39,     /*!< CAN0 RX0 interrupts                                      */
    CAN0_RX1_IRQn                = 40,     /*!< CAN0 RX1 interrupts                                      */
    CAN0_EWMC_IRQn               = 41,     /*!< CAN0 EWMC interrupts                                     */
    EXTI5_9_IRQn                 = 42,     /*!< EXTI[9:5] interrupts                                     */
    TIMER0_BRK_IRQn              = 43,     /*!< TIMER0 break interrupts                                  */
    TIMER0_UP_IRQn               = 44,     /*!< TIMER0 update interrupts                                 */
    TIMER0_TRG_CMT_IRQn          = 45,     /*!< TIMER0 trigger and commutation interrupts                */
    TIMER0_Channel_IRQn          = 46,     /*!< TIMER0 channel capture compare interrupts                */
    TIMER1_IRQn                  = 47,     /*!< TIMER1 interrupt                                         */
    TIMER2_IRQn                  = 48,     /*!< TIMER2 interrupt                                         */
    TIMER3_IRQn                  = 49,     /*!< TIMER3 interrupts                                        */
    I2C0_EV_IRQn                 = 50,     /*!< I2C0 event interrupt                                     */
    I2C0_ER_IRQn                 = 51,     /*!< I2C0 error interrupt                                     */
    I2C1_EV_IRQn                 = 52,     /*!< I2C1 event interrupt                                     */
    I2C1_ER_IRQn                 = 53,     /*!< I2C1 error interrupt                                     */
    SPI0_IRQn                    = 54,     /*!< SPI0 interrupt                                           */
    SPI1_IRQn                    = 55,     /*!< SPI1 interrupt                                           */
    USART0_IRQn                  = 56,     /*!< USART0 interrupt                                         */
    USART1_IRQn                  = 57,     /*!< USART1 interrupt                                         */
    USART2_IRQn                  = 58,     /*!< USART2 interrupt                                         */
    EXTI10_15_IRQn               = 59,     /*!< EXTI[15:10] interrupts                                   */
    RTC_ALARM_IRQn               = 60,     /*!< RTC alarm interrupt EXTI                                 */
    USBFS_WKUP_IRQn              = 61,     /*!< USBFS wakeup interrupt                                   */

    EXMC_IRQn                    = 67,     /*!< EXMC global interrupt                                    */

    TIMER4_IRQn                  = 69,     /*!< TIMER4 global interrupt                                  */
    SPI2_IRQn                    = 70,     /*!< SPI2 global interrupt                                    */
    UART3_IRQn                   = 71,     /*!< UART3 global interrupt                                   */
    UART4_IRQn                   = 72,     /*!< UART4 global interrupt                                   */
    TIMER5_IRQn                  = 73,     /*!< TIMER5 global interrupt                                  */
    TIMER6_IRQn                  = 74,     /*!< TIMER6 global interrupt                                  */
    DMA1_Channel0_IRQn           = 75,     /*!< DMA1 channel0 global interrupt                           */
    DMA1_Channel1_IRQn           = 76,     /*!< DMA1 channel1 global interrupt                           */
    DMA1_Channel2_IRQn           = 77,     /*!< DMA1 channel2 global interrupt                           */
    DMA1_Channel3_IRQn           = 78,     /*!< DMA1 channel3 global interrupt                           */
    DMA1_Channel4_IRQn           = 79,     /*!< DMA1 channel3 global interrupt                           */

    CAN1_TX_IRQn                 = 82,     /*!< CAN1 TX interrupt                                        */
    CAN1_RX0_IRQn                = 83,     /*!< CAN1 RX0 interrupt                                       */
    CAN1_RX1_IRQn                = 84,     /*!< CAN1 RX1 interrupt                                       */
    CAN1_EWMC_IRQn               = 85,     /*!< CAN1 EWMC interrupt                                      */
    USBFS_IRQn                   = 86,     /*!< USBFS global interrupt                                   */

	ECLIC_NUM_INTERRUPTS
} IRQn_Type;

#include <stdint.h>

/* enum definitions */
typedef enum {DISABLE = 0, ENABLE = !DISABLE} EventStatus, ControlStatus;
//typedef enum {FALSE = 0, TRUE = !FALSE} bool;  // nvitya: commented out because of the C++ conflict
typedef enum {RESET = 0, SET = 1,MAX = 0X7FFFFFFF} FlagStatus;
typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrStatus;

/* bit operations */
#define REG32(addr)                  (*(volatile uint32_t *)(uint32_t)(addr))
#define REG16(addr)                  (*(volatile uint16_t *)(uint32_t)(addr))
#define REG8(addr)                   (*(volatile uint8_t *)(uint32_t)(addr))
#define BIT(x)                       ((uint32_t)((uint32_t)0x01U<<(x)))
#define BITS(start, end)             ((0xFFFFFFFFUL << (start)) & (0xFFFFFFFFUL >> (31U - (uint32_t)(end)))) 
#define GET_BITS(regval, start, end) (((regval) & BITS((start),(end))) >> (start))

/* main flash and SRAM memory map */
#define FLASH_BASE            ((uint32_t)0x08000000U)        /*!< main FLASH base address          */
#define SRAM_BASE             ((uint32_t)0x20000000U)        /*!< SRAM0 base address               */
#define OB_BASE               ((uint32_t)0x1FFFF800U)        /*!< OB base address                  */
#define DBG_BASE              ((uint32_t)0xE0042000U)        /*!< DBG base address                 */
#define EXMC_BASE             ((uint32_t)0xA0000000U)        /*!< EXMC register base address       */

/* peripheral memory map */
#define APB1_BUS_BASE         ((uint32_t)0x40000000U)        /*!< apb1 base address                */
#define APB2_BUS_BASE         ((uint32_t)0x40010000U)        /*!< apb2 base address                */
#define AHB1_BUS_BASE         ((uint32_t)0x40018000U)        /*!< ahb1 base address                */
#define AHB3_BUS_BASE         ((uint32_t)0x60000000U)        /*!< ahb3 base address                */

/* advanced peripheral bus 1 memory map */
#define TIMER_BASE            (APB1_BUS_BASE + 0x00000000U)  /*!< TIMER base address               */
#define RTC_BASE              (APB1_BUS_BASE + 0x00002800U)  /*!< RTC base address                 */
#define WWDGT_BASE            (APB1_BUS_BASE + 0x00002C00U)  /*!< WWDGT base address               */
#define FWDGT_BASE            (APB1_BUS_BASE + 0x00003000U)  /*!< FWDGT base address               */
#define SPI_BASE              (APB1_BUS_BASE + 0x00003800U)  /*!< SPI base address                 */
#define USART_BASE            (APB1_BUS_BASE + 0x00004400U)  /*!< USART base address               */
#define I2C_BASE              (APB1_BUS_BASE + 0x00005400U)  /*!< I2C base address                 */
#define CAN_BASE              (APB1_BUS_BASE + 0x00006400U)  /*!< CAN base address                 */
#define BKP_BASE              (APB1_BUS_BASE + 0x00006C00U)  /*!< BKP base address                 */
#define PMU_BASE              (APB1_BUS_BASE + 0x00007000U)  /*!< PMU base address                 */
#define DAC_BASE              (APB1_BUS_BASE + 0x00007400U)  /*!< DAC base address                 */

/* advanced peripheral bus 2 memory map */
#define AFIO_BASE             (APB2_BUS_BASE + 0x00000000U)  /*!< AFIO base address                */
#define EXTI_BASE             (APB2_BUS_BASE + 0x00000400U)  /*!< EXTI base address                */
#define GPIO_BASE             (APB2_BUS_BASE + 0x00000800U)  /*!< GPIO base address                */
#define ADC_BASE              (APB2_BUS_BASE + 0x00002400U)  /*!< ADC base address                 */

/* advanced high performance bus 1 memory map */
#define DMA_BASE              (AHB1_BUS_BASE + 0x00008000U)  /*!< DMA base address                 */
#define RCU_BASE              (AHB1_BUS_BASE + 0x00009000U)  /*!< RCU base address                 */
#define FMC_BASE              (AHB1_BUS_BASE + 0x0000A000U)  /*!< FMC base address                 */
#define CRC_BASE              (AHB1_BUS_BASE + 0x0000B000U)  /*!< CRC base address                 */
#define USBFS_BASE            (AHB1_BUS_BASE + 0x0FFE8000U)  /*!< USBFS base address               */

//---------------------------------------------------------------------------------------
// RCU
//---------------------------------------------------------------------------------------

typedef struct gd32v_rcu_t
{
  volatile uint32_t  CTL;           // 0x00: control register
  volatile uint32_t  CFG0;          // 0x04: clock configuration register 0
  volatile uint32_t  INT;           // 0x08: clock interrupt register
  volatile uint32_t  APB2RST;       // 0x0C: APB2 reset register
  volatile uint32_t  APB1RST;       // 0x10: APB1 reset register
  volatile uint32_t  AHBEN;         // 0x14: AHB1 enable register
  volatile uint32_t  APB2EN;        // 0x18: APB2 enable register
  volatile uint32_t  APB1EN;        // 0x1C: APB1 enable register
  volatile uint32_t  BDCTL;         // 0x20: backup domain control register
  volatile uint32_t  RSTSCK;        // 0x24: reset source / clock register
  volatile uint32_t  AHBRST;        // 0x28: AHB reset register
  volatile uint32_t  CFG1;          // 0x2C: clock configuration register 1
           uint32_t  _reserved;     // 0x30: -
  volatile uint32_t  DSV;           // 0x34: deep-sleep mode voltage register
//
} gd32v_rcu_t;


/* RCU definitions */
#define RCU                             ((gd32v_rcu_t *)(RCU_BASE))

/* registers definitions */



/* bits definitions */
/* RCU_CTL */
#define RCU_CTL_IRC8MEN                 BIT(0)                    /*!< internal high speed oscillator enable */
#define RCU_CTL_IRC8MSTB                BIT(1)                    /*!< IRC8M high speed internal oscillator stabilization flag */
#define RCU_CTL_IRC8MADJ                BITS(3,7)                 /*!< high speed internal oscillator clock trim adjust value */
#define RCU_CTL_IRC8MCALIB              BITS(8,15)                /*!< high speed internal oscillator calibration value register */
#define RCU_CTL_HXTALEN                 BIT(16)                   /*!< external high speed oscillator enable */
#define RCU_CTL_HXTALSTB                BIT(17)                   /*!< external crystal oscillator clock stabilization flag */
#define RCU_CTL_HXTALBPS                BIT(18)                   /*!< external crystal oscillator clock bypass mode enable */
#define RCU_CTL_CKMEN                   BIT(19)                   /*!< HXTAL clock monitor enable */
#define RCU_CTL_PLLEN                   BIT(24)                   /*!< PLL enable */
#define RCU_CTL_PLLSTB                  BIT(25)                   /*!< PLL clock stabilization flag */
#define RCU_CTL_PLL1EN                  BIT(26)                   /*!< PLL1 enable */
#define RCU_CTL_PLL1STB                 BIT(27)                   /*!< PLL1 clock stabilization flag */
#define RCU_CTL_PLL2EN                  BIT(28)                   /*!< PLL2 enable */
#define RCU_CTL_PLL2STB                 BIT(29)                   /*!< PLL2 clock stabilization flag */


#define RCU_CFG0_SCS                    BITS(0,1)                 /*!< system clock switch */
#define RCU_CFG0_SCSS                   BITS(2,3)                 /*!< system clock switch status */
#define RCU_CFG0_AHBPSC                 BITS(4,7)                 /*!< AHB prescaler selection */
#define RCU_CFG0_APB1PSC                BITS(8,10)                /*!< APB1 prescaler selection */
#define RCU_CFG0_APB2PSC                BITS(11,13)               /*!< APB2 prescaler selection */
#define RCU_CFG0_ADCPSC                 BITS(14,15)               /*!< ADC prescaler selection */
#define RCU_CFG0_PLLSEL                 BIT(16)                   /*!< PLL clock source selection */
#define RCU_CFG0_PREDV0_LSB             BIT(17)                   /*!< the LSB of PREDV0 division factor */
#define RCU_CFG0_PLLMF                  BITS(18,21)               /*!< PLL clock multiplication factor */
#define RCU_CFG0_USBFSPSC               BITS(22,23)               /*!< USBFS clock prescaler selection */
#define RCU_CFG0_CKOUT0SEL              BITS(24,27)               /*!< CKOUT0 clock source selection */
#define RCU_CFG0_ADCPSC_2               BIT(28)                   /*!< bit 2 of ADCPSC */
#define RCU_CFG0_PLLMF_4                BIT(29)                   /*!< bit 4 of PLLMF */

/* RCU_INT */
#define RCU_INT_IRC40KSTBIF             BIT(0)                    /*!< IRC40K stabilization interrupt flag */
#define RCU_INT_LXTALSTBIF              BIT(1)                    /*!< LXTAL stabilization interrupt flag */
#define RCU_INT_IRC8MSTBIF              BIT(2)                    /*!< IRC8M stabilization interrupt flag */
#define RCU_INT_HXTALSTBIF              BIT(3)                    /*!< HXTAL stabilization interrupt flag */
#define RCU_INT_PLLSTBIF                BIT(4)                    /*!< PLL stabilization interrupt flag */
#define RCU_INT_PLL1STBIF               BIT(5)                    /*!< PLL1 stabilization interrupt flag */
#define RCU_INT_PLL2STBIF               BIT(6)                    /*!< PLL2 stabilization interrupt flag */
#define RCU_INT_CKMIF                   BIT(7)                    /*!< HXTAL clock stuck interrupt flag */
#define RCU_INT_IRC40KSTBIE             BIT(8)                    /*!< IRC40K stabilization interrupt enable */
#define RCU_INT_LXTALSTBIE              BIT(9)                    /*!< LXTAL stabilization interrupt enable */
#define RCU_INT_IRC8MSTBIE              BIT(10)                   /*!< IRC8M stabilization interrupt enable */
#define RCU_INT_HXTALSTBIE              BIT(11)                   /*!< HXTAL stabilization interrupt enable */
#define RCU_INT_PLLSTBIE                BIT(12)                   /*!< PLL stabilization interrupt enable */
#define RCU_INT_PLL1STBIE               BIT(13)                   /*!< PLL1 stabilization interrupt enable */
#define RCU_INT_PLL2STBIE               BIT(14)                   /*!< PLL2 stabilization interrupt enable */
#define RCU_INT_IRC40KSTBIC             BIT(16)                   /*!< IRC40K stabilization interrupt clear */
#define RCU_INT_LXTALSTBIC              BIT(17)                   /*!< LXTAL stabilization interrupt clear */
#define RCU_INT_IRC8MSTBIC              BIT(18)                   /*!< IRC8M stabilization interrupt clear */
#define RCU_INT_HXTALSTBIC              BIT(19)                   /*!< HXTAL stabilization interrupt clear */
#define RCU_INT_PLLSTBIC                BIT(20)                   /*!< PLL stabilization interrupt clear */
#define RCU_INT_PLL1STBIC               BIT(21)                   /*!< PLL1 stabilization interrupt clear */
#define RCU_INT_PLL2STBIC               BIT(22)                   /*!< PLL2 stabilization interrupt clear */
#define RCU_INT_CKMIC                   BIT(23)                   /*!< HXTAL clock stuck interrupt clear */

/* RCU_APB2RST */
#define RCU_APB2RST_AFRST               BIT(0)                    /*!< alternate function I/O reset */
#define RCU_APB2RST_PARST               BIT(2)                    /*!< GPIO port A reset */
#define RCU_APB2RST_PBRST               BIT(3)                    /*!< GPIO port B reset */
#define RCU_APB2RST_PCRST               BIT(4)                    /*!< GPIO port C reset */
#define RCU_APB2RST_PDRST               BIT(5)                    /*!< GPIO port D reset */
#define RCU_APB2RST_PERST               BIT(6)                    /*!< GPIO port E reset */
#define RCU_APB2RST_ADC0RST             BIT(9)                    /*!< ADC0 reset */
#define RCU_APB2RST_ADC1RST             BIT(10)                   /*!< ADC1 reset */
#define RCU_APB2RST_TIMER0RST           BIT(11)                   /*!< TIMER0 reset */
#define RCU_APB2RST_SPI0RST             BIT(12)                   /*!< SPI0 reset */
#define RCU_APB2RST_USART0RST           BIT(14)                   /*!< USART0 reset */

/* RCU_APB1RST */
#define RCU_APB1RST_TIMER1RST           BIT(0)                    /*!< TIMER1 reset */
#define RCU_APB1RST_TIMER2RST           BIT(1)                    /*!< TIMER2 reset */
#define RCU_APB1RST_TIMER3RST           BIT(2)                    /*!< TIMER3 reset */
#define RCU_APB1RST_TIMER4RST           BIT(3)                    /*!< TIMER4 reset */
#define RCU_APB1RST_TIMER5RST           BIT(4)                    /*!< TIMER5 reset */
#define RCU_APB1RST_TIMER6RST           BIT(5)                    /*!< TIMER6 reset */

#define RCU_APB1RST_WWDGTRST            BIT(11)                   /*!< WWDGT reset */
#define RCU_APB1RST_SPI1RST             BIT(14)                   /*!< SPI1 reset */
#define RCU_APB1RST_SPI2RST             BIT(15)                   /*!< SPI2 reset */
#define RCU_APB1RST_USART1RST           BIT(17)                   /*!< USART1 reset */
#define RCU_APB1RST_USART2RST           BIT(18)                   /*!< USART2 reset */
#define RCU_APB1RST_UART3RST            BIT(19)                   /*!< UART3 reset */
#define RCU_APB1RST_UART4RST            BIT(20)                   /*!< UART4 reset */
#define RCU_APB1RST_I2C0RST             BIT(21)                   /*!< I2C0 reset */
#define RCU_APB1RST_I2C1RST             BIT(22)                   /*!< I2C1 reset */
#define RCU_APB1RST_CAN0RST             BIT(25)                   /*!< CAN0 reset */
#define RCU_APB1RST_CAN1RST             BIT(26)                   /*!< CAN1 reset */
#define RCU_APB1RST_BKPIRST             BIT(27)                   /*!< backup interface reset */
#define RCU_APB1RST_PMURST              BIT(28)                   /*!< PMU reset */
#define RCU_APB1RST_DACRST              BIT(29)                   /*!< DAC reset */

/* RCU_AHBEN */
#define RCU_AHBEN_DMA0EN                BIT(0)                    /*!< DMA0 clock enable */
#define RCU_AHBEN_DMA1EN                BIT(1)                    /*!< DMA1 clock enable */
#define RCU_AHBEN_SRAMSPEN              BIT(2)                    /*!< SRAM clock enable when sleep mode */
#define RCU_AHBEN_FMCSPEN               BIT(4)                    /*!< FMC clock enable when sleep mode */
#define RCU_AHBEN_CRCEN                 BIT(6)                    /*!< CRC clock enable */
#define RCU_AHBEN_EXMCEN                BIT(8)                    /*!< EXMC clock enable */
#define RCU_AHBEN_USBFSEN               BIT(12)                   /*!< USBFS clock enable */

/* RCU_APB2EN */
#define RCU_APB2EN_AFEN                 BIT(0)                    /*!< alternate function IO clock enable */
#define RCU_APB2EN_PAEN                 BIT(2)                    /*!< GPIO port A clock enable */
#define RCU_APB2EN_PBEN                 BIT(3)                    /*!< GPIO port B clock enable */
#define RCU_APB2EN_PCEN                 BIT(4)                    /*!< GPIO port C clock enable */
#define RCU_APB2EN_PDEN                 BIT(5)                    /*!< GPIO port D clock enable */
#define RCU_APB2EN_PEEN                 BIT(6)                    /*!< GPIO port E clock enable */
#define RCU_APB2EN_ADC0EN               BIT(9)                    /*!< ADC0 clock enable */
#define RCU_APB2EN_ADC1EN               BIT(10)                   /*!< ADC1 clock enable */
#define RCU_APB2EN_TIMER0EN             BIT(11)                   /*!< TIMER0 clock enable */
#define RCU_APB2EN_SPI0EN               BIT(12)                   /*!< SPI0 clock enable */
#define RCU_APB2EN_USART0EN             BIT(14)                   /*!< USART0 clock enable */

/* RCU_APB1EN */
#define RCU_APB1EN_TIMER1EN             BIT(0)                    /*!< TIMER1 clock enable */
#define RCU_APB1EN_TIMER2EN             BIT(1)                    /*!< TIMER2 clock enable */
#define RCU_APB1EN_TIMER3EN             BIT(2)                    /*!< TIMER3 clock enable */
#define RCU_APB1EN_TIMER4EN             BIT(3)                    /*!< TIMER4 clock enable */
#define RCU_APB1EN_TIMER5EN             BIT(4)                    /*!< TIMER5 clock enable */
#define RCU_APB1EN_TIMER6EN             BIT(5)                    /*!< TIMER6 clock enable */
#define RCU_APB1EN_WWDGTEN              BIT(11)                   /*!< WWDGT clock enable */
#define RCU_APB1EN_SPI1EN               BIT(14)                   /*!< SPI1 clock enable */
#define RCU_APB1EN_SPI2EN               BIT(15)                   /*!< SPI2 clock enable */
#define RCU_APB1EN_USART1EN             BIT(17)                   /*!< USART1 clock enable */
#define RCU_APB1EN_USART2EN             BIT(18)                   /*!< USART2 clock enable */
#define RCU_APB1EN_UART3EN              BIT(19)                   /*!< UART3 clock enable */
#define RCU_APB1EN_UART4EN              BIT(20)                   /*!< UART4 clock enable */
#define RCU_APB1EN_I2C0EN               BIT(21)                   /*!< I2C0 clock enable */
#define RCU_APB1EN_I2C1EN               BIT(22)                   /*!< I2C1 clock enable */
#define RCU_APB1EN_CAN0EN               BIT(25)                   /*!< CAN0 clock enable */
#define RCU_APB1EN_CAN1EN               BIT(26)                   /*!< CAN1 clock enable */
#define RCU_APB1EN_BKPIEN               BIT(27)                   /*!< backup interface clock enable */
#define RCU_APB1EN_PMUEN                BIT(28)                   /*!< PMU clock enable */
#define RCU_APB1EN_DACEN                BIT(29)                   /*!< DAC clock enable */

/* RCU_BDCTL */
#define RCU_BDCTL_LXTALEN               BIT(0)                    /*!< LXTAL enable */
#define RCU_BDCTL_LXTALSTB              BIT(1)                    /*!< low speed crystal oscillator stabilization flag */
#define RCU_BDCTL_LXTALBPS              BIT(2)                    /*!< LXTAL bypass mode enable */
#define RCU_BDCTL_RTCSRC                BITS(8,9)                 /*!< RTC clock entry selection */
#define RCU_BDCTL_RTCEN                 BIT(15)                   /*!< RTC clock enable */
#define RCU_BDCTL_BKPRST                BIT(16)                   /*!< backup domain reset */

/* RCU_RSTSCK */
#define RCU_RSTSCK_IRC40KEN             BIT(0)                    /*!< IRC40K enable */
#define RCU_RSTSCK_IRC40KSTB            BIT(1)                    /*!< IRC40K stabilization flag */
#define RCU_RSTSCK_RSTFC                BIT(24)                   /*!< reset flag clear */
#define RCU_RSTSCK_EPRSTF               BIT(26)                   /*!< external pin reset flag */
#define RCU_RSTSCK_PORRSTF              BIT(27)                   /*!< power reset flag */
#define RCU_RSTSCK_SWRSTF               BIT(28)                   /*!< software reset flag */
#define RCU_RSTSCK_FWDGTRSTF            BIT(29)                   /*!< free watchdog timer reset flag */
#define RCU_RSTSCK_WWDGTRSTF            BIT(30)                   /*!< window watchdog timer reset flag */
#define RCU_RSTSCK_LPRSTF               BIT(31)                   /*!< low-power reset flag */

/* RCU_AHBRST */
#define RCU_AHBRST_USBFSRST             BIT(12)                   /*!< USBFS reset */

/* RCU_CFG1 */
#define RCU_CFG1_PREDV0                 BITS(0,3)                 /*!< PREDV0 division factor */
#define RCU_CFG1_PREDV1                 BITS(4,7)                 /*!< PREDV1 division factor */
#define RCU_CFG1_PLL1MF                 BITS(8,11)                /*!< PLL1 clock multiplication factor */
#define RCU_CFG1_PLL2MF                 BITS(12,15)               /*!< PLL2 clock multiplication factor */
#define RCU_CFG1_PREDV0SEL              BIT(16)                   /*!< PREDV0 input clock source selection */
#define RCU_CFG1_I2S1SEL                BIT(17)                   /*!< I2S1 clock source selection */
#define RCU_CFG1_I2S2SEL                BIT(18)                   /*!< I2S2 clock source selection  */

/* RCU_DSV */
#define RCU_DSV_DSLPVS                  BITS(0,1)                 /*!< deep-sleep mode voltage select */

/* constants definitions */
/* define the peripheral clock enable bit position and its register index offset */
#define RCU_REGIDX_BIT(regidx, bitpos)      (((uint32_t)(regidx) << 6) | (uint32_t)(bitpos))
#define RCU_REG_VAL(periph)                 (REG32(RCU + ((uint32_t)(periph) >> 6)))
#define RCU_BIT_POS(val)                    ((uint32_t)(val) & 0x1FU)

/* register offset */
/* peripherals enable */
#define AHBEN_REG_OFFSET                0x14U                     /*!< AHB enable register offset */
#define APB1EN_REG_OFFSET               0x1CU                     /*!< APB1 enable register offset */
#define APB2EN_REG_OFFSET               0x18U                     /*!< APB2 enable register offset */

/* peripherals reset */
#define AHBRST_REG_OFFSET               0x28U                     /*!< AHB reset register offset */
#define APB1RST_REG_OFFSET              0x10U                     /*!< APB1 reset register offset */
#define APB2RST_REG_OFFSET              0x0CU                     /*!< APB2 reset register offset */
#define RSTSCK_REG_OFFSET               0x24U                     /*!< reset source/clock register offset */

/* clock control */
#define CTL_REG_OFFSET                  0x00U                     /*!< control register offset */
#define BDCTL_REG_OFFSET                0x20U                     /*!< backup domain control register offset */

/* clock stabilization and stuck interrupt */
#define INT_REG_OFFSET                  0x08U                     /*!< clock interrupt register offset */

/* configuration register */
#define CFG0_REG_OFFSET                 0x04U                     /*!< clock configuration register 0 offset */
#define CFG1_REG_OFFSET                 0x2CU                     /*!< clock configuration register 1 offset */

/* RCU_CFG0 register bit define */
/* system clock source select */
#define CFG0_SCS(regval)                (BITS(0,1) & ((uint32_t)(regval) << 0))
#define RCU_CKSYSSRC_IRC8M              CFG0_SCS(0)                         /*!< system clock source select IRC8M */
#define RCU_CKSYSSRC_HXTAL              CFG0_SCS(1)                         /*!< system clock source select HXTAL */
#define RCU_CKSYSSRC_PLL                CFG0_SCS(2)                         /*!< system clock source select PLL */

/* system clock source select status */
#define CFG0_SCSS(regval)               (BITS(2,3) & ((uint32_t)(regval) << 2))
#define RCU_SCSS_IRC8M                  CFG0_SCSS(0)                        /*!< system clock source select IRC8M */
#define RCU_SCSS_HXTAL                  CFG0_SCSS(1)                        /*!< system clock source select HXTAL */
#define RCU_SCSS_PLL                    CFG0_SCSS(2)                        /*!< system clock source select PLLP */

/* AHB prescaler selection */
#define CFG0_AHBPSC(regval)             (BITS(4,7) & ((uint32_t)(regval) << 4))
#define RCU_AHB_CKSYS_DIV1              CFG0_AHBPSC(0)                      /*!< AHB prescaler select CK_SYS */
#define RCU_AHB_CKSYS_DIV2              CFG0_AHBPSC(8)                      /*!< AHB prescaler select CK_SYS/2 */
#define RCU_AHB_CKSYS_DIV4              CFG0_AHBPSC(9)                      /*!< AHB prescaler select CK_SYS/4 */
#define RCU_AHB_CKSYS_DIV8              CFG0_AHBPSC(10)                     /*!< AHB prescaler select CK_SYS/8 */
#define RCU_AHB_CKSYS_DIV16             CFG0_AHBPSC(11)                     /*!< AHB prescaler select CK_SYS/16 */
#define RCU_AHB_CKSYS_DIV64             CFG0_AHBPSC(12)                     /*!< AHB prescaler select CK_SYS/64 */
#define RCU_AHB_CKSYS_DIV128            CFG0_AHBPSC(13)                     /*!< AHB prescaler select CK_SYS/128 */
#define RCU_AHB_CKSYS_DIV256            CFG0_AHBPSC(14)                     /*!< AHB prescaler select CK_SYS/256 */
#define RCU_AHB_CKSYS_DIV512            CFG0_AHBPSC(15)                     /*!< AHB prescaler select CK_SYS/512 */

/* APB1 prescaler selection */
#define CFG0_APB1PSC(regval)            (BITS(8,10) & ((uint32_t)(regval) << 8))
#define RCU_APB1_CKAHB_DIV1             CFG0_APB1PSC(0)                     /*!< APB1 prescaler select CK_AHB */
#define RCU_APB1_CKAHB_DIV2             CFG0_APB1PSC(4)                     /*!< APB1 prescaler select CK_AHB/2 */
#define RCU_APB1_CKAHB_DIV4             CFG0_APB1PSC(5)                     /*!< APB1 prescaler select CK_AHB/4 */
#define RCU_APB1_CKAHB_DIV8             CFG0_APB1PSC(6)                     /*!< APB1 prescaler select CK_AHB/8 */
#define RCU_APB1_CKAHB_DIV16            CFG0_APB1PSC(7)                     /*!< APB1 prescaler select CK_AHB/16 */

/* APB2 prescaler selection */
#define CFG0_APB2PSC(regval)            (BITS(11,13) & ((uint32_t)(regval) << 11))
#define RCU_APB2_CKAHB_DIV1             CFG0_APB2PSC(0)                     /*!< APB2 prescaler select CK_AHB */
#define RCU_APB2_CKAHB_DIV2             CFG0_APB2PSC(4)                     /*!< APB2 prescaler select CK_AHB/2 */
#define RCU_APB2_CKAHB_DIV4             CFG0_APB2PSC(5)                     /*!< APB2 prescaler select CK_AHB/4 */
#define RCU_APB2_CKAHB_DIV8             CFG0_APB2PSC(6)                     /*!< APB2 prescaler select CK_AHB/8 */
#define RCU_APB2_CKAHB_DIV16            CFG0_APB2PSC(7)                     /*!< APB2 prescaler select CK_AHB/16 */

/* ADC prescaler select */
#define RCU_CKADC_CKAPB2_DIV2           ((uint32_t)0x00000000U)             /*!< ADC prescaler select CK_APB2/2 */
#define RCU_CKADC_CKAPB2_DIV4           ((uint32_t)0x00000001U)             /*!< ADC prescaler select CK_APB2/4 */
#define RCU_CKADC_CKAPB2_DIV6           ((uint32_t)0x00000002U)             /*!< ADC prescaler select CK_APB2/6 */
#define RCU_CKADC_CKAPB2_DIV8           ((uint32_t)0x00000003U)             /*!< ADC prescaler select CK_APB2/8 */
#define RCU_CKADC_CKAPB2_DIV12          ((uint32_t)0x00000005U)             /*!< ADC prescaler select CK_APB2/12 */
#define RCU_CKADC_CKAPB2_DIV16          ((uint32_t)0x00000007U)             /*!< ADC prescaler select CK_APB2/16 */

/* PLL clock source selection */
#define RCU_PLLSRC_IRC8M_DIV2           ((uint32_t)0x00000000U)             /*!< IRC8M/2 clock selected as source clock of PLL */
#define RCU_PLLSRC_HXTAL                RCU_CFG0_PLLSEL                     /*!< HXTAL clock selected as source clock of PLL */

/* PLL clock multiplication factor */
#define PLLMF_4                         RCU_CFG0_PLLMF_4                    /* bit 4 of PLLMF */

#define CFG0_PLLMF(regval)              (BITS(18,21) & ((uint32_t)(regval) << 18))
#define RCU_PLL_MUL2                    CFG0_PLLMF(0)                       /*!< PLL source clock multiply by 2 */
#define RCU_PLL_MUL3                    CFG0_PLLMF(1)                       /*!< PLL source clock multiply by 3 */
#define RCU_PLL_MUL4                    CFG0_PLLMF(2)                       /*!< PLL source clock multiply by 4 */
#define RCU_PLL_MUL5                    CFG0_PLLMF(3)                       /*!< PLL source clock multiply by 5 */
#define RCU_PLL_MUL6                    CFG0_PLLMF(4)                       /*!< PLL source clock multiply by 6 */
#define RCU_PLL_MUL7                    CFG0_PLLMF(5)                       /*!< PLL source clock multiply by 7 */
#define RCU_PLL_MUL8                    CFG0_PLLMF(6)                       /*!< PLL source clock multiply by 8 */
#define RCU_PLL_MUL9                    CFG0_PLLMF(7)                       /*!< PLL source clock multiply by 9 */
#define RCU_PLL_MUL10                   CFG0_PLLMF(8)                       /*!< PLL source clock multiply by 10 */
#define RCU_PLL_MUL11                   CFG0_PLLMF(9)                       /*!< PLL source clock multiply by 11 */
#define RCU_PLL_MUL12                   CFG0_PLLMF(10)                      /*!< PLL source clock multiply by 12 */
#define RCU_PLL_MUL13                   CFG0_PLLMF(11)                      /*!< PLL source clock multiply by 13 */
#define RCU_PLL_MUL14                   CFG0_PLLMF(12)                      /*!< PLL source clock multiply by 14 */
#define RCU_PLL_MUL6_5                  CFG0_PLLMF(13)                      /*!< PLL source clock multiply by 6.5 */
#define RCU_PLL_MUL16                   CFG0_PLLMF(14)                      /*!< PLL source clock multiply by 16 */
#define RCU_PLL_MUL17                   (PLLMF_4 | CFG0_PLLMF(0))           /*!< PLL source clock multiply by 17 */
#define RCU_PLL_MUL18                   (PLLMF_4 | CFG0_PLLMF(1))           /*!< PLL source clock multiply by 18 */
#define RCU_PLL_MUL19                   (PLLMF_4 | CFG0_PLLMF(2))           /*!< PLL source clock multiply by 19 */
#define RCU_PLL_MUL20                   (PLLMF_4 | CFG0_PLLMF(3))           /*!< PLL source clock multiply by 20 */
#define RCU_PLL_MUL21                   (PLLMF_4 | CFG0_PLLMF(4))           /*!< PLL source clock multiply by 21 */
#define RCU_PLL_MUL22                   (PLLMF_4 | CFG0_PLLMF(5))           /*!< PLL source clock multiply by 22 */
#define RCU_PLL_MUL23                   (PLLMF_4 | CFG0_PLLMF(6))           /*!< PLL source clock multiply by 23 */
#define RCU_PLL_MUL24                   (PLLMF_4 | CFG0_PLLMF(7))           /*!< PLL source clock multiply by 24 */
#define RCU_PLL_MUL25                   (PLLMF_4 | CFG0_PLLMF(8))           /*!< PLL source clock multiply by 25 */
#define RCU_PLL_MUL26                   (PLLMF_4 | CFG0_PLLMF(9))           /*!< PLL source clock multiply by 26 */
#define RCU_PLL_MUL27                   (PLLMF_4 | CFG0_PLLMF(10))          /*!< PLL source clock multiply by 27 */
#define RCU_PLL_MUL28                   (PLLMF_4 | CFG0_PLLMF(11))          /*!< PLL source clock multiply by 28 */
#define RCU_PLL_MUL29                   (PLLMF_4 | CFG0_PLLMF(12))          /*!< PLL source clock multiply by 29 */
#define RCU_PLL_MUL30                   (PLLMF_4 | CFG0_PLLMF(13))          /*!< PLL source clock multiply by 30 */
#define RCU_PLL_MUL31                   (PLLMF_4 | CFG0_PLLMF(14))          /*!< PLL source clock multiply by 31 */
#define RCU_PLL_MUL32                   (PLLMF_4 | CFG0_PLLMF(15))          /*!< PLL source clock multiply by 32 */

/* USBFS prescaler select */
#define CFG0_USBPSC(regval)             (BITS(22,23) & ((uint32_t)(regval) << 22))
#define RCU_CKUSB_CKPLL_DIV1_5          CFG0_USBPSC(0)                      /*!< USBFS prescaler select CK_PLL/1.5 */
#define RCU_CKUSB_CKPLL_DIV1            CFG0_USBPSC(1)                      /*!< USBFS prescaler select CK_PLL/1 */
#define RCU_CKUSB_CKPLL_DIV2_5          CFG0_USBPSC(2)                      /*!< USBFS prescaler select CK_PLL/2.5 */
#define RCU_CKUSB_CKPLL_DIV2            CFG0_USBPSC(3)                      /*!< USBFS prescaler select CK_PLL/2 */

/* CKOUT0 clock source selection */
#define CFG0_CKOUT0SEL(regval)          (BITS(24,27) & ((uint32_t)(regval) << 24))
#define RCU_CKOUT0SRC_NONE              CFG0_CKOUT0SEL(0)                   /*!< no clock selected */
#define RCU_CKOUT0SRC_CKSYS             CFG0_CKOUT0SEL(4)                   /*!< system clock selected */
#define RCU_CKOUT0SRC_IRC8M             CFG0_CKOUT0SEL(5)                   /*!< internal 8M RC oscillator clock selected */
#define RCU_CKOUT0SRC_HXTAL             CFG0_CKOUT0SEL(6)                   /*!< high speed crystal oscillator clock (HXTAL) selected */
#define RCU_CKOUT0SRC_CKPLL_DIV2        CFG0_CKOUT0SEL(7)                   /*!< CK_PLL/2 clock selected */
#define RCU_CKOUT0SRC_CKPLL1            CFG0_CKOUT0SEL(8)                   /*!< CK_PLL1 clock selected */
#define RCU_CKOUT0SRC_CKPLL2_DIV2       CFG0_CKOUT0SEL(9)                   /*!< CK_PLL2/2 clock selected */
#define RCU_CKOUT0SRC_EXT1              CFG0_CKOUT0SEL(10)                  /*!< EXT1 selected */
#define RCU_CKOUT0SRC_CKPLL2            CFG0_CKOUT0SEL(11)                  /*!< CK_PLL2 clock selected */

/* RTC clock entry selection */
#define BDCTL_RTCSRC(regval)            (BITS(8,9) & ((uint32_t)(regval) << 8))
#define RCU_RTCSRC_NONE                 BDCTL_RTCSRC(0)                     /*!< no clock selected */
#define RCU_RTCSRC_LXTAL                BDCTL_RTCSRC(1)                     /*!< RTC source clock select LXTAL  */
#define RCU_RTCSRC_IRC40K               BDCTL_RTCSRC(2)                     /*!< RTC source clock select IRC40K */
#define RCU_RTCSRC_HXTAL_DIV_128        BDCTL_RTCSRC(3)                     /*!< RTC source clock select HXTAL/128 */

/* PREDV0 division factor */
#define CFG1_PREDV0(regval)             (BITS(0,3) & ((uint32_t)(regval) << 0))
#define RCU_PREDV0_DIV1                CFG1_PREDV0(0)                       /*!< PREDV0 input source clock not divided */
#define RCU_PREDV0_DIV2                CFG1_PREDV0(1)                       /*!< PREDV0 input source clock divided by 2 */
#define RCU_PREDV0_DIV3                CFG1_PREDV0(2)                       /*!< PREDV0 input source clock divided by 3 */
#define RCU_PREDV0_DIV4                CFG1_PREDV0(3)                       /*!< PREDV0 input source clock divided by 4 */
#define RCU_PREDV0_DIV5                CFG1_PREDV0(4)                       /*!< PREDV0 input source clock divided by 5 */
#define RCU_PREDV0_DIV6                CFG1_PREDV0(5)                       /*!< PREDV0 input source clock divided by 6 */
#define RCU_PREDV0_DIV7                CFG1_PREDV0(6)                       /*!< PREDV0 input source clock divided by 7 */
#define RCU_PREDV0_DIV8                CFG1_PREDV0(7)                       /*!< PREDV0 input source clock divided by 8 */
#define RCU_PREDV0_DIV9                CFG1_PREDV0(8)                       /*!< PREDV0 input source clock divided by 9 */
#define RCU_PREDV0_DIV10               CFG1_PREDV0(9)                       /*!< PREDV0 input source clock divided by 10 */
#define RCU_PREDV0_DIV11               CFG1_PREDV0(10)                      /*!< PREDV0 input source clock divided by 11 */
#define RCU_PREDV0_DIV12               CFG1_PREDV0(11)                      /*!< PREDV0 input source clock divided by 12 */
#define RCU_PREDV0_DIV13               CFG1_PREDV0(12)                      /*!< PREDV0 input source clock divided by 13 */
#define RCU_PREDV0_DIV14               CFG1_PREDV0(13)                      /*!< PREDV0 input source clock divided by 14 */
#define RCU_PREDV0_DIV15               CFG1_PREDV0(14)                      /*!< PREDV0 input source clock divided by 15 */
#define RCU_PREDV0_DIV16               CFG1_PREDV0(15)                      /*!< PREDV0 input source clock divided by 16 */

/* PREDV1 division factor */
#define CFG1_PREDV1(regval)             (BITS(4,7) & ((uint32_t)(regval) << 4))
#define RCU_PREDV1_DIV1                CFG1_PREDV1(0)                       /*!< PREDV1 input source clock not divided */
#define RCU_PREDV1_DIV2                CFG1_PREDV1(1)                       /*!< PREDV1 input source clock divided by 2 */
#define RCU_PREDV1_DIV3                CFG1_PREDV1(2)                       /*!< PREDV1 input source clock divided by 3 */
#define RCU_PREDV1_DIV4                CFG1_PREDV1(3)                       /*!< PREDV1 input source clock divided by 4 */
#define RCU_PREDV1_DIV5                CFG1_PREDV1(4)                       /*!< PREDV1 input source clock divided by 5 */
#define RCU_PREDV1_DIV6                CFG1_PREDV1(5)                       /*!< PREDV1 input source clock divided by 6 */
#define RCU_PREDV1_DIV7                CFG1_PREDV1(6)                       /*!< PREDV1 input source clock divided by 7 */
#define RCU_PREDV1_DIV8                CFG1_PREDV1(7)                       /*!< PREDV1 input source clock divided by 8 */
#define RCU_PREDV1_DIV9                CFG1_PREDV1(8)                       /*!< PREDV1 input source clock divided by 9 */
#define RCU_PREDV1_DIV10               CFG1_PREDV1(9)                       /*!< PREDV1 input source clock divided by 10 */
#define RCU_PREDV1_DIV11               CFG1_PREDV1(10)                      /*!< PREDV1 input source clock divided by 11 */
#define RCU_PREDV1_DIV12               CFG1_PREDV1(11)                      /*!< PREDV1 input source clock divided by 12 */
#define RCU_PREDV1_DIV13               CFG1_PREDV1(12)                      /*!< PREDV1 input source clock divided by 13 */
#define RCU_PREDV1_DIV14               CFG1_PREDV1(13)                      /*!< PREDV1 input source clock divided by 14 */
#define RCU_PREDV1_DIV15               CFG1_PREDV1(14)                      /*!< PREDV1 input source clock divided by 15 */
#define RCU_PREDV1_DIV16               CFG1_PREDV1(15)                      /*!< PREDV1 input source clock divided by 16 */

/* PLL1 clock multiplication factor */
#define CFG1_PLL1MF(regval)             (BITS(8,11) & ((uint32_t)(regval) << 8))
#define RCU_PLL1_MUL8                   CFG1_PLL1MF(6)                      /*!< PLL1 source clock multiply by 8 */
#define RCU_PLL1_MUL9                   CFG1_PLL1MF(7)                      /*!< PLL1 source clock multiply by 9 */
#define RCU_PLL1_MUL10                  CFG1_PLL1MF(8)                      /*!< PLL1 source clock multiply by 10 */
#define RCU_PLL1_MUL11                  CFG1_PLL1MF(9)                      /*!< PLL1 source clock multiply by 11 */
#define RCU_PLL1_MUL12                  CFG1_PLL1MF(10)                     /*!< PLL1 source clock multiply by 12 */
#define RCU_PLL1_MUL13                  CFG1_PLL1MF(11)                     /*!< PLL1 source clock multiply by 13 */
#define RCU_PLL1_MUL14                  CFG1_PLL1MF(12)                     /*!< PLL1 source clock multiply by 14 */
#define RCU_PLL1_MUL15                  CFG1_PLL1MF(13)                     /*!< PLL1 source clock multiply by 15 */
#define RCU_PLL1_MUL16                  CFG1_PLL1MF(14)                     /*!< PLL1 source clock multiply by 16 */
#define RCU_PLL1_MUL20                  CFG1_PLL1MF(15)                     /*!< PLL1 source clock multiply by 20 */

/* PLL2 clock multiplication factor */
#define CFG1_PLL2MF(regval)             (BITS(12,15) & ((uint32_t)(regval) << 12))
#define RCU_PLL2_MUL8                   CFG1_PLL2MF(6)                      /*!< PLL2 source clock multiply by 8 */
#define RCU_PLL2_MUL9                   CFG1_PLL2MF(7)                      /*!< PLL2 source clock multiply by 9 */
#define RCU_PLL2_MUL10                  CFG1_PLL2MF(8)                      /*!< PLL2 source clock multiply by 10 */
#define RCU_PLL2_MUL11                  CFG1_PLL2MF(9)                      /*!< PLL2 source clock multiply by 11 */
#define RCU_PLL2_MUL12                  CFG1_PLL2MF(10)                     /*!< PLL2 source clock multiply by 12 */
#define RCU_PLL2_MUL13                  CFG1_PLL2MF(11)                     /*!< PLL2 source clock multiply by 13 */
#define RCU_PLL2_MUL14                  CFG1_PLL2MF(12)                     /*!< PLL2 source clock multiply by 14 */
#define RCU_PLL2_MUL15                  CFG1_PLL2MF(13)                     /*!< PLL2 source clock multiply by 15 */
#define RCU_PLL2_MUL16                  CFG1_PLL2MF(14)                     /*!< PLL2 source clock multiply by 16 */
#define RCU_PLL2_MUL20                  CFG1_PLL2MF(15)                     /*!< PLL2 source clock multiply by 20 */


/* PREDV0 input clock source selection */
#define RCU_PREDV0SRC_HXTAL             ((uint32_t)0x00000000U)             /*!< HXTAL selected as PREDV0 input source clock */
#define RCU_PREDV0SRC_CKPLL1            RCU_CFG1_PREDV0SEL                  /*!< CK_PLL1 selected as PREDV0 input source clock */

/* I2S1 clock source selection */
#define RCU_I2S1SRC_CKSYS               ((uint32_t)0x00000000U)             /*!< system clock selected as I2S1 source clock */
#define RCU_I2S1SRC_CKPLL2_MUL2         RCU_CFG1_I2S1SEL                    /*!< (CK_PLL2 x 2) selected as I2S1 source clock */

/* I2S2 clock source selection */
#define RCU_I2S2SRC_CKSYS               ((uint32_t)0x00000000U)             /*!< system clock selected as I2S2 source clock */
#define RCU_I2S2SRC_CKPLL2_MUL2         RCU_CFG1_I2S2SEL                    /*!< (CK_PLL2 x 2) selected as I2S2 source clock */


/* deep-sleep mode voltage */
#define DSV_DSLPVS(regval)              (BITS(0,1) & ((uint32_t)(regval) << 0))
#define RCU_DEEPSLEEP_V_1_2             DSV_DSLPVS(0)                       /*!< core voltage is 1.2V in deep-sleep mode */
#define RCU_DEEPSLEEP_V_1_1             DSV_DSLPVS(1)                       /*!< core voltage is 1.1V in deep-sleep mode */
#define RCU_DEEPSLEEP_V_1_0             DSV_DSLPVS(2)                       /*!< core voltage is 1.0V in deep-sleep mode */
#define RCU_DEEPSLEEP_V_0_9             DSV_DSLPVS(3)                       /*!< core voltage is 0.9V in deep-sleep mode */


//---------------------------------------------------------------------------------------
// GPIO
//---------------------------------------------------------------------------------------

typedef struct gd32v_gpio_t
{
  volatile uint32_t  CTL[2]; // 00, 04: GPIO port control registers 0-1
  volatile uint32_t  ISTAT;  // 08: GPIO port input status register
  volatile uint32_t  OCTL;   // 0C: GPIO port output control register
  volatile uint32_t  BOP;    // 10: GPIO port bit operation register
  volatile uint32_t  BC;     // 14: GPIO bit clear register
  volatile uint32_t  LOCK;   // 18: GPIO port configuration lock register
//
} gd32v_gpio_t;

#define GPIOA_BASE    (GPIO_BASE + 0x00000000U)
#define GPIOB_BASE    (GPIO_BASE + 0x00000400U)
#define GPIOC_BASE    (GPIO_BASE + 0x00000800U)
#define GPIOD_BASE    (GPIO_BASE + 0x00000C00U)
#define GPIOE_BASE    (GPIO_BASE + 0x00001000U)

#define GPIOA         ((gd32v_gpio_t *)(GPIOA_BASE))
#define GPIOB         ((gd32v_gpio_t *)(GPIOB_BASE))
#define GPIOC         ((gd32v_gpio_t *)(GPIOC_BASE))
#define GPIOD         ((gd32v_gpio_t *)(GPIOD_BASE))
#define GPIOE         ((gd32v_gpio_t *)(GPIOE_BASE))

typedef struct gd32v_afio_t
{
  volatile uint32_t  EC;           // 00: AFIO event control register
  volatile uint32_t  PCF0;         // 04: AFIO port configuration register 0
  volatile uint32_t  EXTISS[4];    // 08, 0C, 10, 14: AFIO port EXTI sources selection registers
           uint32_t  _reserved;    // 18:
  volatile uint32_t  PCF1;         // 1C: AFIO port configuration register 1
//
} gd32v_afio_t;

#define AFIO                      ((gd32v_afio_t *)(AFIO_BASE))

/* bits definitions */

/* GPIO_CTL0 */
#define GPIO_CTL0_MD0              BITS(0, 1)                /*!< port 0 mode bits */
#define GPIO_CTL0_CTL0             BITS(2, 3)                /*!< pin 0 configuration bits */
#define GPIO_CTL0_MD1              BITS(4, 5)                /*!< port 1 mode bits */
#define GPIO_CTL0_CTL1             BITS(6, 7)                /*!< pin 1 configuration bits */
#define GPIO_CTL0_MD2              BITS(8, 9)                /*!< port 2 mode bits */
#define GPIO_CTL0_CTL2             BITS(10, 11)              /*!< pin 2 configuration bits */
#define GPIO_CTL0_MD3              BITS(12, 13)              /*!< port 3 mode bits */
#define GPIO_CTL0_CTL3             BITS(14, 15)              /*!< pin 3 configuration bits */
#define GPIO_CTL0_MD4              BITS(16, 17)              /*!< port 4 mode bits */
#define GPIO_CTL0_CTL4             BITS(18, 19)              /*!< pin 4 configuration bits */
#define GPIO_CTL0_MD5              BITS(20, 21)              /*!< port 5 mode bits */
#define GPIO_CTL0_CTL5             BITS(22, 23)              /*!< pin 5 configuration bits */
#define GPIO_CTL0_MD6              BITS(24, 25)              /*!< port 6 mode bits */
#define GPIO_CTL0_CTL6             BITS(26, 27)              /*!< pin 6 configuration bits */
#define GPIO_CTL0_MD7              BITS(28, 29)              /*!< port 7 mode bits */
#define GPIO_CTL0_CTL7             BITS(30, 31)              /*!< pin 7 configuration bits */

/* GPIO_CTL1 */
#define GPIO_CTL1_MD8              BITS(0, 1)                /*!< port 8 mode bits */
#define GPIO_CTL1_CTL8             BITS(2, 3)                /*!< pin 8 configuration bits */
#define GPIO_CTL1_MD9              BITS(4, 5)                /*!< port 9 mode bits */
#define GPIO_CTL1_CTL9             BITS(6, 7)                /*!< pin 9 configuration bits */
#define GPIO_CTL1_MD10             BITS(8, 9)                /*!< port 10 mode bits */
#define GPIO_CTL1_CTL10            BITS(10, 11)              /*!< pin 10 configuration bits */
#define GPIO_CTL1_MD11             BITS(12, 13)              /*!< port 11 mode bits */
#define GPIO_CTL1_CTL11            BITS(14, 15)              /*!< pin 11 configuration bits */
#define GPIO_CTL1_MD12             BITS(16, 17)              /*!< port 12 mode bits */
#define GPIO_CTL1_CTL12            BITS(18, 19)              /*!< pin 12 configuration bits */
#define GPIO_CTL1_MD13             BITS(20, 21)              /*!< port 13 mode bits */
#define GPIO_CTL1_CTL13            BITS(22, 23)              /*!< pin 13 configuration bits */
#define GPIO_CTL1_MD14             BITS(24, 25)              /*!< port 14 mode bits */
#define GPIO_CTL1_CTL14            BITS(26, 27)              /*!< pin 14 configuration bits */
#define GPIO_CTL1_MD15             BITS(28, 29)              /*!< port 15 mode bits */
#define GPIO_CTL1_CTL15            BITS(30, 31)              /*!< pin 15 configuration bits */

/* GPIO_ISTAT */
#define GPIO_ISTAT_ISTAT0          BIT(0)                    /*!< pin 0 input status */
#define GPIO_ISTAT_ISTAT1          BIT(1)                    /*!< pin 1 input status */
#define GPIO_ISTAT_ISTAT2          BIT(2)                    /*!< pin 2 input status */
#define GPIO_ISTAT_ISTAT3          BIT(3)                    /*!< pin 3 input status */
#define GPIO_ISTAT_ISTAT4          BIT(4)                    /*!< pin 4 input status */
#define GPIO_ISTAT_ISTAT5          BIT(5)                    /*!< pin 5 input status */
#define GPIO_ISTAT_ISTAT6          BIT(6)                    /*!< pin 6 input status */
#define GPIO_ISTAT_ISTAT7          BIT(7)                    /*!< pin 7 input status */
#define GPIO_ISTAT_ISTAT8          BIT(8)                    /*!< pin 8 input status */
#define GPIO_ISTAT_ISTAT9          BIT(9)                    /*!< pin 9 input status */
#define GPIO_ISTAT_ISTAT10         BIT(10)                   /*!< pin 10 input status */
#define GPIO_ISTAT_ISTAT11         BIT(11)                   /*!< pin 11 input status */
#define GPIO_ISTAT_ISTAT12         BIT(12)                   /*!< pin 12 input status */
#define GPIO_ISTAT_ISTAT13         BIT(13)                   /*!< pin 13 input status */
#define GPIO_ISTAT_ISTAT14         BIT(14)                   /*!< pin 14 input status */
#define GPIO_ISTAT_ISTAT15         BIT(15)                   /*!< pin 15 input status */

/* GPIO_OCTL */
#define GPIO_OCTL_OCTL0            BIT(0)                    /*!< pin 0 output bit */
#define GPIO_OCTL_OCTL1            BIT(1)                    /*!< pin 1 output bit */
#define GPIO_OCTL_OCTL2            BIT(2)                    /*!< pin 2 output bit */
#define GPIO_OCTL_OCTL3            BIT(3)                    /*!< pin 3 output bit */
#define GPIO_OCTL_OCTL4            BIT(4)                    /*!< pin 4 output bit */
#define GPIO_OCTL_OCTL5            BIT(5)                    /*!< pin 5 output bit */
#define GPIO_OCTL_OCTL6            BIT(6)                    /*!< pin 6 output bit */
#define GPIO_OCTL_OCTL7            BIT(7)                    /*!< pin 7 output bit */
#define GPIO_OCTL_OCTL8            BIT(8)                    /*!< pin 8 output bit */
#define GPIO_OCTL_OCTL9            BIT(9)                    /*!< pin 9 output bit */
#define GPIO_OCTL_OCTL10           BIT(10)                   /*!< pin 10 output bit */
#define GPIO_OCTL_OCTL11           BIT(11)                   /*!< pin 11 output bit */
#define GPIO_OCTL_OCTL12           BIT(12)                   /*!< pin 12 output bit */
#define GPIO_OCTL_OCTL13           BIT(13)                   /*!< pin 13 output bit */
#define GPIO_OCTL_OCTL14           BIT(14)                   /*!< pin 14 output bit */
#define GPIO_OCTL_OCTL15           BIT(15)                   /*!< pin 15 output bit */

/* GPIO_BOP */
#define GPIO_BOP_BOP0              BIT(0)                    /*!< pin 0 set bit */
#define GPIO_BOP_BOP1              BIT(1)                    /*!< pin 1 set bit */
#define GPIO_BOP_BOP2              BIT(2)                    /*!< pin 2 set bit */
#define GPIO_BOP_BOP3              BIT(3)                    /*!< pin 3 set bit */
#define GPIO_BOP_BOP4              BIT(4)                    /*!< pin 4 set bit */
#define GPIO_BOP_BOP5              BIT(5)                    /*!< pin 5 set bit */
#define GPIO_BOP_BOP6              BIT(6)                    /*!< pin 6 set bit */
#define GPIO_BOP_BOP7              BIT(7)                    /*!< pin 7 set bit */
#define GPIO_BOP_BOP8              BIT(8)                    /*!< pin 8 set bit */
#define GPIO_BOP_BOP9              BIT(9)                    /*!< pin 9 set bit */
#define GPIO_BOP_BOP10             BIT(10)                   /*!< pin 10 set bit */
#define GPIO_BOP_BOP11             BIT(11)                   /*!< pin 11 set bit */
#define GPIO_BOP_BOP12             BIT(12)                   /*!< pin 12 set bit */
#define GPIO_BOP_BOP13             BIT(13)                   /*!< pin 13 set bit */
#define GPIO_BOP_BOP14             BIT(14)                   /*!< pin 14 set bit */
#define GPIO_BOP_BOP15             BIT(15)                   /*!< pin 15 set bit */
#define GPIO_BOP_CR0               BIT(16)                   /*!< pin 0 clear bit */
#define GPIO_BOP_CR1               BIT(17)                   /*!< pin 1 clear bit */
#define GPIO_BOP_CR2               BIT(18)                   /*!< pin 2 clear bit */
#define GPIO_BOP_CR3               BIT(19)                   /*!< pin 3 clear bit */
#define GPIO_BOP_CR4               BIT(20)                   /*!< pin 4 clear bit */
#define GPIO_BOP_CR5               BIT(21)                   /*!< pin 5 clear bit */
#define GPIO_BOP_CR6               BIT(22)                   /*!< pin 6 clear bit */
#define GPIO_BOP_CR7               BIT(23)                   /*!< pin 7 clear bit */
#define GPIO_BOP_CR8               BIT(24)                   /*!< pin 8 clear bit */
#define GPIO_BOP_CR9               BIT(25)                   /*!< pin 9 clear bit */
#define GPIO_BOP_CR10              BIT(26)                   /*!< pin 10 clear bit */
#define GPIO_BOP_CR11              BIT(27)                   /*!< pin 11 clear bit */
#define GPIO_BOP_CR12              BIT(28)                   /*!< pin 12 clear bit */
#define GPIO_BOP_CR13              BIT(29)                   /*!< pin 13 clear bit */
#define GPIO_BOP_CR14              BIT(30)                   /*!< pin 14 clear bit */
#define GPIO_BOP_CR15              BIT(31)                   /*!< pin 15 clear bit */

/* GPIO_BC */
#define GPIO_BC_CR0                BIT(0)                    /*!< pin 0 clear bit */
#define GPIO_BC_CR1                BIT(1)                    /*!< pin 1 clear bit */
#define GPIO_BC_CR2                BIT(2)                    /*!< pin 2 clear bit */
#define GPIO_BC_CR3                BIT(3)                    /*!< pin 3 clear bit */
#define GPIO_BC_CR4                BIT(4)                    /*!< pin 4 clear bit */
#define GPIO_BC_CR5                BIT(5)                    /*!< pin 5 clear bit */
#define GPIO_BC_CR6                BIT(6)                    /*!< pin 6 clear bit */
#define GPIO_BC_CR7                BIT(7)                    /*!< pin 7 clear bit */
#define GPIO_BC_CR8                BIT(8)                    /*!< pin 8 clear bit */
#define GPIO_BC_CR9                BIT(9)                    /*!< pin 9 clear bit */
#define GPIO_BC_CR10               BIT(10)                   /*!< pin 10 clear bit */
#define GPIO_BC_CR11               BIT(11)                   /*!< pin 11 clear bit */
#define GPIO_BC_CR12               BIT(12)                   /*!< pin 12 clear bit */
#define GPIO_BC_CR13               BIT(13)                   /*!< pin 13 clear bit */
#define GPIO_BC_CR14               BIT(14)                   /*!< pin 14 clear bit */
#define GPIO_BC_CR15               BIT(15)                   /*!< pin 15 clear bit */

/* GPIO_LOCK */
#define GPIO_LOCK_LK0              BIT(0)                    /*!< pin 0 lock bit */
#define GPIO_LOCK_LK1              BIT(1)                    /*!< pin 1 lock bit */
#define GPIO_LOCK_LK2              BIT(2)                    /*!< pin 2 lock bit */
#define GPIO_LOCK_LK3              BIT(3)                    /*!< pin 3 lock bit */
#define GPIO_LOCK_LK4              BIT(4)                    /*!< pin 4 lock bit */
#define GPIO_LOCK_LK5              BIT(5)                    /*!< pin 5 lock bit */
#define GPIO_LOCK_LK6              BIT(6)                    /*!< pin 6 lock bit */
#define GPIO_LOCK_LK7              BIT(7)                    /*!< pin 7 lock bit */
#define GPIO_LOCK_LK8              BIT(8)                    /*!< pin 8 lock bit */
#define GPIO_LOCK_LK9              BIT(9)                    /*!< pin 9 lock bit */
#define GPIO_LOCK_LK10             BIT(10)                   /*!< pin 10 lock bit */
#define GPIO_LOCK_LK11             BIT(11)                   /*!< pin 11 lock bit */
#define GPIO_LOCK_LK12             BIT(12)                   /*!< pin 12 lock bit */
#define GPIO_LOCK_LK13             BIT(13)                   /*!< pin 13 lock bit */
#define GPIO_LOCK_LK14             BIT(14)                   /*!< pin 14 lock bit */
#define GPIO_LOCK_LK15             BIT(15)                   /*!< pin 15 lock bit */
#define GPIO_LOCK_LKK              BIT(16)                   /*!< pin sequence lock key */

/* AFIO_EC */
#define AFIO_EC_PIN                BITS(0, 3)                /*!< event output pin selection */
#define AFIO_EC_PORT               BITS(4, 6)                /*!< event output port selection */
#define AFIO_EC_EOE                BIT(7)                    /*!< event output enable */

/* AFIO_PCF0 */
#define AFIO_PCF0_SPI0_REMAP             BIT(0)              /*!< SPI0 remapping */
#define AFIO_PCF0_I2C0_REMAP             BIT(1)              /*!< I2C0 remapping */
#define AFIO_PCF0_USART0_REMAP           BIT(2)              /*!< USART0 remapping */
#define AFIO_PCF0_USART1_REMAP           BIT(3)              /*!< USART1 remapping */
#define AFIO_PCF0_USART2_REMAP           BITS(4, 5)          /*!< USART2 remapping */
#define AFIO_PCF0_TIMER0_REMAP           BITS(6, 7)          /*!< TIMER0 remapping */
#define AFIO_PCF0_TIMER1_REMAP           BITS(8, 9)          /*!< TIMER1 remapping */
#define AFIO_PCF0_TIMER2_REMAP           BITS(10, 11)        /*!< TIMER2 remapping */
#define AFIO_PCF0_TIMER3_REMAP           BIT(12)             /*!< TIMER3 remapping */
#define AFIO_PCF0_CAN_REMAP              BITS(13, 14)        /*!< CAN remapping */
#define AFIO_PCF0_PD01_REMAP             BIT(15)             /*!< port D0/port D1 mapping on OSC_IN/OSC_OUT */
#define AFIO_PCF0_TIMER4CH3_IREMAP       BIT(16)             /*!< TIMER3 channel3 internal remapping */
#define AFIO_PCF0_SWJ_CFG                BITS(24, 26)        /*!< serial wire JTAG configuration */
#define AFIO_PCF0_SPI2_REMAP             BIT(28)             /*!< SPI2/I2S2 remapping */
#define AFIO_PCF0_TIMER1_ITI1_REMAP      BIT(29)             /*!< TIMER1 internal trigger 1 remapping */

/* AFIO_EXTISS0 */
#define AFIO_EXTI0_SS                    BITS(0, 3)          /*!< EXTI 0 sources selection */
#define AFIO_EXTI1_SS                    BITS(4, 7)          /*!< EXTI 1 sources selection */
#define AFIO_EXTI2_SS                    BITS(8, 11)         /*!< EXTI 2 sources selection */
#define AFIO_EXTI3_SS                    BITS(12, 15)        /*!< EXTI 3 sources selection */

/* AFIO_EXTISS1 */
#define AFIO_EXTI4_SS                    BITS(0, 3)          /*!< EXTI 4 sources selection */
#define AFIO_EXTI5_SS                    BITS(4, 7)          /*!< EXTI 5 sources selection */
#define AFIO_EXTI6_SS                    BITS(8, 11)         /*!< EXTI 6 sources selection */
#define AFIO_EXTI7_SS                    BITS(12, 15)        /*!< EXTI 7 sources selection */

/* AFIO_EXTISS2 */
#define AFIO_EXTI8_SS                    BITS(0, 3)          /*!< EXTI 8 sources selection */
#define AFIO_EXTI9_SS                    BITS(4, 7)          /*!< EXTI 9 sources selection */
#define AFIO_EXTI10_SS                   BITS(8, 11)         /*!< EXTI 10 sources selection */
#define AFIO_EXTI11_SS                   BITS(12, 15)        /*!< EXTI 11 sources selection */

/* AFIO_EXTISS3 */
#define AFIO_EXTI12_SS                   BITS(0, 3)          /*!< EXTI 12 sources selection */
#define AFIO_EXTI13_SS                   BITS(4, 7)          /*!< EXTI 13 sources selection */
#define AFIO_EXTI14_SS                   BITS(8, 11)         /*!< EXTI 14 sources selection */
#define AFIO_EXTI15_SS                   BITS(12, 15)        /*!< EXTI 15 sources selection */

/* AFIO_PCF1 */
#define AFIO_PCF1_EXMC_NADV              BIT(10)             /*!< EXMC_NADV connect/disconnect */

/* constants definitions */
typedef FlagStatus bit_status;

/* GPIO mode values set */
#define GPIO_MODE_SET(n, mode)           ((uint32_t)((uint32_t)(mode) << (4U * (n))))
#define GPIO_MODE_MASK(n)                (0xFU << (4U * (n)))

/* GPIO mode definitions */
#define GPIO_MODE_AIN                    ((uint8_t)0x00U)          /*!< analog input mode */
#define GPIO_MODE_IN_FLOATING            ((uint8_t)0x04U)          /*!< floating input mode */
#define GPIO_MODE_IPD                    ((uint8_t)0x28U)          /*!< pull-down input mode */
#define GPIO_MODE_IPU                    ((uint8_t)0x48U)          /*!< pull-up input mode */
#define GPIO_MODE_OUT_OD                 ((uint8_t)0x14U)          /*!< GPIO output with open-drain */
#define GPIO_MODE_OUT_PP                 ((uint8_t)0x10U)          /*!< GPIO output with push-pull */
#define GPIO_MODE_AF_OD                  ((uint8_t)0x1CU)          /*!< AFIO output with open-drain */
#define GPIO_MODE_AF_PP                  ((uint8_t)0x18U)          /*!< AFIO output with push-pull */

/* GPIO output max speed value */
#define GPIO_OSPEED_10MHZ                ((uint8_t)0x01U)          /*!< output max speed 10MHz */
#define GPIO_OSPEED_2MHZ                 ((uint8_t)0x02U)          /*!< output max speed 2MHz */
#define GPIO_OSPEED_50MHZ                ((uint8_t)0x03U)          /*!< output max speed 50MHz */

/* GPIO event output port definitions */
#define GPIO_EVENT_PORT_GPIOA            ((uint8_t)0x00U)          /*!< event output port A */
#define GPIO_EVENT_PORT_GPIOB            ((uint8_t)0x01U)          /*!< event output port B */
#define GPIO_EVENT_PORT_GPIOC            ((uint8_t)0x02U)          /*!< event output port C */
#define GPIO_EVENT_PORT_GPIOD            ((uint8_t)0x03U)          /*!< event output port D */
#define GPIO_EVENT_PORT_GPIOE            ((uint8_t)0x04U)          /*!< event output port E */

/* GPIO output port source definitions */
#define GPIO_PORT_SOURCE_GPIOA           ((uint8_t)0x00U)          /*!< output port source A */
#define GPIO_PORT_SOURCE_GPIOB           ((uint8_t)0x01U)          /*!< output port source B */
#define GPIO_PORT_SOURCE_GPIOC           ((uint8_t)0x02U)          /*!< output port source C */
#define GPIO_PORT_SOURCE_GPIOD           ((uint8_t)0x03U)          /*!< output port source D */
#define GPIO_PORT_SOURCE_GPIOE           ((uint8_t)0x04U)          /*!< output port source E */

/* GPIO event output pin definitions */
#define GPIO_EVENT_PIN_0                 ((uint8_t)0x00U)          /*!< GPIO event pin 0 */
#define GPIO_EVENT_PIN_1                 ((uint8_t)0x01U)          /*!< GPIO event pin 1 */
#define GPIO_EVENT_PIN_2                 ((uint8_t)0x02U)          /*!< GPIO event pin 2 */
#define GPIO_EVENT_PIN_3                 ((uint8_t)0x03U)          /*!< GPIO event pin 3 */
#define GPIO_EVENT_PIN_4                 ((uint8_t)0x04U)          /*!< GPIO event pin 4 */
#define GPIO_EVENT_PIN_5                 ((uint8_t)0x05U)          /*!< GPIO event pin 5 */
#define GPIO_EVENT_PIN_6                 ((uint8_t)0x06U)          /*!< GPIO event pin 6 */
#define GPIO_EVENT_PIN_7                 ((uint8_t)0x07U)          /*!< GPIO event pin 7 */
#define GPIO_EVENT_PIN_8                 ((uint8_t)0x08U)          /*!< GPIO event pin 8 */
#define GPIO_EVENT_PIN_9                 ((uint8_t)0x09U)          /*!< GPIO event pin 9 */
#define GPIO_EVENT_PIN_10                ((uint8_t)0x0AU)          /*!< GPIO event pin 10 */
#define GPIO_EVENT_PIN_11                ((uint8_t)0x0BU)          /*!< GPIO event pin 11 */
#define GPIO_EVENT_PIN_12                ((uint8_t)0x0CU)          /*!< GPIO event pin 12 */
#define GPIO_EVENT_PIN_13                ((uint8_t)0x0DU)          /*!< GPIO event pin 13 */
#define GPIO_EVENT_PIN_14                ((uint8_t)0x0EU)          /*!< GPIO event pin 14 */
#define GPIO_EVENT_PIN_15                ((uint8_t)0x0FU)          /*!< GPIO event pin 15 */

/* GPIO output pin source definitions */
#define GPIO_PIN_SOURCE_0                ((uint8_t)0x00U)          /*!< GPIO pin source 0 */
#define GPIO_PIN_SOURCE_1                ((uint8_t)0x01U)          /*!< GPIO pin source 1 */
#define GPIO_PIN_SOURCE_2                ((uint8_t)0x02U)          /*!< GPIO pin source 2 */
#define GPIO_PIN_SOURCE_3                ((uint8_t)0x03U)          /*!< GPIO pin source 3 */
#define GPIO_PIN_SOURCE_4                ((uint8_t)0x04U)          /*!< GPIO pin source 4 */
#define GPIO_PIN_SOURCE_5                ((uint8_t)0x05U)          /*!< GPIO pin source 5 */
#define GPIO_PIN_SOURCE_6                ((uint8_t)0x06U)          /*!< GPIO pin source 6 */
#define GPIO_PIN_SOURCE_7                ((uint8_t)0x07U)          /*!< GPIO pin source 7 */
#define GPIO_PIN_SOURCE_8                ((uint8_t)0x08U)          /*!< GPIO pin source 8 */
#define GPIO_PIN_SOURCE_9                ((uint8_t)0x09U)          /*!< GPIO pin source 9 */
#define GPIO_PIN_SOURCE_10               ((uint8_t)0x0AU)          /*!< GPIO pin source 10 */
#define GPIO_PIN_SOURCE_11               ((uint8_t)0x0BU)          /*!< GPIO pin source 11 */
#define GPIO_PIN_SOURCE_12               ((uint8_t)0x0CU)          /*!< GPIO pin source 12 */
#define GPIO_PIN_SOURCE_13               ((uint8_t)0x0DU)          /*!< GPIO pin source 13 */
#define GPIO_PIN_SOURCE_14               ((uint8_t)0x0EU)          /*!< GPIO pin source 14 */
#define GPIO_PIN_SOURCE_15               ((uint8_t)0x0FU)          /*!< GPIO pin source 15 */

/* GPIO pin definitions */
#define GPIO_PIN_0                       BIT(0)                    /*!< GPIO pin 0 */
#define GPIO_PIN_1                       BIT(1)                    /*!< GPIO pin 1 */
#define GPIO_PIN_2                       BIT(2)                    /*!< GPIO pin 2 */
#define GPIO_PIN_3                       BIT(3)                    /*!< GPIO pin 3 */
#define GPIO_PIN_4                       BIT(4)                    /*!< GPIO pin 4 */
#define GPIO_PIN_5                       BIT(5)                    /*!< GPIO pin 5 */
#define GPIO_PIN_6                       BIT(6)                    /*!< GPIO pin 6 */
#define GPIO_PIN_7                       BIT(7)                    /*!< GPIO pin 7 */
#define GPIO_PIN_8                       BIT(8)                    /*!< GPIO pin 8 */
#define GPIO_PIN_9                       BIT(9)                    /*!< GPIO pin 9 */
#define GPIO_PIN_10                      BIT(10)                   /*!< GPIO pin 10 */
#define GPIO_PIN_11                      BIT(11)                   /*!< GPIO pin 11 */
#define GPIO_PIN_12                      BIT(12)                   /*!< GPIO pin 12 */
#define GPIO_PIN_13                      BIT(13)                   /*!< GPIO pin 13 */
#define GPIO_PIN_14                      BIT(14)                   /*!< GPIO pin 14 */
#define GPIO_PIN_15                      BIT(15)                   /*!< GPIO pin 15 */
#define GPIO_PIN_ALL                     BITS(0, 15)               /*!< GPIO pin all */

/* GPIO remap definitions */
#define GPIO_SPI0_REMAP                  ((uint32_t)0x00000001U)   /*!< SPI0 remapping */
#define GPIO_I2C0_REMAP                  ((uint32_t)0x00000002U)   /*!< I2C0 remapping */
#define GPIO_USART0_REMAP                ((uint32_t)0x00000004U)   /*!< USART0 remapping */
#define GPIO_USART1_REMAP                ((uint32_t)0x00000008U)   /*!< USART1 remapping */
#define GPIO_USART2_PARTIAL_REMAP        ((uint32_t)0x00140010U)   /*!< USART2 partial remapping */
#define GPIO_USART2_FULL_REMAP           ((uint32_t)0x00140030U)   /*!< USART2 full remapping */
#define GPIO_TIMER0_PARTIAL_REMAP        ((uint32_t)0x00160040U)   /*!< TIMER0 partial remapping */
#define GPIO_TIMER0_FULL_REMAP           ((uint32_t)0x001600C0U)   /*!< TIMER0 full remapping */
#define GPIO_TIMER1_PARTIAL_REMAP0       ((uint32_t)0x00180100U)   /*!< TIMER1 partial remapping */
#define GPIO_TIMER1_PARTIAL_REMAP1       ((uint32_t)0x00180200U)   /*!< TIMER1 partial remapping */
#define GPIO_TIMER1_FULL_REMAP           ((uint32_t)0x00180300U)   /*!< TIMER1 full remapping */
#define GPIO_TIMER2_PARTIAL_REMAP        ((uint32_t)0x001A0800U)   /*!< TIMER2 partial remapping */
#define GPIO_TIMER2_FULL_REMAP           ((uint32_t)0x001A0C00U)   /*!< TIMER2 full remapping */
#define GPIO_TIMER3_REMAP                ((uint32_t)0x00001000U)   /*!< TIMER3 remapping */
#define GPIO_CAN0_PARTIAL_REMAP          ((uint32_t)0x001D4000U)   /*!< CAN0 partial remapping */
#define GPIO_CAN0_FULL_REMAP             ((uint32_t)0x001D6000U)   /*!< CAN0 full remapping */
#define GPIO_PD01_REMAP                  ((uint32_t)0x00008000U)   /*!< PD01 remapping */
#define GPIO_TIMER4CH3_IREMAP            ((uint32_t)0x00200001U)   /*!< TIMER4 channel3 internal remapping */
#define GPIO_CAN1_REMAP                  ((uint32_t)0x00200040U)   /*!< CAN1 remapping */
#define GPIO_SWJ_NONJTRST_REMAP          ((uint32_t)0x00300100U)   /*!< JTAG-DP,but without NJTRST */
#define GPIO_SWJ_DISABLE_REMAP           ((uint32_t)0x00300200U)   /*!< JTAG-DP disabled */
#define GPIO_SPI2_REMAP                  ((uint32_t)0x00201100U)   /*!< SPI2 remapping */
#define GPIO_TIMER1ITI1_REMAP            ((uint32_t)0x00202000U)   /*!< TIMER1 internal trigger 1 remapping */
#define GPIO_EXMC_NADV_REMAP             ((uint32_t)0x80000400U)   /*!< EXMC_NADV connect/disconnect */


#ifdef cplusplus
}
#endif
#endif 
