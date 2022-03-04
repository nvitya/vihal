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
 *  file:     gfxtextscreen.h
 *  brief:    Kind of text console for graphical screens
 *  version:  1.00
 *  date:     2019-03-24
 *  authors:  nvitya
*/

#ifndef GFXTEXTSCREEN_H_
#define GFXTEXTSCREEN_H_

#include "gfxbase.h"
#include "textscreen.h"

class TGfxTextScreen : public TTextScreen
{
public:

	TGfxBase *	  	disp;
	TGfxFont *      font;

	uint16_t        disp_x;
	uint16_t        disp_y;
	uint16_t        disp_w;
	uint16_t        disp_h;

	uint8_t         charwidth;

	uint8_t         lineheight;

	virtual         ~TGfxTextScreen() { }

	void            InitTextGfx(TGfxBase * adisp, uint16_t x, uint16_t y, uint16_t w, uint16_t h, TGfxFont * amonofont);

public: // textscreen mandatory

	virtual void    DrawChar(unsigned acol, unsigned arow, char ach);
	virtual void    SetCursor();

protected:
	uint16_t        cursor_prev_x = 0;
	uint16_t        cursor_prev_y = 0;
	uint16_t        cursor_prev_on = 0;
	bool            cursor_blink_on = false;

};

#endif /* GFXTEXTSCREEN_H_ */
