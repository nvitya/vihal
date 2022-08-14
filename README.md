# Introduction

VIHAL = Vendor Independent Hardware Access Layer/Library.

It targets bare metal embedded software development, with the following main goals:
 - Easy to use, C++ interfaces for shorter lines like in Arduino
 - Small, simple but effective. Easy to verify.
 - Vendor independent, no intermediate libraries, external dependencies 
 - non-blocking communication interfaces (extensive use of DMA and state machines) which allows simple cooperative multi-tasking
 - license permitting unrestricted commercial usage, without any obligation to the customers
 - Built-in support for multiple popular Microcontrollers
 - Provide SW modules and examples for usual scenarios 
 - Easily extensible

The project development began in 2017, the direct predecessor is my [NVCM](https://github.com/nvitya/nvcm) project (which supports only ARM Cortex-M).

# Quick Start / Examples

For the quick start here are ready to use examples for several popular boards (for Eclipse CDT):
  https://github.com/nvitya/vihaltests
  
To get a quick picture see these [Code Examples](/doc/code_examples.md).

For more information read the documentation here: [/doc/VIHAL_documentation.md](/doc/VIHAL_documentation.md)

# Supported CPU Architectures

__Architecture ID__ | Description
--------------------|------------
[ARMM](armm) | ARM Cortex-M (M0, M3, M4, M7)
[RV32I](rv32i) | 32-bit RISC-V
[RV64G](rv64g) | 64-bit RISC-V
[XTENSA](xtensa) | Tensilica Xtensa LX6 (ESP32)

# Supported Microcontroller Families

Those microcontrollers that share the same drivers belong to the same VIHAL microcontroller family. The families are named usually after the part ordering numbers.

Arch. | Family ID | Sub-Families | Examples | Driver Support
------|-----------|--------------|----------|---------------
[ARMM](/armm)   | [STM32](/armm/STM32) | F0, F1, F3, F4, F7, H7 | STM32F103C8, STM32H750VB | [good](/armm/STM32)
[ARMM](/armm)   | [ATSAM](/armm/ATSAM) | 3X, 4S, E70/S70/V70 | ATSAME3X8E, ATSAME70Q20 | [good](/armm/ATSAM)
[ARMM](/armm)   | [ATSAM_V2](/armm/ATSAM_V2) | D51/E5x, C2x, D10 | ATSAME51J20 | [good](/armm/ATSAM_V2)
[ARMM](/armm)   | [RP](/armm/RP)       | 20  | RP2040 | [good](/armm/RP)
[ARMM](/armm)   | [LPC_V3](/armm/LPC_V3)  | LPC546xx | LPC54608J512 | [partial](/armm/LPC_V3)
[RV32I](/rv32i) | [GD32V](/rv32i/GD32V) | F1 | GD32VF103C8 | [partial](/rv32i/GD32V)
[RV32I](/rv32i) | [ESP](/rv32i/ESP) | C3 | ESP32-C3 | [experimental](/rv32i/ESP)
[RV32I](/rv32i) | [VRV100](/rv32i/VRV100)*  | 103 .. 446 (FPGA only) | VRV100_441 | [good](/rv32i/VRV100)
[RV64G](/rv64g) | [Kendryte](/rv64g/kendryte) | K2 | K210 | [experimental](/rv64g/kendryte)
[XTENSA](/xtensa) | [ESP](/xtensa/ESP) | ESP32 | ESP32 | [experimental](/xtensa/ESP)

*The VRV100 FPGA-based Risc-V SoC is made by me, using the VexRiscV CPU and the SpinalHDL system. The SoC-s are accessible here: https://github.com/nvitya/VRV100

The [NVCM](https://github.com/nvitya/nvcm) project furthermore includes these - rather incomplete - family drivers: XMC, LPC, IMXRT. These can be expected in the future too.

# Peripheral Drivers

The VIHAL has its own peripheral drivers which use only the peripheral register interface,
no MCU vendor libraries are involved. 
As the VIHAL tries to provide the same SW interface for the different microcontrollers,
it does not cover all hardware features of all Microcontrollers.

__Driver__ | __Function__ | __Reference__
-----------|--------------|----------------
CPU Clock | CPU and chip clock setup (concentrating mainly on high performance mode) and clock counter | [reference](/doc/hwclk.adoc)
Pin Cfg. | Pin / Pad configuration | [reference](/doc/hwpins_cfg.adoc)
GPIO | GPIO control | [reference](/doc/hwpins_gpio.adoc)
Ext. IRQ | IRQ handling on GPIO Input Pins | [reference](/doc/hwextirq.md)
DMA | Direct Memory Access (cpu-less data transfers) | [reference](/doc/hwdma.md)
UART | Asynchronous serial communication with DMA support | [reference](/doc/hwuart.md)
SPI Master | 4-wire high speed serial communication | [reference](/doc/hwspi.md)
QSPI Master | High speed serial communication with two (dual) or 4 (quad) parallel data lines, usually only Master mode is supported | [reference](/doc/hwqspi.md)
Internal Flash | Erasing / Writing the internal Flash memory | [reference](/doc/hwintflash.md)
ADC | Analogue to digital converter (Free Running + Recording modes) | [reference](/doc/hwadc.md)
Simple PWM | PWM outputs with adjustable frequency and duty ratio | [reference](/doc/hwpwm.md)
CAN | Control Area Network - two wire industrial communication, (FD mode not supported yet) | [reference](/doc/hwcan.md)
USB Device | USB (Universal Serial Bus) device mode | [reference](/doc/hwusb.md)
Ethernet | Integrated Ethernet Controller | [reference](/doc/hweth.md)
SDCARD (experimental) | Built-in SDMMC for 4-line data modes | ...
SDRAM | SDRAM Initialization | [reference](/doc/hwsdram.md)
LCD | Integrated LCD Controller (in framebuffer mode) | [reference](/doc/hwlcd.md)

# Additional VIHAL Software Components

Beside the Hardware and CPU architecture specific peripheral drivers the VIHAL offers some additional MCU independent software compontents:
 - External HW Modules
   - Graphics Displays
     - Color TFT displays (SPI, Parallel, RGB - framebuffer)
     - LCD / OLED Monochrome displays with SPI or I2C interface
     - Text Screen support
   - Seven Segment display with different drivers (74HC595, TM1638, MAX7219 etc.)   
   - Simple LED-Stripe Handlers (APA102, WS2813)  
   - External memories (SPI / QSPI Flash, PSRAM, I2C EEPROM)
   - Input Devices
     - Keyboard Matrix
     - LED&KEY module
   - Special Sensors (AHT-10, BMP-280)
   - Simple GPIO-based stepper motor control
 - USB Device Support
   - HID device demo
   - CDC device skeleton, USB-Uart module
 - Networking Support
   - UDP Networking (work in progress)
   - TCR Networking (planned)

# Documentation

For more information you find the documentation here: [/doc/VIHAL_documentation.md](/doc/VIHAL_documentation.md) (work in progress)

