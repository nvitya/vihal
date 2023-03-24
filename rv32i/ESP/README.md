# VIHAL Support for RV32I ESP Microcontrollers

__WARNING: Wifi/Bluetooth support is not included and probably they won't ever work with VIHAL__ (For these parts is no hardware documentation available and the Espressif drivers are closed source)

The ESP32-C3 does not have internal flash memory but it has relative large (400 kByte) internal RAM. Even when execution from the cached external flash memory (XIP) is supported by the device, this is not preferred by me. Code execution from RAM is much simpler and faster, and also has a big advantage, that the external Flash also can be used freely for data storage.

The most of the VIHAL tests/examples for the ESP32 use self-flasing.

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
__USB-SERIAL__        | OK      |
__DMA__               | planned |
__QSPI Master__       | OK (Flash only) |
__SPI Self Flashing__ | OK |
__SPI Master__        | work in progress |
__I2C Master__        | planned |
__I2C Slave__         | planned |
__Ext. IRQ.__         | planned |
__ADC__               | planned |
__CAN__               | planned |
__Simple PWM__        | planned |
__WIFI__              | not planned ! |
__Bluetooth__         | not planned ! |

