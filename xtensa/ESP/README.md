# VIHAL Support for ESP32 (Xtensa)

The ESP32 is a very strong and affordable microcontroller. I wanted to support it by the VIHAL, but unfortunately
because of the CPU complexity and lack of information I abandoned it for now.

The Xtensa LX16 is a very special CPU with 16 from 64 Windowed Registers. Compared to Risc-V or ARM Cortex-M
it requires a special initialization and interrupt handlers and I could not find proper examples or documentation.

Here is a site about the windowed api: https://sachin0x18.github.io/posts/demystifying-xtensa-isa/

You can help me out with a complete bare metal (without esp-idf) CPU initialzation and window overflow interrupt handler
(and with the other required stuff i don't know yet anything about).

If this is ready then I can do the VIHAL driver development, as usual...