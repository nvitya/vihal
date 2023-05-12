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
 *  file:     tftlcd_mm16.h
 *  brief:    16 bit parallel TFT LCD Display driver using external memory controller
 *  version:  1.00
 *  date:     2018-02-10
 *  authors:  nvitya
*/

#ifndef TFT_TFTLCD_MM16C_H_
#define TFT_TFTLCD_MM16C_H_

#include "tftlcd.h"

class TTftLcd_mm16c: public TTftLcd
{
public:
	typedef TTftLcd super;

	volatile uint16_t *  cmdreg16 = nullptr;
	volatile uint32_t *  cmdreg32 = nullptr;
	volatile uint16_t *  datareg16 = nullptr;
	volatile uint32_t *  datareg32 = nullptr;

public:
	// interface dependent

	virtual bool InitInterface();

	virtual void WriteCmd(uint8_t adata);
	virtual void WriteData8(uint8_t adata);
	virtual void WriteData16(uint16_t adata);

	virtual void SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t w,  uint16_t h);
	virtual void FillColor(uint16_t acolor, unsigned acount);
  virtual void BlockFill(uint16_t acolor, unsigned acount);

};

#endif /* TFT_TFTLCD_MM16C_H_ */
