/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2021 Viktor Nagy, nvitya
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 * --------------------------------------------------------------------------- */
/*
 *  file:     mcu_builtin.h (VRV100)
 *  brief:    Built-In VRV100 (VexRiscV FPGA) Variants
 *  version:  1.00
 *  date:     2021-10-02
 *  authors:  nvitya
*/

#ifndef __MCU_BUILTIN_H
#define __MCU_BUILTIN_H

#if 0

//----------------------------------------------------------------------
// STM32
//----------------------------------------------------------------------

// STM32F0: Cortex-M0

#elif defined(MCU_VRV153)

  #define MCUF_VRV100
  #define MCUSF_150

  #include "VRV150.h"

#else

  #error "Unknown MCU"

#endif

#endif
