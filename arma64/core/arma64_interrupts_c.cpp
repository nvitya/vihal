/*
 * arma64_c_interrupts.cpp
 *
 *  Created on: Jul 31, 2024
 *      Author: vitya
 */

#include "platform.h"

extern "C" __attribute__((weak))
void exception_handler_c(TInterruptFrame * pframe, intptr_t aindex, uint64_t esr)
{
  uint8_t eclass = ((esr >> 26) & 0x3F);

  if (0 == eclass)  // unknown reason
  {
    while (true) asm("nop");
  }
  else if (1 == eclass) // trapped WF*
  {
    while (true) asm("nop");
  }
  else if (3 == eclass) // trapped MRC, MCR
  {
    while (true) asm("nop");
  }
  else if (4 == eclass) // trapped MCRR, MRRC
  {
    while (true) asm("nop");
  }
  else if (5 == eclass) // trapped MRC, MCR
  {
    while (true) asm("nop");
  }
  else if (6 == eclass) // trapped LDC, STC
  {
    while (true) asm("nop");
  }
  else if (7 == eclass) // trapped SME, SVE
  {
    while (true) asm("nop");
  }
  else if (10 == eclass) // trapped LD64B, ST64B
  {
    while (true) asm("nop");
  }
  else if (12 == eclass) // trapped MRRC
  {
    while (true) asm("nop");
  }
  else if (13 == eclass) // Branch Target Exception
  {
    while (true) asm("nop");
  }
  else if (14 == eclass) // Illegal Executioin state
  {
    while (true) asm("nop");
  }
  else if (17 == eclass) // SVC instruction in AArch32
  {
    while (true) asm("nop");
  }
  else if (0x14 == eclass) // Trapped MSSR, MRRS
  {
    while (true) asm("nop");
  }
  else if (0x15 == eclass) // SVC Instuction in AArch64
  {
    while (true) asm("nop");
  }
  else if (0x18 == eclass) // Trapped MSR, MRS
  {
    while (true) asm("nop");
  }
  else if (0x3D == eclass) // PMU exception
  {
    while (true) asm("nop");
  }
  else
  {
    while (true) asm("nop");
  }
}


extern "C" __attribute__((weak))
void irq_handler_c(TInterruptFrame * aframe, intptr_t aindex)
{
  while (true)
  {
    asm("nop");
  }
}

extern "C" __attribute__((weak))
void serror_handler_c(TInterruptFrame * aframe, intptr_t aindex)
{

  while (true)
  {
    asm("nop");
  }
}
