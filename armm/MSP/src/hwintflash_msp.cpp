/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2023 Viktor Nagy, nvitya
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
 *  file:     hwintflash_msp.cpp
 *  brief:    TI MSPM0G Internal Flash Driver
 *  date:     2023-12-13
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>

#include "platform.h"

#include "hwintflash.h"

#include "traces.h"

uint32_t THwIntFlash_msp::EraseSize(uint32_t aaddress)
{
  return 1024;
}

bool THwIntFlash_msp::HwInit()
{
	regs = &(FLASHCTL->GEN);

	bank_count = 1;
	smallest_write = 8;  // = flash word in the reference manual
	pagesize = 8;        // = no more than 8 bytes are supported at mspm0G3507

	bytesize = ((FACTORYREGION->SRAMFLASH & 0xFFF) << 10);  // size signalized in kBytes

	// fix parameters:
	start_address = 0x00000000;

	return true;
}

void THwIntFlash_msp::PrepareWriteProtection()
{
  // address, length are set
	uint32_t bstart = (address >> 10); // 1k alignment
	uint32_t bend   = ((address + length) >> 10); // 1k alignment
	volatile uint32_t * wpreg;
	uint32_t tmp;

	wprot_reg[0] = 0xFFFFFFFF;
	wprot_reg[1] = 0x00000FFF;  // for 128k Flash

	while (bstart <= bend)
	{
		if (bstart < 32)  // WPROT-A, 1k sector bits
		{
			wprot_reg[0] &= ~(1 << bstart);
			++bstart;
		}
		else // 8k groups
		{
			uint32_t bitidx = ((bstart - 32) >> 3);
			wprot_reg[1] &= ~(1 << bitidx);
			bstart += 8;
		}
	}

	ClearWriteProtection();
}

void THwIntFlash_msp::ClearWriteProtection()
{
	regs->CMDWEPROTA = wprot_reg[0];
	regs->CMDWEPROTB = wprot_reg[1];
}

__attribute__((section(".text_ITCRAM")))
void THwIntFlash_msp::CmdEraseBlock()
{
	ClearWriteProtection();

	regs->CMDTYPE = (0
		| (4  <<  4)  // SIZE(3): 0 = 1*64-bit, 1 = 2*64-bit, 2=4*64-bit, 3=8*64-bit, 4=16*64-bit / flash sector, 5 = the whole bank
		| (2  <<  0)  // COMMAND(3): 0 = noop, 1 = program, 2 = erase, 3 = read veryfy, 6 = blank verify
  );
	regs->CMDADDR = address;
	regs->CMDEXEC = 1;

	while (regs->STATCMD & (1 << 2))
	{
		// wait for finish
	}

	if (regs->STATCMD != 3)
	{
		//TRACE("Erase error: %08X\r\n", regs->STATCMD);
		__NOP(); // for breakpoint
	}
}

__attribute__((section(".text_ITCRAM")))
void THwIntFlash_msp::CmdWritePage(uint32_t wordcnt)
{
	ClearWriteProtection();

	if (wordcnt <  2)  wordcnt = 2;
	if (wordcnt > 32)  wordcnt = 32;

	uint32_t scode;
	if      (wordcnt >= 32)  scode = 4;
	else if (wordcnt >= 16)  scode = 3;
	else if (wordcnt >=  8)  scode = 2;
	else if (wordcnt >=  4)  scode = 1;
	else                     scode = 0;

	regs->CMDTYPE = (0
		| (scode <<  4)  // SIZE(3): 0 = 1*64-bit, 1 = 2*64-bit, 2=4*64-bit, 3=8*64-bit, 4=16*64-bit / flash sector, 5 = the whole bank
		| (1     <<  0)  // COMMAND(3): 0 = noop, 1 = program, 2 = erase, 3 = read veryfy, 6 = blank verify
  );
	regs->CMDADDR = address;

	// fill the data buffers from the dstaddr
	uint32_t * endaddr = srcaddr + wordcnt;
	uint32_t * rdst = (uint32_t *)&regs->CMDDATA0;
	while (srcaddr < endaddr)
	{
		*rdst++ = *srcaddr++;
	}

	dstaddr = (uint32_t *)(address + (wordcnt << 2)); // update the dstaddr

	regs->CMDBYTEN = 0x1FF;

	// start the command
	regs->CMDEXEC = 1;

	while (regs->STATCMD & (1 << 2))
	{
		// wait for finish
	}

	if (regs->STATCMD != 3)
	{
		//TRACE("Program page error: %08X\r\n", regs->STATCMD);
		__NOP(); // for breakpoint
	}
}

void THwIntFlash_msp::Run()
{
	if (0 == state)
	{
		return;  // idle
	}

	if (INTFLASH_STATE_ERASE == state)
	{
		switch (phase)
		{
			case 0: // start
				errorcode = 0;
				remaining = length;
				PrepareWriteProtection();
				++phase;
				// no break!

			case 1: // prepare / continue
				ebchunk = EraseSize(address);
				if (remaining < ebchunk)  ebchunk = remaining;  // important for the uint remaining

				CmdEraseBlock();

				address += ebchunk;
				remaining -= ebchunk;
				if (0 == remaining)
				{
					completed = true;
					state = 0;
				}
				break;
		}
	}
	else if (INTFLASH_STATE_WRITEMEM == state)
	{
		switch (phase)
		{
			case 0: // start
				errorcode = 0;
				remaining = length;
				PrepareWriteProtection();
				++phase;
				// no break!

			case 1: // continue
				chunksize = pagesize;
				chunksize -= (address & pagemask);
				if (remaining < chunksize)  chunksize = remaining;

				CmdWritePage(chunksize >> 2);  // increments the srcaddr, dstaddr internally

				address   += chunksize;
				remaining -= chunksize;
				if (0 == remaining)
				{
					completed = true;
					state = 0;
				}
				break;
		}
	}
	else if (INTFLASH_STATE_COPY == state)  // with automatic erase, block alignment is mandatory
	{
		switch (phase)
		{
			case 0: // start
				errorcode = 0;
				remaining = length;
				PrepareWriteProtection();
				++phase;
				// no break!

			case 1: // prepare / continue
			{
				ebchunk = EraseSize(address);
				if (remaining < ebchunk)  ebchunk = remaining;  // important for the uint remaining

				ebremaining = ebchunk;

				// compare contents for wrong 0 bits
				uint32_t * sptr = srcaddr;
				uint32_t * dptr = dstaddr;
				uint32_t * endaddr = srcaddr + (ebchunk >> 2);
				bool match = true;
				while (sptr < endaddr)
				{
					uint32_t sv = *sptr++;
					uint32_t dv = *dptr++;
					if (sv != dv)
					{
						match = false;
						break;
					}
				}

				if (match)
				{
					//TRACE("Flash content match at %08X\r\n", address);
					srcaddr += (ebchunk >> 2); // adjust addresses here, which normally incremented during write
					dstaddr += (ebchunk >> 2);
					address += ebchunk;
					phase = 10; // block finished
				}
				else // erase required
				{
					//TRACE("Flash content erase required at %08X\r\n", address);
					CmdEraseBlock();
					phase = 6; // continue at write
				}
				break;
			}

			case 6: // start / continue write
			{
				chunksize = pagesize;
				chunksize -= (address & pagemask);
				if (ebremaining < chunksize)  chunksize = ebremaining;

				CmdWritePage(chunksize >> 2);  // increments the srcaddr internally

				address     += chunksize;
				ebremaining -= chunksize;
				if (0 == ebremaining)
				{
					phase = 10;
				}
				break;
			}

			case 10: // block finished
			{
				remaining -= ebchunk;
				if (remaining > 0)
				{
					phase = 1; Run(); return; // continue with the block
				}
				else
				{
					completed = true;
					state = 0;
				}
				break;
			}
		}
	}
	else // invalid state
	{
		completed = true;
		errorcode = 0;
		state = 0;
	}
}
