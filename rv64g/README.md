# VIHAL Support for 64-bit RISC-V (RV64G)

I've started experimenting with RV64 with the Kendryte K210 using a Maix Bit board.

In 2024 I've bought a Milk-V Duo and a Lichee RV Nano with the CV1800/SG2002 dual core processor.
The cooperation between the Little (700 MHz) RV64 core and the Linux (1000 MHz) core works fine using shared memory.

## Supported 64-bit RISC-V Microcontrollers

Family ID | Sub-Families | Examples | Driver Support
----------|--------------|----------|---------------
[kendryte](/rv64g/kendryte) | K2 | K210 | [experimental](/rv64g/kendryte)
[sg2000](/rv64g/sg2000) | SG200x | SG2002, CV1800 | [partial](/rv64g/sg2000)

