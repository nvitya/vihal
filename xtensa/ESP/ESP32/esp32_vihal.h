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
 *  file:     esp32_vihal.h
 *  brief:    VIHAL ESP32 include and own definitions
 *            the original Espressif peripheral definitions are not so usable
 *            so I made my own peripheral structs like the most ARM vendors provide
 *  created:  2022-01-29
 *  authors:  nvitya
*/

#pragma once

#include <cstddef>
#include "stdint.h"

#define BOOTLOADER_BUILD 1

#define SOC_CAPS_ECO_VER    SOC_CAPS_ECO_VER_MAX

// add some basic xtensa includes

//#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
//#include <xtensa/config/system.h>
#include <cpu_ll.h>
#include <mpu_ll.h>

#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_cntl_struct.h"

//-----------------------------------------------------------------------------
// Espressif includes
//-----------------------------------------------------------------------------

#include "soc/soc.h"
#include "soc/soc_caps.h"

#include "soc/dport_reg.h"
//#include "soc/system_reg.h"
#include "soc/uart_reg.h"
#include "soc/uhci_reg.h"
#include "soc/gpio_sig_map.h"

/************************************************

             VIHAL DEFINITIONS

************************************************/

//-----------------------------------------------------------------------------
// DPORT
//-----------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t PRO_BOOT_REMAP_CTRL;          // 0x000
  volatile uint32_t APP_BOOT_REMAP_CTRL;          // 0x004
  volatile uint32_t ACCESS_CHECK;                 // 0x008
  volatile uint32_t PRO_DPORT_APB_MASK0;          // 0x00C
  volatile uint32_t PRO_DPORT_APB_MASK1;          // 0x010
  volatile uint32_t APP_DPORT_APB_MASK0;          // 0x014
  volatile uint32_t APP_DPORT_APB_MASK1;          // 0x018
  volatile uint32_t PERI_CLK_EN;                  // 0x01C
  volatile uint32_t PERI_RST_EN;                  // 0x020
  volatile uint32_t WIFI_BB_CFG;                  // 0x024
  volatile uint32_t WIFI_BB_CFG_2;                // 0x028
  volatile uint32_t APPCPU_CTRL_A;                // 0x02C
  volatile uint32_t APPCPU_CTRL_B;                // 0x030
  volatile uint32_t APPCPU_CTRL_C;                // 0x034
  volatile uint32_t APPCPU_CTRL_D;                // 0x038
  volatile uint32_t CPU_PER_CONF;                 // 0x03C
  volatile uint32_t PRO_CACHE_CTRL;               // 0x040
  volatile uint32_t PRO_CACHE_CTRL1;              // 0x044
  volatile uint32_t PRO_CACHE_LOCK_0_ADDR;        // 0x048
  volatile uint32_t PRO_CACHE_LOCK_1_ADDR;        // 0x04C
  volatile uint32_t PRO_CACHE_LOCK_2_ADDR;        // 0x050
  volatile uint32_t PRO_CACHE_LOCK_3_ADDR;        // 0x054
  volatile uint32_t APP_CACHE_CTRL;               // 0x058
  volatile uint32_t APP_CACHE_CTRL1;              // 0x05C
  volatile uint32_t APP_CACHE_LOCK_0_ADDR;        // 0x060
  volatile uint32_t APP_CACHE_LOCK_1_ADDR;        // 0x064
  volatile uint32_t APP_CACHE_LOCK_2_ADDR;        // 0x068
  volatile uint32_t APP_CACHE_LOCK_3_ADDR;        // 0x06C
  volatile uint32_t TRACEMEM_MUX_MODE;            // 0x070
  volatile uint32_t PRO_TRACEMEM_ENA;             // 0x074
  volatile uint32_t APP_TRACEMEM_ENA;             // 0x078
  volatile uint32_t CACHE_MUX_MODE;               // 0x07C
  volatile uint32_t IMMU_PAGE_MODE;               // 0x080
  volatile uint32_t DMMU_PAGE_MODE;               // 0x084
  volatile uint32_t ROM_MPU_ENA;                  // 0x088
  volatile uint32_t MEM_PD_MASK;                  // 0x08C
  volatile uint32_t ROM_PD_CTRL;                  // 0x090
  volatile uint32_t ROM_FO_CTRL;                  // 0x094
  volatile uint32_t SRAM_PD_CTRL_0;               // 0x098
  volatile uint32_t SRAM_PD_CTRL_1;               // 0x09C
  volatile uint32_t SRAM_FO_CTRL_0;               // 0x0A0
  volatile uint32_t SRAM_FO_CTRL_1;               // 0x0A4
  volatile uint32_t IRAM_DRAM_AHB_SEL;            // 0x0A8
  volatile uint32_t TAG_FO_CTRL;                  // 0x0AC
  volatile uint32_t AHB_LITE_MASK;                // 0x0B0
  volatile uint32_t AHB_MPU_TABLE_0;              // 0x0B4
  volatile uint32_t AHB_MPU_TABLE_1;              // 0x0B8
  volatile uint32_t HOST_INF_SEL;                 // 0x0BC
  volatile uint32_t PERIP_CLK_EN;                 // 0x0C0
  volatile uint32_t PERIP_RST_EN;                 // 0x0C4
  volatile uint32_t SLAVE_SPI_CONFIG;             // 0x0C8
  volatile uint32_t WIFI_CLK_EN;                  // 0x0CC
  volatile uint32_t CORE_RST_EN;                  // 0x0D0
  volatile uint32_t BT_LPCK_DIV_INT;              // 0x0D4
  volatile uint32_t BT_LPCK_DIV_FRAC;             // 0x0D8
  volatile uint32_t CPU_INTR_FROM_CPU_0;          // 0x0DC
  volatile uint32_t CPU_INTR_FROM_CPU_1;          // 0x0E0
  volatile uint32_t CPU_INTR_FROM_CPU_2;          // 0x0E4
  volatile uint32_t CPU_INTR_FROM_CPU_3;          // 0x0E8
  volatile uint32_t PRO_INTR_STATUS_0;            // 0x0EC
  volatile uint32_t PRO_INTR_STATUS_1;            // 0x0F0
  volatile uint32_t PRO_INTR_STATUS_2;            // 0x0F4
  volatile uint32_t APP_INTR_STATUS_0;            // 0x0F8
  volatile uint32_t APP_INTR_STATUS_1;            // 0x0FC
  volatile uint32_t APP_INTR_STATUS_2;            // 0x100
  volatile uint32_t PRO_MAC_INTR_MAP;             // 0x104
  volatile uint32_t PRO_MAC_NMI_MAP;              // 0x108
  volatile uint32_t PRO_BB_INT_MAP;               // 0x10C
  volatile uint32_t PRO_BT_MAC_INT_MAP;           // 0x110
  volatile uint32_t PRO_BT_BB_INT_MAP;            // 0x114
  volatile uint32_t PRO_BT_BB_NMI_MAP;            // 0x118
  volatile uint32_t PRO_RWBT_IRQ_MAP;             // 0x11C
  volatile uint32_t PRO_RWBLE_IRQ_MAP;            // 0x120
  volatile uint32_t PRO_RWBT_NMI_MAP;             // 0x124
  volatile uint32_t PRO_RWBLE_NMI_MAP;            // 0x128
  volatile uint32_t PRO_SLC0_INTR_MAP;            // 0x12C
  volatile uint32_t PRO_SLC1_INTR_MAP;            // 0x130
  volatile uint32_t PRO_UHCI0_INTR_MAP;           // 0x134
  volatile uint32_t PRO_UHCI1_INTR_MAP;           // 0x138
  volatile uint32_t PRO_TG_T0_LEVEL_INT_MAP;      // 0x13C
  volatile uint32_t PRO_TG_T1_LEVEL_INT_MAP;      // 0x140
  volatile uint32_t PRO_TG_WDT_LEVEL_INT_MAP;     // 0x144
  volatile uint32_t PRO_TG_LACT_LEVEL_INT_MAP;    // 0x148
  volatile uint32_t PRO_TG1_T0_LEVEL_INT_MAP;     // 0x14C
  volatile uint32_t PRO_TG1_T1_LEVEL_INT_MAP;     // 0x150
  volatile uint32_t PRO_TG1_WDT_LEVEL_INT_MAP;    // 0x154
  volatile uint32_t PRO_TG1_LACT_LEVEL_INT_MAP;   // 0x158
  volatile uint32_t PRO_GPIO_INTERRUPT_MAP;       // 0x15C
  volatile uint32_t PRO_GPIO_INTERRUPT_NMI_MAP;   // 0x160
  volatile uint32_t PRO_CPU_INTR_FROM_CPU_0_MAP;  // 0x164
  volatile uint32_t PRO_CPU_INTR_FROM_CPU_1_MAP;  // 0x168
  volatile uint32_t PRO_CPU_INTR_FROM_CPU_2_MAP;  // 0x16C
  volatile uint32_t PRO_CPU_INTR_FROM_CPU_3_MAP;  // 0x170
  volatile uint32_t PRO_SPI_INTR_0_MAP;           // 0x174
  volatile uint32_t PRO_SPI_INTR_1_MAP;           // 0x178
  volatile uint32_t PRO_SPI_INTR_2_MAP;           // 0x17C
  volatile uint32_t PRO_SPI_INTR_3_MAP;           // 0x180
  volatile uint32_t PRO_I2S0_INT_MAP;             // 0x184
  volatile uint32_t PRO_I2S1_INT_MAP;             // 0x188
  volatile uint32_t PRO_UART_INTR_MAP;            // 0x18C
  volatile uint32_t PRO_UART1_INTR_MAP;           // 0x190
  volatile uint32_t PRO_UART2_INTR_MAP;           // 0x194
  volatile uint32_t PRO_SDIO_HOST_INTERRUPT_MAP;  // 0x198
  volatile uint32_t PRO_EMAC_INT_MAP;             // 0x19C
  volatile uint32_t PRO_PWM0_INTR_MAP;            // 0x1A0
  volatile uint32_t PRO_PWM1_INTR_MAP;            // 0x1A4
  volatile uint32_t PRO_PWM2_INTR_MAP;            // 0x1A8
  volatile uint32_t PRO_PWM3_INTR_MAP;            // 0x1AC
  volatile uint32_t PRO_LEDC_INT_MAP;             // 0x1B0
  volatile uint32_t PRO_EFUSE_INT_MAP;            // 0x1B4
  volatile uint32_t PRO_CAN_INT_MAP;              // 0x1B8
  volatile uint32_t PRO_RTC_CORE_INTR_MAP;        // 0x1BC
  volatile uint32_t PRO_RMT_INTR_MAP;             // 0x1C0
  volatile uint32_t PRO_PCNT_INTR_MAP;            // 0x1C4
  volatile uint32_t PRO_I2C_EXT0_INTR_MAP;        // 0x1C8
  volatile uint32_t PRO_I2C_EXT1_INTR_MAP;        // 0x1CC
  volatile uint32_t PRO_RSA_INTR_MAP;             // 0x1D0
  volatile uint32_t PRO_SPI1_DMA_INT_MAP;         // 0x1D4
  volatile uint32_t PRO_SPI2_DMA_INT_MAP;         // 0x1D8
  volatile uint32_t PRO_SPI3_DMA_INT_MAP;         // 0x1DC
  volatile uint32_t PRO_WDG_INT_MAP;              // 0x1E0
  volatile uint32_t PRO_TIMER_INT1_MAP;           // 0x1E4
  volatile uint32_t PRO_TIMER_INT2_MAP;           // 0x1E8
  volatile uint32_t PRO_TG_T0_EDGE_INT_MAP;       // 0x1EC
  volatile uint32_t PRO_TG_T1_EDGE_INT_MAP;       // 0x1F0
  volatile uint32_t PRO_TG_WDT_EDGE_INT_MAP;      // 0x1F4
  volatile uint32_t PRO_TG_LACT_EDGE_INT_MAP;     // 0x1F8
  volatile uint32_t PRO_TG1_T0_EDGE_INT_MAP;      // 0x1FC
  volatile uint32_t PRO_TG1_T1_EDGE_INT_MAP;      // 0x200
  volatile uint32_t PRO_TG1_WDT_EDGE_INT_MAP;     // 0x204
  volatile uint32_t PRO_TG1_LACT_EDGE_INT_MAP;    // 0x208
  volatile uint32_t PRO_MMU_IA_INT_MAP;           // 0x20C
  volatile uint32_t PRO_MPU_IA_INT_MAP;           // 0x210
  volatile uint32_t PRO_CACHE_IA_INT_MAP;         // 0x214
  volatile uint32_t APP_MAC_INTR_MAP;             // 0x218
  volatile uint32_t APP_MAC_NMI_MAP;              // 0x21C
  volatile uint32_t APP_BB_INT_MAP;               // 0x220
  volatile uint32_t APP_BT_MAC_INT_MAP;           // 0x224
  volatile uint32_t APP_BT_BB_INT_MAP;            // 0x228
  volatile uint32_t APP_BT_BB_NMI_MAP;            // 0x22C
  volatile uint32_t APP_RWBT_IRQ_MAP;             // 0x230
  volatile uint32_t APP_RWBLE_IRQ_MAP;            // 0x234
  volatile uint32_t APP_RWBT_NMI_MAP;             // 0x238
  volatile uint32_t APP_RWBLE_NMI_MAP;            // 0x23C
  volatile uint32_t APP_SLC0_INTR_MAP;            // 0x240
  volatile uint32_t APP_SLC1_INTR_MAP;            // 0x244
  volatile uint32_t APP_UHCI0_INTR_MAP;           // 0x248
  volatile uint32_t APP_UHCI1_INTR_MAP;           // 0x24C
  volatile uint32_t APP_TG_T0_LEVEL_INT_MAP;      // 0x250
  volatile uint32_t APP_TG_T1_LEVEL_INT_MAP;      // 0x254
  volatile uint32_t APP_TG_WDT_LEVEL_INT_MAP;     // 0x258
  volatile uint32_t APP_TG_LACT_LEVEL_INT_MAP;    // 0x25C
  volatile uint32_t APP_TG1_T0_LEVEL_INT_MAP;     // 0x260
  volatile uint32_t APP_TG1_T1_LEVEL_INT_MAP;     // 0x264
  volatile uint32_t APP_TG1_WDT_LEVEL_INT_MAP;    // 0x268
  volatile uint32_t APP_TG1_LACT_LEVEL_INT_MAP;   // 0x26C
  volatile uint32_t APP_GPIO_INTERRUPT_MAP;       // 0x270
  volatile uint32_t APP_GPIO_INTERRUPT_NMI_MAP;   // 0x274
  volatile uint32_t APP_CPU_INTR_FROM_CPU_0_MAP;  // 0x278
  volatile uint32_t APP_CPU_INTR_FROM_CPU_1_MAP;  // 0x27C
  volatile uint32_t APP_CPU_INTR_FROM_CPU_2_MAP;  // 0x280
  volatile uint32_t APP_CPU_INTR_FROM_CPU_3_MAP;  // 0x284
  volatile uint32_t APP_SPI_INTR_0_MAP;           // 0x288
  volatile uint32_t APP_SPI_INTR_1_MAP;           // 0x28C
  volatile uint32_t APP_SPI_INTR_2_MAP;           // 0x290
  volatile uint32_t APP_SPI_INTR_3_MAP;           // 0x294
  volatile uint32_t APP_I2S0_INT_MAP;             // 0x298
  volatile uint32_t APP_I2S1_INT_MAP;             // 0x29C
  volatile uint32_t APP_UART_INTR_MAP;            // 0x2A0
  volatile uint32_t APP_UART1_INTR_MAP;           // 0x2A4
  volatile uint32_t APP_UART2_INTR_MAP;           // 0x2A8
  volatile uint32_t APP_SDIO_HOST_INTERRUPT_MAP;  // 0x2AC
  volatile uint32_t APP_EMAC_INT_MAP;             // 0x2B0
  volatile uint32_t APP_PWM0_INTR_MAP;            // 0x2B4
  volatile uint32_t APP_PWM1_INTR_MAP;            // 0x2B8
  volatile uint32_t APP_PWM2_INTR_MAP;            // 0x2BC
  volatile uint32_t APP_PWM3_INTR_MAP;            // 0x2C0
  volatile uint32_t APP_LEDC_INT_MAP;             // 0x2C4
  volatile uint32_t APP_EFUSE_INT_MAP;            // 0x2C8
  volatile uint32_t APP_CAN_INT_MAP;              // 0x2CC
  volatile uint32_t APP_RTC_CORE_INTR_MAP;        // 0x2D0
  volatile uint32_t APP_RMT_INTR_MAP;             // 0x2D4
  volatile uint32_t APP_PCNT_INTR_MAP;            // 0x2D8
  volatile uint32_t APP_I2C_EXT0_INTR_MAP;        // 0x2DC
  volatile uint32_t APP_I2C_EXT1_INTR_MAP;        // 0x2E0
  volatile uint32_t APP_RSA_INTR_MAP;             // 0x2E4
  volatile uint32_t APP_SPI1_DMA_INT_MAP;         // 0x2E8
  volatile uint32_t APP_SPI2_DMA_INT_MAP;         // 0x2EC
  volatile uint32_t APP_SPI3_DMA_INT_MAP;         // 0x2F0
  volatile uint32_t APP_WDG_INT_MAP;              // 0x2F4
  volatile uint32_t APP_TIMER_INT1_MAP;           // 0x2F8
  volatile uint32_t APP_TIMER_INT2_MAP;           // 0x2FC
  volatile uint32_t APP_TG_T0_EDGE_INT_MAP;       // 0x300
  volatile uint32_t APP_TG_T1_EDGE_INT_MAP;       // 0x304
  volatile uint32_t APP_TG_WDT_EDGE_INT_MAP;      // 0x308
  volatile uint32_t APP_TG_LACT_EDGE_INT_MAP;     // 0x30C
  volatile uint32_t APP_TG1_T0_EDGE_INT_MAP;      // 0x310
  volatile uint32_t APP_TG1_T1_EDGE_INT_MAP;      // 0x314
  volatile uint32_t APP_TG1_WDT_EDGE_INT_MAP;     // 0x318
  volatile uint32_t APP_TG1_LACT_EDGE_INT_MAP;    // 0x31C
  volatile uint32_t APP_MMU_IA_INT_MAP;           // 0x320
  volatile uint32_t APP_MPU_IA_INT_MAP;           // 0x324
  volatile uint32_t APP_CACHE_IA_INT_MAP;         // 0x328
  volatile uint32_t AHBLITE_MPU_TABLE_UART;       // 0x32C
  volatile uint32_t AHBLITE_MPU_TABLE_SPI1;       // 0x330
  volatile uint32_t AHBLITE_MPU_TABLE_SPI0;       // 0x334
  volatile uint32_t AHBLITE_MPU_TABLE_GPIO;       // 0x338
  volatile uint32_t AHBLITE_MPU_TABLE_FE2;        // 0x33C
  volatile uint32_t AHBLITE_MPU_TABLE_FE;         // 0x340
  volatile uint32_t AHBLITE_MPU_TABLE_TIMER;      // 0x344
  volatile uint32_t AHBLITE_MPU_TABLE_RTC;        // 0x348
  volatile uint32_t AHBLITE_MPU_TABLE_IO_MUX;     // 0x34C
  volatile uint32_t AHBLITE_MPU_TABLE_WDG;        // 0x350
  volatile uint32_t AHBLITE_MPU_TABLE_HINF;       // 0x354
  volatile uint32_t AHBLITE_MPU_TABLE_UHCI1;      // 0x358
  volatile uint32_t AHBLITE_MPU_TABLE_MISC;       // 0x35C
  volatile uint32_t AHBLITE_MPU_TABLE_I2C;        // 0x360
  volatile uint32_t AHBLITE_MPU_TABLE_I2S0;       // 0x364
  volatile uint32_t AHBLITE_MPU_TABLE_UART1;      // 0x368
  volatile uint32_t AHBLITE_MPU_TABLE_BT;         // 0x36C
  volatile uint32_t AHBLITE_MPU_TABLE_BT_BUFFER;  // 0x370
  volatile uint32_t AHBLITE_MPU_TABLE_I2C_EXT0;   // 0x374
  volatile uint32_t AHBLITE_MPU_TABLE_UHCI0;      // 0x378
  volatile uint32_t AHBLITE_MPU_TABLE_SLCHOST;    // 0x37C
  volatile uint32_t AHBLITE_MPU_TABLE_RMT;        // 0x380
  volatile uint32_t AHBLITE_MPU_TABLE_PCNT;       // 0x384
  volatile uint32_t AHBLITE_MPU_TABLE_SLC;        // 0x388
  volatile uint32_t AHBLITE_MPU_TABLE_LEDC;       // 0x38C
  volatile uint32_t AHBLITE_MPU_TABLE_EFUSE;      // 0x390
  volatile uint32_t AHBLITE_MPU_TABLE_SPI_ENCRYPT; // 0x394
  volatile uint32_t AHBLITE_MPU_TABLE_BB;         // 0x398
  volatile uint32_t AHBLITE_MPU_TABLE_PWM0;       // 0x39C
  volatile uint32_t AHBLITE_MPU_TABLE_TIMERGROUP; // 0x3A0
  volatile uint32_t AHBLITE_MPU_TABLE_TIMERGROUP1; // 0x3A4
  volatile uint32_t AHBLITE_MPU_TABLE_SPI2;       // 0x3A8
  volatile uint32_t AHBLITE_MPU_TABLE_SPI3;       // 0x3AC
  volatile uint32_t AHBLITE_MPU_TABLE_APB_CTRL;   // 0x3B0
  volatile uint32_t AHBLITE_MPU_TABLE_I2C_EXT1;   // 0x3B4
  volatile uint32_t AHBLITE_MPU_TABLE_SDIO_HOST;  // 0x3B8
  volatile uint32_t AHBLITE_MPU_TABLE_EMAC;       // 0x3BC
  volatile uint32_t AHBLITE_MPU_TABLE_CAN;        // 0x3C0
  volatile uint32_t AHBLITE_MPU_TABLE_PWM1;       // 0x3C4
  volatile uint32_t AHBLITE_MPU_TABLE_I2S1;       // 0x3C8
  volatile uint32_t AHBLITE_MPU_TABLE_UART2;      // 0x3CC
  volatile uint32_t AHBLITE_MPU_TABLE_PWM2;       // 0x3D0
  volatile uint32_t AHBLITE_MPU_TABLE_PWM3;       // 0x3D4
  volatile uint32_t AHBLITE_MPU_TABLE_RWBT;       // 0x3D8
  volatile uint32_t AHBLITE_MPU_TABLE_BTMAC;      // 0x3DC
  volatile uint32_t AHBLITE_MPU_TABLE_WIFIMAC;    // 0x3E0
  volatile uint32_t AHBLITE_MPU_TABLE_PWR;        // 0x3E4
  volatile uint32_t MEM_ACCESS_DBUG0;             // 0x3E8
  volatile uint32_t MEM_ACCESS_DBUG1;             // 0x3EC
  volatile uint32_t PRO_DCACHE_DBUG[10];          // 0x3F0 .. 0x414
  volatile uint32_t APP_DCACHE_DBUG[10];          // 0x418 .. 0x43C
  volatile uint32_t PRO_CPU_RECORD_CTRL;          // 0x440
  volatile uint32_t PRO_CPU_RECORD_STATUS;        // 0x444
  volatile uint32_t PRO_CPU_RECORD_PID;           // 0x448
  volatile uint32_t PRO_CPU_RECORD_PDEBUGINST;    // 0x44C
  volatile uint32_t PRO_CPU_RECORD_PDEBUGSTATUS;  // 0x450
  volatile uint32_t PRO_CPU_RECORD_PDEBUGDATA;    // 0x454
  volatile uint32_t PRO_CPU_RECORD_PDEBUGPC;      // 0x458
  volatile uint32_t PRO_CPU_RECORD_PDEBUGLS0STAT; // 0x45C
  volatile uint32_t PRO_CPU_RECORD_PDEBUGLS0ADDR; // 0x460
  volatile uint32_t PRO_CPU_RECORD_PDEBUGLS0DATA; // 0x464
  volatile uint32_t APP_CPU_RECORD_CTRL;          // 0x468
  volatile uint32_t APP_CPU_RECORD_STATUS;        // 0x46C
  volatile uint32_t APP_CPU_RECORD_PID;           // 0x470
  volatile uint32_t APP_CPU_RECORD_PDEBUGINST;    // 0x474
  volatile uint32_t APP_CPU_RECORD_PDEBUGSTATUS;  // 0x478
  volatile uint32_t APP_CPU_RECORD_PDEBUGDATA;    // 0x47C
  volatile uint32_t APP_CPU_RECORD_PDEBUGPC;      // 0x480
  volatile uint32_t APP_CPU_RECORD_PDEBUGLS0STAT; // 0x484
  volatile uint32_t APP_CPU_RECORD_PDEBUGLS0ADDR; // 0x488
  volatile uint32_t APP_CPU_RECORD_PDEBUGLS0DATA; // 0x48C
  volatile uint32_t RSA_PD_CTRL;                  // 0x490
  volatile uint32_t ROM_MPU_TABLE[4];             // 0x494 .. 0x4A0
  volatile uint32_t SHROM_MPU_TABLE[24];          // 0x4A4 .. 0x500
  volatile uint32_t IMMU_TABLE[16];               // 0x504
  volatile uint32_t DMMU_TABLE[16];               // 0x544
  volatile uint32_t PRO_INTRUSION_CTRL;           // 0x584
  volatile uint32_t PRO_INTRUSION_STATUS;         // 0x588
  volatile uint32_t APP_INTRUSION_CTRL;           // 0x58C
  volatile uint32_t APP_INTRUSION_STATUS;         // 0x590
  volatile uint32_t FRONT_END_MEM_PD;             // 0x594
  volatile uint32_t MMU_IA_INT_EN;                // 0x598
  volatile uint32_t MPU_IA_INT_EN;                // 0x59C
  volatile uint32_t CACHE_IA_INT_EN;              // 0x5A0
  volatile uint32_t SECURE_BOOT_CTRL;             // 0x5A4
  volatile uint32_t SPI_DMA_CHAN_SEL;             // 0x5A8
  volatile uint32_t PRO_VECBASE_CTRL;             // 0x5AC
  volatile uint32_t PRO_VECBASE_SET;              // 0x5B0
  volatile uint32_t APP_VECBASE_CTRL;             // 0x5B4
  volatile uint32_t APP_VECBASE_SET;              // 0x5B8
//
} esp32_dport_t;

#define DPORT        ((esp32_dport_t *)(DR_REG_DPORT_BASE))

//-----------------------------------------------------------------------------
// GPIO
//-----------------------------------------------------------------------------

#define ESP_GPIO_COUNT   40
#define ESP_PAD_COUNT    40

// The ESP32 "pad"-s are referred mostly by their GPIO number, which is different from the package pin number
// And additionally some pads have officially special names, although they also can be used as GPIO

#define PAD_GPIO0    0
#define PAD_GPIO1    1
#define PAD_GPIO2    2
#define PAD_GPIO3    3
#define PAD_GPIO4    4
#define PAD_GPIO5    5
#define PAD_GPIO6    6
#define PAD_GPIO7    7
#define PAD_GPIO8    8
#define PAD_GPIO9    9
#define PAD_GPIO10  10
#define PAD_GPIO11  11
#define PAD_GPIO12  12
#define PAD_GPIO13  13
#define PAD_GPIO14  14
#define PAD_GPIO15  15
#define PAD_GPIO16  16
#define PAD_GPIO17  17
#define PAD_GPIO18  18
#define PAD_GPIO19  19
#define PAD_GPIO20  20
#define PAD_GPIO21  21
#define PAD_GPIO22  22
#define PAD_GPIO23  23
#define PAD_GPIO24  24
#define PAD_GPIO25  25
#define PAD_GPIO26  26
#define PAD_GPIO27  27
#define PAD_GPIO28  28
#define PAD_GPIO29  29
#define PAD_GPIO30  30
#define PAD_GPIO31  31
#define PAD_GPIO32  32
#define PAD_GPIO33  33
#define PAD_GPIO34  34
#define PAD_GPIO35  35
#define PAD_GPIO36  36
#define PAD_GPIO37  37
#define PAD_GPIO38  38
#define PAD_GPIO39  39

// pads with special names

#define PAD_U0TXD          1
#define PAD_U0RXD          3
#define PAD_SD_CLK         6
#define PAD_SD_DATA_0      7
#define PAD_SD_DATA_1      8
#define PAD_SD_DATA_2      9
#define PAD_SD_DATA_3     10
#define PAD_SD_CMD        11
#define PAD_MTDI          12
#define PAD_MTCK          13
#define PAD_MTMS          14
#define PAD_MTDO          15
#define PAD_32K_XP        32
#define PAD_32K_XN        33
#define PAD_VDET_1        34
#define PAD_VDET_2        35
#define PAD_SENSOR_VP     36
#define PAD_SENSOR_CAPP   37
#define PAD_SENSOR_CAPN   37
#define PAD_SENSOR_VN     39


typedef struct
{
  volatile uint32_t   REG;
  volatile uint32_t   SET;
  volatile uint32_t   CLR;
//
} esp_gpio_out_t;

typedef struct
{
  volatile uint32_t   REG;
  volatile uint32_t   SET;
  volatile uint32_t   CLR;
//
} esp_gpio_enable_t;

typedef struct
{
  volatile uint32_t   REG;
  volatile uint32_t   SET;
  volatile uint32_t   CLR;
//
} esp_gpio_irqstat_t;

typedef struct
{
  uint32_t               _pad_000;
  esp_gpio_out_t         OUT[2];     // output (data, set, clear)
  uint32_t               _pad_01C;
  esp_gpio_enable_t      ENABLE[2];  // output enable
  volatile uint32_t      STRAP;      // bootstrap pin value
  volatile uint32_t      IN[2];      // input
  esp_gpio_irqstat_t     STATUS[2];  // interrupt status register
  uint32_t               _pad_05C;
  volatile uint32_t      ACPU_INT;
  volatile uint32_t      ACPU_NMI_INT;
  volatile uint32_t      PCPU_INT;
  volatile uint32_t      PCPU_NMI_INT;
  uint32_t               _pad_070;
  volatile uint32_t      ACPU_INT1;
  volatile uint32_t      ACPU_NMI_INT1;
  volatile uint32_t      PCPU_INT1;
  volatile uint32_t      PCPU_NMI_INT1;
  uint32_t               _pad_084;
  volatile uint32_t      PIN[40];    // pin configuration
  uint32_t               _pad_128;
  uint32_t               _pad_12C;
  volatile uint32_t      FUNC_IN_SEL_CFG[256];  // Peripheral function n input selection
  volatile uint32_t      FUNC_OUT_SEL_CFG[40];  // Peripheral output selection for GPIOn
//
} esp_gpio_t;

#if 0
  // definition check:

  static_assert(offsetof(esp_gpio_t, STRAP)    == 0x038, "strap offset error");
  static_assert(offsetof(esp_gpio_t, PCPU_INT) == 0x068, "pcpu offset error");
  static_assert(offsetof(esp_gpio_t, PIN)      == 0x088, "pin offset error");
  static_assert(offsetof(esp_gpio_t, FUNC_IN_SEL_CFG) == 0x130, "func_in_sel_cfg offset error");
  static_assert(offsetof(esp_gpio_t, FUNC_OUT_SEL_CFG) == 0x530, "func_out_sel_cfg offset error");
#endif

typedef struct
{
  volatile uint32_t   PIN_CTRL;  // Clock output configuration
  volatile uint32_t   CFG[36];   // Configuration register, the order here is different from GPIO numbering !
//
} esp_io_mux_t;

//-----------------------------------------------------------------------------
// Instances
//-----------------------------------------------------------------------------

#define GPIO         ((esp_gpio_t *)(0x3FF44000))
#define IO_MUX     ((esp_io_mux_t *)(0x3FF49000))

//-----------------------------------------------------------------------------
// UART
//-----------------------------------------------------------------------------

typedef struct
{
  volatile uint32_t FIFO;            // 0x00
  volatile uint32_t INT_RAW;         // 0x04
  volatile uint32_t INT_ST;          // 0x08
  volatile uint32_t INT_ENA;         // 0x0C
  volatile uint32_t INT_CLR;         // 0x10
  volatile uint32_t CLKDIV;          // 0x14
  volatile uint32_t AUTOBAUD;        // 0x18
  volatile uint32_t STATUS;          // 0x1C
  volatile uint32_t CONF0;           // 0x20
  volatile uint32_t CONF1;           // 0x24
  volatile uint32_t LOWPULSE;        // 0x28
  volatile uint32_t HIGHPULSE;       // 0x2C
  volatile uint32_t RXD_CNT;         // 0x30
  volatile uint32_t FLOW_CONF;       // 0x34
  volatile uint32_t SLEEP_CONF;      // 0x38
  volatile uint32_t SWFC_CONF;       // 0x3C
  volatile uint32_t IDLE_CONF;       // 0x40
  volatile uint32_t RS485_CONF;      // 0x44
  volatile uint32_t AT_CMD_PRECNT;   // 0x48
  volatile uint32_t AT_CMD_POSTCNT;  // 0x4c
  volatile uint32_t AT_CMD_GAPTOUT;  // 0x50
  volatile uint32_t AT_CMD_CHAR;     // 0x54
  volatile uint32_t MEM_CONF;        // 0x58
  volatile uint32_t MEM_TX_STATUS;   // 0x5c
  volatile uint32_t MEM_RX_STATUS;   // 0x60
  volatile uint32_t MEM_CNT_STATUS;  // 0x64
  volatile uint32_t POSPULSE;        // 0x68
  volatile uint32_t NEGPULSE;        // 0x6c
  volatile uint32_t _pad70;          // 0x70
  volatile uint32_t _pad74;          // 0x74
  volatile uint32_t DATE;            // 0x78
  volatile uint32_t ID;              // 0x7C
//
} esp32_uart_t;

#define UART0     ((esp32_uart_t *)(DR_REG_UART_BASE))
#define UART1     ((esp32_uart_t *)(DR_REG_UART1_BASE))
#define UART2     ((esp32_uart_t *)(DR_REG_UART2_BASE))

//#define UHCI0     ((esp_uhci_t *)(DR_REG_UHCI0_BASE))
//#define UHCI1     ((esp_uhci_t *)(DR_REG_UHCI1_BASE))



