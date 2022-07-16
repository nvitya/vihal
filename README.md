# Introduction

VIHAL = Vendor Independent Hardware Access Layer/Library.

It targets bare metal embedded software development, with the following main goals:
 - Easy to use
 - Small, simple but effective. Easy to verify.
 - Vendor independent
 - No intermediate libraries, external dependencies 
 - non-blocking communication interfaces (extensive use of DMA and state machines) which allows simple cooperative multi-tasking
 - license permitting unrestricted commercial usage (without warranty)
 - Built-in support for multiple popular Microcontrollers
 - Support multiple development environments
 - Provide SW modules and examples for usual scenarios
 - Ready to use examples for popular evaluation/development boards
 - Easily extensible with additional microcontrollers or architectures

The project development began in 2017, the direct predecessor is my NVCM project (which supports only ARM Cortex-M).

The VIHAL tries to provide the same SW interface for the different microcontrollers,
and so it does not cover all hardware features of all Microcontrollers.

# Quick Start

For the quick start here are ready to use examples for several popular boards (for Eclipse CDT):
  https://github.com/nvitya/vihaltests

# Peripheral Drivers

The VIHAL has its own peripheral drivers which uses only the peripheral register interface,
no MCU vendor libraries are involved. Developing a peripheral driver for VIHAL can be a harder task,
depending on the peripheral complexity.

## Included CPU Architectures

__Architecture ID__ | Description
--------------------|------------
ARMM  | ARM Cortex-M (M0, M3, M4, M7)
RV32I | 32-bit RISC-V
RV64G | 64-bit RISC-V (experimental)
XTENSA | Tensilica Xtensa LX6 (ESP32, experimental)

## Currently Included MCU Families

Those microcontrollers that share the same drivers belong to the same VIHAL microcontroller family. The families are named usually after the part ordering numbers.

Arch. | Family ID | Sub-Families | Examples
----------|-------|--------------|---------
ARMM   | STM32    |  F0, F1, F3, F4, F7, H7 | STM32F103C8, STM32H750VB
ARMM   | ATSAM    | ARMM | 3X, 4S, E70/S70/V70 | ATSAME3X8E, ATSAME70Q20
ARMM   | ATSAM_V2 | ARMM | D51/E5x, C2x, D10 | ATSAME51J20
ARMM   | LPC_V3   | ARMM | LPC546xx | LPC54608J512
RV32I  | GD32V    | F1 | GD32VF103C8
RV32I  | VRV100*  | 103 .. 446 (FPGA only) | VRV100_441
RV64G  | Kendryte | K2 | K210
XTENSA | ESP      | ESP32 | ESP32

*The VRV100 FPGA-based Risc-V SoC is made by me, using the VexRiscV CPU and the SpinalHDL system. The SoC-s are accessible here: https://github.com/nvitya/VRV100

The NVCM project furthermore includes these - rather incomplete - family drivers: XMC, LPC, IMXRT. These can be expected in the future too.

# Users Manual

I will write a users manual sometime ...

