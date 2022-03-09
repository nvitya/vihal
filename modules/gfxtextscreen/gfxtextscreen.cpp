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
 *  file:     gfxtextscreen.cpp
 *  brief:    Kind of text console for graphical screens
 *  version:  1.00
 *  date:     2019-03-24
 *  authors:  nvitya
*/

#include <gfxtextscreen.h>

void TGfxTextScreen::InitTextGfx(TGfxBase * adisp, uint16_t x, uint16_t y, uint16_t w, uint16_t h, TGfxFont * amonofont)
{
	// the buffers must be already initialized !

	if (!screenbuf || (buf_size == 0))
	{
		return;
	}

	disp = adisp;

	disp_x = x;
	disp_y = y;
	disp_w = w;
	disp_h = h;

	font = amonofont;

	lineheight = font->y_advance;
	charwidth = font->CharWidth('W');

	cols = disp_w / charwidth;
	rows = disp_h / lineheight;

	// smaller buffer was given, reduce the cols / rows
	while (cols * rows > buf_size)
	{
		if (cols > rows)
		{
			--cols;
		}
		else
		{
			--rows;
		}
	}

	// initial clear
	disp->FillRect(disp_x, disp_y, disp_w, disp_h, disp->bgcolor);
}

void TGfxTextScreen::DrawChar(unsigned acol, unsigned arow, char ach)
{
	unsigned saddr = AddrFromColRow(acol, arow);
	disp->SetCursor(disp_x + acol * charwidth, disp_y + lineheight * arow);
	TGfxGlyph * glyph = font->GetGlyph(screenbuf[saddr]);
	if (!glyph)  glyph = font->GetGlyph('.');  // replace non-existing characters with dot

	uint16_t orig_bgcolor = disp->bgcolor;
	uint16_t orig_color = disp->color;

	if (cursor_on && (acol == cursor_x) && (arow == cursor_y))
	{
		disp->bgcolor = orig_color;
		disp->color   = orig_bgcolor;
	}

	disp->DrawGlyph(font, glyph);

	disp->color   = orig_color;
	disp->bgcolor = orig_bgcolor;
}

void TGfxTextScreen::SetCursor()
{
	if ((cursor_x != cursor_prev_x) || (cursor_y != cursor_prev_y))
	{
		unsigned addr;

		addr = AddrFromColRow(cursor_prev_x, cursor_prev_y);
		changemap[addr >> 5] |= (1 << (addr & 31));

		addr = AddrFromColRow(cursor_x, cursor_y);
		changemap[addr >> 5] |= (1 << (addr & 31));

		screenchanged = true;

		cursor_prev_x = cursor_x;
		cursor_prev_y = cursor_y;
	}
}
