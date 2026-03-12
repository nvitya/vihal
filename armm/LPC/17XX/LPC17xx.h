/******************************************************************************
 * @file:    LPC17xx.h
 * @purpose: CMSIS Cortex-M3 Core Peripheral Access Layer Header File for 
 *           NXP LPC17xx Device Series 
 * @version: V1.09
 * @date:    17. March 2010
 *----------------------------------------------------------------------------
 *
 * Copyright (C) 2008 ARM Limited. All rights reserved.
 *
 * ARM Limited (ARM) is supplying this software for use with Cortex-M3 
 * processor based microcontrollers.  This file can be freely distributed 
 * within development tools that are supporting such ARM based processors. 
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#ifndef __LPC17xx_H__
#define __LPC17xx_H__

/*
 * ==========================================================================
 * ---------- Interrupt Number Definition -----------------------------------
 * ==========================================================================
 */

typedef enum IRQn
{
/******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
  NonMaskableInt_IRQn           = -14,      /*!< 2 Non Maskable Interrupt                         */
  MemoryManagement_IRQn         = -12,      /*!< 4 Cortex-M3 Memory Management Interrupt          */
  BusFault_IRQn                 = -11,      /*!< 5 Cortex-M3 Bus Fault Interrupt                  */
  UsageFault_IRQn               = -10,      /*!< 6 Cortex-M3 Usage Fault Interrupt                */
  SVCall_IRQn                   = -5,       /*!< 11 Cortex-M3 SV Call Interrupt                   */
  DebugMonitor_IRQn             = -4,       /*!< 12 Cortex-M3 Debug Monitor Interrupt             */
  PendSV_IRQn                   = -2,       /*!< 14 Cortex-M3 Pend SV Interrupt                   */
  SysTick_IRQn                  = -1,       /*!< 15 Cortex-M3 System Tick Interrupt               */

/******  LPC17xx Specific Interrupt Numbers *******************************************************/
  WDT_IRQn                      = 0,        /*!< Watchdog Timer Interrupt                         */
  TIMER0_IRQn                   = 1,        /*!< Timer0 Interrupt                                 */
  TIMER1_IRQn                   = 2,        /*!< Timer1 Interrupt                                 */
  TIMER2_IRQn                   = 3,        /*!< Timer2 Interrupt                                 */
  TIMER3_IRQn                   = 4,        /*!< Timer3 Interrupt                                 */
  UART0_IRQn                    = 5,        /*!< UART0 Interrupt                                  */
  UART1_IRQn                    = 6,        /*!< UART1 Interrupt                                  */
  UART2_IRQn                    = 7,        /*!< UART2 Interrupt                                  */
  UART3_IRQn                    = 8,        /*!< UART3 Interrupt                                  */
  PWM1_IRQn                     = 9,        /*!< PWM1 Interrupt                                   */
  I2C0_IRQn                     = 10,       /*!< I2C0 Interrupt                                   */
  I2C1_IRQn                     = 11,       /*!< I2C1 Interrupt                                   */
  I2C2_IRQn                     = 12,       /*!< I2C2 Interrupt                                   */
  SPI_IRQn                      = 13,       /*!< SPI Interrupt                                    */
  SSP0_IRQn                     = 14,       /*!< SSP0 Interrupt                                   */
  SSP1_IRQn                     = 15,       /*!< SSP1 Interrupt                                   */
  PLL0_IRQn                     = 16,       /*!< PLL0 Lock (Main PLL) Interrupt                   */
  RTC_IRQn                      = 17,       /*!< Real Time Clock Interrupt                        */
  EINT0_IRQn                    = 18,       /*!< External Interrupt 0 Interrupt                   */
  EINT1_IRQn                    = 19,       /*!< External Interrupt 1 Interrupt                   */
  EINT2_IRQn                    = 20,       /*!< External Interrupt 2 Interrupt                   */
  EINT3_IRQn                    = 21,       /*!< External Interrupt 3 Interrupt                   */
  ADC_IRQn                      = 22,       /*!< A/D Converter Interrupt                          */
  BOD_IRQn                      = 23,       /*!< Brown-Out Detect Interrupt                       */
  USB_IRQn                      = 24,       /*!< USB Interrupt                                    */
  CAN_IRQn                      = 25,       /*!< CAN Interrupt                                    */
  DMA_IRQn                      = 26,       /*!< General Purpose DMA Interrupt                    */
  I2S_IRQn                      = 27,       /*!< I2S Interrupt                                    */
  ENET_IRQn                     = 28,       /*!< Ethernet Interrupt                               */
  RIT_IRQn                      = 29,       /*!< Repetitive Interrupt Timer Interrupt             */
  MCPWM_IRQn                    = 30,       /*!< Motor Control PWM Interrupt                      */
  QEI_IRQn                      = 31,       /*!< Quadrature Encoder Interface Interrupt           */
  PLL1_IRQn                     = 32,       /*!< PLL1 Lock (USB PLL) Interrupt                    */
  USBActivity_IRQn              = 33,       /* USB Activity interrupt                             */
  CANActivity_IRQn              = 34,       /* CAN Activity interrupt                             */
  RESERVE35_IRQn                = 35,       /* tk 19.11.13 die letzte durchverdrahtete IRQ Quelle */
} IRQn_Type;


/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/* Configuration of the Cortex-M3 Processor and Core Peripherals */
#define __MPU_PRESENT             1         /*!< MPU present or not                               */
#define __NVIC_PRIO_BITS          5         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */


#include "core_cm3.h"                       /* Cortex-M3 processor and core peripherals           */
//#include "system_LPC17xx.h"                 /* System Header                                      */


/******************************************************************************/
/*                Device Specific Peripheral registers structures             */
/******************************************************************************/

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/*------------- System Control (SC) ------------------------------------------*/
typedef struct
{
  __IO uint32_t FLASHCFG;               /* Flash Accelerator Module           */
       uint32_t RESERVED0[31];
  __IO uint32_t PLL0CON;                /* Clocking and Power Control         */
  __IO uint32_t PLL0CFG;
  __I  uint32_t PLL0STAT;
  __O  uint32_t PLL0FEED;
       uint32_t RESERVED1[4];
  __IO uint32_t PLL1CON;
  __IO uint32_t PLL1CFG;
  __I  uint32_t PLL1STAT;
  __O  uint32_t PLL1FEED;
       uint32_t RESERVED2[4];
  __IO uint32_t PCON;
  __IO uint32_t PCONP;
       uint32_t RESERVED3[15];
  __IO uint32_t CCLKCFG;
  __IO uint32_t USBCLKCFG;
  __IO uint32_t CLKSRCSEL;
  __IO uint32_t	CANSLEEPCLR;
  __IO uint32_t	CANWAKEFLAGS;
       uint32_t RESERVED4[10];
  __IO uint32_t EXTINT;                 /* External Interrupts                */
       uint32_t RESERVED5;
  __IO uint32_t EXTMODE;
  __IO uint32_t EXTPOLAR;
       uint32_t RESERVED6[12];
  __IO uint32_t RSID;                   /* Reset                              */
       uint32_t RESERVED7[7];
  __IO uint32_t SCS;                    /* Syscon Miscellaneous Registers     */
  __IO uint32_t IRCTRIM;                /* Clock Dividers                     */
  __IO uint32_t PCLKSEL0;
  __IO uint32_t PCLKSEL1;
       uint32_t RESERVED8[4];
  __IO uint32_t USBIntSt;               /* USB Device/OTG Interrupt Register  */
  __IO uint32_t DMAREQSEL;
  __IO uint32_t CLKOUTCFG;              /* Clock Output Configuration         */
 } LPC_SC_TypeDef;

/*------------- Pin Connect Block (PINCON) -----------------------------------*/
typedef struct
{
  __IO uint32_t PINSEL0;
  __IO uint32_t PINSEL1;
  __IO uint32_t PINSEL2;
  __IO uint32_t PINSEL3;
  __IO uint32_t PINSEL4;
  __IO uint32_t PINSEL5;
  __IO uint32_t PINSEL6;
  __IO uint32_t PINSEL7;
  __IO uint32_t PINSEL8;
  __IO uint32_t PINSEL9;
  __IO uint32_t PINSEL10;
       uint32_t RESERVED0[5];
  __IO uint32_t PINMODE0;
  __IO uint32_t PINMODE1;
  __IO uint32_t PINMODE2;
  __IO uint32_t PINMODE3;
  __IO uint32_t PINMODE4;
  __IO uint32_t PINMODE5;
  __IO uint32_t PINMODE6;
  __IO uint32_t PINMODE7;
  __IO uint32_t PINMODE8;
  __IO uint32_t PINMODE9;
  __IO uint32_t PINMODE_OD0;
  __IO uint32_t PINMODE_OD1;
  __IO uint32_t PINMODE_OD2;
  __IO uint32_t PINMODE_OD3;
  __IO uint32_t PINMODE_OD4;
  __IO uint32_t I2CPADCFG;
} LPC_PINCON_TypeDef;

/*------------- General Purpose Input/Output (GPIO) --------------------------*/
typedef struct
{
  union {
    __IO uint32_t FIODIR;
    struct {
      __IO uint16_t FIODIRL;
      __IO uint16_t FIODIRH;
    };
    struct {
      __IO uint8_t  FIODIR0;
      __IO uint8_t  FIODIR1;
      __IO uint8_t  FIODIR2;
      __IO uint8_t  FIODIR3;
    };
  };
  uint32_t RESERVED0[3];
  union {
    __IO uint32_t FIOMASK;
    struct {
      __IO uint16_t FIOMASKL;
      __IO uint16_t FIOMASKH;
    };
    struct {
      __IO uint8_t  FIOMASK0;
      __IO uint8_t  FIOMASK1;
      __IO uint8_t  FIOMASK2;
      __IO uint8_t  FIOMASK3;
    };
  };
  union {
    __IO uint32_t FIOPIN;
    struct {
      __IO uint16_t FIOPINL;
      __IO uint16_t FIOPINH;
    };
    struct {
      __IO uint8_t  FIOPIN0;
      __IO uint8_t  FIOPIN1;
      __IO uint8_t  FIOPIN2;
      __IO uint8_t  FIOPIN3;
    };
  };
  union {
    __IO uint32_t FIOSET;
    struct {
      __IO uint16_t FIOSETL;
      __IO uint16_t FIOSETH;
    };
    struct {
      __IO uint8_t  FIOSET0;
      __IO uint8_t  FIOSET1;
      __IO uint8_t  FIOSET2;
      __IO uint8_t  FIOSET3;
    };
  };
  union {
    __O  uint32_t FIOCLR;
    struct {
      __O  uint16_t FIOCLRL;
      __O  uint16_t FIOCLRH;
    };
    struct {
      __O  uint8_t  FIOCLR0;
      __O  uint8_t  FIOCLR1;
      __O  uint8_t  FIOCLR2;
      __O  uint8_t  FIOCLR3;
    };
  };
} LPC_GPIO_TypeDef;

typedef struct
{
  __I  uint32_t IntStatus;
  __I  uint32_t IO0IntStatR;
  __I  uint32_t IO0IntStatF;
  __O  uint32_t IO0IntClr;
  __IO uint32_t IO0IntEnR;
  __IO uint32_t IO0IntEnF;
       uint32_t RESERVED0[3];
  __I  uint32_t IO2IntStatR;
  __I  uint32_t IO2IntStatF;
  __O  uint32_t IO2IntClr;
  __IO uint32_t IO2IntEnR;
  __IO uint32_t IO2IntEnF;
} LPC_GPIOINT_TypeDef;

/*------------- Timer (TIM) --------------------------------------------------*/
typedef struct
{
  __IO uint32_t IR;
  __IO uint32_t TCR;
  __IO uint32_t TC;
  __IO uint32_t PR;
  __IO uint32_t PC;
  __IO uint32_t MCR;
  __IO uint32_t MR0;
  __IO uint32_t MR1;
  __IO uint32_t MR2;
  __IO uint32_t MR3;
  __IO uint32_t CCR;
  __I  uint32_t CR0;
  __I  uint32_t CR1;
       uint32_t RESERVED0[2];
  __IO uint32_t EMR;
       uint32_t RESERVED1[12];
  __IO uint32_t CTCR;
} LPC_TIM_TypeDef;

/*------------- Pulse-Width Modulation (PWM) ---------------------------------*/
typedef struct
{
  __IO uint32_t IR;
  __IO uint32_t TCR;
  __IO uint32_t TC;
  __IO uint32_t PR;
  __IO uint32_t PC;
  __IO uint32_t MCR;
  __IO uint32_t MR0;
  __IO uint32_t MR1;
  __IO uint32_t MR2;
  __IO uint32_t MR3;
  __IO uint32_t CCR;
  __I  uint32_t CR0;
  __I  uint32_t CR1;
  __I  uint32_t CR2;
  __I  uint32_t CR3;
       uint32_t RESERVED0;
  __IO uint32_t MR4;
  __IO uint32_t MR5;
  __IO uint32_t MR6;
  __IO uint32_t PCR;
  __IO uint32_t LER;
       uint32_t RESERVED1[7];
  __IO uint32_t CTCR;
} LPC_PWM_TypeDef;

/*------------- Universal Asynchronous Receiver Transmitter (UART) -----------*/
typedef struct
{
  union {
  __I  uint8_t  RBR;
  __O  uint8_t  THR;
  __IO uint8_t  DLL;
       uint32_t RESERVED0;
  };
  union {
  __IO uint8_t  DLM;
  __IO uint32_t IER;
  };
  union {
  __I  uint32_t IIR;
  __O  uint8_t  FCR;
  };
  __IO uint8_t  LCR;
       uint8_t  RESERVED1[7];
  __I  uint8_t  LSR;
       uint8_t  RESERVED2[7];
  __IO uint8_t  SCR;
       uint8_t  RESERVED3[3];
  __IO uint32_t ACR;
  __IO uint8_t  ICR;
       uint8_t  RESERVED4[3];
  __IO uint8_t  FDR;
       uint8_t  RESERVED5[7];
  __IO uint8_t  TER;
       uint8_t  RESERVED6[39];
  __IO uint32_t FIFOLVL;
} LPC_UART_TypeDef;

typedef struct
{
  union {
  __I  uint8_t  RBR;
  __O  uint8_t  THR;
  __IO uint8_t  DLL;
       uint32_t RESERVED0;
  };
  union {
  __IO uint8_t  DLM;
  __IO uint32_t IER;
  };
  union {
  __I  uint32_t IIR;
  __O  uint8_t  FCR;
  };
  __IO uint8_t  LCR;
       uint8_t  RESERVED1[7];
  __I  uint8_t  LSR;
       uint8_t  RESERVED2[7];
  __IO uint8_t  SCR;
       uint8_t  RESERVED3[3];
  __IO uint32_t ACR;
  __IO uint8_t  ICR;
       uint8_t  RESERVED4[3];
  __IO uint8_t  FDR;
       uint8_t  RESERVED5[7];
  __IO uint8_t  TER;
       uint8_t  RESERVED6[39];
  __IO uint32_t FIFOLVL;
} LPC_UART0_TypeDef;

typedef struct
{
  union {
  __I  uint8_t  RBR;
  __O  uint8_t  THR;
  __IO uint8_t  DLL;
       uint32_t RESERVED0;
  };
  union {
  __IO uint8_t  DLM;
  __IO uint32_t IER;
  };
  union {
  __I  uint32_t IIR;
  __O  uint8_t  FCR;
  };
  __IO uint8_t  LCR;
       uint8_t  RESERVED1[3];
  __IO uint8_t  MCR;
       uint8_t  RESERVED2[3];
  __I  uint8_t  LSR;
       uint8_t  RESERVED3[3];
  __I  uint8_t  MSR;
       uint8_t  RESERVED4[3];
  __IO uint8_t  SCR;
       uint8_t  RESERVED5[3];
  __IO uint32_t ACR;
       uint32_t RESERVED6;
  __IO uint32_t FDR;
       uint32_t RESERVED7;
  __IO uint8_t  TER;
       uint8_t  RESERVED8[27];
  __IO uint8_t  RS485CTRL;
       uint8_t  RESERVED9[3];
  __IO uint8_t  ADRMATCH;
       uint8_t  RESERVED10[3];
  __IO uint8_t  RS485DLY;
       uint8_t  RESERVED11[3];
  __IO uint32_t FIFOLVL;
} LPC_UART1_TypeDef;

/*------------- Serial Peripheral Interface (SPI) ----------------------------*/
typedef struct
{
  __IO uint32_t SPCR;
  __I  uint32_t SPSR;
  __IO uint32_t SPDR;
  __IO uint32_t SPCCR;
       uint32_t RESERVED0[3];
  __IO uint32_t SPINT;
} LPC_SPI_TypeDef;

/*------------- Synchronous Serial Communication (SSP) -----------------------*/
typedef struct
{
  __IO uint32_t CR0;
  __IO uint32_t CR1;
  __IO uint32_t DR;
  __I  uint32_t SR;
  __IO uint32_t CPSR;
  __IO uint32_t IMSC;
  __IO uint32_t RIS;
  __IO uint32_t MIS;
  __IO uint32_t ICR;
  __IO uint32_t DMACR;
} LPC_SSP_TypeDef;

/*------------- Inter-Integrated Circuit (I2C) -------------------------------*/
typedef struct
{
  __IO uint32_t I2CONSET;
  __I  uint32_t I2STAT;
  __IO uint32_t I2DAT;
  __IO uint32_t I2ADR0;
  __IO uint32_t I2SCLH;
  __IO uint32_t I2SCLL;
  __O  uint32_t I2CONCLR;
  __IO uint32_t MMCTRL;
  __IO uint32_t I2ADR1;
  __IO uint32_t I2ADR2;
  __IO uint32_t I2ADR3;
  __I  uint32_t I2DATA_BUFFER;
  __IO uint32_t I2MASK0;
  __IO uint32_t I2MASK1;
  __IO uint32_t I2MASK2;
  __IO uint32_t I2MASK3;
} LPC_I2C_TypeDef;

/*------------- Inter IC Sound (I2S) -----------------------------------------*/
typedef struct
{
  __IO uint32_t I2SDAO;
  __IO uint32_t I2SDAI;
  __O  uint32_t I2STXFIFO;
  __I  uint32_t I2SRXFIFO;
  __I  uint32_t I2SSTATE;
  __IO uint32_t I2SDMA1;
  __IO uint32_t I2SDMA2;
  __IO uint32_t I2SIRQ;
  __IO uint32_t I2STXRATE;
  __IO uint32_t I2SRXRATE;
  __IO uint32_t I2STXBITRATE;
  __IO uint32_t I2SRXBITRATE;
  __IO uint32_t I2STXMODE;
  __IO uint32_t I2SRXMODE;
} LPC_I2S_TypeDef;

/*------------- Repetitive Interrupt Timer (RIT) -----------------------------*/
typedef struct
{
  __IO uint32_t RICOMPVAL;
  __IO uint32_t RIMASK;
  __IO uint8_t  RICTRL;
       uint8_t  RESERVED0[3];
  __IO uint32_t RICOUNTER;
} LPC_RIT_TypeDef;

/*------------- Real-Time Clock (RTC) ----------------------------------------*/
typedef struct
{
  __IO uint8_t  ILR;
       uint8_t  RESERVED0[7];
  __IO uint8_t  CCR;
       uint8_t  RESERVED1[3];
  __IO uint8_t  CIIR;
       uint8_t  RESERVED2[3];
  __IO uint8_t  AMR;
       uint8_t  RESERVED3[3];
  __I  uint32_t CTIME0;
  __I  uint32_t CTIME1;
  __I  uint32_t CTIME2;
  __IO uint8_t  SEC;
       uint8_t  RESERVED4[3];
  __IO uint8_t  MIN;
       uint8_t  RESERVED5[3];
  __IO uint8_t  HOUR;
       uint8_t  RESERVED6[3];
  __IO uint8_t  DOM;
       uint8_t  RESERVED7[3];
  __IO uint8_t  DOW;
       uint8_t  RESERVED8[3];
  __IO uint16_t DOY;
       uint16_t RESERVED9;
  __IO uint8_t  MONTH;
       uint8_t  RESERVED10[3];
  __IO uint16_t YEAR;
       uint16_t RESERVED11;
  __IO uint32_t CALIBRATION;
  __IO uint32_t GPREG0;
  __IO uint32_t GPREG1;
  __IO uint32_t GPREG2;
  __IO uint32_t GPREG3;
  __IO uint32_t GPREG4;
  __IO uint8_t  RTC_AUXEN;
       uint8_t  RESERVED12[3];
  __IO uint8_t  RTC_AUX;
       uint8_t  RESERVED13[3];
  __IO uint8_t  ALSEC;
       uint8_t  RESERVED14[3];
  __IO uint8_t  ALMIN;
       uint8_t  RESERVED15[3];
  __IO uint8_t  ALHOUR;
       uint8_t  RESERVED16[3];
  __IO uint8_t  ALDOM;
       uint8_t  RESERVED17[3];
  __IO uint8_t  ALDOW;
       uint8_t  RESERVED18[3];
  __IO uint16_t ALDOY;
       uint16_t RESERVED19;
  __IO uint8_t  ALMON;
       uint8_t  RESERVED20[3];
  __IO uint16_t ALYEAR;
       uint16_t RESERVED21;
} LPC_RTC_TypeDef;

/*------------- Watchdog Timer (WDT) -----------------------------------------*/
typedef struct
{
  __IO uint8_t  WDMOD;
       uint8_t  RESERVED0[3];
  __IO uint32_t WDTC;
  __O  uint8_t  WDFEED;
       uint8_t  RESERVED1[3];
  __I  uint32_t WDTV;
  __IO uint32_t WDCLKSEL;
} LPC_WDT_TypeDef;

/*------------- Analog-to-Digital Converter (ADC) ----------------------------*/
typedef struct
{
  __IO uint32_t ADCR;
  __IO uint32_t ADGDR;
       uint32_t RESERVED0;
  __IO uint32_t ADINTEN;
  __I  uint32_t ADDR0;
  __I  uint32_t ADDR1;
  __I  uint32_t ADDR2;
  __I  uint32_t ADDR3;
  __I  uint32_t ADDR4;
  __I  uint32_t ADDR5;
  __I  uint32_t ADDR6;
  __I  uint32_t ADDR7;
  __I  uint32_t ADSTAT;
  __IO uint32_t ADTRM;
} LPC_ADC_TypeDef;

/*------------- Digital-to-Analog Converter (DAC) ----------------------------*/
typedef struct
{
  __IO uint32_t DACR;
  __IO uint32_t DACCTRL;
  __IO uint16_t DACCNTVAL;
} LPC_DAC_TypeDef;

/*------------- Motor Control Pulse-Width Modulation (MCPWM) -----------------*/
typedef struct
{
  __I  uint32_t MCCON;
  __O  uint32_t MCCON_SET;
  __O  uint32_t MCCON_CLR;
  __I  uint32_t MCCAPCON;
  __O  uint32_t MCCAPCON_SET;
  __O  uint32_t MCCAPCON_CLR;
  __IO uint32_t MCTIM0;
  __IO uint32_t MCTIM1;
  __IO uint32_t MCTIM2;
  __IO uint32_t MCPER0;
  __IO uint32_t MCPER1;
  __IO uint32_t MCPER2;
  __IO uint32_t MCPW0;
  __IO uint32_t MCPW1;
  __IO uint32_t MCPW2;
  __IO uint32_t MCDEADTIME;
  __IO uint32_t MCCCP;
  __IO uint32_t MCCR0;
  __IO uint32_t MCCR1;
  __IO uint32_t MCCR2;
  __I  uint32_t MCINTEN;
  __O  uint32_t MCINTEN_SET;
  __O  uint32_t MCINTEN_CLR;
  __I  uint32_t MCCNTCON;
  __O  uint32_t MCCNTCON_SET;
  __O  uint32_t MCCNTCON_CLR;
  __I  uint32_t MCINTFLAG;
  __O  uint32_t MCINTFLAG_SET;
  __O  uint32_t MCINTFLAG_CLR;
  __O  uint32_t MCCAP_CLR;
} LPC_MCPWM_TypeDef;

/*------------- Quadrature Encoder Interface (QEI) ---------------------------*/
typedef struct
{
  __O  uint32_t QEICON;
  __I  uint32_t QEISTAT;
  __IO uint32_t QEICONF;
  __I  uint32_t QEIPOS;
  __IO uint32_t QEIMAXPOS;
  __IO uint32_t CMPOS0;
  __IO uint32_t CMPOS1;
  __IO uint32_t CMPOS2;
  __I  uint32_t INXCNT;
  __IO uint32_t INXCMP;
  __IO uint32_t QEILOAD;
  __I  uint32_t QEITIME;
  __I  uint32_t QEIVEL;
  __I  uint32_t QEICAP;
  __IO uint32_t VELCOMP;
  __IO uint32_t FILTER;
       uint32_t RESERVED0[998];
  __O  uint32_t QEIIEC;
  __O  uint32_t QEIIES;
  __I  uint32_t QEIINTSTAT;
  __I  uint32_t QEIIE;
  __O  uint32_t QEICLR;
  __O  uint32_t QEISET;
} LPC_QEI_TypeDef;

/*------------- Controller Area Network (CAN) --------------------------------*/
typedef struct
{
  __IO uint32_t mask[512];              /* ID Masks                           */
} LPC_CANAF_RAM_TypeDef;

typedef struct                          /* Acceptance Filter Registers        */
{
  __IO uint32_t AFMR;
  __IO uint32_t SFF_sa;
  __IO uint32_t SFF_GRP_sa;
  __IO uint32_t EFF_sa;
  __IO uint32_t EFF_GRP_sa;
  __IO uint32_t ENDofTable;
  __I  uint32_t LUTerrAd;
  __I  uint32_t LUTerr;
  __IO uint32_t FCANIE;
  __IO uint32_t FCANIC0;
  __IO uint32_t FCANIC1;
} LPC_CANAF_TypeDef;

typedef struct                          /* Central Registers                  */
{
  __I  uint32_t CANTxSR;
  __I  uint32_t CANRxSR;
  __I  uint32_t CANMSR;
} LPC_CANCR_TypeDef;

typedef struct                          /* Controller Registers               */
{
  __IO uint32_t MOD;
  __O  uint32_t CMR;
  __IO uint32_t GSR;
  __I  uint32_t ICR;
  __IO uint32_t IER;
  __IO uint32_t BTR;
  __IO uint32_t EWL;
  __I  uint32_t SR;
  __IO uint32_t RFS;
  __IO uint32_t RID;
  __IO uint32_t RDA;
  __IO uint32_t RDB;
  __IO uint32_t TFI1;
  __IO uint32_t TID1;
  __IO uint32_t TDA1;
  __IO uint32_t TDB1;
  __IO uint32_t TFI2;
  __IO uint32_t TID2;
  __IO uint32_t TDA2;
  __IO uint32_t TDB2;
  __IO uint32_t TFI3;
  __IO uint32_t TID3;
  __IO uint32_t TDA3;
  __IO uint32_t TDB3;
} LPC_CAN_TypeDef;

/*------------- General Purpose Direct Memory Access (GPDMA) -----------------*/
typedef struct                          /* Common Registers                   */
{
  __I  uint32_t DMACIntStat;
  __I  uint32_t DMACIntTCStat;
  __O  uint32_t DMACIntTCClear;
  __I  uint32_t DMACIntErrStat;
  __O  uint32_t DMACIntErrClr;
  __I  uint32_t DMACRawIntTCStat;
  __I  uint32_t DMACRawIntErrStat;
  __I  uint32_t DMACEnbldChns;
  __IO uint32_t DMACSoftBReq;
  __IO uint32_t DMACSoftSReq;
  __IO uint32_t DMACSoftLBReq;
  __IO uint32_t DMACSoftLSReq;
  __IO uint32_t DMACConfig;
  __IO uint32_t DMACSync;
} LPC_GPDMA_TypeDef;

typedef struct                          /* Channel Registers                  */
{
  __IO uint32_t DMACCSrcAddr;
  __IO uint32_t DMACCDestAddr;
  __IO uint32_t DMACCLLI;
  __IO uint32_t DMACCControl;
  __IO uint32_t DMACCConfig;
} LPC_GPDMACH_TypeDef;

/*------------- Universal Serial Bus (USB) -----------------------------------*/
typedef struct
{
  __I  uint32_t HcRevision;             /* USB Host Registers                 */
  __IO uint32_t HcControl;
  __IO uint32_t HcCommandStatus;
  __IO uint32_t HcInterruptStatus;
  __IO uint32_t HcInterruptEnable;
  __IO uint32_t HcInterruptDisable;
  __IO uint32_t HcHCCA;
  __I  uint32_t HcPeriodCurrentED;
  __IO uint32_t HcControlHeadED;
  __IO uint32_t HcControlCurrentED;
  __IO uint32_t HcBulkHeadED;
  __IO uint32_t HcBulkCurrentED;
  __I  uint32_t HcDoneHead;
  __IO uint32_t HcFmInterval;
  __I  uint32_t HcFmRemaining;
  __I  uint32_t HcFmNumber;
  __IO uint32_t HcPeriodicStart;
  __IO uint32_t HcLSTreshold;
  __IO uint32_t HcRhDescriptorA;
  __IO uint32_t HcRhDescriptorB;
  __IO uint32_t HcRhStatus;
  __IO uint32_t HcRhPortStatus1;
  __IO uint32_t HcRhPortStatus2;
       uint32_t RESERVED0[40];
  __I  uint32_t Module_ID;

  __I  uint32_t OTGIntSt;               /* USB On-The-Go Registers            */
  __IO uint32_t OTGIntEn;
  __O  uint32_t OTGIntSet;
  __O  uint32_t OTGIntClr;
  __IO uint32_t OTGStCtrl;
  __IO uint32_t OTGTmr;
       uint32_t RESERVED1[58];

  __I  uint32_t USBDevIntSt;            /* USB Device Interrupt Registers     */
  __IO uint32_t USBDevIntEn;
  __O  uint32_t USBDevIntClr;
  __O  uint32_t USBDevIntSet;

  __O  uint32_t USBCmdCode;             /* USB Device SIE Command Registers   */
  __I  uint32_t USBCmdData;

  __I  uint32_t USBRxData;              /* USB Device Transfer Registers      */
  __O  uint32_t USBTxData;
  __I  uint32_t USBRxPLen;
  __O  uint32_t USBTxPLen;
  __IO uint32_t USBCtrl;
  __O  uint32_t USBDevIntPri;

  __I  uint32_t USBEpIntSt;             /* USB Device Endpoint Interrupt Regs */
  __IO uint32_t USBEpIntEn;
  __O  uint32_t USBEpIntClr;
  __O  uint32_t USBEpIntSet;
  __O  uint32_t USBEpIntPri;

  __IO uint32_t USBReEp;                /* USB Device Endpoint Realization Reg*/
  __O  uint32_t USBEpInd;
  __IO uint32_t USBMaxPSize;

  __I  uint32_t USBDMARSt;              /* USB Device DMA Registers           */
  __O  uint32_t USBDMARClr;
  __O  uint32_t USBDMARSet;
       uint32_t RESERVED2[9];
  __IO uint32_t USBUDCAH;
  __I  uint32_t USBEpDMASt;
  __O  uint32_t USBEpDMAEn;
  __O  uint32_t USBEpDMADis;
  __I  uint32_t USBDMAIntSt;
  __IO uint32_t USBDMAIntEn;
       uint32_t RESERVED3[2];
  __I  uint32_t USBEoTIntSt;
  __O  uint32_t USBEoTIntClr;
  __O  uint32_t USBEoTIntSet;
  __I  uint32_t USBNDDRIntSt;
  __O  uint32_t USBNDDRIntClr;
  __O  uint32_t USBNDDRIntSet;
  __I  uint32_t USBSysErrIntSt;
  __O  uint32_t USBSysErrIntClr;
  __O  uint32_t USBSysErrIntSet;
       uint32_t RESERVED4[15];

  union {
  __I  uint32_t I2C_RX;                 /* USB OTG I2C Registers              */
  __O  uint32_t I2C_TX;
  };
  __I  uint32_t I2C_STS;
  __IO uint32_t I2C_CTL;
  __IO uint32_t I2C_CLKHI;
  __O  uint32_t I2C_CLKLO;
       uint32_t RESERVED5[824];

  union {
  __IO uint32_t USBClkCtrl;             /* USB Clock Control Registers        */
  __IO uint32_t OTGClkCtrl;
  };
  union {
  __I  uint32_t USBClkSt;
  __I  uint32_t OTGClkSt;
  };
} LPC_USB_TypeDef;

/*------------- Ethernet Media Access Controller (EMAC) ----------------------*/
typedef struct
{
  __IO uint32_t MAC1;                   /* MAC Registers                      */
  __IO uint32_t MAC2;
  __IO uint32_t IPGT;
  __IO uint32_t IPGR;
  __IO uint32_t CLRT;
  __IO uint32_t MAXF;
  __IO uint32_t SUPP;
  __IO uint32_t TEST;
  __IO uint32_t MCFG;
  __IO uint32_t MCMD;
  __IO uint32_t MADR;
  __O  uint32_t MWTD;
  __I  uint32_t MRDD;
  __I  uint32_t MIND;
       uint32_t RESERVED0[2];
  __IO uint32_t SA0;
  __IO uint32_t SA1;
  __IO uint32_t SA2;
       uint32_t RESERVED1[45];
  __IO uint32_t Command;                /* Control Registers                  */
  __I  uint32_t Status;
  __IO uint32_t RxDescriptor;
  __IO uint32_t RxStatus;
  __IO uint32_t RxDescriptorNumber;
  __I  uint32_t RxProduceIndex;
  __IO uint32_t RxConsumeIndex;
  __IO uint32_t TxDescriptor;
  __IO uint32_t TxStatus;
  __IO uint32_t TxDescriptorNumber;
  __IO uint32_t TxProduceIndex;
  __I  uint32_t TxConsumeIndex;
       uint32_t RESERVED2[10];
  __I  uint32_t TSV0;
  __I  uint32_t TSV1;
  __I  uint32_t RSV;
       uint32_t RESERVED3[3];
  __IO uint32_t FlowControlCounter;
  __I  uint32_t FlowControlStatus;
       uint32_t RESERVED4[34];
  __IO uint32_t RxFilterCtrl;           /* Rx Filter Registers                */
  __IO uint32_t RxFilterWoLStatus;
  __IO uint32_t RxFilterWoLClear;
       uint32_t RESERVED5;
  __IO uint32_t HashFilterL;
  __IO uint32_t HashFilterH;
       uint32_t RESERVED6[882];
  __I  uint32_t IntStatus;              /* Module Control Registers           */
  __IO uint32_t IntEnable;
  __O  uint32_t IntClear;
  __O  uint32_t IntSet;
       uint32_t RESERVED7;
  __IO uint32_t PowerDown;
       uint32_t RESERVED8;
  __IO uint32_t Module_ID;
} LPC_EMAC_TypeDef;

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif


/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/* Base addresses                                                             */
#define LPC_FLASH_BASE        (0x00000000UL)
#define LPC_RAM_BASE          (0x10000000UL)
#define LPC_GPIO_BASE         (0x2009C000UL)
#define LPC_APB0_BASE         (0x40000000UL)
#define LPC_APB1_BASE         (0x40080000UL)
#define LPC_AHB_BASE          (0x50000000UL)
#define LPC_CM3_BASE          (0xE0000000UL)

/* APB0 peripherals                                                           */
#define LPC_WDT_BASE          (LPC_APB0_BASE + 0x00000)
#define LPC_TIM0_BASE         (LPC_APB0_BASE + 0x04000)
#define LPC_TIM1_BASE         (LPC_APB0_BASE + 0x08000)
#define LPC_UART0_BASE        (LPC_APB0_BASE + 0x0C000)
#define LPC_UART1_BASE        (LPC_APB0_BASE + 0x10000)
#define LPC_PWM1_BASE         (LPC_APB0_BASE + 0x18000)
#define LPC_I2C0_BASE         (LPC_APB0_BASE + 0x1C000)
#define LPC_SPI_BASE          (LPC_APB0_BASE + 0x20000)
#define LPC_RTC_BASE          (LPC_APB0_BASE + 0x24000)
#define LPC_GPIOINT_BASE      (LPC_APB0_BASE + 0x28080)
#define LPC_PINCON_BASE       (LPC_APB0_BASE + 0x2C000)
#define LPC_SSP1_BASE         (LPC_APB0_BASE + 0x30000)
#define LPC_ADC_BASE          (LPC_APB0_BASE + 0x34000)
#define LPC_CANAF_RAM_BASE    (LPC_APB0_BASE + 0x38000)
#define LPC_CANAF_BASE        (LPC_APB0_BASE + 0x3C000)
#define LPC_CANCR_BASE        (LPC_APB0_BASE + 0x40000)
#define LPC_CAN1_BASE         (LPC_APB0_BASE + 0x44000)
#define LPC_CAN2_BASE         (LPC_APB0_BASE + 0x48000)
#define LPC_I2C1_BASE         (LPC_APB0_BASE + 0x5C000)

/* APB1 peripherals                                                           */
#define LPC_SSP0_BASE         (LPC_APB1_BASE + 0x08000)
#define LPC_DAC_BASE          (LPC_APB1_BASE + 0x0C000)
#define LPC_TIM2_BASE         (LPC_APB1_BASE + 0x10000)
#define LPC_TIM3_BASE         (LPC_APB1_BASE + 0x14000)
#define LPC_UART2_BASE        (LPC_APB1_BASE + 0x18000)
#define LPC_UART3_BASE        (LPC_APB1_BASE + 0x1C000)
#define LPC_I2C2_BASE         (LPC_APB1_BASE + 0x20000)
#define LPC_I2S_BASE          (LPC_APB1_BASE + 0x28000)
#define LPC_RIT_BASE          (LPC_APB1_BASE + 0x30000)
#define LPC_MCPWM_BASE        (LPC_APB1_BASE + 0x38000)
#define LPC_QEI_BASE          (LPC_APB1_BASE + 0x3C000)
#define LPC_SC_BASE           (LPC_APB1_BASE + 0x7C000)

/* AHB peripherals                                                            */
#define LPC_EMAC_BASE         (LPC_AHB_BASE  + 0x00000)
#define LPC_GPDMA_BASE        (LPC_AHB_BASE  + 0x04000)
#define LPC_GPDMACH0_BASE     (LPC_AHB_BASE  + 0x04100)
#define LPC_GPDMACH1_BASE     (LPC_AHB_BASE  + 0x04120)
#define LPC_GPDMACH2_BASE     (LPC_AHB_BASE  + 0x04140)
#define LPC_GPDMACH3_BASE     (LPC_AHB_BASE  + 0x04160)
#define LPC_GPDMACH4_BASE     (LPC_AHB_BASE  + 0x04180)
#define LPC_GPDMACH5_BASE     (LPC_AHB_BASE  + 0x041A0)
#define LPC_GPDMACH6_BASE     (LPC_AHB_BASE  + 0x041C0)
#define LPC_GPDMACH7_BASE     (LPC_AHB_BASE  + 0x041E0)
#define LPC_USB_BASE          (LPC_AHB_BASE  + 0x0C000)

/* GPIOs                                                                      */
#define LPC_GPIO0_BASE        (LPC_GPIO_BASE + 0x00000)
#define LPC_GPIO1_BASE        (LPC_GPIO_BASE + 0x00020)
#define LPC_GPIO2_BASE        (LPC_GPIO_BASE + 0x00040)
#define LPC_GPIO3_BASE        (LPC_GPIO_BASE + 0x00060)
#define LPC_GPIO4_BASE        (LPC_GPIO_BASE + 0x00080)


/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/
#define LPC_SC                ((LPC_SC_TypeDef        *) LPC_SC_BASE       )
#define LPC_GPIO0             ((LPC_GPIO_TypeDef      *) LPC_GPIO0_BASE    )
#define LPC_GPIO1             ((LPC_GPIO_TypeDef      *) LPC_GPIO1_BASE    )
#define LPC_GPIO2             ((LPC_GPIO_TypeDef      *) LPC_GPIO2_BASE    )
#define LPC_GPIO3             ((LPC_GPIO_TypeDef      *) LPC_GPIO3_BASE    )
#define LPC_GPIO4             ((LPC_GPIO_TypeDef      *) LPC_GPIO4_BASE    )
#define LPC_WDT               ((LPC_WDT_TypeDef       *) LPC_WDT_BASE      )
#define LPC_TIM0              ((LPC_TIM_TypeDef       *) LPC_TIM0_BASE     )
#define LPC_TIM1              ((LPC_TIM_TypeDef       *) LPC_TIM1_BASE     )
#define LPC_TIM2              ((LPC_TIM_TypeDef       *) LPC_TIM2_BASE     )
#define LPC_TIM3              ((LPC_TIM_TypeDef       *) LPC_TIM3_BASE     )
#define LPC_RIT               ((LPC_RIT_TypeDef       *) LPC_RIT_BASE      )
#define LPC_UART0             ((LPC_UART0_TypeDef     *) LPC_UART0_BASE    )
#define LPC_UART1             ((LPC_UART1_TypeDef     *) LPC_UART1_BASE    )
#define LPC_UART2             ((LPC_UART_TypeDef      *) LPC_UART2_BASE    )
#define LPC_UART3             ((LPC_UART_TypeDef      *) LPC_UART3_BASE    )
#define LPC_PWM1              ((LPC_PWM_TypeDef       *) LPC_PWM1_BASE     )
#define LPC_I2C0              ((LPC_I2C_TypeDef       *) LPC_I2C0_BASE     )
#define LPC_I2C1              ((LPC_I2C_TypeDef       *) LPC_I2C1_BASE     )
#define LPC_I2C2              ((LPC_I2C_TypeDef       *) LPC_I2C2_BASE     )
#define LPC_I2S               ((LPC_I2S_TypeDef       *) LPC_I2S_BASE      )
#define LPC_SPI               ((LPC_SPI_TypeDef       *) LPC_SPI_BASE      )
#define LPC_RTC               ((LPC_RTC_TypeDef       *) LPC_RTC_BASE      )
#define LPC_GPIOINT           ((LPC_GPIOINT_TypeDef   *) LPC_GPIOINT_BASE  )
#define LPC_PINCON            ((LPC_PINCON_TypeDef    *) LPC_PINCON_BASE   )
#define LPC_SSP0              ((LPC_SSP_TypeDef       *) LPC_SSP0_BASE     )
#define LPC_SSP1              ((LPC_SSP_TypeDef       *) LPC_SSP1_BASE     )
#define LPC_ADC               ((LPC_ADC_TypeDef       *) LPC_ADC_BASE      )
#define LPC_DAC               ((LPC_DAC_TypeDef       *) LPC_DAC_BASE      )
#define LPC_CANAF_RAM         ((LPC_CANAF_RAM_TypeDef *) LPC_CANAF_RAM_BASE)
#define LPC_CANAF             ((LPC_CANAF_TypeDef     *) LPC_CANAF_BASE    )
#define LPC_CANCR             ((LPC_CANCR_TypeDef     *) LPC_CANCR_BASE    )
#define LPC_CAN1              ((LPC_CAN_TypeDef       *) LPC_CAN1_BASE     )
#define LPC_CAN2              ((LPC_CAN_TypeDef       *) LPC_CAN2_BASE     )
#define LPC_MCPWM             ((LPC_MCPWM_TypeDef     *) LPC_MCPWM_BASE    )
#define LPC_QEI               ((LPC_QEI_TypeDef       *) LPC_QEI_BASE      )
#define LPC_EMAC              ((LPC_EMAC_TypeDef      *) LPC_EMAC_BASE     )
#define LPC_GPDMA             ((LPC_GPDMA_TypeDef     *) LPC_GPDMA_BASE    )
#define LPC_GPDMACH0          ((LPC_GPDMACH_TypeDef   *) LPC_GPDMACH0_BASE )
#define LPC_GPDMACH1          ((LPC_GPDMACH_TypeDef   *) LPC_GPDMACH1_BASE )
#define LPC_GPDMACH2          ((LPC_GPDMACH_TypeDef   *) LPC_GPDMACH2_BASE )
#define LPC_GPDMACH3          ((LPC_GPDMACH_TypeDef   *) LPC_GPDMACH3_BASE )
#define LPC_GPDMACH4          ((LPC_GPDMACH_TypeDef   *) LPC_GPDMACH4_BASE )
#define LPC_GPDMACH5          ((LPC_GPDMACH_TypeDef   *) LPC_GPDMACH5_BASE )
#define LPC_GPDMACH6          ((LPC_GPDMACH_TypeDef   *) LPC_GPDMACH6_BASE )
#define LPC_GPDMACH7          ((LPC_GPDMACH_TypeDef   *) LPC_GPDMACH7_BASE )
#define LPC_USB               ((LPC_USB_TypeDef       *) LPC_USB_BASE      )

//-------------------------------------------------------------------------------
/*
#define UART_IER_RBR		0x01
#define UART_IER_THRE	  0x02
#define UART_IER_RLS		0x04

#define UART_IIR_PEND	  0x01
#define UART_IIR_RLS		0x03
#define UART_IIR_RDA		0x02
#define UART_IIR_CTI		0x06
#define UART_IIR_THRE	  0x01

#define UART_LSR_RDR		0x01
#define UART_LSR_OE		  0x02
#define UART_LSR_PE		  0x04
#define UART_LSR_FE		  0x08
#define UART_LSR_BI		  0x10
#define UART_LSR_THRE	  0x20
#define UART_LSR_TEMT	  0x40
#define UART_LSR_RXFE	  0x80

//-------------------------------------------------------------------------------

#define SC_PCONP_PCUART0  0x00000008
#define SC_PCONP_PCUART1  0x00000010
*/
//-------------------------------------------------------------------------------

// P0.0
#define PINCON_PINSEL0_P0_0                 (0x00000000)
#define PINCON_PINSEL0_RD1                  (0x00000001)
#define PINCON_PINSEL0_TXD3                 (0x00000002)
#define PINCON_PINSEL0_SDA1                 (0x00000003)

#define PINCON_PINMODE0_P0_0_PULLUP         (0x00000000)
#define PINCON_PINMODE0_P0_0_REPEATER       (0x00000001)
#define PINCON_PINMODE0_P0_0_FLOAT          (0x00000002)
#define PINCON_PINMODE0_P0_0_PULLDOWN       (0x00000003)

#define PINCON_PINMODE_OD0_P0_0_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD0_P0_0_OPEN_DRAIN  (0x00000001)

// P0.1
#define PINCON_PINSEL0_P0_1                 (0x00000000)
#define PINCON_PINSEL0_TD1                  (0x00000004)
#define PINCON_PINSEL0_RXD3                 (0x00000008)
#define PINCON_PINSEL0_SCL1                 (0x0000000C)

#define PINCON_PINMODE0_P0_1_PULLUP         (0x00000000)
#define PINCON_PINMODE0_P0_1_REPEATER       (0x00000004)
#define PINCON_PINMODE0_P0_1_FLOAT          (0x00000008)
#define PINCON_PINMODE0_P0_1_PULLDOWN       (0x0000000C)

#define PINCON_PINMODE_OD0_P0_1_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD0_P0_1_OPEN_DRAIN  (0x00000002)

// P0.2
#define PINCON_PINSEL0_P0_2                 (0x00000000)
#define PINCON_PINSEL0_TXD0                 (0x00000010)
#define PINCON_PINSEL0_AD0_7                (0x00000020)

#define PINCON_PINMODE0_P0_2_PULLUP         (0x00000000)
#define PINCON_PINMODE0_P0_2_REPEATER       (0x00000010)
#define PINCON_PINMODE0_P0_2_FLOAT          (0x00000020)
#define PINCON_PINMODE0_P0_2_PULLDOWN       (0x00000030)

#define PINCON_PINMODE_OD0_P0_2_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD0_P0_2_OPEN_DRAIN  (0x00000004)

// P0.3
#define PINCON_PINSEL0_P0_3                 (0x00000000)
#define PINCON_PINSEL0_RXD0                 (0x00000040)
#define PINCON_PINSEL0_AD0_6                (0x00000080)

#define PINCON_PINMODE0_P0_3_PULLUP         (0x00000000)
#define PINCON_PINMODE0_P0_3_REPEATER       (0x00000040)
#define PINCON_PINMODE0_P0_3_FLOAT          (0x00000080)
#define PINCON_PINMODE0_P0_3_PULLDOWN       (0x000000C0)

#define PINCON_PINMODE_OD0_P0_3_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD0_P0_3_OPEN_DRAIN  (0x00000008)

// P0.4
#define PINCON_PINSEL0_P0_4                 (0x00000000)
#define PINCON_PINSEL0_I2SRX_CLK            (0x00000100)
#define PINCON_PINSEL0_RD2                  (0x00000200)
#define PINCON_PINSEL0_CAP2_0               (0x00000300)

#define PINCON_PINMODE0_P0_4_PULLUP         (0x00000000)
#define PINCON_PINMODE0_P0_4_REPEATER       (0x00000100)
#define PINCON_PINMODE0_P0_4_FLOAT          (0x00000200)
#define PINCON_PINMODE0_P0_4_PULLDOWN       (0x00000300)

#define PINCON_PINMODE_OD0_P0_4_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD0_P0_4_OPEN_DRAIN  (0x00000010)

// P0.5
#define PINCON_PINSEL0_P0_5                 (0x00000000)
#define PINCON_PINSEL0_I2SRX_WS             (0x00000400)
#define PINCON_PINSEL0_TD2                  (0x00000800)
#define PINCON_PINSEL0_CAP2_1               (0x00000C00)

#define PINCON_PINMODE0_P0_5_PULLUP         (0x00000000)
#define PINCON_PINMODE0_P0_5_REPEATER       (0x00000400)
#define PINCON_PINMODE0_P0_5_FLOAT          (0x00000800)
#define PINCON_PINMODE0_P0_5_PULLDOWN       (0x00000C00)

#define PINCON_PINMODE_OD0_P0_5_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD0_P0_5_OPEN_DRAIN  (0x00000020)

// P0.6
#define PINCON_PINSEL0_P0_6                 (0x00000000)
#define PINCON_PINSEL0_I2SRX_SDA            (0x00001000)
#define PINCON_PINSEL0_SSEL1                (0x00002000)
#define PINCON_PINSEL0_MAT2_0               (0x00003000)

#define PINCON_PINMODE0_P0_6_PULLUP         (0x00000000)
#define PINCON_PINMODE0_P0_6_REPEATER       (0x00001000)
#define PINCON_PINMODE0_P0_6_FLOAT          (0x00002000)
#define PINCON_PINMODE0_P0_6_PULLDOWN       (0x00003000)

#define PINCON_PINMODE_OD0_P0_6_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD0_P0_6_OPEN_DRAIN  (0x00000040)

// P0.7
#define PINCON_PINSEL0_P0_7                 (0x00000000)
#define PINCON_PINSEL0_I2STX_CLK            (0x00004000)
#define PINCON_PINSEL0_SCK1                 (0x00008000)
#define PINCON_PINSEL0_MAT2_1               (0x0000C000)

#define PINCON_PINMODE0_P0_7_PULLUP         (0x00000000)
#define PINCON_PINMODE0_P0_7_REPEATER       (0x00004000)
#define PINCON_PINMODE0_P0_7_FLOAT          (0x00008000)
#define PINCON_PINMODE0_P0_7_PULLDOWN       (0x0000C000)

#define PINCON_PINMODE_OD0_P0_7_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD0_P0_7_OPEN_DRAIN  (0x00000080)

// P0.8
#define PINCON_PINSEL0_P0_8                 (0x00000000)
#define PINCON_PINSEL0_I2STX_WS             (0x00010000)
#define PINCON_PINSEL0_MISO1                (0x00020000)
#define PINCON_PINSEL0_MAT2_2               (0x00030000)

#define PINCON_PINMODE0_P0_8_PULLUP         (0x00000000)
#define PINCON_PINMODE0_P0_8_REPEATER       (0x00010000)
#define PINCON_PINMODE0_P0_8_FLOAT          (0x00020000)
#define PINCON_PINMODE0_P0_8_PULLDOWN       (0x00030000)

#define PINCON_PINMODE_OD0_P0_8_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD0_P0_8_OPEN_DRAIN  (0x00000100)

// P0.9
#define PINCON_PINSEL0_P0_9                 (0x00000000)
#define PINCON_PINSEL0_I2STX_SDA            (0x00040000)
#define PINCON_PINSEL0_MOSI1                (0x00080000)
#define PINCON_PINSEL0_MAT2_3               (0x000C0000)

#define PINCON_PINMODE0_P0_9_PULLUP         (0x00000000)
#define PINCON_PINMODE0_P0_9_REPEATER       (0x00040000)
#define PINCON_PINMODE0_P0_9_FLOAT          (0x00080000)
#define PINCON_PINMODE0_P0_9_PULLDOWN       (0x000C0000)

#define PINCON_PINMODE_OD0_P0_9_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD0_P0_9_OPEN_DRAIN  (0x00000200)

// P0.10
#define PINCON_PINSEL0_P0_10                (0x00000000)
#define PINCON_PINSEL0_TXD2                 (0x00100000)
#define PINCON_PINSEL0_SDA2                 (0x00200000)
#define PINCON_PINSEL0_MAT3_0               (0x00300000)

#define PINCON_PINMODE0_P0_10_PULLUP        (0x00000000)
#define PINCON_PINMODE0_P0_10_REPEATER      (0x00100000)
#define PINCON_PINMODE0_P0_10_FLOAT         (0x00200000)
#define PINCON_PINMODE0_P0_10_PULLDOWN      (0x00300000)

#define PINCON_PINMODE_OD0_P0_10_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_10_OPEN_DRAIN (0x00000400)

// P0.11
#define PINCON_PINSEL0_P0_11                (0x00000000)
#define PINCON_PINSEL0_RXD2                 (0x00400000)
#define PINCON_PINSEL0_SCL2                 (0x00800000)
#define PINCON_PINSEL0_MAT3_1               (0x00C00000)

#define PINCON_PINMODE0_P0_11_PULLUP        (0x00000000)
#define PINCON_PINMODE0_P0_11_REPEATER      (0x00400000)
#define PINCON_PINMODE0_P0_11_FLOAT         (0x00800000)
#define PINCON_PINMODE0_P0_11_PULLDOWN      (0x00C00000)

#define PINCON_PINMODE_OD0_P0_11_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_11_OPEN_DRAIN (0x00000800)

// P0.12 - Not present
// P0.13 - Not present
// P0.14 - Not present

// P0.15
#define PINCON_PINSEL0_P0_15_MASK           (0xC0000000)
#define PINCON_PINSEL0_P0_15                (0x00000000)
#define PINCON_PINSEL0_TXD1                 (0x40000000)
#define PINCON_PINSEL0_SCK0                 (0x80000000)
#define PINCON_PINSEL0_SCK                  (0xC0000000)

#define PINCON_PINMODE0_P0_15_MASK          (0xC0000000)
#define PINCON_PINMODE0_P0_15_PULLUP        (0x00000000)
#define PINCON_PINMODE0_P0_15_REPEATER      (0x40000000)
#define PINCON_PINMODE0_P0_15_FLOAT         (0x80000000)
#define PINCON_PINMODE0_P0_15_PULLDOWN      (0xC0000000)

#define PINCON_PINMODE_OD0_P0_15_MASK       (0x00008000)
#define PINCON_PINMODE_OD0_P0_15_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_15_OPEN_DRAIN (0x00008000)

// P0.16
#define PINCON_PINSEL1_P0_16_MASK           (0x00000003)
#define PINCON_PINSEL1_P0_16                (0x00000000)
#define PINCON_PINSEL1_RXD1                 (0x00000001)
#define PINCON_PINSEL1_SSEL0                (0x00000002)
#define PINCON_PINSEL1_SSEL                 (0x00000003)

#define PINCON_PINMODE1_P0_16_MASK          (0x00000003)
#define PINCON_PINMODE1_P0_16_PULLUP        (0x00000000)
#define PINCON_PINMODE1_P0_16_REPEATER      (0x00000001)
#define PINCON_PINMODE1_P0_16_FLOAT         (0x00000002)
#define PINCON_PINMODE1_P0_16_PULLDOWN      (0x00000003)

#define PINCON_PINMODE_OD0_P0_16_MASK       (0x00010000)
#define PINCON_PINMODE_OD0_P0_16_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_16_OPEN_DRAIN (0x00010000)

// P0.17
#define PINCON_PINSEL1_P0_17_MASK           (0x0000000C)
#define PINCON_PINSEL1_P0_17                (0x00000000)
#define PINCON_PINSEL1_CTS1                 (0x00000004)
#define PINCON_PINSEL1_MISO0                (0x00000008)
#define PINCON_PINSEL1_MISO                 (0x0000000C)

#define PINCON_PINMODE1_P0_17_MASK          (0x0000000C)
#define PINCON_PINMODE1_P0_17_PULLUP        (0x00000000)
#define PINCON_PINMODE1_P0_17_REPEATER      (0x00000004)
#define PINCON_PINMODE1_P0_17_FLOAT         (0x00000008)
#define PINCON_PINMODE1_P0_17_PULLDOWN      (0x0000000C)

#define PINCON_PINMODE_OD0_P0_17_MASK       (0x00020000)
#define PINCON_PINMODE_OD0_P0_17_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_17_OPEN_DRAIN (0x00020000)

// P0.18
#define PINCON_PINSEL1_P0_18_MASK           (0x00000030)
#define PINCON_PINSEL1_P0_18                (0x00000000)
#define PINCON_PINSEL1_DCD1                 (0x00000010)
#define PINCON_PINSEL1_MOSI0                (0x00000020)
#define PINCON_PINSEL1_MOSI                 (0x00000030)

#define PINCON_PINMODE1_P0_18_MASK          (0x00000030)
#define PINCON_PINMODE1_P0_18_PULLUP        (0x00000000)
#define PINCON_PINMODE1_P0_18_REPEATER      (0x00000010)
#define PINCON_PINMODE1_P0_18_FLOAT         (0x00000020)
#define PINCON_PINMODE1_P0_18_PULLDOWN      (0x00000030)

#define PINCON_PINMODE_OD0_P0_18_MASK       (0x00040000)
#define PINCON_PINMODE_OD0_P0_18_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_18_OPEN_DRAIN (0x00040000)

// P0.19
#define PINCON_PINSEL1_P0_19                (0x00000000)
#define PINCON_PINSEL1_DSR1                 (0x00000040)
#define PINCON_PINSEL1_SDA1                 (0x000000C0)

#define PINCON_PINMODE1_P0_19_PULLUP        (0x00000000)
#define PINCON_PINMODE1_P0_19_REPEATER      (0x00000040)
#define PINCON_PINMODE1_P0_19_FLOAT         (0x00000080)
#define PINCON_PINMODE1_P0_19_PULLDOWN      (0x000000C0)

#define PINCON_PINMODE_OD0_P0_19_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_19_OPEN_DRAIN (0x00080000)

// P0.20
#define PINCON_PINSEL1_P0_20                (0x00000000)
#define PINCON_PINSEL1_DTR1                 (0x00000100)
#define PINCON_PINSEL1_SCL1                 (0x00000300)

#define PINCON_PINMODE1_P0_20_PULLUP        (0x00000000)
#define PINCON_PINMODE1_P0_20_REPEATER      (0x00000100)
#define PINCON_PINMODE1_P0_20_FLOAT         (0x00000200)
#define PINCON_PINMODE1_P0_20_PULLDOWN      (0x00000300)

#define PINCON_PINMODE_OD0_P0_20_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_20_OPEN_DRAIN (0x00100000)

// P0.21
#define PINCON_PINSEL1_P0_21                (0x00000000)
#define PINCON_PINSEL1_RI1                  (0x00000400)
#define PINCON_PINSEL1_RD1                  (0x00000C00)

#define PINCON_PINMODE1_P0_21_PULLUP        (0x00000000)
#define PINCON_PINMODE1_P0_21_REPEATER      (0x00000400)
#define PINCON_PINMODE1_P0_21_FLOAT         (0x00000800)
#define PINCON_PINMODE1_P0_21_PULLDOWN      (0x00000C00)

#define PINCON_PINMODE_OD0_P0_21_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_21_OPEN_DRAIN (0x00200000)

// P0.22
#define PINCON_PINSEL1_P0_22                (0x00000000)
#define PINCON_PINSEL1_RTS1                 (0x00001000)
#define PINCON_PINSEL1_TD1                  (0x00003000)

#define PINCON_PINMODE1_P0_22_PULLUP        (0x00000000)
#define PINCON_PINMODE1_P0_22_REPEATER      (0x00001000)
#define PINCON_PINMODE1_P0_22_FLOAT         (0x00002000)
#define PINCON_PINMODE1_P0_22_PULLDOWN      (0x00003000)

#define PINCON_PINMODE_OD0_P0_22_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_22_OPEN_DRAIN (0x00400000)

// P0.23
#define PINCON_PINSEL1_P0_23                (0x00000000)
#define PINCON_PINSEL1_AD0_0                (0x00004000)
#define PINCON_PINSEL1_I2SRX_CLK            (0x00008000)
#define PINCON_PINSEL1_CAP3_0               (0x0000C000)

#define PINCON_PINMODE1_P0_23_PULLUP        (0x00000000)
#define PINCON_PINMODE1_P0_23_REPEATER      (0x00004000)
#define PINCON_PINMODE1_P0_23_FLOAT         (0x00008000)
#define PINCON_PINMODE1_P0_23_PULLDOWN      (0x0000C000)

#define PINCON_PINMODE_OD0_P0_23_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_23_OPEN_DRAIN (0x00800000)

// P0.24
#define PINCON_PINSEL1_P0_24                (0x00000000)
#define PINCON_PINSEL1_AD0_1                (0x00010000)
#define PINCON_PINSEL1_I2SRX_WS             (0x00020000)
#define PINCON_PINSEL1_CAP3_1               (0x00030000)

#define PINCON_PINMODE1_P0_24_PULLUP        (0x00000000)
#define PINCON_PINMODE1_P0_24_REPEATER      (0x00010000)
#define PINCON_PINMODE1_P0_24_FLOAT         (0x00020000)
#define PINCON_PINMODE1_P0_24_PULLDOWN      (0x00030000)

#define PINCON_PINMODE_OD0_P0_24_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_24_OPEN_DRAIN (0x01000000)

// P0.25
#define PINCON_PINSEL1_P0_25                (0x00000000)
#define PINCON_PINSEL1_AD0_2                (0x00040000)
#define PINCON_PINSEL1_I2SRX_SDA            (0x00080000)
#define PINCON_PINSEL1_TXD3                 (0x000C0000)

#define PINCON_PINMODE1_P0_25_PULLUP        (0x00000000)
#define PINCON_PINMODE1_P0_25_REPEATER      (0x00040000)
#define PINCON_PINMODE1_P0_25_FLOAT         (0x00080000)
#define PINCON_PINMODE1_P0_25_PULLDOWN      (0x000C0000)

#define PINCON_PINMODE_OD0_P0_25_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_25_OPEN_DRAIN (0x02000000)

// P0.26
#define PINCON_PINSEL1_P0_26                (0x00000000)
#define PINCON_PINSEL1_AD0_3                (0x00100000)
#define PINCON_PINSEL1_AOUT                 (0x00200000)
#define PINCON_PINSEL1_RXD3                 (0x00300000)

#define PINCON_PINMODE1_P0_26_PULLUP        (0x00000000)
#define PINCON_PINMODE1_P0_26_REPEATER      (0x00100000)
#define PINCON_PINMODE1_P0_26_FLOAT         (0x00200000)
#define PINCON_PINMODE1_P0_26_PULLDOWN      (0x00300000)

#define PINCON_PINMODE_OD0_P0_26_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_26_OPEN_DRAIN (0x04000000)

// P0.27
#define PINCON_PINSEL1_P0_27                (0x00000000)
#define PINCON_PINSEL1_SDA0                 (0x00400000)
#define PINCON_PINSEL1_USB_SDA              (0x00800000)

#define PINCON_PINMODE1_P0_27_PULLUP        (0x00000000)
#define PINCON_PINMODE1_P0_27_REPEATER      (0x00400000)
#define PINCON_PINMODE1_P0_27_FLOAT         (0x00800000)
#define PINCON_PINMODE1_P0_27_PULLDOWN      (0x00C00000)

#define PINCON_PINMODE_OD0_P0_27_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_27_OPEN_DRAIN (0x08000000)

// P0.28
#define PINCON_PINSEL1_P0_28                (0x00000000)
#define PINCON_PINSEL1_SCL0                 (0x01000000)
#define PINCON_PINSEL1_USB_SCL              (0x02000000)

#define PINCON_PINMODE1_P0_28_PULLUP        (0x00000000)
#define PINCON_PINMODE1_P0_28_REPEATER      (0x01000000)
#define PINCON_PINMODE1_P0_28_FLOAT         (0x02000000)
#define PINCON_PINMODE1_P0_28_PULLDOWN      (0x03000000)

#define PINCON_PINMODE_OD0_P0_28_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_28_OPEN_DRAIN (0x10000000)

// P0.29
#define PINCON_PINSEL1_P0_29                (0x00000000)
#define PINCON_PINSEL1_USB_D_PLUS           (0x04000000)

#define PINCON_PINMODE1_P0_29_PULLUP        (0x00000000)
#define PINCON_PINMODE1_P0_29_REPEATER      (0x04000000)
#define PINCON_PINMODE1_P0_29_FLOAT         (0x08000000)
#define PINCON_PINMODE1_P0_29_PULLDOWN      (0x0C000000)

#define PINCON_PINMODE_OD0_P0_29_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_29_OPEN_DRAIN (0x20000000)

// P0.30
#define PINCON_PINSEL1_P0_30                (0x00000000)
#define PINCON_PINSEL1_USB_D_MINUS          (0x10000000)

#define PINCON_PINMODE1_P0_30_PULLUP        (0x00000000)
#define PINCON_PINMODE1_P0_30_REPEATER      (0x10000000)
#define PINCON_PINMODE1_P0_30_FLOAT         (0x20000000)
#define PINCON_PINMODE1_P0_30_PULLDOWN      (0x30000000)

#define PINCON_PINMODE_OD0_P0_30_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD0_P0_30_OPEN_DRAIN (0x40000000)

// P0.31 - Not present

//-------------------------------------------------------------------------------

// P1.0
#define PINCON_PINSEL2_P1_0                 (0x00000000)
#define PINCON_PINSEL2_ENET_TXD0            (0x00000001)

#define PINCON_PINMODE2_P1_0_PULLUP         (0x00000000)
#define PINCON_PINMODE2_P1_0_REPEATER       (0x00000001)
#define PINCON_PINMODE2_P1_0_FLOAT          (0x00000002)
#define PINCON_PINMODE2_P1_0_PULLDOWN       (0x00000003)

#define PINCON_PINMODE_OD1_P1_0_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD1_P1_0_OPEN_DRAIN  (0x00000001)

// P1.1
#define PINCON_PINSEL2_P1_1                 (0x00000000)
#define PINCON_PINSEL2_ENET_TXD1            (0x00000004)

#define PINCON_PINMODE2_P1_1_PULLUP         (0x00000000)
#define PINCON_PINMODE2_P1_1_REPEATER       (0x00000004)
#define PINCON_PINMODE2_P1_1_FLOAT          (0x00000008)
#define PINCON_PINMODE2_P1_1_PULLDOWN       (0x0000000C)

#define PINCON_PINMODE_OD1_P1_1_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD1_P1_1_OPEN_DRAIN  (0x00000002)

// P1.2 - Not present
// P1.3 - Not present

// P1.4
#define PINCON_PINSEL2_P1_4                 (0x00000000)
#define PINCON_PINSEL2_ENET_TX_EN           (0x00000100)

#define PINCON_PINMODE2_P1_4_PULLUP         (0x00000000)
#define PINCON_PINMODE2_P1_4_REPEATER       (0x00000100)
#define PINCON_PINMODE2_P1_4_FLOAT          (0x00000200)
#define PINCON_PINMODE2_P1_4_PULLDOWN       (0x00000300)

#define PINCON_PINMODE_OD1_P1_4_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD1_P1_4_OPEN_DRAIN  (0x00000010)

// P1.5 - Not present
// P1.6 - Not present
// P1.7 - Not present

// P1.8
#define PINCON_PINSEL2_P1_8                 (0x00000000)
#define PINCON_PINSEL2_ENET_CRS             (0x00010000)

#define PINCON_PINMODE2_P1_8_PULLUP         (0x00000000)
#define PINCON_PINMODE2_P1_8_REPEATER       (0x00010000)
#define PINCON_PINMODE2_P1_8_FLOAT          (0x00020000)
#define PINCON_PINMODE2_P1_8_PULLDOWN       (0x00030000)

#define PINCON_PINMODE_OD1_P1_8_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD1_P1_8_OPEN_DRAIN  (0x00000100)

// P1.9
#define PINCON_PINSEL2_P1_9                 (0x00000000)
#define PINCON_PINSEL2_ENET_RXD0            (0x00040000)

#define PINCON_PINMODE2_P1_9_PULLUP         (0x00000000)
#define PINCON_PINMODE2_P1_9_REPEATER       (0x00040000)
#define PINCON_PINMODE2_P1_9_FLOAT          (0x00080000)
#define PINCON_PINMODE2_P1_9_PULLDOWN       (0x000C0000)

#define PINCON_PINMODE_OD1_P1_9_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD1_P1_9_OPEN_DRAIN  (0x00000200)

// P1.10
#define PINCON_PINSEL2_P1_10                (0x00000000)
#define PINCON_PINSEL2_ENET_RXD1            (0x00100000)

#define PINCON_PINMODE2_P1_10_PULLUP        (0x00000000)
#define PINCON_PINMODE2_P1_10_REPEATER      (0x00100000)
#define PINCON_PINMODE2_P1_10_FLOAT         (0x00200000)
#define PINCON_PINMODE2_P1_10_PULLDOWN      (0x00300000)

#define PINCON_PINMODE_OD1_P1_10_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_10_OPEN_DRAIN (0x00000400)

// P1.11 - Not present
// P1.12 - Not present
// P1.13 - Not present

// P1.14
#define PINCON_PINSEL2_P1_14                (0x00000000)
#define PINCON_PINSEL2_ENET_RX_ER           (0x10000000)

#define PINCON_PINMODE2_P1_14_PULLUP        (0x00000000)
#define PINCON_PINMODE2_P1_14_REPEATER      (0x10000000)
#define PINCON_PINMODE2_P1_14_FLOAT         (0x20000000)
#define PINCON_PINMODE2_P1_14_PULLDOWN      (0x30000000)

#define PINCON_PINMODE_OD1_P1_14_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_14_OPEN_DRAIN (0x00004000)

// P1.15
#define PINCON_PINSEL2_P1_15                (0x00000000)
#define PINCON_PINSEL2_ENET_REF_CLK         (0x40000000)

#define PINCON_PINMODE2_P1_15_PULLUP        (0x00000000)
#define PINCON_PINMODE2_P1_15_REPEATER      (0x40000000)
#define PINCON_PINMODE2_P1_15_FLOAT         (0x80000000)
#define PINCON_PINMODE2_P1_15_PULLDOWN      (0xC0000000)

#define PINCON_PINMODE_OD1_P1_15_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_15_OPEN_DRAIN (0x00008000)

// P1.16
#define PINCON_PINSEL3_P1_16                (0x00000000)
#define PINCON_PINSEL3_ENET_MDC             (0x00000001)

#define PINCON_PINMODE3_P1_16_PULLUP        (0x00000000)
#define PINCON_PINMODE3_P1_16_REPEATER      (0x00000001)
#define PINCON_PINMODE3_P1_16_FLOAT         (0x00000002)
#define PINCON_PINMODE3_P1_16_PULLDOWN      (0x00000003)

#define PINCON_PINMODE_OD1_P1_16_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_16_OPEN_DRAIN (0x00010000)

// P1.17
#define PINCON_PINSEL3_P1_17                (0x00000000)
#define PINCON_PINSEL3_ENET_MDIO            (0x00000040)

#define PINCON_PINMODE3_P1_17_PULLUP        (0x00000000)
#define PINCON_PINMODE3_P1_17_REPEATER      (0x00000004)
#define PINCON_PINMODE3_P1_17_FLOAT         (0x00000008)
#define PINCON_PINMODE3_P1_17_PULLDOWN      (0x0000000C)

#define PINCON_PINMODE_OD1_P1_17_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_17_OPEN_DRAIN (0x00020000)

// P1.18
#define PINCON_PINSEL3_P1_18_MASK           (0x00000030)
#define PINCON_PINSEL3_P1_18                (0x00000000)
#define PINCON_PINSEL3_USB_UP               (0x00000010)
#define PINCON_PINSEL3_PWM1_1               (0x00000020)
#define PINCON_PINSEL3_CAP1_0               (0x00000030)

#define PINCON_PINMODE3_P1_18_PULLUP        (0x00000000)
#define PINCON_PINMODE3_P1_18_REPEATER      (0x00000010)
#define PINCON_PINMODE3_P1_18_FLOAT         (0x00000020)
#define PINCON_PINMODE3_P1_18_PULLDOWN      (0x00000030)

#define PINCON_PINMODE_OD1_P1_18_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_18_OPEN_DRAIN (0x00040000)

// P1.19
#define PINCON_PINSEL3_P1_19                (0x00000000)
#define PINCON_PINSEL3_MC0A                 (0x00000040)
#define PINCON_PINSEL3_USB_PPW              (0x00000080)
#define PINCON_PINSEL3_CAP1_1               (0x000000C0)

#define PINCON_PINMODE3_P1_19_PULLUP        (0x00000000)
#define PINCON_PINMODE3_P1_19_REPEATER      (0x00000040)
#define PINCON_PINMODE3_P1_19_FLOAT         (0x00000080)
#define PINCON_PINMODE3_P1_19_PULLDOWN      (0x000000C0)

#define PINCON_PINMODE_OD1_P1_19_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_19_OPEN_DRAIN (0x00080000)

// P1.20
#define PINCON_PINSEL3_P1_20                (0x00000000)
#define PINCON_PINSEL3_MCFB0                (0x00000100)
#define PINCON_PINSEL3_PWM1_2               (0x00000200)
#define PINCON_PINSEL3_SCK0                 (0x00000300)

#define PINCON_PINMODE3_P1_20_PULLUP        (0x00000000)
#define PINCON_PINMODE3_P1_20_REPEATER      (0x00000100)
#define PINCON_PINMODE3_P1_20_FLOAT         (0x00000200)
#define PINCON_PINMODE3_P1_20_PULLDOWN      (0x00000300)

#define PINCON_PINMODE_OD1_P1_20_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_20_OPEN_DRAIN (0x00100000)

// P1.21
#define PINCON_PINSEL3_P1_21                (0x00000000)
#define PINCON_PINSEL3_MCABORT              (0x00000400)
#define PINCON_PINSEL3_PWM1_3               (0x00000800)
#define PINCON_PINSEL3_SSEL0                (0x00000C00)

#define PINCON_PINMODE3_P1_21_PULLUP        (0x00000000)
#define PINCON_PINMODE3_P1_21_REPEATER      (0x00000400)
#define PINCON_PINMODE3_P1_21_FLOAT         (0x00000800)
#define PINCON_PINMODE3_P1_21_PULLDOWN      (0x00000C00)

#define PINCON_PINMODE_OD1_P1_21_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_21_OPEN_DRAIN (0x00200000)

// P1.22
#define PINCON_PINSEL3_P1_22                (0x00000000)
#define PINCON_PINSEL3_MC0B                 (0x00001000)
#define PINCON_PINSEL3_USB_PWRD             (0x00002000)
#define PINCON_PINSEL3_MAT1_0               (0x00003000)

#define PINCON_PINMODE3_P1_22_PULLUP        (0x00000000)
#define PINCON_PINMODE3_P1_22_REPEATER      (0x00001000)
#define PINCON_PINMODE3_P1_22_FLOAT         (0x00002000)
#define PINCON_PINMODE3_P1_22_PULLDOWN      (0x00003000)

#define PINCON_PINMODE_OD1_P1_22_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_22_OPEN_DRAIN (0x00400000)

// P1.23
#define PINCON_PINSEL3_P1_23                (0x00000000)
#define PINCON_PINSEL3_MCFB1                (0x00004000)
#define PINCON_PINSEL3_PWM1_4               (0x00008000)
#define PINCON_PINSEL3_MISO0                (0x0000C000)

#define PINCON_PINMODE3_P1_23_PULLUP        (0x00000000)
#define PINCON_PINMODE3_P1_23_REPEATER      (0x00004000)
#define PINCON_PINMODE3_P1_23_FLOAT         (0x00008000)
#define PINCON_PINMODE3_P1_23_PULLDOWN      (0x0000C000)

#define PINCON_PINMODE_OD1_P1_23_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_23_OPEN_DRAIN (0x00800000)

// P1.24
#define PINCON_PINSEL3_P1_24                (0x00000000)
#define PINCON_PINSEL3_MCFB2                (0x00010000)
#define PINCON_PINSEL3_PWM1_5               (0x00020000)
#define PINCON_PINSEL3_MOSI0                (0x00030000)

#define PINCON_PINMODE3_P1_24_PULLUP        (0x00000000)
#define PINCON_PINMODE3_P1_24_REPEATER      (0x00010000)
#define PINCON_PINMODE3_P1_24_FLOAT         (0x00020000)
#define PINCON_PINMODE3_P1_24_PULLDOWN      (0x00030000)

#define PINCON_PINMODE_OD1_P1_24_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_24_OPEN_DRAIN (0x01000000)

// P1.25
#define PINCON_PINSEL3_P1_25                (0x00000000)
#define PINCON_PINSEL3_MC1A                 (0x00040000)
#define PINCON_PINSEL3_MAT1_1               (0x000C0000)

#define PINCON_PINMODE3_P1_25_PULLUP        (0x00000000)
#define PINCON_PINMODE3_P1_25_REPEATER      (0x00040000)
#define PINCON_PINMODE3_P1_25_FLOAT         (0x00080000)
#define PINCON_PINMODE3_P1_25_PULLDOWN      (0x000C0000)

#define PINCON_PINMODE_OD1_P1_25_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_25_OPEN_DRAIN (0x02000000)

// P1.26
#define PINCON_PINSEL3_P1_26                (0x00000000)
#define PINCON_PINSEL3_MC1B                 (0x00100000)
#define PINCON_PINSEL3_PWM1_6               (0x00200000)
#define PINCON_PINSEL3_CAP0_0               (0x00300000)

#define PINCON_PINMODE3_P1_26_PULLUP        (0x00000000)
#define PINCON_PINMODE3_P1_26_REPEATER      (0x00100000)
#define PINCON_PINMODE3_P1_26_FLOAT         (0x00200000)
#define PINCON_PINMODE3_P1_26_PULLDOWN      (0x00300000)

#define PINCON_PINMODE_OD1_P1_26_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_26_OPEN_DRAIN (0x04000000)

// P1.27
#define PINCON_PINSEL3_P1_27                (0x00000000)
#define PINCON_PINSEL3_CLKOUT               (0x00400000)
#define PINCON_PINSEL3_USB_OVRCR            (0x00800000)
#define PINCON_PINSEL3_CAP0_1               (0x00C00000)

#define PINCON_PINMODE3_P1_27_PULLUP        (0x00000000)
#define PINCON_PINMODE3_P1_27_REPEATER      (0x00400000)
#define PINCON_PINMODE3_P1_27_FLOAT         (0x00800000)
#define PINCON_PINMODE3_P1_27_PULLDOWN      (0x00C00000)

#define PINCON_PINMODE_OD1_P1_27_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_27_OPEN_DRAIN (0x08000000)

// P1.28
#define PINCON_PINSEL3_P1_28                (0x00000000)
#define PINCON_PINSEL3_MC2A                 (0x01000000)
#define PINCON_PINSEL3_PCAP1_0              (0x02000000)
#define PINCON_PINSEL3_MAT0_0               (0x03000000)

#define PINCON_PINMODE3_P1_28_PULLUP        (0x00000000)
#define PINCON_PINMODE3_P1_28_REPEATER      (0x01000000)
#define PINCON_PINMODE3_P1_28_FLOAT         (0x02000000)
#define PINCON_PINMODE3_P1_28_PULLDOWN      (0x03000000)

#define PINCON_PINMODE_OD1_P1_28_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_28_OPEN_DRAIN (0x10000000)

// P1.29
#define PINCON_PINSEL3_P1_29                (0x00000000)
#define PINCON_PINSEL3_MC2B                 (0x04000000)
#define PINCON_PINSEL3_PCAP_1_1             (0x08000000)
#define PINCON_PINSEL3_MAT0_1               (0x0C000000)

#define PINCON_PINMODE3_P1_29_PULLUP        (0x00000000)
#define PINCON_PINMODE3_P1_29_REPEATER      (0x04000000)
#define PINCON_PINMODE3_P1_29_FLOAT         (0x08000000)
#define PINCON_PINMODE3_P1_29_PULLDOWN      (0x0C000000)

#define PINCON_PINMODE_OD1_P1_29_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_29_OPEN_DRAIN (0x20000000)

// P1.30
#define PINCON_PINSEL3_P1_30                (0x00000000)
#define PINCON_PINSEL3_VBUS                 (0x20000000)
#define PINCON_PINSEL3_AD0_4                (0x30000000)

#define PINCON_PINMODE3_P1_30_PULLUP        (0x00000000)
#define PINCON_PINMODE3_P1_30_REPEATER      (0x10000000)
#define PINCON_PINMODE3_P1_30_FLOAT         (0x20000000)
#define PINCON_PINMODE3_P1_30_PULLDOWN      (0x30000000)

#define PINCON_PINMODE_OD1_P1_30_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_30_OPEN_DRAIN (0x40000000)

// P1.31
#define PINCON_PINSEL3_P1_31                (0x00000000)
#define PINCON_PINSEL3_SCK1                 (0x08000000)
#define PINCON_PINSEL3_AD0_5                (0xC0000000)

#define PINCON_PINMODE3_P1_31_PULLUP        (0x00000000)
#define PINCON_PINMODE3_P1_31_REPEATER      (0x40000000)
#define PINCON_PINMODE3_P1_31_FLOAT         (0x80000000)
#define PINCON_PINMODE3_P1_31_PULLDOWN      (0xC0000000)

#define PINCON_PINMODE_OD1_P1_31_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD1_P1_31_OPEN_DRAIN (0x80000000)

//-------------------------------------------------------------------------------

// P2.0
#define PINCON_PINSEL4_P2_0                 (0x00000000)
#define PINCON_PINSEL4_PWM1_1               (0x00000001)
#define PINCON_PINSEL4_TXD1                 (0x00000002)

#define PINCON_PINMODE4_P2_0_PULLUP         (0x00000000)
#define PINCON_PINMODE4_P2_0_REPEATER       (0x00000001)
#define PINCON_PINMODE4_P2_0_FLOAT          (0x00000002)
#define PINCON_PINMODE4_P2_0_PULLDOWN       (0x00000003)

#define PINCON_PINMODE_OD2_P2_0_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD2_P2_0_OPEN_DRAIN  (0x00000001)

// P2.1
#define PINCON_PINSEL4_P2_1                 (0x00000000)
#define PINCON_PINSEL4_PWM1_2               (0x00000004)
#define PINCON_PINSEL4_RXD1                 (0x00000008)

#define PINCON_PINMODE4_P2_1_PULLUP         (0x00000000)
#define PINCON_PINMODE4_P2_1_REPEATER       (0x00000004)
#define PINCON_PINMODE4_P2_1_FLOAT          (0x00000008)
#define PINCON_PINMODE4_P2_1_PULLDOWN       (0x0000000C)

#define PINCON_PINMODE_OD2_P2_1_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD2_P2_1_OPEN_DRAIN  (0x00000002)

// P2.2
#define PINCON_PINSEL4_P2_2                 (0x00000000)
#define PINCON_PINSEL4_PWM1_3               (0x00000010)
#define PINCON_PINSEL4_CTS1                 (0x00000020)

#define PINCON_PINMODE4_P2_2_PULLUP         (0x00000000)
#define PINCON_PINMODE4_P2_2_REPEATER       (0x00000010)
#define PINCON_PINMODE4_P2_2_FLOAT          (0x00000020)
#define PINCON_PINMODE4_P2_2_PULLDOWN       (0x00000030)

#define PINCON_PINMODE_OD2_P2_2_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD2_P2_2_OPEN_DRAIN  (0x00000004)

// P2.3
#define PINCON_PINSEL4_P2_3                 (0x00000000)
#define PINCON_PINSEL4_PWM1_4               (0x00000040)
#define PINCON_PINSEL4_DCD1                 (0x00000080)

#define PINCON_PINMODE4_P2_3_PULLUP         (0x00000000)
#define PINCON_PINMODE4_P2_3_REPEATER       (0x00000040)
#define PINCON_PINMODE4_P2_3_FLOAT          (0x00000080)
#define PINCON_PINMODE4_P2_3_PULLDOWN       (0x000000C0)

#define PINCON_PINMODE_OD2_P2_3_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD2_P2_3_OPEN_DRAIN  (0x00000008)

// P2.4
#define PINCON_PINSEL4_P2_4                 (0x00000000)
#define PINCON_PINSEL4_PWM1_5               (0x00000100)
#define PINCON_PINSEL4_DSR1                 (0x00000200)

#define PINCON_PINMODE4_P2_4_PULLUP         (0x00000000)
#define PINCON_PINMODE4_P2_4_REPEATER       (0x00000100)
#define PINCON_PINMODE4_P2_4_FLOAT          (0x00000200)
#define PINCON_PINMODE4_P2_4_PULLDOWN       (0x00000300)

#define PINCON_PINMODE_OD2_P2_4_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD2_P2_4_OPEN_DRAIN  (0x00000010)

// P2.5
#define PINCON_PINSEL4_P2_5                 (0x00000000)
#define PINCON_PINSEL4_PWM1_6               (0x00000400)
#define PINCON_PINSEL4_DTR1                 (0x00000800)

#define PINCON_PINMODE4_P2_5_PULLUP         (0x00000000)
#define PINCON_PINMODE4_P2_5_REPEATER       (0x00000400)
#define PINCON_PINMODE4_P2_5_FLOAT          (0x00000800)
#define PINCON_PINMODE4_P2_5_PULLDOWN       (0x00000C00)

#define PINCON_PINMODE_OD2_P2_5_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD2_P2_5_OPEN_DRAIN  (0x00000020)

// P2.6
#define PINCON_PINSEL4_P2_6                 (0x00000000)
#define PINCON_PINSEL4_PCAP1_0              (0x00001000)
#define PINCON_PINSEL4_RI1                  (0x00002000)

#define PINCON_PINMODE4_P2_6_PULLUP         (0x00000000)
#define PINCON_PINMODE4_P2_6_REPEATER       (0x00001000)
#define PINCON_PINMODE4_P2_6_FLOAT          (0x00002000)
#define PINCON_PINMODE4_P2_6_PULLDOWN       (0x00003000)

#define PINCON_PINMODE_OD2_P2_6_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD2_P2_6_OPEN_DRAIN  (0x00000040)

// P2.7
#define PINCON_PINSEL4_P2_7_MASK            (0x0000C000)
#define PINCON_PINSEL4_P2_7                 (0x00000000)
#define PINCON_PINSEL4_RD2                  (0x00004000)
#define PINCON_PINSEL4_RTS1                 (0x00008000)

#define PINCON_PINMODE4_P2_7_MASK           (0x0000C000)
#define PINCON_PINMODE4_P2_7_PULLUP         (0x00000000)
#define PINCON_PINMODE4_P2_7_REPEATER       (0x00004000)
#define PINCON_PINMODE4_P2_7_FLOAT          (0x00008000)
#define PINCON_PINMODE4_P2_7_PULLDOWN       (0x0000C000)

#define PINCON_PINMODE_OD2_P2_7_MASK        (0x00000080)
#define PINCON_PINMODE_OD2_P2_7_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD2_P2_7_OPEN_DRAIN  (0x00000080)

// P2.8
#define PINCON_PINSEL4_P2_8_MASK            (0x00030000)
#define PINCON_PINSEL4_P2_8                 (0x00000000)
#define PINCON_PINSEL4_TD2                  (0x00010000)
#define PINCON_PINSEL4_TXD2                 (0x00020000)
#define PINCON_PINSEL4_ENET_MDC             (0x00030000)

#define PINCON_PINMODE4_P2_8_MASK           (0x0003C000)
#define PINCON_PINMODE4_P2_8_PULLUP         (0x00000000)
#define PINCON_PINMODE4_P2_8_REPEATER       (0x00010000)
#define PINCON_PINMODE4_P2_8_FLOAT          (0x00020000)
#define PINCON_PINMODE4_P2_8_PULLDOWN       (0x00030000)

#define PINCON_PINMODE_OD2_P2_8_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD2_P2_8_OPEN_DRAIN  (0x00000100)

// P2.9
#define PINCON_PINSEL4_P2_9_MASK            (0x000C0000)
#define PINCON_PINSEL4_P2_9                 (0x00000000)
#define PINCON_PINSEL4_USB_CONNECT          (0x00040000)
#define PINCON_PINSEL4_RXD2                 (0x00080000)
#define PINCON_PINSEL4_ENET_MDIO            (0x000C0000)

#define PINCON_PINMODE4_P2_9_PULLUP         (0x00000000)
#define PINCON_PINMODE4_P2_9_REPEATER       (0x00040000)
#define PINCON_PINMODE4_P2_9_FLOAT          (0x00080000)
#define PINCON_PINMODE4_P2_9_PULLDOWN       (0x000C0000)

#define PINCON_PINMODE_OD2_P2_9_NORMAL      (0x00000000)
#define PINCON_PINMODE_OD2_P2_9_OPEN_DRAIN  (0x00000200)

// P2.10
#define PINCON_PINSEL4_P2_10                (0x00000000)
#define PINCON_PINSEL4_EINT0                (0x00100000)
#define PINCON_PINSEL4_NMI                  (0x00200000)

#define PINCON_PINMODE4_P2_10_PULLUP        (0x00000000)
#define PINCON_PINMODE4_P2_10_REPEATER      (0x00100000)
#define PINCON_PINMODE4_P2_10_FLOAT         (0x00200000)
#define PINCON_PINMODE4_P2_10_PULLDOWN      (0x00300000)

#define PINCON_PINMODE_OD2_P2_10_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD2_P2_10_OPEN_DRAIN (0x00000400)

// P2.11
#define PINCON_PINSEL4_P2_11                (0x00000000)
#define PINCON_PINSEL4_EINT1                (0x00400000)
#define PINCON_PINSEL4_I2STX_CLK            (0x00C00000)

#define PINCON_PINMODE4_P2_11_PULLUP        (0x00000000)
#define PINCON_PINMODE4_P2_11_REPEATER      (0x00400000)
#define PINCON_PINMODE4_P2_11_FLOAT         (0x00800000)
#define PINCON_PINMODE4_P2_11_PULLDOWN      (0x00C00000)

#define PINCON_PINMODE_OD2_P2_11_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD2_P2_11_OPEN_DRAIN (0x00000800)

// P2.12
#define PINCON_PINSEL4_P2_12                (0x00000000)
#define PINCON_PINSEL4_EINT2                (0x01000000)
#define PINCON_PINSEL4_I2STX_WS             (0x03000000)

#define PINCON_PINMODE4_P2_12_PULLUP        (0x00000000)
#define PINCON_PINMODE4_P2_12_REPEATER      (0x01000000)
#define PINCON_PINMODE4_P2_12_FLOAT         (0x02000000)
#define PINCON_PINMODE4_P2_12_PULLDOWN      (0x03000000)

#define PINCON_PINMODE_OD2_P2_12_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD2_P2_12_OPEN_DRAIN (0x00001000)

// P2.13 Bits 27:26 (9.3.15: Es waren Bits 23:22 genutzt!)
#define PINCON_PINSEL4_P2_13_MASK           (0x0C000000)
#define PINCON_PINSEL4_P2_13                (0x00000000)
#define PINCON_PINSEL4_EINT3                (0x04000000)
#define PINCON_PINSEL4_I2STX_SDA            (0x0C000000)

#define PINCON_PINMODE4_P2_13_PULLUP        (0x00000000)
#define PINCON_PINMODE4_P2_13_REPEATER      (0x04000000)
#define PINCON_PINMODE4_P2_13_FLOAT         (0x08000000)
#define PINCON_PINMODE4_P2_13_PULLDOWN      (0x0C000000)

#define PINCON_PINMODE_OD2_P2_13_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD2_P2_13_OPEN_DRAIN (0x00000800)

// P2.14 - Not present
// P2.15 - Not present
// P2.16 - Not present
// P2.17 - Not present
// P2.18 - Not present
// P2.19 - Not present
// P2.20 - Not present
// P2.21 - Not present
// P2.22 - Not present
// P2.23 - Not present
// P2.24 - Not present
// P2.25 - Not present
// P2.26 - Not present
// P2.27 - Not present
// P2.28 - Not present
// P2.29 - Not present
// P2.30 - Not present
// P2.31 - Not present

//------------------------------------------------------------------------------

// P3.0 - Not present
// P3.1 - Not present
// P3.2 - Not present
// P3.3 - Not present
// P3.4 - Not present
// P3.5 - Not present
// P3.6 - Not present
// P3.7 - Not present
// P3.8 - Not present
// P3.9 - Not present
// P3.10 - Not present
// P3.11 - Not present
// P3.12 - Not present
// P3.13 - Not present
// P3.14 - Not present
// P3.15 - Not present
// P3.16 - Not present
// P3.17 - Not present
// P3.18 - Not present
// P3.19 - Not present
// P3.20 - Not present
// P3.21 - Not present
// P3.22 - Not present
// P3.23 - Not present
// P3.24 - Not present

// P3.25
#define PINCON_PINSEL7_P3_25                (0x00000000)
#define PINCON_PINSEL7_MAT0_0               (0x00080000)
#define PINCON_PINSEL7_PWM1_2               (0x000C0000)

#define PINCON_PINMODE7_P3_25_PULLUP        (0x00000000)
#define PINCON_PINMODE7_P3_25_REPEATER      (0x00040000)
#define PINCON_PINMODE7_P3_25_FLOAT         (0x00080000)
#define PINCON_PINMODE7_P3_25_PULLDOWN      (0x000C0000)

#define PINCON_PINMODE_OD3_P3_25_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD3_P3_25_OPEN_DRAIN (0x02000000)

// P3.26
#define PINCON_PINSEL7_P3_26                (0x00000000)
#define PINCON_PINSEL7_STCLK                (0x00100000)
#define PINCON_PINSEL7_MAT0_1               (0x00200000)
#define PINCON_PINSEL7_PWM1_3               (0x00300000)

#define PINCON_PINMODE7_P3_26_PULLUP        (0x00000000)
#define PINCON_PINMODE7_P3_26_REPEATER      (0x00100000)
#define PINCON_PINMODE7_P3_26_FLOAT         (0x00200000)
#define PINCON_PINMODE7_P3_26_PULLDOWN      (0x00300000)

#define PINCON_PINMODE_OD3_P3_26_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD3_P3_26_OPEN_DRAIN (0x04000000)

// P3.27 - Not present
// P3.28 - Not present
// P3.29 - Not present
// P3.30 - Not present
// P3.31 - Not present

//------------------------------------------------------------------------------

// P4.0 - Not present
// P4.1 - Not present
// P4.2 - Not present
// P4.3 - Not present
// P4.4 - Not present
// P4.5 - Not present
// P4.6 - Not present
// P4.7 - Not present
// P4.8 - Not present
// P4.9 - Not present
// P4.10 - Not present
// P4.11 - Not present
// P4.12 - Not present
// P4.13 - Not present
// P4.14 - Not present
// P4.15 - Not present
// P4.16 - Not present
// P4.17 - Not present
// P4.18 - Not present
// P4.19 - Not present
// P4.20 - Not present
// P4.21 - Not present
// P4.22 - Not present
// P4.23 - Not present
// P4.24 - Not present
// P4.25 - Not present
// P4.26 - Not present
// P4.27 - Not present

// P4.28
#define PINCON_PINSEL9_P4_28                (0x00000000)
#define PINCON_PINSEL9_RX_MCLK              (0x01000000)
#define PINCON_PINSEL9_MAT2_0               (0x02000000)
#define PINCON_PINSEL9_TXD3                 (0x03000000)

#define PINCON_PINMODE9_P4_28_PULLUP        (0x00000000)
#define PINCON_PINMODE9_P4_28_REPEATER      (0x01000000)
#define PINCON_PINMODE9_P4_28_FLOAT         (0x02000000)
#define PINCON_PINMODE9_P4_28_PULLDOWN      (0x03000000)

#define PINCON_PINMODE_OD4_P4_28_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD4_P4_28_OPEN_DRAIN (0x10000000)

// P4.29
#define PINCON_PINSEL9_P4_29                (0x00000000)
#define PINCON_PINSEL9_TX_MCLK              (0x04000000)
#define PINCON_PINSEL9_MAT2_1               (0x08000000)
#define PINCON_PINSEL9_RXD3                 (0x0C000000)

#define PINCON_PINMODE9_P4_29_PULLUP        (0x00000000)
#define PINCON_PINMODE9_P4_29_REPEATER      (0x04000000)
#define PINCON_PINMODE9_P4_29_FLOAT         (0x08000000)
#define PINCON_PINMODE9_P4_29_PULLDOWN      (0x0C000000)

#define PINCON_PINMODE_OD4_P4_29_NORMAL     (0x00000000)
#define PINCON_PINMODE_OD4_P4_29_OPEN_DRAIN (0x20000000)

// P4.30 - Not present
// P4.31 - Not present

//-------------------------------------------------------------------------------

// TPIU interface
#define PINCON_PINSEL10_TPIU_DISABLED       (0x00000000)
#define PINCON_PINSEL10_TPIU_ENABLED        (0x00000008)

//-------------------------------------------------------------------------------
// GPIO0
//-------------------------------------------------------------------------------

// P0.0
#define GPIO0_FIODIR_P0_0_INPUT             (0x00000000)
#define GPIO0_FIODIR_P0_0_OUTPUT            (0x00000001)
#define GPIO_P0_0                           (0x00000001)

// P0.1
#define GPIO0_FIODIR_P0_1_INPUT             (0x00000000)
#define GPIO0_FIODIR_P0_1_OUTPUT            (0x00000002)
#define GPIO_P0_1                           (0x00000002)

// P0.2
#define GPIO0_FIODIR_P0_2_INPUT             (0x00000000)
#define GPIO0_FIODIR_P0_2_OUTPUT            (0x00000004)
#define GPIO_P0_2                           (0x00000004)

// P0.3
#define GPIO0_FIODIR_P0_3_INPUT             (0x00000000)
#define GPIO0_FIODIR_P0_3_OUTPUT            (0x00000008)
#define GPIO_P0_3                           (0x00000008)

// P0.4
#define GPIO0_FIODIR_P0_4_INPUT             (0x00000000)
#define GPIO0_FIODIR_P0_4_OUTPUT            (0x00000010)
#define GPIO_P0_4                           (0x00000010)

// P0.5
#define GPIO0_FIODIR_P0_5_INPUT             (0x00000000)
#define GPIO0_FIODIR_P0_5_OUTPUT            (0x00000020)
#define GPIO_P0_5                           (0x00000020)

// P0.6
#define GPIO0_FIODIR_P0_6_INPUT             (0x00000000)
#define GPIO0_FIODIR_P0_6_OUTPUT            (0x00000040)
#define GPIO_P0_6                           (0x00000040)

// P0.7
#define GPIO0_FIODIR_P0_7_INPUT             (0x00000000)
#define GPIO0_FIODIR_P0_7_OUTPUT            (0x00000080)
#define GPIO_P0_7                           (0x00000080)

// P0.8
#define GPIO0_FIODIR_P0_8_INPUT             (0x00000000)
#define GPIO0_FIODIR_P0_8_OUTPUT            (0x00000100)
#define GPIO_P0_8                           (0x00000100)

// P0.9
#define GPIO0_FIODIR_P0_9_INPUT             (0x00000000)
#define GPIO0_FIODIR_P0_9_OUTPUT            (0x00000200)
#define GPIO_P0_9                           (0x00000200)

// P0.10
#define GPIO0_FIODIR_P0_10_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_10_OUTPUT           (0x00000400)
#define GPIO_P0_10                          (0x00000400)

// P0.11
#define GPIO0_FIODIR_P0_11_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_11_OUTPUT           (0x00000800)
#define GPIO_P0_11                          (0x00000800)

// P0.12
#define GPIO0_FIODIR_P0_12_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_12_OUTPUT           (0x00001000)
#define GPIO_P0_12                          (0x00001000)

// P0.13
#define GPIO0_FIODIR_P0_13_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_13_OUTPUT           (0x00002000)
#define GPIO_P0_13                          (0x00002000)

// P0.14
#define GPIO0_FIODIR_P0_14_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_14_OUTPUT           (0x00004000)
#define GPIO_P0_14                          (0x00004000)
                                                 
// P0.15
#define GPIO0_FIODIR_P0_15_MASK             (0x00008000)
#define GPIO0_FIODIR_P0_15_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_15_OUTPUT           (0x00008000)
#define GPIO_P0_15                          (0x00008000)

// P0.16
#define GPIO0_FIODIR_P0_16_MASK             (0x00010000)
#define GPIO0_FIODIR_P0_16_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_16_OUTPUT           (0x00010000)
#define GPIO_P0_16                          (0x00010000)

// P0.17
#define GPIO0_FIODIR_P0_17_MASK             (0x00020000)
#define GPIO0_FIODIR_P0_17_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_17_OUTPUT           (0x00020000)
#define GPIO_P0_17                          (0x00020000)

// P0.18
#define GPIO0_FIODIR_P0_18_MASK             (0x00040000)
#define GPIO0_FIODIR_P0_18_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_18_OUTPUT           (0x00040000)
#define GPIO_P0_18                          (0x00040000)

// P0.19
#define GPIO0_FIODIR_P0_19_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_19_OUTPUT           (0x00080000)
#define GPIO_P0_19                          (0x00080000)

// P0.20
#define GPIO0_FIODIR_P0_20_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_20_OUTPUT           (0x00100000)
#define GPIO_P0_20                          (0x00100000)

// P0.21
#define GPIO0_FIODIR_P0_21_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_21_OUTPUT           (0x00200000)
#define GPIO_P0_21                          (0x00200000)

// P0.22
#define GPIO0_FIODIR_P0_22_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_22_OUTPUT           (0x00400000)
#define GPIO_P0_22                          (0x00400000)
                                                 
// P0.23
#define GPIO0_FIODIR_P0_23_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_23_OUTPUT           (0x00800000)
#define GPIO_P0_23                          (0x00800000)

// P0.24
#define GPIO0_FIODIR_P0_24_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_24_OUTPUT           (0x01000000)
#define GPIO_P0_24                          (0x01000000)

// P0.25
#define GPIO0_FIODIR_P0_25_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_25_OUTPUT           (0x02000000)
#define GPIO_P0_25                          (0x02000000)

// P0.26
#define GPIO0_FIODIR_P0_26_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_26_OUTPUT           (0x04000000)
#define GPIO_P0_26                          (0x04000000)
                                                 
// P0.27
#define GPIO0_FIODIR_P0_27_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_27_OUTPUT           (0x08000000)
#define GPIO_P0_27                          (0x08000000)

// P0.28
#define GPIO0_FIODIR_P0_28_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_28_OUTPUT           (0x10000000)
#define GPIO_P0_28                          (0x10000000)

// P0.29
#define GPIO0_FIODIR_P0_29_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_29_OUTPUT           (0x20000000)
#define GPIO_P0_29                          (0x20000000)

// P0.30
#define GPIO0_FIODIR_P0_30_INPUT            (0x00000000)
#define GPIO0_FIODIR_P0_30_OUTPUT           (0x40000000)
#define GPIO_P0_30                          (0x40000000)
                                                 
// P0.31 - Not present

//-------------------------------------------------------------------------------
// GPIO1
//-------------------------------------------------------------------------------

// P1.0
#define GPIO1_FIODIR_P1_0_INPUT             (0x00000000)
#define GPIO1_FIODIR_P1_0_OUTPUT            (0x00000001)
#define GPIO_P1_0                           (0x00000001)
#define GPIO_MASK_P1_0                      (0xFFFFFFFE)

// P1.1
#define GPIO1_FIODIR_P1_1_INPUT             (0x00000000)
#define GPIO1_FIODIR_P1_1_OUTPUT            (0x00000002)
#define GPIO_P1_1                           (0x00000002)
#define GPIO_MASK_P1_1                      (0xFFFFFFFC)

// P1.2 - Not present
// P1.3 - Not present

// P1.4
#define GPIO1_FIODIR_P1_4_INPUT             (0x00000000)
#define GPIO1_FIODIR_P1_4_OUTPUT            (0x00000010)
#define GPIO_P1_4                           (0x00000010)
#define GPIO_MASK_P1_4                      (0xFFFFFFEF)

// P1.5 - Not present
// P1.6 - Not present
// P1.7 - Not present

// P1.8
#define GPIO1_FIODIR_P1_8_INPUT             (0x00000000)
#define GPIO1_FIODIR_P1_8_OUTPUT            (0x00000100)
#define GPIO_P1_8                           (0x00000100)
#define GPIO_MASK_P1_8                      (0xFFFFFEFF)

// P1.9
#define GPIO1_FIODIR_P1_9_INPUT             (0x00000000)
#define GPIO1_FIODIR_P1_9_OUTPUT            (0x00000200)
#define GPIO_P1_9                           (0x00000200)
#define GPIO_MASK_P1_9                      (0xFFFFFCFF)

// P1.10
#define GPIO1_FIODIR_P1_10_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_10_OUTPUT           (0x00000400)
#define GPIO_P1_10                          (0x00000400)
#define GPIO_MASK_P1_10                     (0xFFFFFBFF)

// P1.11 - Not present
// P1.12 - Not present
// P1.13 - Not present

// P1.14
#define GPIO1_FIODIR_P1_14_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_14_OUTPUT           (0x00004000)
#define GPIO_P1_14                          (0x00004000)
#define GPIO_MASK_P1_14                     (0xFFFFBFFF)
                                                 
// P1.15
#define GPIO1_FIODIR_P1_15_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_15_OUTPUT           (0x00008000)
#define GPIO_P1_15                          (0x00008000)
#define GPIO_MASK_P1_15                     (0xFFFF7FFF)

// P1.16
#define GPIO1_FIODIR_P1_16_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_16_OUTPUT           (0x00010000)
#define GPIO_P1_16                          (0x00010000)
#define GPIO_MASK_P1_16                     (0xFFFEFFFF)

// P1.17
#define GPIO1_FIODIR_P1_17_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_17_OUTPUT           (0x00020000)
#define GPIO_P1_17                          (0x00020000)
#define GPIO_MASK_P1_17                     (0xFFFCFFFF)

// P1.18
#define GPIO1_FIODIR_P1_18_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_18_OUTPUT           (0x00040000)
#define GPIO_P1_18                          (0x00040000)
#define GPIO_MASK_P1_18                     (0xFFFBFFFF)

// P1.19
#define GPIO1_FIODIR_P1_19_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_19_OUTPUT           (0x00080000)
#define GPIO_P1_19                          (0x00080000)
#define GPIO_MASK_P1_19                     (0xFFF7FFFF)

// P1.20
#define GPIO1_FIODIR_P1_20_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_20_OUTPUT           (0x00100000)
#define GPIO_P1_20                          (0x00100000)
#define GPIO_MASK_P1_20                     (0xFFEFFFFF)

// P1.21
#define GPIO1_FIODIR_P1_21_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_21_OUTPUT           (0x00200000)
#define GPIO_P1_21                          (0x00200000)
#define GPIO_MASK_P1_21                     (0xFFCFFFFF)

// P1.22
#define GPIO1_FIODIR_P1_22_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_22_OUTPUT           (0x00400000)
#define GPIO_P1_22                          (0x00400000)
#define GPIO_MASK_P1_22                     (0xFFBFFFFF)
                                                 
// P1.23
#define GPIO1_FIODIR_P1_23_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_23_OUTPUT           (0x00800000)
#define GPIO_P1_23                          (0x00800000)
#define GPIO_MASK_P1_23                     (0xFF7FFFFF)

// P1.24
#define GPIO1_FIODIR_P1_24_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_24_OUTPUT           (0x01000000)
#define GPIO_P1_24                          (0x01000000)
#define GPIO_MASK_P1_24                     (0xFEFFFFFF)

// P1.25
#define GPIO1_FIODIR_P1_25_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_25_OUTPUT           (0x02000000)
#define GPIO_P1_25                          (0x02000000)
#define GPIO_MASK_P1_25                     (0xFCFFFFFF)

// P1.26
#define GPIO1_FIODIR_P1_26_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_26_OUTPUT           (0x04000000)
#define GPIO_P1_26                          (0x04000000)
#define GPIO_MASK_P1_26                     (0xFBFFFFFF)
                                                 
// P1.27
#define GPIO1_FIODIR_P1_27_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_27_OUTPUT           (0x08000000)
#define GPIO_P1_27                          (0x08000000)
#define GPIO_MASK_P1_27                     (0xF7FFFFFF)

// P1.28
#define GPIO1_FIODIR_P1_28_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_28_OUTPUT           (0x10000000)
#define GPIO_P1_28                          (0x10000000)
#define GPIO_MASK_P1_28                     (0xEFFFFFFF)

// P1.29
#define GPIO1_FIODIR_P1_29_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_29_OUTPUT           (0x20000000)
#define GPIO_P1_29                          (0x20000000)
#define GPIO_MASK_P1_29                     (0xCFFFFFFF)

// P1.30
#define GPIO1_FIODIR_P1_30_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_30_OUTPUT           (0x40000000)
#define GPIO_P1_30                          (0x40000000)
#define GPIO_MASK_P1_30                     (0xBFFFFFFF)
                                                 
// P1.31
#define GPIO1_FIODIR_P1_31_INPUT            (0x00000000)
#define GPIO1_FIODIR_P1_31_OUTPUT           (0x80000000)
#define GPIO_P1_31                          (0x80000000)
#define GPIO_MASK_P1_31                     (0x7FFFFFFF)

//-------------------------------------------------------------------------------
// GPIO2
//-------------------------------------------------------------------------------

// P2.0
#define GPIO2_FIODIR_P2_0_INPUT             (0x00000000)
#define GPIO2_FIODIR_P2_0_OUTPUT            (0x00000001)
#define GPIO_P2_0                           (0x00000001)

// P2.1
#define GPIO2_FIODIR_P2_1_INPUT             (0x00000000)
#define GPIO2_FIODIR_P2_1_OUTPUT            (0x00000002)
#define GPIO_P2_1                           (0x00000002)

// P2.2
#define GPIO2_FIODIR_P2_2_INPUT             (0x00000000)
#define GPIO2_FIODIR_P2_2_OUTPUT            (0x00000004)
#define GPIO_P2_2                           (0x00000004)

// P2.3
#define GPIO2_FIODIR_P2_3_INPUT             (0x00000000)
#define GPIO2_FIODIR_P2_3_OUTPUT            (0x00000008)
#define GPIO_P2_3                           (0x00000008)

// P2.4
#define GPIO2_FIODIR_P2_4_INPUT             (0x00000000)
#define GPIO2_FIODIR_P2_4_OUTPUT            (0x00000010)
#define GPIO_P2_4                           (0x00000010)

// P2.5
#define GPIO2_FIODIR_P2_5_INPUT             (0x00000000)
#define GPIO2_FIODIR_P2_5_OUTPUT            (0x00000020)
#define GPIO_P2_5                           (0x00000020)

// P2.6
#define GPIO2_FIODIR_P2_6_INPUT             (0x00000000)
#define GPIO2_FIODIR_P2_6_OUTPUT            (0x00000040)
#define GPIO_P2_6                           (0x00000040)

// P2.7
#define GPIO2_FIODIR_P2_7_MASK              (0x00000080)
#define GPIO2_FIODIR_P2_7_INPUT             (0x00000000)
#define GPIO2_FIODIR_P2_7_OUTPUT            (0x00000080)
#define GPIO_P2_7                           (0x00000080)

// P2.8
#define GPIO2_FIODIR_P2_8_MASK              (0x00000100)
#define GPIO2_FIODIR_P2_8_INPUT             (0x00000000)
#define GPIO2_FIODIR_P2_8_OUTPUT            (0x00000100)
#define GPIO_P2_8                           (0x00000100)

// P2.9
#define GPIO2_FIODIR_P2_9_INPUT             (0x00000000)
#define GPIO2_FIODIR_P2_9_OUTPUT            (0x00000200)
#define GPIO_P2_9                           (0x00000200)

// P2.10
#define GPIO2_FIODIR_P2_10_INPUT            (0x00000000)
#define GPIO2_FIODIR_P2_10_OUTPUT           (0x00000400)
#define GPIO_P2_10                          (0x00000400)

// P2.11
#define GPIO2_FIODIR_P2_11_INPUT            (0x00000000)
#define GPIO2_FIODIR_P2_11_OUTPUT           (0x00000800)
#define GPIO_P2_11                          (0x00000800)

// P2.12
#define GPIO2_FIODIR_P2_12_INPUT            (0x00000000)
#define GPIO2_FIODIR_P2_12_OUTPUT           (0x00001000)
#define GPIO_P2_12                          (0x00001000)

// P2.13
#define GPIO2_FIODIR_P2_13_INPUT            (0x00000000)
#define GPIO2_FIODIR_P2_13_OUTPUT           (0x00002000)
#define GPIO_P2_13                          (0x00002000)

// P2.14 - Not present
// P2.15 - Not present
// P2.16 - Not present
// P2.17 - Not present
// P2.18 - Not present
// P2.19 - Not present
// P2.20 - Not present
// P2.21 - Not present
// P2.22 - Not present
// P2.23 - Not present
// P2.24 - Not present

// P2.25
#define GPIO2_FIODIR_P2_25_INPUT            (0x00000000)
#define GPIO2_FIODIR_P2_25_OUTPUT           (0x02000000)
#define GPIO_P2_25                          (0x02000000)

// P2.26
#define GPIO2_FIODIR_P2_26_INPUT            (0x00000000)
#define GPIO2_FIODIR_P2_26_OUTPUT           (0x04000000)
#define GPIO_P2_26                          (0x04000000)
                                                 
// P2.27 - Not present
// P2.28 - Not present
// P2.29 - Not present
// P2.30 - Not present
// P2.31 - Not present

//-------------------------------------------------------------------------------
// GPIO3
//-------------------------------------------------------------------------------

// P3.0 - Not present
// P3.1 - Not present
// P3.2 - Not present
// P3.3 - Not present
// P3.4 - Not present
// P3.5 - Not present
// P3.6 - Not present
// P3.7 - Not present
// P3.8 - Not present
// P3.9 - Not present
// P3.10 - Not present
// P3.11 - Not present
// P3.12 - Not present
// P3.13 - Not present
// P3.14 - Not present
// P3.15 - Not present
// P3.16 - Not present
// P3.17 - Not present
// P3.18 - Not present
// P3.19 - Not present
// P3.20 - Not present
// P3.21 - Not present
// P3.22 - Not present
// P3.23 - Not present
// P3.24 - Not present

// P3.25
#define GPIO3_FIODIR_P3_25_INPUT            (0x00000000)
#define GPIO3_FIODIR_P3_25_OUTPUT           (0x02000000)
#define GPIO_P3_25                          (0x02000000)

// P3.26
#define GPIO3_FIODIR_P3_26_INPUT            (0x00000000)
#define GPIO3_FIODIR_P3_26_OUTPUT           (0x04000000)
#define GPIO_P3_26                          (0x04000000)
                                                 
// P3.27 - Not present
// P3.28 - Not present
// P3.29 - Not present
// P3.30 - Not present
// P3.31 - Not present

//-------------------------------------------------------------------------------
// GPIO4
//-------------------------------------------------------------------------------

// P4.0 - Not present
// P4.1 - Not present
// P4.2 - Not present
// P4.3 - Not present
// P4.4 - Not present
// P4.5 - Not present
// P4.6 - Not present
// P4.7 - Not present
// P4.8 - Not present
// P4.9 - Not present
// P4.10 - Not present
// P4.11 - Not present
// P4.12 - Not present
// P4.13 - Not present
// P4.14 - Not present
// P4.15 - Not present
// P4.16 - Not present
// P4.17 - Not present
// P4.18 - Not present
// P4.19 - Not present
// P4.20 - Not present
// P4.21 - Not present
// P4.22 - Not present
// P4.23 - Not present
// P4.24 - Not present
// P4.25 - Not present
// P4.26 - Not present
// P4.27 - Not present

// P4.28
#define GPIO4_FIODIR_P4_28_INPUT            (0x00000000)
#define GPIO4_FIODIR_P4_28_OUTPUT           (0x10000000)
#define GPIO_P4_28                          (0x10000000)

// P4.29
#define GPIO4_FIODIR_P4_29_INPUT            (0x00000000)
#define GPIO4_FIODIR_P4_29_OUTPUT           (0x20000000)
#define GPIO_P4_29                          (0x20000000)

// P4.30 - Not present
// P4.31 - Not present

//------------------------------------------------------------------------------
// PCON
//------------------------------------------------------------------------------

// PM
#define PCON_PM00             (0x00000000)
#define PCON_PM01             (0x00000001)
//#define PCON_PM10           (0x00000002)
#define PCON_PM11             (0x00000003)

// BODRPM
#define PCON_BODRPM           (0x00000004)
#define PCON_BODRPM_ENABLED   (0x00000000)
#define PCON_BODRPM_DISABLED  (0x00000004)

// BOGD
#define PCON_BOGD             (0x00000008)
#define PCON_BOGD_ENABLED     (0x00000000)
#define PCON_BOGD_DISABLED    (0x00000008)

// BORD
#define PCON_BORD             (0x00000010)
#define PCON_BORD_ENABLED     (0x00000000)
#define PCON_BORD_DISABLED    (0x00000010)

// SMFLAG
#define PCON_SMFLAG           (0x00000100)

// DSFLAG
#define PCON_DSFLAG           (0x00000200)

// PDFLAG
#define PCON_PDFLAG           (0x00000400)

// DPDFLAG
#define PCON_DPDFLAG          (0x00000800)

//------------------------------------------------------------------------------
// PCONP
//------------------------------------------------------------------------------

// Timer/Counter 0 power/clock control bit.
#define PCONP_PCTIM0_ENABLED    (0x00000002)  
#define PCONP_PCTIM0_DISABLED   (0x00000000)  

// Timer/Counter 1 power/clock control bit.
#define PCONP_PCTIM1_ENABLED    (0x00000004)  
#define PCONP_PCTIM1_DISABLED   (0x00000000)  

// UART0 power/clock control bit.
#define PCONP_PCUART0_ENABLED   (0x00000008)  
#define PCONP_PCUART0_DISABLED  (0x00000000)  

// UART1 power/clock control bit.
#define PCONP_PCUART1_ENABLED   (0x00000010)  
#define PCONP_PCUART1_DISABLED  (0x00000000)  

// PWM1 power/clock control bit.
#define PCONP_PCPWM1_ENABLED    (0x00000040)  
#define PCONP_PCPWM1_DISABLED   (0x00000000)  

// The I2C0 interface power/clock control bit.
#define PCONP_PCI2C0_ENABLED    (0x00000080)  
#define PCONP_PCI2C0_DISABLED   (0x00000000)  

// The SPI interface power/clock control bit.        
#define PCONP_PCSPI_ENABLED     (0x00000100)  
#define PCONP_PCSPI_DISABLED    (0x00000000)  

// The RTC power/clock control bit.
#define PCONP_PCRTC_ENABLED     (0x00000200)  
#define PCONP_PCRTC_DISABLED    (0x00000000)  

// The SSP 1 interface power/clock control bit.
#define PCONP_PCSSP1_ENABLED    (0x00000400)  
#define PCONP_PCSSP1_DISABLED   (0x00000000)  

// A/D converter (ADC) power/clock control bit. 
#define PCONP_PCADC_ENABLED     (0x00001000)  
#define PCONP_PCADC_DISABLED    (0x00000000)  

// CAN Controller 1 power/clock control bit.
#define PCONP_PCCAN1_ENABLED    (0x00002000)  
#define PCONP_PCCAN1_DISABLED   (0x00000000)  

// CAN Controller 2 power/clock control bit.
#define PCONP_PCCAN2_ENABLED    (0x00004000)  
#define PCONP_PCCAN2_DISABLED   (0x00000000)  

// GPIO power/clock control bit.
#define PCONP_PCGPIO_ENABLED    (0x00008000)  
#define PCONP_PCGPIO_DISABLED   (0x00000000)  

// Repetitive Interrupt Timer power/clock control bit.
#define PCONP_PCRIT_ENABLED     (0x00010000)  
#define PCONP_PCRIT_DISABLED    (0x00000000)  

// Motor Control PWM
#define PCONP_PCMCPWM_ENABLED   (0x00020000)  
#define PCONP_PCMCPWM_DISABLED  (0x00000000)  

// Quadrature Encoder Interface power/clock control bit.
#define PCONP_PCQEI_ENABLED     (0x00040000)  
#define PCONP_PCQEI_DISABLED    (0x00000000)  

// The I2C1 interface power/clock control bit.
#define PCONP_PCI2C1_ENABLED    (0x00080000)  
#define PCONP_PCI2C1_DISABLED   (0x00000000)  

// The SSP0 interface power/clock control bit.
#define PCONP_PCSSP0_ENABLED    (0x00200000)  
#define PCONP_PCSSP0_DISABLED   (0x00000000)  

// Timer 2 power/clock control bit.
#define PCONP_PCTIM2_ENABLED    (0x00400000)   
#define PCONP_PCTIM2_DISABLED   (0x00000000)   

// Timer 3 power/clock control bit.
#define PCONP_PCTIM3_ENABLED    (0x00800000)   
#define PCONP_PCTIM3_DISABLED   (0x00000000)   

// UART 2 power/clock control bit.
#define PCONP_PCUART2_ENABLED   (0x01000000)   
#define PCONP_PCUART2_DISABLED  (0x00000000)   

// UART 3 power/clock control bit.
#define PCONP_PCUART3_ENABLED   (0x02000000)   
#define PCONP_PCUART3_DISABLED  (0x00000000)   

// I2C interface 2 power/clock control bit.
#define PCONP_PCI2C2_ENABLED    (0x04000000)   
#define PCONP_PCI2C2_DISABLED   (0x00000000)   

// I2S interface power/clock control bit.
#define PCONP_PCI2S_ENABLED     (0x08000000)   
#define PCONP_PCI2S_DISABLED    (0x00000000)   

// GPDMA function power/clock control bit.
#define PCONP_PCGPDMA_ENABLED   (0x20000000)   
#define PCONP_PCGPDMA_DISABLED  (0x00000000)   

// Ethernet block power/clock control bit.
#define PCONP_PCENET_ENABLED    (0x40000000)   
#define PCONP_PCENET_DISABLED   (0x00000000)   

// USB interface power/clock control bit.
#define PCONP_PCUSB_ENABLED     (0x80000000)   
#define PCONP_PCUSB_DISABLED    (0x00000000)   

//------------------------------------------------------------------------------
// CLKOUTCFG register
//------------------------------------------------------------------------------

// CLKOUTSEL
#define CLKOUTCFG_CLKOUTSEL_CPU_CLOCK   (0x00000000)
#define CLKOUTCFG_CLKOUTSEL_MAIN_CLOCK  (0x00000001)
#define CLKOUTCFG_CLKOUTSEL_RC_CLOCK    (0x00000002)
#define CLKOUTCFG_CLKOUTSEL_USB_CLOCK   (0x00000003)
#define CLKOUTCFG_CLKOUTSEL_RTC_CLOCK   (0x00000004)

// CLKOUTDIV
#define CLKOUTCFG_CLKOUTDIV_1           (0x00000000)
#define CLKOUTCFG_CLKOUTDIV_2           (0x00000010)
#define CLKOUTCFG_CLKOUTDIV_3           (0x00000020)
#define CLKOUTCFG_CLKOUTDIV_4           (0x00000030)
#define CLKOUTCFG_CLKOUTDIV_5           (0x00000040)
#define CLKOUTCFG_CLKOUTDIV_6           (0x00000050)
#define CLKOUTCFG_CLKOUTDIV_7           (0x00000060)
#define CLKOUTCFG_CLKOUTDIV_8           (0x00000070)
#define CLKOUTCFG_CLKOUTDIV_9           (0x00000080)
#define CLKOUTCFG_CLKOUTDIV_10          (0x00000090)
#define CLKOUTCFG_CLKOUTDIV_11          (0x000000A0)
#define CLKOUTCFG_CLKOUTDIV_12          (0x000000B0)
#define CLKOUTCFG_CLKOUTDIV_13          (0x000000C0)
#define CLKOUTCFG_CLKOUTDIV_14          (0x000000D0)
#define CLKOUTCFG_CLKOUTDIV_15          (0x000000E0)
#define CLKOUTCFG_CLKOUTDIV_16          (0x000000F0)

// CLKOUT_EN
#define CLKOUTCFG_CLKOUT_EN             (0x00000100)
#define CLKOUTCFG_CLKOUT_EN_ENABLE      (0x00000100)
#define CLKOUTCFG_CLKOUT_EN_DISABLE     (0x00000000)

// CLKOUT_ACT
#define CLKOUTCFG_CLKOUT_ACT            (0x00000200)

//------------------------------------------------------------------------------
// SCS
//------------------------------------------------------------------------------

// OSCRANGE
#define SCS_OSCRANGE_MASK               (0x00000010)
#define SCS_OSCRANGE                    (0x00000010)
#define SCS_OSCRANGE_1MHZ_15MHZ         (0x00000000)
#define SCS_OSCRANGE_15MHZ_25MHZ        (0x00000010)

// OSCEN
#define SCS_OSCEN_MASK                  (0x00000020)
#define SCS_OSCEN                       (0x00000020)
#define SCS_OSCEN_ENABLE                (0x00000020)
#define SCS_OSCEN_DISABLE               (0x00000000)

// OSCSTAT
#define SCS_OSCSTAT                     (0x00000040)
#define SCS_OSCSTAT_READY               (0x00000040)
#define SCS_OSCSTAT_NOT_READY           (0x00000000)

//------------------------------------------------------------------------------
// CLKSRCSEL
//------------------------------------------------------------------------------

#define CLKSRCSEL_CLKSRC_RC_OSC         (0x00000000)
#define CLKSRCSEL_CLKSRC_MAIN_OSC       (0x00000001)
#define CLKSRCSEL_CLKSRC_RTC_OSC        (0x00000002)

//------------------------------------------------------------------------------
// PLL0CON
//------------------------------------------------------------------------------

// PLLE0
#define PLL0CON_PLLE0_MASK              (0x00000001)
#define PLL0CON_PLLE0_ENABLE            (0x00000001)
#define PLL0CON_PLLE0_DISABLE           (0x00000000)

// PLLC0
#define PLL0CON_PLLC0_MASK              (0x00000002)
#define PLL0CON_PLLC0_CONNECT           (0x00000002)
#define PLL0CON_PLLC0_DISCONNECT        (0x00000000)

//------------------------------------------------------------------------------
// PLL0STAT
//------------------------------------------------------------------------------

// PLLE0_STAT 
#define PLL0STAT_PLLE0_STAT               (0x01000000) 
#define PLL0STAT_PLLE0_STAT_ENABLED       (0x01000000) 
#define PLL0STAT_PLLE0_STAT_DISABLED      (0x00000000) 

// PLLC0_STAT 
#define PLL0STAT_PLLC0_STAT               (0x02000000) 
#define PLL0STAT_PLLC0_STAT_CONNECTED     (0x02000000) 
#define PLL0STAT_PLLC0_STAT_DISCONNECTED  (0x00000000) 

// PLOCK0
#define PLL0STAT_PLOCK0                   (0x04000000) 
#define PLL0STAT_PLOCK0_LOCKED            (0x04000000) 
#define PLL0STAT_PLOCK0_NOT_LOCKED        (0x00000000) 

//------------------------------------------------------------------------------
// PCLKSEL0 
//------------------------------------------------------------------------------

// Peripheral clock selection for WDT
#define PCLKSEL0_PCLK_WDT_CCLK_4       (0x00000000) 
#define PCLKSEL0_PCLK_WDT_CCLK         (0x00000001) 
#define PCLKSEL0_PCLK_WDT_CCLK_2       (0x00000002) 
#define PCLKSEL0_PCLK_WDT_CCLK_8       (0x00000003) 

// Peripheral clock selection for TIMER0
#define PCLKSEL0_PCLK_TIMER0_CCLK_4    (0x00000000)
#define PCLKSEL0_PCLK_TIMER0_CCLK      (0x00000004)
#define PCLKSEL0_PCLK_TIMER0_CCLK_2    (0x00000008)
#define PCLKSEL0_PCLK_TIMER0_CCLK_8    (0x0000000C)

// Peripheral clock selection for TIMER1 
#define PCLKSEL0_PCLK_TIMER1_CCLK_4    (0x00000000)
#define PCLKSEL0_PCLK_TIMER1_CCLK      (0x00000010)
#define PCLKSEL0_PCLK_TIMER1_CCLK_2    (0x00000020)
#define PCLKSEL0_PCLK_TIMER1_CCLK_8    (0x00000030)

// Peripheral clock selection for UART0 
#define PCLKSEL0_PCLK_UART0_CCLK_4     (0x00000000)
#define PCLKSEL0_PCLK_UART0_CCLK       (0x00000040)
#define PCLKSEL0_PCLK_UART0_CCLK_2     (0x00000080)
#define PCLKSEL0_PCLK_UART0_CCLK_8     (0x000000C0)

// Peripheral clock selection for UART1 
#define PCLKSEL0_PCLK_UART1_CCLK_4     (0x00000000)
#define PCLKSEL0_PCLK_UART1_CCLK       (0x00000100)
#define PCLKSEL0_PCLK_UART1_CCLK_2     (0x00000200)
#define PCLKSEL0_PCLK_UART1_CCLK_8     (0x00000300)

// Peripheral clock selection for PWM1
#define PCLKSEL0_PCLK_PWM1_CCLK_4      (0x00000000)
#define PCLKSEL0_PCLK_PWM1_CCLK        (0x00001000)
#define PCLKSEL0_PCLK_PWM1_CCLK_2      (0x00002000)
#define PCLKSEL0_PCLK_PWM1_CCLK_8      (0x00003000)

// Peripheral clock selection for I2C0 
#define PCLKSEL0_PCLK_I2C0_CCLK_4      (0x00000000)
#define PCLKSEL0_PCLK_I2C0_CCLK        (0x00004000)
#define PCLKSEL0_PCLK_I2C0_CCLK_2      (0x00008000)
#define PCLKSEL0_PCLK_I2C0_CCLK_8      (0x0000C000)

// Peripheral clock selection for SPI 
#define PCLKSEL0_PCLK_SPI_CCLK_4       (0x00000000)
#define PCLKSEL0_PCLK_SPI_CCLK         (0x00010000)
#define PCLKSEL0_PCLK_SPI_CCLK_2       (0x00020000)
#define PCLKSEL0_PCLK_SPI_CCLK_8       (0x00030000)
 
// Peripheral clock selection for SSP1
#define PCLKSEL0_PCLK_SSP1_CCLK_4      (0x00000000)
#define PCLKSEL0_PCLK_SSP1_CCLK        (0x00100000)
#define PCLKSEL0_PCLK_SSP1_CCLK_2      (0x00200000)
#define PCLKSEL0_PCLK_SSP1_CCLK_8      (0x00300000)

// Peripheral clock selection for DAC 
#define PCLKSEL0_PCLK_DAC_CCLK_4       (0x00000000)
#define PCLKSEL0_PCLK_DAC_CCLK         (0x00400000)
#define PCLKSEL0_PCLK_DAC_CCLK_2       (0x00800000)
#define PCLKSEL0_PCLK_DAC_CCLK_8       (0x00C00000)

// Peripheral clock selection for ADC 
#define PCLKSEL0_PCLK_ADC_CCLK_4       (0x00000000)
#define PCLKSEL0_PCLK_ADC_CCLK         (0x01000000)
#define PCLKSEL0_PCLK_ADC_CCLK_2       (0x02000000)
#define PCLKSEL0_PCLK_ADC_CCLK_8       (0x03000000)

// Peripheral clock selection for CAN1 
#define PCLKSEL0_PCLK_CAN1_CCLK_4      (0x00000000)
#define PCLKSEL0_PCLK_CAN1_CCLK        (0x04000000)
#define PCLKSEL0_PCLK_CAN1_CCLK_2      (0x08000000)
#define PCLKSEL0_PCLK_CAN1_CCLK_6      (0x0C000000)

// Peripheral clock selection for CAN2 
#define PCLKSEL0_PCLK_CAN2_CCLK_4      (0x00000000)
#define PCLKSEL0_PCLK_CAN2_CCLK        (0x10000000)
#define PCLKSEL0_PCLK_CAN2_CCLK_2      (0x20000000)
#define PCLKSEL0_PCLK_CAN2_CCLK_6      (0x30000000)

// Peripheral clock selection for CAN acceptance filtering 
#define PCLKSEL0_PCLK_ACF_CCLK_4       (0x00000000)
#define PCLKSEL0_PCLK_ACF_CCLK         (0x40000000)
#define PCLKSEL0_PCLK_ACF_CCLK_2       (0x80000000)
#define PCLKSEL0_PCLK_ACF_CCLK_6       (0xC0000000)

//------------------------------------------------------------------------------
// PCLKSEL1 
//------------------------------------------------------------------------------

// Peripheral clock selection for the Quadrature Encoder Interface
#define PCLKSEL1_PCLK_QEI_CCLK_4       (0x00000000)
#define PCLKSEL1_PCLK_QEI_CCLK         (0x00000001)
#define PCLKSEL1_PCLK_QEI_CCLK_2       (0x00000002)
#define PCLKSEL1_PCLK_QEI_CCLK_8       (0x00000003)

// Peripheral clock selection for GPIO interrupts 
#define PCLKSEL1_PCLK_GPIOINT_CCLK_4   (0x00000000) 
#define PCLKSEL1_PCLK_GPIOINT_CCLK     (0x00000004) 
#define PCLKSEL1_PCLK_GPIOINT_CCLK_2   (0x00000008) 
#define PCLKSEL1_PCLK_GPIOINT_CCLK_8   (0x0000000C) 

// Peripheral clock selection for the Pin Connect block
#define PCLKSEL1_PCLK_PCB_CCLK_4       (0x00000000)
#define PCLKSEL1_PCLK_PCB_CCLK         (0x00000010)
#define PCLKSEL1_PCLK_PCB_CCLK_2       (0x00000020)
#define PCLKSEL1_PCLK_PCB_CCLK_8       (0x00000030)

// Peripheral clock selection for I2C1 
#define PCLKSEL1_PCLK_I2C1_CCLK_4      (0x00000000) 
#define PCLKSEL1_PCLK_I2C1_CCLK        (0x00000040) 
#define PCLKSEL1_PCLK_I2C1_CCLK_2      (0x00000080) 
#define PCLKSEL1_PCLK_I2C1_CCLK_8      (0x000000C0) 

// Peripheral clock selection for SSP0
#define PCLKSEL1_PCLK_SSP0_CCLK_4      (0x00000000) 
#define PCLKSEL1_PCLK_SSP0_CCLK        (0x00000400) 
#define PCLKSEL1_PCLK_SSP0_CCLK_2      (0x00000800) 
#define PCLKSEL1_PCLK_SSP0_CCLK_8      (0x00000C00) 

// Peripheral clock selection for TIMER2
#define PCLKSEL1_PCLK_TIMER2_CCLK_4    (0x00000000)
#define PCLKSEL1_PCLK_TIMER2_CCLK      (0x00001000)
#define PCLKSEL1_PCLK_TIMER2_CCLK_2    (0x00002000)
#define PCLKSEL1_PCLK_TIMER2_CCLK_8    (0x00003000)

// Peripheral clock selection for TIMER3 
#define PCLKSEL1_PCLK_TIMER3_CCLK_4    (0x00000000)
#define PCLKSEL1_PCLK_TIMER3_CCLK      (0x00004000)
#define PCLKSEL1_PCLK_TIMER3_CCLK_2    (0x00008000)
#define PCLKSEL1_PCLK_TIMER3_CCLK_8    (0x0000C000)

// Peripheral clock selection for UART2 
#define PCLKSEL1_PCLK_UART2_CCLK_4     (0x00000000)
#define PCLKSEL1_PCLK_UART2_CCLK       (0x00010000)
#define PCLKSEL1_PCLK_UART2_CCLK_2     (0x00020000)
#define PCLKSEL1_PCLK_UART2_CCLK_8     (0x00030000)

// Peripheral clock selection for UART3 
#define PCLKSEL1_PCLK_UART3_CCLK_4     (0x00000000)
#define PCLKSEL1_PCLK_UART3_CCLK       (0x00040000)
#define PCLKSEL1_PCLK_UART3_CCLK_2     (0x00080000)
#define PCLKSEL1_PCLK_UART3_CCLK_8     (0x000C0000)

// Peripheral clock selection for I2C2 
#define PCLKSEL1_PCLK_I2C2_CCLK_4      (0x00000000)
#define PCLKSEL1_PCLK_I2C2_CCLK        (0x00100000)
#define PCLKSEL1_PCLK_I2C2_CCLK_2      (0x00200000)
#define PCLKSEL1_PCLK_I2C2_CCLK_8      (0x00300000)

// Peripheral clock selection for I2S 
#define PCLKSEL1_PCLK_I2S_CCLK_4       (0x00000000)
#define PCLKSEL1_PCLK_I2S_CCLK         (0x00400000)
#define PCLKSEL1_PCLK_I2S_CCLK_2       (0x00800000)
#define PCLKSEL1_PCLK_I2S_CCLK_8       (0x00C00000)

// Peripheral clock selection for Repetitive Interrupt Timer
#define PCLKSEL1_PCLK_RIT_CCLK_4       (0x00000000) 
#define PCLKSEL1_PCLK_RIT_CCLK         (0x04000000) 
#define PCLKSEL1_PCLK_RIT_CCLK_2       (0x08000000) 
#define PCLKSEL1_PCLK_RIT_CCLK_8       (0x0C000000) 

// Peripheral clock selection for the System Control block
#define PCLKSEL1_PCLK_SYSCON_CCLK_4    (0x00000000) 
#define PCLKSEL1_PCLK_SYSCON_CCLK      (0x10000000) 
#define PCLKSEL1_PCLK_SYSCON_CCLK_2    (0x20000000) 
#define PCLKSEL1_PCLK_SYSCON_CCLK_8    (0x30000000) 

// Peripheral clock selection for the Motor Control PWM
#define PCLKSEL1_PCLK_MC_CCLK_4        (0x00000000) 
#define PCLKSEL1_PCLK_MC_CCLK          (0x40000000) 
#define PCLKSEL1_PCLK_MC_CCLK_2        (0x80000000) 
#define PCLKSEL1_PCLK_MC_CCLK_8        (0xC0000000) 

//------------------------------------------------------------------------------
// LCR
//------------------------------------------------------------------------------

// Word length
#define UART_LCR_WORD_LENGTH_5_BIT       (0x00000000)    // 5-bit character length
#define UART_LCR_WORD_LENGTH_6_BIT       (0x00000001)    // 6-bit character length
#define UART_LCR_WORD_LENGTH_7_BIT       (0x00000002)    // 7-bit character length
#define UART_LCR_WORD_LENGTH_8_BIT       (0x00000003)    // 8-bit character length

// Stop bit select 
#define UART_LCR_1_STOP_BIT              (0x00000000)    // 1 stop bit. 
#define UART_LCR_2_STOP_BIT              (0x00000004)    // 2 stop bits (1.5 if UnLCR[1:0]=00).

// Parity enable 
#define UART_LCR_PARITY_CHECK_DISABLE    (0x00000000)    // Disable parity generation and checking.
#define UART_LCR_PARITY_CHECK_ENABLE     (0x00000008)    // Enable parity generation and checking.

// Parity select
#define UART_LCR_NO_PARITY               (0x00000000) 
#define UART_LCR_ODD_PARITY              (0x00000000)    // Odd parity. Number of 1s in the transmitted character and the attached parity bit will be odd.
#define UART_LCR_EVEN_PARITY             (0x00000010)    // Even Parity. Number of 1s in the transmitted character and the attached parity bit will be even.
#define UART_LCR_FORCED_1_PARITY         (0x00000020)    // Forced "1" stick parity.
#define UART_LCR_FORCED_0_PARITY         (0x00000030)    // Forced "0" stick parity.
 
// Break Control 
#define UART_LCR_BREAK_DISABLE           (0x00000000)    // Disable break transmission.
#define UART_LCR_BREAK_ENABLE            (0x00000040)    // Enable break transmission. Output pin UARTn TXD is forced to logic 0 when UnLCR[6] is active high.

// Divisor Latch Access Bit (DLAB)
#define UART_LCR_DIVISOR_ACCESS_DISABLE  (0x00000000)    // Disable access to Divisor Latches.
#define UART_LCR_DIVISOR_ACCESS_ENABLE   (0x00000080)    // Enable access to Divisor Latches.

//------------------------------------------------------------------------------
// LSR
//------------------------------------------------------------------------------

// Receiver Data Ready
#define UART_LSR_RDR      (0x00000001)

// Overrun Error
#define UART_LSR_OE       (0x00000002)

// Parity Error 
#define UART_LSR_PE       (0x00000004)

// Framing Error
#define UART_LSR_FE       (0x00000008)

// Break Interrupt
#define UART_LSR_BI       (0x00000010)

// Transmitter Holding Register Empty
#define UART_LSR_THRE     (0x00000020)

// Transmitter Empty
#define UART_LSR_TEMT     (0x00000040)

// Error in RX FIFO
#define UART_LSR_RXFE     (0x00000080)

//------------------------------------------------------------------------------
// IER
//------------------------------------------------------------------------------

// Enables the Receive Data Available interrupt for UARTn. It also controls the Character Receive Time-out interrupt.
#define UART_IER_RBR_INTERRUPT                      (0x00000001)
#define UART_IER_RBR_INTERRUPT_ENABLE               (0x00000001)
#define UART_IER_RBR_INTERRUPT_DISABLE              (0x00000000)

// Enables the THRE interrupt for UARTn. The status of this can be read from UnLSR.
#define UART_IER_THRE_INTERRUPT                     (0x00000002)
#define UART_IER_THRE_INTERRUPT_ENABLE              (0x00000002)
#define UART_IER_THRE_INTERRUPT_DISABLE             (0x00000000)

// Enables the UARTn RX line status interrupts. The status of this interrupt can be read from UnLSR[4:1].
#define UART_IER_RX_LINE_STATUS_INTERRUPT           (0x00000004)
#define UART_IER_RX_LINE_STATUS_INTERRUPT_ENABLE    (0x00000004)
#define UART_IER_RX_LINE_STATUS_INTERRUPT_DISABLE   (0x00000000)

// Enables the end of auto-baud interrupt.
#define UART_IER_ABEO_INTERRUPT                     (0x00000010)
#define UART_IER_ABEO_INTERRUPT_ENABLE              (0x00000010)
#define UART_IER_ABEO_INTERRUPT_DISABLE             (0x00000000)

// Enables the auto-baud time-out interrupt.
#define UART_IER_ABTO_INTERRUPT                     (0x00000020)
#define UART_IER_ABTO_INTERRUPT_ENABLE              (0x00000020)
#define UART_IER_ABTO_INTERRUPT_DISABLE             (0x00000000)

//------------------------------------------------------------------------------
// IIR
//------------------------------------------------------------------------------

#define UART_IIR_PEND	              (0x00000001)
#define UART_IIR_RLS		            (0x00000003)
#define UART_IIR_RDA		            (0x00000002)
#define UART_IIR_CTI		            (0x00000006)
#define UART_IIR_THRE	              (0x00000001)

//------------------------------------------------------------------------------
// FCR
//------------------------------------------------------------------------------

// FIFO Enable
#define UART_FCR_FIFO               (0x00000001)
#define UART_FCR_FIFO_ENABLE        (0x00000001)
#define UART_FCR_FIFO_DISABLE       (0x00000000)

// RX FIFO Reset 
#define UART_FCR_RX_FIFO_RESET      (0x00000002)

// TX FIFO Reset 
#define UART_FCR_TX_FIFO_RESET      (0x00000004)

// DMA Mode Select
#define UART_FCR_DMA_MODE           (0x00000008)
#define UART_FCR_DMA_MODE_ENABLE    (0x00000008)
#define UART_FCR_DMA_MODE_DISABLE   (0x00000000)

// RX Trigger level
#define UART_FCR_RX_TRIGGER_LEVEL_0 (0x00000000)  // 1 character
#define UART_FCR_RX_TRIGGER_LEVEL_1 (0x00000040)  // 4 character
#define UART_FCR_RX_TRIGGER_LEVEL_2 (0x00000080)  // 8 character
#define UART_FCR_RX_TRIGGER_LEVEL_3 (0x000000C0)  // 14 character

//------------------------------------------------------------------------------
// TER
//------------------------------------------------------------------------------

// TXEN
#define UART_TER_TXEN               (0x00000080)
#define UART_TER_TXEN_ENABLE        (0x00000080)
#define UART_TER_TXEN_DISABLE       (0x00000000)

//------------------------------------------------------------------------------
// I2C
//------------------------------------------------------------------------------

// I2CONSET : I2C Control Set register
#define I2C_I2CONSET_AA                 (0x00000004)  // Assert acknowledge flag
#define I2C_I2CONSET_SI                 (0x00000008)  // I2C interrupt flag
#define I2C_I2CONSET_STO                (0x00000010)  // STOP flag
#define I2C_I2CONSET_STA                (0x00000020)  // START flag
#define I2C_I2CONSET_I2EN               (0x00000040)  // I2C interface enable

// I2CONCLR : I2C Control Clear register
#define I2C_I2CONCLR_AAC                (0x00000004)  // Assert acknowledge flag
#define I2C_I2CONCLR_SIC                (0x00000008)  // I2C interrupt flag
#define I2C_I2CONCLR_STOC               (0x00000010)  // STOP flag
#define I2C_I2CONCLR_STAC               (0x00000020)  // START flag
#define I2C_I2CONCLR_I2ENC              (0x00000040)  // I2C interface enable

// I2MMCTRL : I2C Monitor mode control register
#define I2C_MMCTRL_MM_ENA               (0x00000001)  // Monitor mode enable
#define I2C_MMCTRL_MM_ENA_ENABLE        (0x00000001)
#define I2C_MMCTRL_MM_ENA_DISABLE       (0x00000000)

#define I2C_MMCTRL_ENA_SCL              (0x00000002)  // SCL output enable
#define I2C_MMCTRL_ENA_SCL_ENABLE       (0x00000002)
#define I2C_MMCTRL_ENA_SCL_DISABLE      (0x00000000)

#define I2C_MMCTRL_MATCH_ALL            (0x00000004)  // Select interrupt register match.
#define I2C_MMCTRL_MATCH_ALL_ENABLE     (0x00000004)
#define I2C_MMCTRL_MATCH_ALL_DISABLE    (0x00000000)

//------------------------------------------------------------------------------
// SSP CR0
//------------------------------------------------------------------------------

// DSS - Data Size Select
#define SSP_CR0_DSS_4_BIT         (0x00000003)  // 4-bit transfer
#define SSP_CR0_DSS_5_BIT         (0x00000004)  // 5-bit transfer
#define SSP_CR0_DSS_6_BIT         (0x00000005)  // 6-bit transfer
#define SSP_CR0_DSS_7_BIT         (0x00000006)  // 7-bit transfer
#define SSP_CR0_DSS_8_BIT         (0x00000007)  // 8-bit transfer
#define SSP_CR0_DSS_9_BIT         (0x00000008)  // 9-bit transfer
#define SSP_CR0_DSS_10_BIT        (0x00000009)  // 10-bit transfer
#define SSP_CR0_DSS_11_BIT        (0x0000000A)  // 11-bit transfer
#define SSP_CR0_DSS_12_BIT        (0x0000000B)  // 12-bit transfer
#define SSP_CR0_DSS_13_BIT        (0x0000000C)  // 13-bit transfer
#define SSP_CR0_DSS_14_BIT        (0x0000000D)  // 14-bit transfer
#define SSP_CR0_DSS_15_BIT        (0x0000000E)  // 15-bit transfer
#define SSP_CR0_DSS_16_BIT        (0x0000000F)  // 16-bit transfer

// FRF - Frame Format
#define SSP_CR0_FRF_SPI           (0x00000000)  // SPI
#define SSP_CR0_FRF_TI            (0x00000010)  // TI
#define SSP_CR0_FRF_MICROWIRE     (0x00000030)  // Microwire

// CPOL - Clock Out Polarity
#define SSP_CR0_CPOL_LOW          (0x00000000)  // SSP controller maintains the bus clock low between frames
#define SSP_CR0_CPOL_HIGH         (0x00000040)  // SSP controller maintains the bus clock high between frames

// CPHA - Clock Out Phase
#define SSP_CR0_CPHA_FIRST_CLOCK  (0x00000000)  // SSP controller captures serial data on the first clock transition of the frame, that is, the transition away from the inter-frame state of the clock line
#define SSP_CR0_CPHA_SECOND_CLOCK (0x00000080)  // SSP controller captures serial data on the second clock transition of the frame, that is, the transition back to the inter-frame state of the clock line

// SCR - Serial Clock Rate
#define SSP_CR0_SCR_MASK          (0x0000FF00)

//------------------------------------------------------------------------------
// SSP CR1
//------------------------------------------------------------------------------

// LBM - Loop Back Mode
#define SSP_CR1_LBM_NORMAL        (0x00000000)  // During normal operation
#define SSP_CR1_LBM_LOOPBACK      (0x00000001)  // Serial input is taken from the serial output (MOSI or MISO) rather than the serial input pin (MISO or MOSI respectively)

// SSE - SSP Enable
#define SSP_CR1_SSE_MASK          (0x00000002)
#define SSP_CR1_SSE_DISABLE       (0x00000000)  // The SSP controller is disabled
#define SSP_CR1_SSE_ENABLE        (0x00000002)  // The SSP controller will interact with other devices on the serial bus

// MS - MS Master/Slave Mode
#define SSP_CR1_MS_MASTER         (0x00000000)  // The SSP controller acts as a master on the bus, driving the SCLK, MOSI, and SSEL lines and receiving the MISO line
#define SSP_CR1_MS_SLAVE          (0x00000004)  // The SSP controller acts as a slave on the bus, driving MISO line and receiving SCLK, MOSI, and SSEL lines
 
// SOD - Slave Output Disable     
#define SSP_CR1_MS_SOD_DISABLE    (0x00000000)  // MISO pin disabled
#define SSP_CR1_MS_SOD_ENABLE     (0x00000008)  // MISO pin enabled

//------------------------------------------------------------------------------
// SSP SR
//------------------------------------------------------------------------------

// SR - Transmit FIFO Empty
#define SSP_SR_TFE_MASK           (0x00000001)
#define SSP_SR_TFE_EMPTY          (0x00000001)
#define SSP_SR_TFE_NOT_EMPTY      (0x00000000)

// TNF - Transmit FIFO Not Full
#define SSP_SR_TNF_MASK           (0x00000002)
#define SSP_SR_TNF_FULL           (0x00000000)
#define SSP_SR_TNF_NOT_FULL       (0x00000002)
 
// RNE - Receive FIFO Not Empty
#define SSP_SR_RNE_MASK           (0x00000004)
#define SSP_SR_RNE_EMPTY          (0x00000000)
#define SSP_SR_RNE_NOT_EMPTY      (0x00000004)

// RFF - Receive FIFO Full
#define SSP_SR_RFF_MASK           (0x00000008)
#define SSP_SR_RFF_EMPTY          (0x00000000)
#define SSP_SR_RFF_NOT_EMPTY      (0x00000008)

// BSY - Busy
#define SSP_SR_BSY_MASK           (0x00000010)
#define SSP_SR_BSY_IDLE           (0x00000000)
#define SSP_SR_BSY_BUSY           (0x00000010)

//------------------------------------------------------------------------------
// SSP IMSC
//------------------------------------------------------------------------------

// RORIM
#define SSP_IMSC_RORIM_DISABLE    (0x00000000)  // Receive Overrun
#define SSP_IMSC_RORIM_ENABLE     (0x00000001)

// RTIM
#define SSP_IMSC_RTIM_DISABLE     (0x00000000)  // Receive Timeout
#define SSP_IMSC_RTIM_ENABLE      (0x00000002)

// RXIM
#define SSP_IMSC_RXIM_DISABLE     (0x00000000)  // Rx FIFO is at least half full
#define SSP_IMSC_RXIM_ENABLE      (0x00000004)

// TXIM
#define SSP_IMSC_TXIM_DISABLE     (0x00000000)  // Tx FIFO is at least half full
#define SSP_IMSC_TXIM_ENABLE      (0x00000008)

//------------------------------------------------------------------------------
// SSP DMACR
//------------------------------------------------------------------------------

// RXDMAE - Receive DMA enable
#define SSP_DMACR_RXDMAE_DISABLE  (0x00000000)
#define SSP_DMACR_RXDMAE_ENABLE   (0x00000001)

// TXDMAE - Transmit DMA enable
#define SSP_DMACR_TXDMAE_DISABLE  (0x00000000)
#define SSP_DMACR_TXDMAE_ENABLE   (0x00000002)

//------------------------------------------------------------------------------

#endif  // __LPC17xx_H__
