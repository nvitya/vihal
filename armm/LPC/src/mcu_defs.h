
#ifndef __MCU_DEFS_H
#define __MCU_DEFS_H

#define HW_DMA_MAX_COUNT  4092

#define MAX_CLOCK_SPEED         100000000
#define MCU_INTERNAL_RC_SPEED     4000000

#define INDEPENDENT_GPIO
#define HW_GPIO_REGS  LPC_GPIO_TypeDef
#define HW_SPI_REGS   LPC_SSP_TypeDef
#define HW_DMA_REGS 	LPC_GPDMACH_TypeDef

inline void __attribute__((always_inline)) mcu_preinit_code()
{
}

#endif // __MCU_DEFS_H
