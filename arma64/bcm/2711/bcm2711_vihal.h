/*
 *  file:     bcm2711_vihal.h
 *  brief:    BCM2711 VIHAL definitions
 *  date:     2024-07-18
 *  authors:  nvitya
*/

#ifndef BCM2711_VIHAL_H_
#define BCM2711_VIHAL_H_

#include "stdint.h"

//----------------------------------------------------------------------------------

struct systimer_regs_t
{
	volatile uint32_t   CS;     // 00
	volatile uint32_t   CLO;    // 04
	volatile uint32_t   CHI;    // 08
	volatile uint32_t   CMP[4]; // 0C
};

// TODO: handle legacy / normal addresses  (0x7E.. / 0xFE..)

#define HW_SYSTIMER_BASE      0x7E003000

#define HW_GPIO_BASE          0x7E200000
#define HW_GPIO_MAX_PINS      58

// clocks

#define CLOCKCNT_SPEED           1000000
#define HWUART_BASE_CLOCK       48000000

#endif /* BCM2711_VIHAL_H_ */
