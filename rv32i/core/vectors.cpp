// vector.cpp

#include "platform.h"

extern "C"
{

void Default_Handler(void);
void Exception_Handler(void);

/* Peripheral handlers */
void IRQ_Handler_01       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_02       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_03       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_04       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_05       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_06       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_07       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_08       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_09       ( void ) __attribute__ ((weak, alias("Default_Handler")));

void IRQ_Handler_10       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_11       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_12       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_13       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_14       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_15       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_16       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_17       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_18       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_19       ( void ) __attribute__ ((weak, alias("Default_Handler")));

void IRQ_Handler_20       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_21       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_22       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_23       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_24       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_25       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_26       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_27       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_28       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_29       ( void ) __attribute__ ((weak, alias("Default_Handler")));

void IRQ_Handler_30       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_31       ( void ) __attribute__ ((weak, alias("Default_Handler")));

typedef void (* pHandler)(void);

__attribute__ ((naked, section(".startup"),aligned(256)))
void __isr_vectors(void)
{
  asm("j Exception_Handler");
  asm("j IRQ_Handler_01");
  asm("j IRQ_Handler_02");
  asm("j IRQ_Handler_03");
  asm("j IRQ_Handler_04");
  asm("j IRQ_Handler_05");
  asm("j IRQ_Handler_06");
  asm("j IRQ_Handler_07");
  asm("j IRQ_Handler_08");
  asm("j IRQ_Handler_09");
  asm("j IRQ_Handler_10");
  asm("j IRQ_Handler_11");
  asm("j IRQ_Handler_12");
  asm("j IRQ_Handler_13");
  asm("j IRQ_Handler_14");
  asm("j IRQ_Handler_15");
  asm("j IRQ_Handler_16");
  asm("j IRQ_Handler_17");
  asm("j IRQ_Handler_18");
  asm("j IRQ_Handler_19");
  asm("j IRQ_Handler_20");
  asm("j IRQ_Handler_21");
  asm("j IRQ_Handler_22");
  asm("j IRQ_Handler_23");
  asm("j IRQ_Handler_24");
  asm("j IRQ_Handler_25");
  asm("j IRQ_Handler_26");
  asm("j IRQ_Handler_27");
  asm("j IRQ_Handler_28");
  asm("j IRQ_Handler_29");
  asm("j IRQ_Handler_30");
  asm("j IRQ_Handler_31");
}

// Processor ends up here if an unexpected interrupt occurs or a
// specific handler is not present in the application code.
// When in DEBUG, trigger a debug exception to clearly notify
// the user of the exception and help identify the cause.

void __attribute__ ((section(".after_vectors"),weak,interrupt))
Default_Handler(void)
{
#if 0 // todo: detect if the debugger is connected
  if (CoreDebug->DHCSR & 1)
#endif
  {
    asm("ebreak");
  }

  while (1)
  {
    asm("nop");
  }

  asm("mret");
}

__attribute__ ((section(".after_vectors"),weak,interrupt))
void Exception_Handler (void)
{
#if 0 // todo: detect if the debugger is connected
  if (CoreDebug->DHCSR & 1)
#endif
  {
    asm("ebreak");
  }

  while (1)
  {
    asm("nop");
  }
}

} // extern "C"
