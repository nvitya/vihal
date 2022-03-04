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
 *  file:     textscreen.h
 *  brief:    Kind of text console for graphical screens
 *  version:  1.00
 *  date:     2019-03-24
 *  authors:  nvitya
*/

#ifndef TEXTSCREEN_H_
#define TEXTSCREEN_H_

#include "stdint.h"
#include <stdarg.h>

class TTextScreen
{
protected:
	// row[0] is always at the bottom
	uint8_t *       screenbuf = nullptr;
	uint32_t *      changemap = nullptr;

	unsigned        buf_size = 0;

	void            SetScreenBufChar(unsigned aaddr, char ach);

public:
	bool            screenchanged = false; // this is useful for the applications

public:
	virtual ~TTextScreen() { }

	unsigned 				cols = 0;
	unsigned 				rows = 0;

	unsigned        cposx;
	unsigned        cposy;

	bool            cursor_on = true;
	unsigned        cursor_x = 0;
	unsigned        cursor_y = 0;

	void            InitCharBuf(unsigned acols, unsigned arows, uint8_t * ascreenbuf, uint8_t * achangemap);

	void            WriteChar(char ach);

	void            SetPos(unsigned acol, unsigned arow);

	void            printf(const char * fmt, ...);
	void            printf_va(const char * fmt, va_list arglist);

	void            Refresh();

	void            Clear();

	virtual void    Run();

	inline unsigned AddrFromColRow(unsigned acol, unsigned arow)  { return (arow * cols) + acol; }

public: // implemented in the child

	virtual void    DrawChar(unsigned acol, unsigned arow, char ach);
	virtual void    SetCursor();
};

#endif /* TEXTSCREEN_H_ */
