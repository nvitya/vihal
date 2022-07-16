# VIHAL Support for Kendryte K210

The Kendryte K210 is a very powerful 64-bit embedded RISC-V processor at an affordable price.

I own a MaiX Bit and a Maix Dock board. I've begun the development, had a good progress but
once the openocd stopped working. It seems that the JTAG lines are demaged at the Maix Bit board I've used and I took a while until I got another one.
So I plan to provide more drivers in the near future.

## Microcontrollers

The full list of built-in microcontrollers can be found here:
[/rv32i/GD32V/src/mcu_builtin.h](/rv32i/GD32V/src/mcu_builtin.h)

Sub Family | Built-In MCU Examples
-----------|--------------
F1 | GD32VF103CB

## Driver Status

  Driver              | Status  |
----------------------|---------|
__CPU Clock__         | planned |
__Pin Cfg. + GPIO__   | planned |
__UART__              | planned |
__DMA__               | planned |
__SPI Master__        | planned |
__I2C Master__        | planned |
__I2C Slave__         | planned |
__Ext. IRQ.__         | planned |
__ADC__               | planned |
__Simple PWM__        | planned |
