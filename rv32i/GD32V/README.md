# VIHAL Support for GD32Vxx

The GD32V is a cheap device with RV32I RISC-V architecture.
I developed the peripheral drivers using a Longan Nano board which has a GD32VF108TB (128k Flash + 32k SRAM) on it.
The peripherals try to clone the STM32F1xx family.
The GD32VF1 outperforms the STM32F1 thanks to faster Flash and higher clock speed (108 vs 72 MHz).

## VIHAL Driver Status for the GD32V Integrated Peripherals

  Driver              | Status  |
----------------------|---------|
__CPU Clock__         | OK      |
__Pin Cfg. + GPIO__   | OK      |
__UART__              | OK      |
__DMA__               | OK      |
__SPI Master__        | OK      |
__I2C Master__        | planned |
__I2C Slave__         | planned |
__Ext. IRQ.__         | OK      |
__ADC__               | planned |
__DAC__               | planned |
__CAN__               | planned |
__Simple PWM__        | planned |
__USB Device__        | planned |


