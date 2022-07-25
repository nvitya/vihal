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
 *  file:     mcu_defs.h (Kendryte)
 *  brief:    Kendryte special MCU definitions
 *  created:  2022-02-01
 *  authors:  nvitya
*/

#ifndef _MCU_DEFS_H
#define _MCU_DEFS_H

#define MCUF_KENDRYTE
#define SELF_FLASHING_RAM_ADDR  0x80000000
#define SELF_FLASHING_SPI_ADDR  0x00000000
#define HW_DMA_MAX_COUNT             32764

#define MCU_INTERNAL_RC_SPEED      8000000


inline void __attribute__((always_inline)) mcu_preinit_code()
{
}

#endif
