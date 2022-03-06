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
// file:     monolcd.h
// brief:    Monochrome LCD control base class
// created:  2022-03-06
// authors:  nvitya

#ifndef SRC_MONOLCD_H_
#define SRC_MONOLCD_H_

#include "platform.h"
#include "gfxbase.h"

typedef enum
{
	MLCD_CTRL_UNKNOWN = 0,
	MLCD_CTRL_UC1701,
	MLCD_CTRL_PCD8544, // = Nokia 5110
	MLCD_CTRL_HX1230,
	MLCD_CTRL_ST75256
//
} TMonoLcdCtrlType;

#define MLCD_CTRL_NOKIA5110   MLCD_CTRL_PCD8544

class TMonoLcd : public TGfxBase
{
public:
	bool              initialized = false;

	TMonoLcdCtrlType  ctrltype = MLCD_CTRL_UNKNOWN;
	bool              mirrorx = false;

	uint32_t          updatecnt = 0; // for screen update control
	uint32_t          lastupdate = 0x12345678;

	int               updatestate = 0;

	uint8_t    			  rotation = 0;
	uint8_t           contrast = 0x8F;
	bool              externalvcc = false;

	uint16_t          aw_x = 0;
	uint16_t          aw_y = 0;

	uint8_t *         pdispbuf;

	virtual ~TMonoLcd() {} // to avoid warning

public:
	// interface dependent

	virtual bool InitInterface();

	//virtual void ResetPanel();

	virtual void WriteCmd(uint8_t adata);
	virtual void WriteData(uint8_t adata);

public:
	// interface independent functions

	bool Init(TMonoLcdCtrlType atype, uint16_t awidth, uint16_t aheight, uint8_t * adispbuf);

	void InitPanel();
	void SetDisplayOn(bool aon);

	void SetRotation(uint8_t m);

	virtual void FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	virtual void DrawPixel(int16_t x, int16_t y, uint16_t color);

	virtual void SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t w,  uint16_t h);
	virtual void SetAddrWindowStart(uint16_t x0, uint16_t y0);
	virtual void FillColor(uint16_t acolor, unsigned acount);

	bool UpdateFinished();
	virtual void Run();
};

#endif /* SRC_MONOLCD_H_ */
