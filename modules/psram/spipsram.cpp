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
// file:     spipsram.h
// brief:    PSRAM Memory Handling with SPI/QSPI
// created:  2022-03-23
// authors:  nvitya

#include "spipsram.h"
//#include "traces.h"

bool TSpiPsram::Init()
{
  initialized = false;

  if (qspi && qspi->initialized)
  {
    // using qspi
  }
  else if (spi && spi->initialized)
  {
    // using spi
  }
  else
  {
    return false;
  }

  // read ID value
  ReadIdCode(); // might be invalid.

  unsigned trycnt = 0;
  while (!ReadIdCode())
  {
    ++trycnt;
    if (trycnt >= 3)
    {
      return false;
    }
  }

  // process ID code

  if ((idcode == 0) || (idcode == 0x00FFFFFF))
  {
    // SPI communication error
    return false;
  }

  bytesize = (1 << (idcode >> 16));

  initialized = true;

  return true;
}

bool TSpiPsram::StartReadMem(unsigned aaddr, void * adstptr, unsigned alen)
{
  if (!initialized)
  {
    errorcode = HWERR_NOTINIT;
    completed = true;
    return false;
  }

  if (!completed)
  {
    errorcode = HWERR_BUSY;  // this might be overwriten later
    return false;
  }

  dataptr = (uint8_t *)adstptr;
  datalen = alen;
  address = aaddr;

  state = PSRAM_STATE_READMEM; // read memory
  phase = 0;

  errorcode = 0;
  completed = false;

  Run();

  return (errorcode == 0);
}

bool TSpiPsram::StartWriteMem(unsigned aaddr, void* asrcptr, unsigned alen)
{
  if (!initialized)
  {
    errorcode = HWERR_NOTINIT;
    completed = true;
    return false;
  }

  if (!completed)
  {
    errorcode = HWERR_BUSY;  // this might be overwriten later
    return false;
  }

  dataptr = (uint8_t *)asrcptr;
  datalen = alen;
  address = aaddr;

  state = PSRAM_STATE_WRITEMEM; // write page
  phase = 0;

  errorcode = 0;
  completed = false;

  Run();

  return (errorcode == 0);
}

void TSpiPsram::WaitForComplete()
{
  while (!completed)
  {
    Run();
  }
}

bool TSpiPsram::ReadIdCode()
{
  if (qspi)
  {
    qspi->StartReadData(0x9F, 0, &rxbuf[0], 4);
    qspi->WaitFinish();
  }
  else
  {
    spi->StartTransfer(0x9F, 0, SPITR_CMD1, 4, nullptr, &rxbuf[0]);
    spi->WaitFinish();
  }

	idcode = (*(unsigned *)&(rxbuf[0]) & 0xFFFFFF);
	if ((idcode == 0) || (idcode == 0x00FFFFFF))
	{
		// SPI communication error
		return false;
	}

	return true;
}

void TSpiPsram::Run()
{
  if (qspi)
  {
    qspi->Run();
  }
  else if (spi)
  {
    spi->Run();
  }
  else
  {
    return;
  }

	if (0 == state)
	{
		// idle
		return;
	}

	if (PSRAM_STATE_READMEM == state)  // read memory
	{
    switch (phase)
    {
      case 0: // start
        remaining = datalen;
        phase = 2;
        // no break !

      case 2: // start read next data chunk
        if (remaining == 0)
        {
          // finished.
          phase = 10; Run();  // phase jump
          return;
        }

        // start data phase

        chunksize = pagesize - (address & pagemask); // page size
        if (chunksize > remaining)  chunksize = remaining;

        CmdRead();
        phase = 3;
        break;

      case 3:  // wait for completion
        if (CmdFinished())
        {
          // read next chunk
          address   += chunksize;
          dataptr   += chunksize;
          remaining -= chunksize;
          phase = 2; Run();  // phase jump
          return;
        }

        // TODO: timeout handling

        break;

      case 10: // finished
        completed = true;
        state = 0; // go to idle
        break;
    }
	}
	else if (PSRAM_STATE_WRITEMEM == state)  // write memory
	{
		switch (phase)
		{
			case 0: // initialization
				remaining = datalen;
				++phase;
				break;

			// write next chunk
			case 1: // start wirte next data chunk
				if (remaining == 0)
				{
					// finished.
					phase = 20;  Run();  // phase jump
					return;
				}

        chunksize = pagesize - (address & pagemask); // page size
        if (chunksize > remaining)  chunksize = remaining;

        CmdWrite();
				++phase;
				break;

			case 2:
        if (!CmdFinished())
				{
          // TODO: timeout
					return;
				}

				// Write finished.
				address += chunksize;
				dataptr += chunksize;
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


bool TSpiPsram::CmdReadStatus()
{
  rxbuf[0] = 0xFF;
  rxbuf[1] = 0xFF;
  rxbuf[2] = 0xFF;
  rxbuf[3] = 0xFF;

  if (qspi)
  {
    qspi->StartReadData(0x05, 0, &rxbuf[0], 4);  // some QSPI drivers support only 32 bit access
  }
  else
  {
    spi->StartTransfer(0x05, 0, SPITR_CMD1, 1, nullptr, &rxbuf[0]);
  }
	return true;
}

void TSpiPsram::ResetChip()
{
  if (qspi)
  {
    qspi->StartWriteData(0x66, 0, nullptr, 0);
    qspi->WaitFinish();

    qspi->StartWriteData(0x99, 0, nullptr, 0);
    qspi->WaitFinish();
  }
  else
  {
    spi->StartTransfer(0x66, 0, SPITR_CMD1, 0, nullptr, nullptr);
    spi->WaitFinish();

    spi->StartTransfer(0x99, 0, SPITR_CMD1, 0, nullptr, nullptr);
    spi->WaitFinish();
  }
}

bool TSpiPsram::CmdFinished()
{
  if (qspi)
  {
    return !qspi->busy;
  }
  else
  {
    return spi->finished;
  }
}

void TSpiPsram::CmdRead()
{
  if (qspi)
  {
    if (qspi->multi_line_count == 4)
    {
      // the 0xEB does not work sometime, requires mode implementation
      qspi->StartReadData(0xEB | QSPICM_SMM | QSPICM_ADDR3 | QSPICM_DUMMY3, address, dataptr, chunksize);
    }
    else
    {
      qspi->StartReadData(0x0B | QSPICM_SSS | QSPICM_ADDR | QSPICM_DUMMY1, address, dataptr, chunksize);
    }
  }
  else
  {
    // normal read command up to 30 MHz, no dummy
    spi->StartTransfer(0x03, address, SPITR_CMD1 | SPITR_ADDR3, datalen, nullptr, dataptr);
  }
}

void TSpiPsram::CmdWrite()
{
  if (qspi)
  {
    if ((qspi->multi_line_count == 4) && ((idcode & 0xFF) != 0xC2))
    {
      qspi->StartWriteData(0x38 | QSPICM_SMM | QSPICM_ADDR, address, dataptr, chunksize);
    }
    else
    {
      qspi->StartWriteData(0x02 | QSPICM_SSS | QSPICM_ADDR, address, dataptr, chunksize);
    }
  }
  else
  {
    spi->StartTransfer(0x02, address, SPITR_CMD1 | SPITR_ADDR3, chunksize, dataptr, nullptr);
  }
}

