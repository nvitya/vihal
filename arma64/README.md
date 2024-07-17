# VIHAL Support for 64-bit ARM-A, Linux and Bare Metal


## Supported 64-bit ARM-A Microcontrollers

Family ID | Sub-Families | Examples | Driver Support
----------|--------------|----------|---------------
[bcm](/arma64/bcm) | 2711 | BCM2711 (Raspberry PI4) | [experimental](/arma64/bcm)


## Raspberry PI4 AMP Setup with JTAG Debugging

AMP = Assymetric multi-processing

Target scenario: the first three cores are running 64-bit linux, the fourth core is
running bare-metal (BM) totally separated.

On a 2GByte RPI4 Version:

A small amount of the RAM must be reserved for the BM-Core, like 8 MBytes.

Extra kernel parameters (/boot/firmware/cmdline.txt):

  maxcpus=3 mem=2040M

The Linux won't miss this 8 MByte RAM, but this is rather huge for an embedded real-time system.
Probably 64 kByte RAM would be also enough for the 98% of the possible applications.
