/*
 *  file:     esp32_vihal_asm.h
 *  brief:    VIHAL ESP32 include for the assembly parts
 *  created:  2022-06-26
 *  authors:  nvitya
*/

#pragma once

// add some basic xtensa includes

#define BOOTLOADER_BUILD 1

#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/panic_reason.h>
#include <xtensa/xtensa_context.h>
#include <xtensa/config/core-isa.h>
//#include <xtensa/config/system.h>

#include "esp32_rtos_defs.h"



