# VIHAL Support for RV32I ESP Microcontrollers

I own two ESP32-C3 Boards to develop the necessary drivers.

## Microcontrollers

The full list of built-in microcontrollers can be found here:
[/rv32i/ESP/src/mcu_builtin.h](/rv32i/ESP/src/mcu_builtin.h)

Sub Family | Built-In MCU Examples
-----------|--------------
32C3 | ESP32C3

## Driver Status

  Driver              | Status  |
----------------------|---------|
__CPU Clock__         | OK      |
__Pin Cfg. + GPIO__   | OK      |
__UART__              | OK      |
__DMA__               | planned |
__SPI Master__        | partial, no SPI flash access yet|
__I2C Master__        | planned |
__I2C Slave__         | planned |
__Ext. IRQ.__         | planned |
__ADC__               | planned |
__CAN__               | planned |
__Simple PWM__        | planned |

## Important note
Unfortunately I'm stuck with accessing the external flash memory. This is crucial to provide self-flashing applications, which are required for this device.
(The external flash interface - spimem - is not documented, however some Espressif sources are available)
