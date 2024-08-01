/*
 * arma64_c_interrupts.cpp
 *
 *  Created on: Jul 31, 2024
 *      Author: vitya
 */

#include "platform.h"

extern "C" __attribute__((weak))
void exception_handler_c(TInterruptFrame * pframe, intptr_t aindex)
{
  while (true)
  {
    asm("nop");
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
