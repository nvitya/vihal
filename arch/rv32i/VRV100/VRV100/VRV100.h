// VRV100: VexRiscV based, standardized FPGA Softcore + SoC

#ifndef __VRV100_H
#define __VRV100_H

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

typedef struct
{
  volatile uint32_t  DATA; 					  // 0x00
  volatile uint32_t  STATUS;   			  // 0x04
  volatile uint32_t  CONFIG;          // 0x08
  volatile uint32_t  CLOCK_DIVIDER;   // W 0x0C SPI frequency = FCLK / (2 * clockDivider)
  volatile uint32_t  SSSETUP;    			// W 0x10 time between chip select enable and the next byte
  volatile uint32_t  SSHOLD;          // W 0x14 time between the last byte transmission and the chip select disable
  volatile uint32_t  SSDISABLE;       // W 0x18 time between chip select disable and chip select enable
//
} vexriscv_spim_t;

// bootloader application header
// position dependent of the used FPGA bitstream size, which always comes first

typedef struct
{
  volatile uint32_t  signature;   	  // = 0xC0DEBA5E
  volatile uint32_t  length;  		    //
  volatile uint32_t  addr_load;       //
  volatile uint32_t  addr_entry;      //
  volatile uint32_t  _reserved10;			//
  volatile uint32_t  compid;          //
  volatile uint32_t  csum_body;       //
  volatile uint32_t  csum_head;       //
//
} bootblock_header_t;

typedef struct
{
  volatile uint32_t  compid;              // SOC compatibility ID like 0x01530001
  volatile uint32_t  clock_speed_mhz;     // system clock speed in MHz
  volatile uint32_t  bootblock_staddr;    // bootblock storage address in the SPI Flash
  volatile uint32_t  ocram_size;          // size of the OC RAM beginning at 0x80000000, the first 1k is used by the bootloader
  volatile uint32_t  sdram_size;          // size of the SDRAM beginning at 0x40000000
  volatile uint32_t  periph_mul;          // 1 = 1xSPIM + 1xUART, 2 = 2xSPIM + 2xUART
  volatile uint32_t  _reserved[0];        //
//
} bootrom_info_t; // 64 bytes fixed at address 0x80000100


#if !defined(MCU_FIXED_SPEED)
  #define MCU_FIXED_SPEED  100000000  // 100 MHz by default
#endif

#define BOOTBLOCK_SIGNATURE     0xC0DEBA5E
#define BOOTROM_INFO_ADDRESS    0x800003C0

#define GPIOA_BASE   0xF0000000
#define GPIOB_BASE   0xF0001000

#define UART1_BASE   0xF0010000
#define UART2_BASE   0xF0011000

#define SPIM1_BASE   0xF0040000
#define SPIM2_BASE   0xF0041000

#define UART_TXFIFO_DEPTH  1024
#define SPIM_TXFIFO_DEPTH   512

#define SPIM_DATA_VALID    (1u << 31)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __VRV100_H */

