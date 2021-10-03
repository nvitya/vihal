// VRV150: VexRiscV based, standardized FPGA Softcore + SoC

#ifndef __VRV150_H
#define __VRV150_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

typedef struct
{
  volatile uint32_t  INDATA;
  volatile uint32_t  OUTDATA;
  volatile uint32_t  DIR;        // bitx: 1 = output, 0 = intput
  volatile uint32_t  OUTSET;
  volatile uint32_t  OUTCLR;
  volatile uint32_t  DIRSET;
  volatile uint32_t  DIRCLR;
//
} vexriscv_gpio_t;

typedef struct
{
  volatile uint32_t  DATA;
  volatile uint32_t  STATUS;          // bit16..23: TX fifo free count, bit24..31: RX fifo fill count
  volatile uint32_t  CLOCK_DIVIDER;   // the clock is already divided by 8
  volatile uint32_t  FRAME_CONFIG;
//
} vexriscv_uart_t;

#define GPIOA_BASE   0xF0000000
#define GPIOB_BASE   0xF0001000

#define UART1_BASE   0xF0010000
#define UART2_BASE   0xF0011000

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __VRV150_H */

