# VIHAL Support for Kendryte K210

The Kendryte K210 is a very powerful 64-bit embedded RISC-V processor at an affordable price.
Unfortunately a proper documentation is missing for this Microcontroller. Kendryte provides a [Standalone SDK](https://github.com/kendryte/kendryte-standalone-sdk), where you can __mostly__ find the peripheral registers and their bit definitions.

## Microcontrollers

The full list of built-in microcontrollers can be found here:
[/rv64g/kendryte/src/mcu_builtin.h](/rv64g/kendryte/src/mcu_builtin.h)

Sub Family | Built-In MCU Examples
-----------|--------------
K2 | K210

## Driver Status

  Driver              | Status  
----------------------|---------
__CPU Clock__         | planned 
__Pin Cfg. + GPIO__   | OK 
__UART__              | OK 
__DMA__               | planned 
__SPI Master__        | planned 
__Self Flashing__     | planned 
__I2C Master__        | planned 
__I2C Slave__         | planned 
__Ext. IRQ.__         | planned 
__ADC__               | planned 
__Simple PWM__        | planned 

K210 Driver Notes:
  - Only one processor core supported yet
  - I'm not planning to support the special peripherals like AI processor, audio processor. You can still take the sdk parts for these.  

## Debugging

For the debugging you need a special OpenOCD version from Kendryte: https://github.com/kendryte/openocd-kendryte

The debugging, especially the first connection is pretty unstable. I need to force the K210 board to bootloader mode (pressing both boot and reset buttons, and releasing the reset button first). You have to be careful with the JTAG pins (or power supply supply?), because I have a Maix board where the JTAG does not work anymore.

