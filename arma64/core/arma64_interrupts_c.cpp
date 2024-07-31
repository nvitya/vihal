/*
 * arma64_c_interrupts.cpp
 *
 *  Created on: Jul 31, 2024
 *      Author: vitya
 */

#include "platform.h"

extern "C" void exception_handler_c() //void * aframe)
{
  //volatile TInterruptFrame * pframe = aframe;

  while (true)
  {
    asm("nop");
  }
}


extern "C" void irq_handler_c(void * aframe)
{
  //volatile TInterruptFrame * pframe = aframe;

  while (true)
  {
  }
}

extern "C" void serror_handler_c(void * aframe)
{
  //volatile TInterruptFrame * pframe = aframe;

  while (true)
  {
  }
}
