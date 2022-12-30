/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2022 Viktor Nagy, nvitya
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
 *  file:     hwqspi_esp.h
 *  brief:    Espressif QSPI driver (for SPI1 only)
 *  date:     2022-12-23
 *  authors:  nvitya
*/

#ifndef HWQSPI_ESP_H_
#define HWQSPI_ESP_H_

#include "platform.h"

#define HWQSPI_PRE_ONLY
#include "hwqspi.h"

class THwQspi_esp : public THwQspi_pre
{
public:
	esp_spi_mem_t *   mregs = nullptr;


	bool              Init();

	virtual bool      InitInterface(); // override

	int               StartReadData(unsigned acmd, unsigned address, void * dstptr, unsigned len);
	int               StartWriteData(unsigned acmd, unsigned address, void * srcptr, unsigned len);
	void              Run();

	unsigned          runstate = 0;

protected:
  volatile uint32_t *  pcmdreg = nullptr;
  volatile uint32_t *  pwregs = nullptr;  // pointer to the 16 data registers
  uint32_t             cmd_usr_bit = 0;

  uint32_t             mlqio = 0;
  uint32_t             mldio = 0;

  uint32_t             chunksize = 0;


  unsigned             basespeed = 0;

  void                 StartNextChunk();

};

#define HWQSPI_IMPL THwQspi_esp

#endif // def HWQSPI_ESP_H_
