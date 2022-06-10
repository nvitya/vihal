# VIHAL Support for RP2040

The RP2040 does not have internal flash memory but it has relative large (264 kByte) internal RAM.
However from the external flash memory cached XIP execution is supported, this is not preferred by me.
Code execution from RAM is much simpler and faster, therefore I provided examples and linker scripts only for execution from
RAM (sometimes with self-flashing). This has a big advantage, that the external Flash also can be used freely
for data storage.

## VIHAL Driver Status for the RP2040 Integrated Peripherals

  Driver              | Status  |
----------------------|---------|
__CPU Clock__         | OK      |
__Pin Cfg. + GPIO__   | OK      |
__UART__              | OK      |
__DMA__               | OK, with circular support |
__SPI Master__        | OK      |
__QSPI__              | OK, ext. flash only |
__SPI Self Flashing__ | OK      |
__I2C Master__        | planned |
__I2C Slave__         | planned |
__Ext. IRQ.__         | planned |
__ADC__               | OK      |
__Simple PWM__        | OK      |
__USB Device__        | OK      |
__PIO__               | -       |

