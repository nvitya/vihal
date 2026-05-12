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
 *  file:     hwintflash_py32.cpp
 *  brief:    Internal Flash Handling for PY32
 *  date:     2026-05-12
 *  authors:  nvitya
 *  notes:    The PY32 allows rewriting of the same memory locations (into zeroes only)
*/

#include <stdio.h>
#include <stdarg.h>

#include "platform.h"

#include "hwintflash_py32.h"
#include "hwintflash.h"

#include "py32_utils.h"

#include "traces.h"

uint32_t THwIntFlash_py32::EraseSize(uint32_t aaddress)
{
  return (1 << fixblock_size_shift);
}

int THwIntFlash_py32::BlockIdFromAddress(uint32_t aaddress)
{
  if ((aaddress < start_address) || (aaddress >= start_address + bytesize))
  {
    return -1;
  }

  uint32_t fladdr = aaddress - start_address;

  return (fladdr >> fixblock_size_shift);
}


bool THwIntFlash_py32::HwInit()
{
	//stm32_start_internal_hs_rc_osc();  // required for Flash Writes

	regs = FLASH;

	bytesize = FLASH_SIZE;

	pagesize = 128; // used here as burst length
	bank_count = 1;

	regs->SR = 0xFFFF; // clear all error flags

  smallest_write = 4;
  fixblock_size_shift = 12;

	start_address = FLASH_BASE;

	return true;
}


void THwIntFlash_py32::Unlock()
{
	if (regs->CR & FLASH_CR_LOCK_Msk)
	{
		regs->KEYR = 0x45670123;
		regs->KEYR = 0xCDEF89AB;

		if (regs->CR & FLASH_CR_LOCK_Msk)
		{
			TRACE("Flash Unlock Failed !\r\n");
		}
	}
}

bool THwIntFlash_py32::CmdFinished()
{
	if (regs->SR & FLASH_SR_BSY)
	{
		return false;
	}

	if (regs->SR & FLASH_SR_EOP)
	{
		regs->SR = FLASH_SR_EOP; // clear EOP flag
	}

	regs->CR = 0; // clear all CMD flags

	return true;
}

void THwIntFlash_py32::CmdEraseBlock()
{

	regs->SR = FLASH_SR_EOP | FLASH_SR_WRPERR; // clear status flags

	regs->CR = FLASH_CR_SER | FLASH_CR_EOPIE; // prepare page erase

	uint32_t * pu32 = (uint32_t *)(address);
	*pu32 = 0xFFFFFFFF;  // writing to this sector starts the erase operation
}

void THwIntFlash_py32::CmdWritePage()
{
	regs->SR = FLASH_SR_EOP | FLASH_SR_WRPERR; // clear status flags
	regs->CR = FLASH_CR_PG | FLASH_CR_EOPIE; // prepare page program
}

void THwIntFlash_py32::Run()
{
	uint32_t n;

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

				Unlock();

				++phase;
				// no break!

			case 1: // prepare / continue
				ebchunk = EraseSize(address);
				if (remaining < ebchunk)  ebchunk = remaining;  // important for the uint remaining

				CmdEraseBlock();
				++phase;
				break;

			case 2: // check finished
				if (CmdFinished())
				{
					address += ebchunk;
					remaining -= ebchunk;
					if (remaining > 0)
					{
						phase = 1; Run(); return; // continue with the next page
					}
					else
					{
						completed = true;
						state = 0;
					}
				}
				break;
		}
	}
	else if (INTFLASH_STATE_WRITEMEM == state)
	{
		switch (phase)
		{
			case 0: // start

				Unlock();

				errorcode = 0;
				remaining = length;
				++phase;
				// no break!

			case 1: // prepare / continue
			{
				chunksize = pagesize;
				if (remaining < chunksize)  chunksize = remaining;

				CmdWritePage();  // initiate page writing

				uint32_t * endaddr = srcaddr + ((chunksize >> 2) - 1);  // the last word is written separately !
				while (srcaddr < endaddr)
				{
					*dstaddr++ = *srcaddr++;
				}

				// writing a word after setting the PGSTART triggers the actual page writing
				regs->CR |= FLASH_CR_PGSTRT;
				*dstaddr++ = *srcaddr++;  // writing the last word

				++phase;
				break;
			}

			case 2:
				if (CmdFinished()) // wait the write to be ready
				{
					address += chunksize;
					remaining -= chunksize;
					if (remaining > 0)
					{
						phase = 1; Run(); return; // continue with the next page
					}
					else
					{
						completed = true;
						state = 0;
					}
				}
				break;
		}
	}
	else if (INTFLASH_STATE_COPY == state)  // with automatic erase, block alignment is mandatory
	{
		switch (phase)
		{
			case 0: // start

				Unlock();

				errorcode = 0;
				remaining = length;
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
				bool erase_required = false;
				bool match = true;
				while (sptr < endaddr)
				{
					uint32_t sv = *sptr++;
					uint32_t dv = *dptr++;

					if (sv != dv)
					{
						match = false;
						if ((sv & dv) != sv)  // if the flash content has elsewhere zeroes as the source
						{
							erase_required = true;
							break;
						}
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
				else if (erase_required)
				{
					//TRACE("Flash content erase required at %08X\r\n", address);
					CmdEraseBlock();
					phase = 4;
				}
				else
				{
					//TRACE("Flash content overwrite at %08X\r\n", address);
					phase = 6; Run(); return; // jump to write
				}
				break;
			}

			case 4: // wait the erase to finish
				if (CmdFinished())
				{
					phase = 6; Run(); return; // jump to write
				}
				break;

			case 6: // start / continue write
			{
				chunksize = pagesize;
				if (ebremaining < chunksize)  chunksize = ebremaining;

				CmdWritePage();  // initiate page writing

				uint32_t * endaddr = srcaddr + ((chunksize >> 2) - 1);  // the last word is written separately !
				while (srcaddr < endaddr)
				{
					*dstaddr++ = *srcaddr++;
				}

				// writing a word after setting the PGSTART triggers the actual page writing
				regs->CR |= FLASH_CR_PGSTRT;
				*dstaddr++ = *srcaddr++;  // writing the last word

				++phase;
				break;
			}

			case 7:
				if (CmdFinished()) // wait the write to be ready
				{
					address += chunksize;
					ebremaining -= chunksize;
					if (ebremaining > 0)
					{
						phase = 6; Run(); return; // continue with the next page
					}
					else
					{
						phase = 10; Run(); return;
					}
				}
				break;

			case 10: // block finished
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
	else // invalid state
	{
		completed = true;
		errorcode = 0;
		state = 0;
	}
}

