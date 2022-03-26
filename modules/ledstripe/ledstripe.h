/* -----------------------------------------------------------------------------
 * This file is a part of the NVCM Tests project: https://github.com/nvitya/nvcmtests
 * Copyright (c) 2018 Viktor Nagy, nvitya
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
 *  file:     ledstripe.h
 *  brief:    WS2813 / APA 102 led stripe handler
 *  date:     2022-03-26
 *  authors:  nvitya
*/

#ifndef SRC_LEDSTRIPE_H_
#define SRC_LEDSTRIPE_H_

#include "platform.h"
#include "hwspi.h"

class TLedStripe_base
{
public:
  uint32_t       ledcount = 1;
  uint32_t *     dmabuf32 = nullptr;
  uint8_t *      dmabuf8  = nullptr;
  THwSpi *       spi = nullptr;
  uint32_t       spisendlen = 0;

  uint32_t       updatecount = 0;

  uint32_t       bufsize = 0;

public:
  void           Init(THwSpi * aspi, uint32_t aledcount, uint8_t * admabuf, unsigned abufsize);

  void           Run();
  void           Update();
  bool           UpdateFinished();

protected:
  uint32_t       prev_updatecount = 0;
};

class TLedStripe_apa102 : public TLedStripe_base
{
private:
  typedef TLedStripe_base super;
public:

  uint8_t        intensity = 31;  // global intensity

public:
	void           Init(THwSpi * aspi, uint32_t aledcount, uint8_t * admabuf, unsigned abufsize);

	inline void    SetRGB(uint16_t aled, uint8_t r, uint8_t g, uint8_t b)
	{
		dmabuf32[aled + 1] = ((intensity >> 3) | 0xE0) | (b << 8) | (g << 16) | (r << 24);
	}

};

class TLedStripe_ws2813 : public TLedStripe_base
{
private:
  typedef TLedStripe_base super;

public:
  unsigned       resetbytes = 30; // both head and tail

  void           Init(THwSpi * aspi, uint32_t aledcount, uint8_t * admabuf, unsigned abufsize);

  void           SetRGB(uint16_t aled, uint8_t r, uint8_t g, uint8_t b); // more complex
};

#endif /* SRC_LEDSTRIPE_H_ */
