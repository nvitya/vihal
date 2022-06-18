// vector.cpp

#include "platform.h"

extern "C"
{

void Default_Handler(void);

/* Peripherals handlers */
void IRQ_Handler_00       ( void ) __attribute__ ((weak, alias("Default_Handler")));
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

extern const pHandler __isr_vectors[];

__attribute__((aligned(64),used))
const pHandler __isr_vectors[] =
{
  (pHandler) IRQ_Handler_00,  // exception 00:
  (pHandler) IRQ_Handler_01,  // exception 01:
  (pHandler) IRQ_Handler_02,  // exception 02:
  (pHandler) IRQ_Handler_03,  // exception 03:
  (pHandler) IRQ_Handler_04,  // exception 04:
  (pHandler) IRQ_Handler_05,  // exception 05:
  (pHandler) IRQ_Handler_06,  // exception 06:
  (pHandler) IRQ_Handler_07,  // exception 07:
  (pHandler) IRQ_Handler_08,  // exception 08:
  (pHandler) IRQ_Handler_09,  // exception 09:
  (pHandler) IRQ_Handler_10,  // exception 10:
  (pHandler) IRQ_Handler_11,  // exception 11:
  (pHandler) IRQ_Handler_12,  // exception 12:
  (pHandler) IRQ_Handler_13,  // exception 13:
  (pHandler) IRQ_Handler_14,  // exception 14:
  (pHandler) IRQ_Handler_15,  // exception 15:

  (pHandler) IRQ_Handler_16,  // irq 00:
  (pHandler) IRQ_Handler_17,  // irq 01:
  (pHandler) IRQ_Handler_18,  // irq 02:
  (pHandler) IRQ_Handler_19,  // irq 03: Machine software interrupt
  (pHandler) IRQ_Handler_20,  // irq 04:
  (pHandler) IRQ_Handler_21,  // irq 05:
  (pHandler) IRQ_Handler_22,  // irq 06:
  (pHandler) IRQ_Handler_23,  // irq 07: Machine timer interrupt
  (pHandler) IRQ_Handler_24,  // irq 08:
  (pHandler) IRQ_Handler_25,  // irq 09:
  (pHandler) IRQ_Handler_26,  // irq 10:
  (pHandler) IRQ_Handler_27,  // irq 11: Machine external interrupt
  (pHandler) IRQ_Handler_28,  // irq 12:
  (pHandler) IRQ_Handler_29,  // irq 13:
  (pHandler) IRQ_Handler_30,  // irq 14:
  (pHandler) IRQ_Handler_31   // irq 15:
};


__attribute__ ((section(".after_vectors"),weak))
void mcu_interrupt_controller_init()
{
  //
}

// Processor ends up here if an unexpected interrupt occurs or a
// specific handler is not present in the application code.
// When in DEBUG, trigger a debug exception to clearly notify
// the user of the exception and help identify the cause.

void __attribute__ ((section(".after_vectors"),weak))
Default_Handler(void)
{
  volatile uint32_t irqid = cpu_csr_read(CSR_MCAUSE); // get this for debugging

  while (1)
  {
    asm("nop");
  }

  asm("mret");
}

} // extern "C"
