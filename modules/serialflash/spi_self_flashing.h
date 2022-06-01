/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     spi_self_flashing.h
 *  brief:    SPI Self Flashing Interface
 *  date:     2022-06-01
 *  authors:  nvitya
*/

#ifndef SPI_SELF_FLASHING_RP_H_
#define SPI_SELF_FLASHING_RP_H_

#ifdef SPI_SELF_FLASHING

#include "platform.h"
#include "spiflash.h"

bool spi_self_flashing(TSpiFlash * spiflash);

#endif

#endif /* SPI_SELF_FLASHING_H_ */
