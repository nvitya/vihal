# VIHAL Support for RP2040

The RP2040 does not have internal flash memory but it has relative large (264 kByte) internal RAM.
However from the external flash memory cached XIP execution is supported, this is not preferred by me.
Code execution from RAM is much simpler and faster, therefore I provided examples and linker scripts only for execution from
RAM (sometimes with self-flashing). This has a big advantage, that the external Flash also can be used freely
for data storage.

The most of the VIHAL tests/examples for the RP2040 use self-flasing.

## VIHAL Driver Status for the RP2040 Integrated Peripherals

  Driver              | Status  |
----------------------|---------|
__CPU Clock__         | OK      |
__Pin Cfg. + GPIO__   | OK      |
__UART__              | OK      |
__DMA__               | OK (1)  |
__SPI Master__        | OK      |
__QSPI__              | OK (2) |
__SPI Self Flashing__ | OK      |
__I2C Master__        | OK      |
__I2C Slave__         | planned |
__Ext. IRQ.__         | OK      |
__ADC__               | OK      |
__Simple PWM__        | OK      |
__USB Device__        | OK      |
__PIO__               | - (3)   |


__Notes__:

1. Circular DMA buffer support uses additional helper channels, wich are allocated automatically from channel 11 backwards. Therefore it is recommended using only channels 0-7.
2. The QSPI hardware is very much restricted to handle the external Flash memory only
3. The PIO unit is a very Raspberry PI specific one, no vendor independent driver is possible here

