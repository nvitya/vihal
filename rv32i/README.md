# VIHAL Support for 32-bit RISC-V (RV32I)

There are several real boards and FPGA soft-cores available with the RV32I architecture.
I own a Longan Nano (GD32VF) two ESP32-C3 boards and several FPGA Boards which can run soft cores.

## Supported 32-bit RISC-V Microcontrollers

Family ID | Sub-Families | Examples | Driver Support
----------|--------------|----------|---------------
[GD32V](/rv32i/GD32V) | F1 | GD32VF103C8 | [partial](/rv32i/GD32V)
[ESP](/rv32i/ESP) | C3 | ESP32-C3 | [work in progress](/rv32i/ESP)
[VRV100](/rv32i/VRV100)*  | 103 .. 446 (FPGA only) | VRV100_441 | [good](/rv32i/VRV100)

*The VRV100 FPGA-based Risc-V SoC is made by me, using the VexRiscV CPU and the SpinalHDL system. The SoC-s are accessible here: https://github.com/nvitya/VRV100

## RISC-V GCC Compiler

The easiest way to obtain a RISC-V compiler (for embedded devices) to download from the xPack releases repository:

https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases

__Note:__
Add the "_zicsr" to the "Target Processor" / "Other extensions" if you get "unrecognized opcode csrw ..."
