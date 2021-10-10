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
// file:     spiflash.cpp
// brief:    SPI Flash Memory Implementation
// created:  2018-02-10
// authors:  nvitya

#include "spiflash.h"

bool TSpiFlash::InitInherited()
{
	if (!spi)
	{
		return false;
	}

	return true;
}

bool TSpiFlash::ReadIdCode()
{
	spi->StartTransfer(0x9F, 0, SPITR_CMD1, 3, nullptr, &rxbuf[0]);
	spi->WaitFinish();

	unsigned * up = (unsigned *)&(rxbuf[0]);
	idcode = (*up >> 8);

	if ((idcode == 0) || (idcode == 0x00FFFFFF))
	{
		// SPI communication error
		return false;
	}

	return true;
}

void TSpiFlash::Run()
{
	if (!spi)
	{
		return;
	}

	spi->Run();

	if (0 == state)
	{
		// idle
		return;
	}

	if (SERIALFLASH_STATE_READMEM == state)  // read memory
	{
		switch (phase)
		{
			case 0: // start
				// normal read command up to 30 MHz, no dummy
				spi->StartTransfer(0x03, address, SPITR_CMD1 | SPITR_ADDR3, datalen, nullptr, dataptr);
				phase = 1;
				break;
			case 1: // wait for complete
				if (spi->finished)
				{
					completed = true;
					state = 0;
				}
			  break;
		}
	}
	else if (SERIALFLASH_STATE_WRITEMEM == state)  // write memory
	{
		switch (phase)
		{
			case 0: // initialization
				remaining = datalen;
				++phase;
				break;

			// write next chunk
			case 1: // set write enable
				if (remaining == 0)
				{
					// finished.
					phase = 20;  Run();  // phase jump
					return;
				}

				StartWriteEnable();
				++phase;
				break;

			case 2: // write data
				if (!spi->finished)
				{
					return;
				}

				// program page command
				spi->StartTransfer(0x03, address, SPITR_CMD1 | SPITR_ADDR3, datalen, dataptr, nullptr);
				++phase;
				break;

			case 4: // wait for page write completition
				if (spi->finished)
				{
					++phase;  Run();  // phase jump
					return;
				}
				break;

			case 5: // read status register, repeat until BUSY flag is set
				StartReadStatus();
				++phase;
				break;

			case 6:
				if (!spi->finished)
				{
					return;
				}

				if (rxbuf[0] & 1) // busy
				{
					// repeat status register read
					StartReadStatus();
					return;
				}
				// Write finished.
				phase = 1; Run();  // phase jump

				// TODO: timeout
				break;

			case 20: // finished
				completed = true;
				state = 0; // go to idle
				break;
		}
	}
	else if (SERIALFLASH_STATE_ERASE == state)  // erase memory
	{
		switch (phase)
		{
			case 0: // initialization

				// correct start address if necessary
				if (address & erasemask)
				{
					// correct the length too
					datalen += (address & erasemask);
					address &= ~erasemask;
				}

				// round up to 4k the data length
				datalen = ((datalen + erasemask) & ~erasemask);

				remaining = datalen;
				++phase;
				break;

			case 1: // erase next sector
				if (remaining == 0)
				{
					// finished.
					phase = 20; Run();  // phase jump
					return;
				}

			  // set write enable first
				StartWriteEnable();
				++phase;
				break;

			case 2: // erase sector / block
				if (!spi->finished) // write enable finished ?
				{
					return;
				}

				if (has4kerase && ((address & 0xFFFF) || (datalen < 0x10000)))
				{
					// 4k sector erase
					chunksize = 0x01000;
					txbuf[0] = 0x20;
				}
				else
				{
					// 64k block erase
					chunksize = 0x10000;
					txbuf[0] = 0xD8;
				}

				// page / block erase command
				spi->StartTransfer(txbuf[0], address, SPITR_CMD1 | SPITR_ADDR3, 0, nullptr, nullptr);
				++phase;
				break;

			case 3: // wait for erase command completition
				if (!spi->finished)
				{
					return;
				}

			  // read status register, repeat until BUSY flag is set
				StartReadStatus();
				++phase;
				break;

			case 4:
				if (!spi->finished)
				{
					return;
				}

				if (rxbuf[0] & 1) // busy
				{
					// repeat status register read
					// TODO: timeout
					StartReadStatus();
					return;
				}

				// erase finished.
				address   += chunksize;
				remaining -= chunksize;
				phase = 1; Run();  // phase jump
				break;

			case 20: // finished
				completed = true;
				state = 0; // go to idle
				break;
		}
	}
}


bool TSpiFlash::StartReadStatus()
{
	spi->StartTransfer(0x05, 0, SPITR_CMD1, 1, nullptr, &rxbuf[0]);
	return true;
}

bool TSpiFlash::StartWriteEnable()
{
  // set write enable
  spi->StartTransfer(0x06, 0, SPITR_CMD1, 0, nullptr, nullptr);
  return true;
}


void TSpiFlash::ResetChip()
{
  spi->StartTransfer(0x66, 0, SPITR_CMD1, 0, nullptr, nullptr);
  spi->WaitFinish();

  spi->StartTransfer(0x99, 0, SPITR_CMD1, 0, nullptr, nullptr);
  spi->WaitFinish();
}
