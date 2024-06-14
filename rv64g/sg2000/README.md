# VIHAL Support for Cvitek/Sophgo CV1800/SG200x

The Milk-V Duo is a cheap Linux board with Hard Real-Time possibility due its isolated extra RV64G core.
The 64 MByte RAM on the original Milk-V Duo (CV1800) limits it to run a special slimmed down linux version, but still works fine.

The updated Milk-V Duo versions, the Milk-V Duo 256 or Milk-V Duo S, has at least 256 MBytes of RAM, so they can run normal 
Linux systems too, like Debian or Ubuntu.

The small RV64G Core have a specific memory range where it runs, this can be loaded with bare-metal code like VIHAL-based applications.
The required resource table is embedded now in the VIHAL in order to be able to load the VIHAL applications via remote processor control.

The CV1800/SG200x chips are relatively good documented, theese are freely available.
The RV64G Cores are XuanTie C906 Open-Source Risc-V Cores, I've found a documentation here: 
[XuanTie-OpenC906-UserManual.pdf](https://occ-intl-prod.oss-ap-southeast-1.aliyuncs.com/resource/XuanTie-OpenC906-UserManual.pdf)

## Microcontrollers

The full list of built-in microcontrollers can be found here:
[/rv64g/sg2000/src/mcu_builtin.h](/rv64g/sg2000/src/mcu_builtin.h)

Sub Family | Built-In MCU Examples
-----------|--------------
SG2000 | CV1800, SG2002

## Driver Status

  Driver              | Status
----------------------|---------
__CPU Clock__         | fix
__Pin Cfg. + GPIO__   | OK
__UART__              | OK
__DMA__               | OK
__SPI Master__        | OK
__QSPI Master__ (SPI NOR) | planned
__Self Flashing__     | -
__I2C Master__        | planned
__I2C Slave__         | planned
__Ext. IRQ.__         | OK
__Simple PWM__        | OK

sg2000 Driver Notes:
  - Only the small (700 MHz) processor core supported, on the other one better to run Linux

## Debugging

For the debugging you need to switch the four pins to their JTAG function (for example using the cvi_pinmux utility).
Then use a cklink hw debugger, what you can get from a cheap and widely available STM32F103 blue pill board.

