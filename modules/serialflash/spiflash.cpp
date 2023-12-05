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
//#include "traces.h"

bool TSpiFlash::Init()
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

  // bit size is specially coded !
  unsigned sizeshift = ((idcode >> 16) & 0xFF);
  if (sizeshift >= 0x20)  sizeshift -= 6;

  bytesize = (1 << sizeshift);

  if (qspi && (4 == qspi->multi_line_count))
  {
    // The Quad mode must be enabled (switch HOLD and WP pins to IO2 and IO3)
    // This is done usually with status register write but the bit number is manufacturer specific

    unsigned qenable = 0;
    unsigned qenable_len = 0;
    uint8_t  qenable_cmd = 0x01; // write register

    unsigned char mnfid = (idcode & 0xFF);

    if ((0x9D == mnfid) || (0xC2 == mnfid))
    {
      // 0x9D = ISSI
      // 0xC2 = MXIC / Macronix
      // Quad enable is status bit 6
      qenable = 0x40;
      qenable_len = 1;
    }
    else if (0xEF == mnfid) // winbond
    {
      // Quad enable is status register bit 9
      qenable = 0x200;
      qenable_len = 2;
    }
    else if (0x20 == mnfid) // micron
    {
      qenable_cmd = 0x61; // enhanced volatile configuration register
      qenable_len = 1;
      qenable     = 0xF7; // bit4: 0 = disable RESET and HOLD, 1 = enable reset and hold
    }
    else
    {
      // unknown SPI chip hardware
      qspi->multi_line_count = 2; // falling back to dual mode
    }

    if (qenable_len)
    {
      CmdWriteEnable();
      qspi->WaitFinish();

      qspi->StartWriteData(qenable_cmd, 0, &qenable, qenable_len);
      qspi->WaitFinish();

      if (0x01 == qenable_cmd)
      {
        do
        {
          CmdReadStatus();
          qspi->WaitFinish();
        }
        while (rxbuf[0] & 1);
      }
    }
  }

  erasemask = (has4kerase ? 0x0FFF : 0xFFFF);

  initialized = true;

  return true;
}

bool TSpiFlash::StartReadMem(unsigned aaddr, void * adstptr, unsigned alen)
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

  state = SERIALFLASH_STATE_READMEM; // read memory
  phase = 0;

  errorcode = 0;
  completed = false;

  Run();

  return (errorcode == 0);
}

bool TSpiFlash::StartWriteMem(unsigned aaddr, void* asrcptr, unsigned alen)
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

  state = SERIALFLASH_STATE_WRITEMEM; // write page
  phase = 0;

  errorcode = 0;
  completed = false;

  Run();

  return (errorcode == 0);
}

bool TSpiFlash::StartEraseMem(unsigned aaddr, unsigned alen)
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

  dataptr = nullptr;
  datalen = alen;
  address = aaddr;

  state = SERIALFLASH_STATE_ERASE; // erase memory
  phase = 0;

  errorcode = 0;
  completed = false;

  Run();

  return (errorcode == 0);
}

void TSpiFlash::WaitForComplete()
{
  while (!completed)
  {
    Run();
  }
}

bool TSpiFlash::ReadIdCode()
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

void TSpiFlash::Run()
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

	if (SERIALFLASH_STATE_READMEM == state)  // read memory
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

        chunksize = maxchunksize;
        if (qspi && (chunksize > HW_DMA_MAX_COUNT))  chunksize = HW_DMA_MAX_COUNT;
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

				CmdWriteEnable();
				++phase;
				break;

			case 2: // write data
        if (!CmdFinished())
				{
					return;
				}

        chunksize = 256 - (address & 0xFF); // page size
        if (chunksize > remaining)  chunksize = remaining;

        CmdProgramPage();
				++phase;
				break;

			case 3: // wait for page write completition
        if (CmdFinished())
				{
					++phase;  Run();  // phase jump
					return;
				}
				break;

			case 4: // read status register, repeat until BUSY flag is set
				CmdReadStatus();
				++phase;
				break;

			case 5:
        if (!CmdFinished())
				{
					return;
				}

				if (rxbuf[0] & 1) // busy
				{
					// repeat status register read
					CmdReadStatus();
					return;
				}
				// Write finished.
				address += chunksize;
				dataptr += chunksize;
				remaining -= chunksize;
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
				CmdWriteEnable();
				++phase;
				break;

			case 2: // erase sector / block
        if (!CmdFinished()) // write enable finished ?
				{
					return;
				}

        CmdEraseBlock();  // chunksize will be set
				++phase;
				break;

			case 3: // wait for erase command completition
        if (!CmdFinished())
				{
					return;
				}

			  // read status register, repeat until BUSY flag is set
				CmdReadStatus();
				++phase;
				break;

			case 4:
        if (!CmdFinished())
				{
					return;
				}

				if (rxbuf[0] & 1) // busy
				{
					// repeat status register read
					// TODO: timeout
					CmdReadStatus();
					return;
				}

				// erase finished.
				if (chunksize > remaining)  chunksize = remaining;
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


bool TSpiFlash::CmdReadStatus()
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

bool TSpiFlash::CmdWriteEnable()
{
  if (qspi)
  {
    qspi->StartWriteData(0x06, 0, nullptr, 0);
  }
  else
  {
    spi->StartTransfer(0x06, 0, SPITR_CMD1, 0, nullptr, nullptr);
  }
  return true;
}


void TSpiFlash::ResetChip()
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

bool TSpiFlash::CmdFinished()
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

void TSpiFlash::CmdRead()
{
  if (qspi)
  {
    if (qspi->multi_line_count == 4)
    {
      // 0x6B is safer
      qspi->StartReadData(0x6B | QSPICM_SSM | QSPICM_ADDR | QSPICM_DUMMY, address, dataptr, chunksize);

      // the 0xEB does not work sometime, requires mode implementation
      //qspi.StartReadData(0xEB | QSPICM_SMM | QSPICM_ADDR3 | QSPICM_MODE1 | QSPICM_DUMMY2, address, dataptr, chunksize);
    }
    else if (qspi->multi_line_count == 2)
    {
      // 0x3B is safer
      qspi->StartReadData(0x3B | QSPICM_SSM | QSPICM_ADDR | QSPICM_DUMMY, address, dataptr, chunksize);

      // 0xBB does not work sometime, requires mode impementation and enable quad command (?)
      //qspi.StartReadData(0xBB | QSPICM_SMM | QSPICM_ADDR | QSPICM_MODE1, address, dataptr, chunksize);
    }
    else
    {
      qspi->StartReadData(0x0B | QSPICM_SSS | QSPICM_ADDR | QSPICM_DUMMY, address, dataptr, chunksize);
    }
  }
  else
  {
    // normal read command up to 30 MHz, no dummy
    spi->StartTransfer(0x03, address, SPITR_CMD1 | SPITR_ADDR3, datalen, nullptr, dataptr);
  }
}

void TSpiFlash::CmdProgramPage()
{
  if (qspi)
  {
    if ( (qspi->multi_line_count == 4)
         && ((idcode & 0xFF) != 0xC2)
         && ((idcode & 0xFF) != 0x20)   // was extremely slow on micron
       )
    {
      qspi->StartWriteData(0x32 | QSPICM_SSM | QSPICM_ADDR, address, dataptr, chunksize);
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

void TSpiFlash::CmdEraseBlock()
{
  uint8_t cmd;
  if (has4kerase && ((address & 0xFFFF) || (datalen < 0x10000)))
  {
    // 4k sector erase
    chunksize = 0x01000;
    cmd = 0x20;
  }
  else
  {
    // 64k block erase
    chunksize = 0x10000;
    cmd = 0xD8;
  }

  //TRACE("ERASE: cmd=%02X  addr=%06X  chunk=%u\r\n", cmd, address, chunksize);

  if (qspi)
  {
    qspi->StartWriteData(cmd | QSPICM_SSS | QSPICM_ADDR, address, nullptr, 0);
  }
  else
  {
    // page / block erase command
    spi->StartTransfer(cmd, address, SPITR_CMD1 | SPITR_ADDR3, 0, nullptr, nullptr);
  }
}
