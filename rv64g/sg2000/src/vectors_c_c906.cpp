// vector.cpp

#include "platform.h"

extern "C"
{

void Default_Handler(void);

void EXC_Handler_00       ( void ) __attribute__ ((weak, alias("Default_EXC_Handler")));
void EXC_Handler_01       ( void ) __attribute__ ((weak, alias("Default_EXC_Handler")));
void EXC_Handler_02       ( void ) __attribute__ ((weak, alias("Default_EXC_Handler")));
void EXC_Handler_03       ( void ) __attribute__ ((weak, alias("Default_EXC_Handler")));
void EXC_Handler_04       ( void ) __attribute__ ((weak, alias("Default_EXC_Handler")));
void EXC_Handler_05       ( void ) __attribute__ ((weak, alias("Default_EXC_Handler")));
void EXC_Handler_06       ( void ) __attribute__ ((weak, alias("Default_EXC_Handler")));
void EXC_Handler_07       ( void ) __attribute__ ((weak, alias("Default_EXC_Handler")));
void EXC_Handler_08       ( void ) __attribute__ ((weak, alias("Default_EXC_Handler")));
void EXC_Handler_09       ( void ) __attribute__ ((weak, alias("Default_EXC_Handler")));
void EXC_Handler_10       ( void ) __attribute__ ((weak, alias("Default_EXC_Handler")));
void EXC_Handler_11       ( void ) __attribute__ ((weak, alias("Default_EXC_Handler")));
void EXC_Handler_12       ( void ) __attribute__ ((weak, alias("Default_EXC_Handler")));
void EXC_Handler_13       ( void ) __attribute__ ((weak, alias("Default_EXC_Handler")));
void EXC_Handler_14       ( void ) __attribute__ ((weak, alias("Default_EXC_Handler")));
void EXC_Handler_15       ( void ) __attribute__ ((weak, alias("Default_EXC_Handler")));

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
void IRQ_Handler_32       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_33       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_34       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_35       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_36       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_37       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_38       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_39       ( void ) __attribute__ ((weak, alias("Default_Handler")));

void IRQ_Handler_40       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_41       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_42       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_43       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_44       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_45       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_46       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_47       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_48       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_49       ( void ) __attribute__ ((weak, alias("Default_Handler")));

void IRQ_Handler_50       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_51       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_52       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_53       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_54       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_55       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_56       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_57       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_58       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_59       ( void ) __attribute__ ((weak, alias("Default_Handler")));

void IRQ_Handler_60       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_61       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_62       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler_63       ( void ) __attribute__ ((weak, alias("Default_Handler")));


typedef void (* pHandler)(void);

extern const pHandler __isr_exc_vectors[];
extern const pHandler __isr_irq_vectors[];

__attribute__((aligned(64),used))
const pHandler __isr_exc_vectors[] =
{
  (pHandler) EXC_Handler_00,  // exception 00:
  (pHandler) EXC_Handler_01,  // exception 01:
  (pHandler) EXC_Handler_02,  // exception 02:
  (pHandler) EXC_Handler_03,  // exception 03:
  (pHandler) EXC_Handler_04,  // exception 04:
  (pHandler) EXC_Handler_05,  // exception 05:
  (pHandler) EXC_Handler_06,  // exception 06:
  (pHandler) EXC_Handler_07,  // exception 07:
  (pHandler) EXC_Handler_08,  // exception 08:
  (pHandler) EXC_Handler_09,  // exception 09:
  (pHandler) EXC_Handler_10,  // exception 10:
  (pHandler) EXC_Handler_11,  // exception 11:
  (pHandler) EXC_Handler_12,  // exception 12:
  (pHandler) EXC_Handler_13,  // exception 13:
  (pHandler) EXC_Handler_14,  // exception 14:
  (pHandler) EXC_Handler_15   // exception 15:
};

__attribute__((aligned(64),used))
const pHandler __isr_irq_vectors[] =
{
  // core internal interrupts

  (pHandler) IRQ_Handler_00,  // irq 00:
  (pHandler) IRQ_Handler_01,  // irq 01:
  (pHandler) IRQ_Handler_02,  // irq 02:
  (pHandler) IRQ_Handler_03,  // irq 03: Machine software interrupt
  (pHandler) IRQ_Handler_04,  // irq 04:
  (pHandler) IRQ_Handler_05,  // irq 05:
  (pHandler) IRQ_Handler_06,  // irq 06:
  (pHandler) IRQ_Handler_07,  // irq 07: Machine timer interrupt
  (pHandler) IRQ_Handler_08,  // irq 08:
  (pHandler) IRQ_Handler_09,  // irq 09:
  (pHandler) IRQ_Handler_10,  // irq 10:
  (pHandler) IRQ_Handler_11,  // irq 11: Machine external interrupt, catched and converted to 16-63
  (pHandler) IRQ_Handler_12,  // irq 12:
  (pHandler) IRQ_Handler_13,  // irq 13:
  (pHandler) IRQ_Handler_14,  // irq 14:
  (pHandler) IRQ_Handler_15,  // irq 15:

  // special external interrupts

  (pHandler) IRQ_Handler_16,  //
  (pHandler) IRQ_Handler_17,  //
  (pHandler) IRQ_Handler_18,  //
  (pHandler) IRQ_Handler_19,  //

  (pHandler) IRQ_Handler_20,  //
  (pHandler) IRQ_Handler_21,  //
  (pHandler) IRQ_Handler_22,  //
  (pHandler) IRQ_Handler_23,  //
  (pHandler) IRQ_Handler_24,  //
  (pHandler) IRQ_Handler_25,  //
  (pHandler) IRQ_Handler_26,  //
  (pHandler) IRQ_Handler_27,  //
  (pHandler) IRQ_Handler_28,  //
  (pHandler) IRQ_Handler_29,  //

  (pHandler) IRQ_Handler_30,  //
  (pHandler) IRQ_Handler_31,  //
  (pHandler) IRQ_Handler_32,  //
  (pHandler) IRQ_Handler_33,  //
  (pHandler) IRQ_Handler_34,  //
  (pHandler) IRQ_Handler_35,  //
  (pHandler) IRQ_Handler_36,  //
  (pHandler) IRQ_Handler_37,  //
  (pHandler) IRQ_Handler_38,  //
  (pHandler) IRQ_Handler_39,  //

  (pHandler) IRQ_Handler_40,  //
  (pHandler) IRQ_Handler_41,  //
  (pHandler) IRQ_Handler_42,  //
  (pHandler) IRQ_Handler_43,  //
  (pHandler) IRQ_Handler_44,  //
  (pHandler) IRQ_Handler_45,  //
  (pHandler) IRQ_Handler_46,  //
  (pHandler) IRQ_Handler_47,  //
  (pHandler) IRQ_Handler_48,  //
  (pHandler) IRQ_Handler_49,  //

  (pHandler) IRQ_Handler_50,  //
  (pHandler) IRQ_Handler_51,  //
  (pHandler) IRQ_Handler_52,  //
  (pHandler) IRQ_Handler_53,  //
  (pHandler) IRQ_Handler_54,  //
  (pHandler) IRQ_Handler_55,  //
  (pHandler) IRQ_Handler_56,  //
  (pHandler) IRQ_Handler_57,  //
  (pHandler) IRQ_Handler_58,  //
  (pHandler) IRQ_Handler_59,  //

  (pHandler) IRQ_Handler_60,  //
  (pHandler) IRQ_Handler_61,  //
  (pHandler) IRQ_Handler_62,  //
  (pHandler) IRQ_Handler_63   //
};


__attribute__ ((section(".after_vectors"),weak))
void mcu_interrupt_controller_init()
{
  // disable all interrupts
  PLIC->H0_MIE[0] = 0;
  PLIC->H0_MIE[1] = 0;
  PLIC->H0_MIE[2] = 0;
  PLIC->H0_SIE[0] = 0;
  PLIC->H0_SIE[1] = 0;
  PLIC->H0_SIE[2] = 0;

  PLIC->PER = 1;     // enable S-Mode access to PLIC
  PLIC->H0_MTH = 0;  // M-Mode interrupt threshold, 0 = all interrupts
  PLIC->H0_STH = 0;  // S-Mode interrupt threshold, 0 = all interrupts

  PLIC->H0_MCLAIM = PLIC->H0_MCLAIM;
  PLIC->IP[0] = 0;   // clear all pending interrupts
  PLIC->IP[1] = 0;

  // Enable all interrupt sources (the interrupts are still disabled)
  uint32_t mie = (0
    | (0 <<  1)  // SSIE
    | (1 <<  3)  // MSIE: machine software interrupt
    | (0 <<  5)  // STIE
    | (1 <<  7)  // MTIE: macthine timer interrupt
    | (0 <<  9)  // SEIE
    | (1 << 11)  // MEIE: machine external interrupt
  );

  cpu_csr_setbits(CSR_MIE, mie);
  cpu_csr_clrbits(CSR_MIP, mie);  // clear pending interrupts
}

// Processor ends up here if an unexpected interrupt occurs or a
// specific handler is not present in the application code.

void __attribute__ ((section(".after_vectors"),weak))
Default_EXC_Handler(void)
{
  volatile intptr_t irqid = cpu_csr_read(CSR_MCAUSE); // get this for debugging

  while (1)
  {
    asm("nop");
  }

  asm("mret");
}


void __attribute__ ((section(".after_vectors"),weak))
Default_Handler(void)
{
  volatile intptr_t irqid = cpu_csr_read(CSR_MCAUSE); // get this for debugging

  while (1)
  {
    asm("nop");
  }

  asm("mret");
}

} // extern "C"
