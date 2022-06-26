/*******************************************************************************
Copyright (c) 2006-2015 Cadence Design Systems Inc.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

/******************************************************************************
  Xtensa-specific interrupt and exception functions for RTOS ports.
  Also see xtensa_intr_asm.S.
******************************************************************************/

#include <stdlib.h>

//#include <xtensa/config/core.h>

#include "esp32_rtos_defs.h"

//#include "freertos/FreeRTOS.h"
//#include "freertos/portable.h"
#include "xtensa/xtensa_api.h"

//#include "sdkconfig.h"
//#include "esp_rom_sys.h"

/* Handler table is in xtensa_intr_asm.S */

extern "C" xt_exc_handler _xt_exception_table[];


/*
  Default handler for unhandled exceptions.
  CHANGED: We do this in panic.c now
*/

extern "C" void xt_unhandled_exception(XtExcFrame *frame)
{
  while (true)
  {
    //
  }
}

/*
  This function registers a handler for the specified exception.
  The function returns the address of the previous handler.
  On error, it returns 0.
*/
extern "C" xt_exc_handler xt_set_exception_handler(int n, xt_exc_handler f)
{
  while (true)
  {
    //
  }
}

/* Handler table is in xtensa_intr_asm.S */

typedef struct xt_handler_table_entry {
    void * handler;
    void * arg;
} xt_handler_table_entry;

extern xt_handler_table_entry _xt_interrupt_table[XCHAL_NUM_INTERRUPTS*portNUM_PROCESSORS];


/*
  Default handler for unhandled interrupts.
*/
extern "C" void xt_unhandled_interrupt(void * arg)
{
  while (true)
  {
    //
  }
}

//Returns true if handler for interrupt is not the default unhandled interrupt handler
extern "C" bool xt_int_has_handler(int intr, int cpu)
{
  while (true)
  {
    //
  }
}

/*
  This function registers a handler for the specified interrupt. The "arg"
  parameter specifies the argument to be passed to the handler when it is
  invoked. The function returns the address of the previous handler.
  On error, it returns 0.
*/
extern "C" xt_handler xt_set_interrupt_handler(int n, xt_handler f, void * arg)
{
  return f;
}

