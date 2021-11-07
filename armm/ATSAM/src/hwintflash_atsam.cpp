/* -----------------------------------------------------------------------------
 * This file is a part of the NVCM project: https://github.com/nvitya/nvcm
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
 *  file:     hwintflash_atsam.h
 *  brief:    Internal Flash Handling for ATSAM
 *  version:  1.00
 *  date:     2019-04-07
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>

#include "platform.h"

#include "hwintflash_atsam.h"
#include "hwintflash.h"

#include "traces.h"

#if   defined(MCUSF_3X)
  #define HWINTFLASH_EBMUL    1
  #define HWINTFLASH_EBSHIFT  0
  #define HWINTFLASH_ADDRESS  0x00080000
#elif defined(MCUSF_E70)
  #define HWINTFLASH_EBMUL   16
  #define HWINTFLASH_EBSHIFT  4
  #define HWINTFLASH_ADDRESS  0x00400000
#else
  #define HWINTFLASH_EBMUL    8
  #define HWINTFLASH_EBSHIFT  3
  #define HWINTFLASH_ADDRESS  0x00400000
#endif

uint32_t THwIntFlash_atsam::EraseSize(uint32_t aaddress)
{
  return (pagesize << HWINTFLASH_EBSHIFT);
}

bool THwIntFlash_atsam::HwInit()
{
	int i;

#ifdef EFC
	regs = EFC;
#else
	regs = EFC0;
#endif

	// first get the Flash Descriptor

	regs->EEFC_FCR = 0
		| (0    <<  0)  // FCMD(8): 0 = get flash descriptor
		| (0    <<  8)  // FARG(16): no arguments required
		| (0x5A << 24)  // FKEY(8): must be 5A
	;

	while (!CmdFinished())
	{
		// wait
	}

	uint32_t fdesc[16];

	for (i = 0; i < 16; ++i)
	{
		fdesc[i] = regs->EEFC_FRR;
	}

	bytesize  = fdesc[1];
	pagesize  = fdesc[2];

	ctrl_bytesize = bytesize;

	// fix parameters:
	smallest_write = 8;  // ATSAM3X
	start_address = HWINTFLASH_ADDRESS;

#ifdef EFC1
	ctrl_count = 2;
	bytesize = (bytesize << 1);
#else
	ctrl_count = 1;
#endif

  ctrl2_addr = start_address + ctrl_bytesize;

	return true;
}

#if defined(MCUSF_3X)

void THwIntFlash_atsam::CmdEraseBlock()
{
  CmdClearPageBuffer();

  uint32_t staddr;
  if (address >= ctrl2_addr)
  {
    regs = EFC1;
    staddr = ctrl2_addr;
  }
  else
  {
    regs = EFC0;
    staddr = start_address;
  }
  uint32_t page = (address - staddr) / pagesize;

  regs->EEFC_FCR = 0
    | (3     <<  0)  // FCMD(8): 3 = Erase Page and write page
    | (page  <<  8)  // FARG(16): page number
    | (0x5A  << 24)  // FKEY(8): must be 5A
  ;
}

void THwIntFlash_atsam::CmdWritePage()
{
  uint32_t staddr;
  if (address >= ctrl2_addr)
  {
    regs = EFC1;
    staddr = ctrl2_addr;
  }
  else
  {
    regs = EFC0;
    staddr = start_address;
  }
  uint32_t page = (address - staddr) / pagesize;

  regs->EEFC_FCR = 0
    | (1     <<  0)  // FCMD(8): 1 = Write Page
    | (page  <<  8)  // FARG(16): page number
    | (0x5A  << 24)  // FKEY(8): must be 5A
  ;
}

void THwIntFlash_atsam::SelectController(uint32_t aaddress)
{
  if (aaddress >= ctrl2_addr)
  {
    regs = EFC1;
  }
  else
  {
    regs = EFC0;
  }
}

#else // E70, 4S

void THwIntFlash_atsam::CmdEraseBlock()
{
  uint32_t ebnum = (address - start_address) / EraseSize(address);

  #if HWINTFLASH_EBMUL == 8

    regs->EEFC_FCR = 0
      | (7     <<  0)  // FCMD(8): 7 = Erase Pages
      | (1     <<  8)  // FARG(0..1): 1 = 8x PAGES
      | (0     << 10)  // FARG(2): not used
      | (ebnum << 11)  // FARG(3..15): page number
      | (0x5A  << 24)  // FKEY(8): must be 5A
    ;

  #else

    regs->EEFC_FCR = 0
      | (7     <<  0)  // FCMD(8): 7 = Erase Pages
      | (2     <<  8)  // FARG(0..1): 2 = 16x PAGES (8k)
      | (0     << 10)  // FARG(2..3): not used
      | (ebnum << 12)  // FARG(4..15): page number
      | (0x5A  << 24)  // FKEY(8): must be 5A
    ;

  #endif
}

void THwIntFlash_atsam::CmdWritePage()
{
	uint32_t page = (address - start_address) / pagesize;

	regs->EEFC_FCR = 0
		| (1     <<  0)  // FCMD(8): 1 = Write Page
		| (page  <<  8)  // FARG(16): page number
		| (0x5A  << 24)  // FKEY(8): must be 5A
	;
}

#endif

void THwIntFlash_atsam::CmdClearPageBuffer()
{
  // there is no such command on this MCU, so it has to be done manually

  uint32_t baseaddr = (address & ~pagemask);

  uint32_t * dptr = (uint32_t *)baseaddr;
  uint32_t * endptr = dptr + (pagesize >> 2);

  while (dptr < endptr)
  {
    *dptr++ = 0xFFFFFFFF;
  }
}


