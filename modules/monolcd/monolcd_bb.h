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
// file:     monolcd_bb.h
// brief:    Monochrome LCD GPIO bit-bang control interface (where the SPI cannot be used)
// created:  2022-03-06
// authors:  nvitya

#ifndef MONOLCD_BB_H_
#define MONOLCD_BB_H_

#include "monolcd.h"
#include "hwpins.h"

class TMonoLcd_bb : public TMonoLcd
{
public:
	// interface dependent
	TGpioPin        pin_ce;
	TGpioPin        pin_clk;
	TGpioPin        pin_din;
	TGpioPin 	      pin_reset;

	int             update_byte_batch = 16;  // determines the CPU time in the Run()

	virtual bool InitInterface();

	virtual void WriteCmd(uint8_t adata);

	//virtual void WriteData8(uint8_t adata);
	//virtual void WriteData16(uint16_t adata);

	//virtual void SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t w,  uint16_t h);
	//virtual void FillColor(uint16_t acolor, unsigned acount);

	virtual void Run(); // constantly updates the display

protected:
	uint8_t *       dataptr;
	uint32_t        bytes_remaining;

};



#endif /* MONOLCD_BB_H_ */
