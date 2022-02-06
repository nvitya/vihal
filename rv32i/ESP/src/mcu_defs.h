/*
 *  file:     mcu_defs.h (ESP)
 *  brief:    ESP Risc-V MCU Family definitions
 *  version:  1.00
 *  date:     2022-02-03
 *  authors:  nvitya
*/

#pragma once

#include "rv32i_cpu.h"

#define MAX_CLOCK_SPEED         160000000

#define MCU_INTERNAL_RC_SPEED     1750000 // ring oscillator

#define HW_DMA_MAX_COUNT            32764

#define CLOCKCNT  (cpu_csr_read(0x7E2))  // use the mpccr instead of the mtime

inline void __attribute__((always_inline)) mcu_preinit_code()
{
}

inline void __attribute__((always_inline)) mcu_postinit_code()
{
}

