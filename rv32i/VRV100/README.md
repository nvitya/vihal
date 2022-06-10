# VIHAL Support for the VRV100 (RV32I using VexRiscV) family

The VRV100 RISC-V SoC project is created by me (nvitya) making some ready to use variants using
the VexRiscV project (https://github.com/SpinalHDL/VexRiscv).

Actually there is not much driver development needed here, because these FPGA soft-core have only
a few fix peripherals, which intended to be extended via APB (using HDL code).

Self-flashing is availabe using the FPGA configuration external Flash device, so you can develop
software here like on any other microcontrollers using a JTAG hardware.
Beware that the VexRiscV requires its own openocd.


## VIHAL Driver Status for the VRV100 Family

  Driver              | Status  |
----------------------|---------|
__CPU Clock__         | OK      |
__Pin Cfg. + GPIO__   | not required |
__UART__              | OK      |
__DMA__               | not available |
__SPI Master__        | OK      |
__SPI Self Flashing__ | OK      |


You can define your own hardware and made it accessible over APB. See my VRV100 examples.