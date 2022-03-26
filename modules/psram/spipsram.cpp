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

#include "clockcnt.h"
#include "spipsram.h"
//#include "traces.h"

bool TSpiPsram::Init()
{
  initialized = false;

  if (qspi && qspi->initialized)
  {
    // using qspi
    if (2 == qspi->multi_line_count)  qspi->multi_line_count = 1;  // dual is not supported.
    effective_speed = qspi->speed * qspi->multi_line_count;
  }
  else if (spi && spi->initialized)
  {
    // using spi
    effective_speed = spi->speed;
  }
  else
  {
    return false;
  }

#if 1
  delay_us(150);

  if (qspi && (qspi->multi_line_count == 4))
  {
    qpimode = true;
    ResetChip();
    qpimode = false;
  }
  ResetChip();

  delay_us(5);
#endif

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

  pagemask = pagesize - 1;

  // process ID code

  bytesize = ((2048*1024) << ((idcode >> 21) & 7));

  // calculate max chunk size from the maximum CE low time
  unsigned bits_pro_cs_low = ((effective_speed / 1000) * t_cem_ns) / 1000000;
  int max_bytes_pro_cs = (bits_pro_cs_low >> 3) - 4;
  if (max_bytes_pro_cs < 4)  max_bytes_pro_cs = 4;

  maxchunksize = pagesize;
  while (maxchunksize > max_bytes_pro_cs)
  {
    maxchunksize >>= 1;
  }

  //EnterQpiMode();  // only when possible

  initialized = true;
  phase = 0;

  return true;
}

bool TSpiPsram::AddTransaction(TPsramTra * atra)
{
  if (curtra)
  {
    // search the last tra
    TPsramTra * tra = curtra;
    while (tra->next)
    {
      if (tra == atra)
      {
        break;
      }
      tra = tra->next;
    }

    if (tra == atra) // already added
    {
      return false; // already added
    }

    tra->next = atra;
  }
  else
  {
    // set as first
    curtra = atra;
  }

  atra->completed = false;
  atra->error = 0;
  atra->next = nullptr;

  return true;
}

void TSpiPsram::StartReadMem(TPsramTra * atra, unsigned aaddr, void * adstptr, unsigned alen)
{
  if (!AddTransaction(atra))
  {
    // application error: already added
    return;
  }

  atra->iswrite = 0;
  atra->address = aaddr;
  atra->dataptr = (uint8_t *)adstptr;
  atra->datalen = alen;

  Run();
}

void TSpiPsram::StartWriteMem(TPsramTra * atra, unsigned aaddr, void * asrcptr, unsigned alen)
{
  if (!AddTransaction(atra))
  {
    // application error: already added
    return;
  }

  atra->iswrite = 1;
  atra->address = aaddr;
  atra->dataptr = (uint8_t *)asrcptr;
  atra->datalen = alen;

  Run();
}

void TSpiPsram::WaitFinish(TPsramTra * atra)
{
  while (!atra->completed)
  {
    Run();
  }
}

void TSpiPsram::RunTransaction()
{
  if (!curtra->completed && phase)
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
      curtra->error = HWERR_NOTINIT;
      curtra->completed = true; // transaction finished.
      return;
    }
  }

  if (0 == phase)  // transaction start
  {
    istx = (0 != curtra->iswrite);
    address = curtra->address;
    dataptr = (uint8_t *)curtra->dataptr;
    datalen = curtra->datalen;
    remaining = datalen;
    phase = 10;
  }

  switch (phase)
  {
  case 10: // read next chunk
    if (remaining == 0)
    {
      // finished.
      curtra->completed = true;
      return;
    }

    // start data phase

    chunksize = pagesize - (address & pagemask); // page size
    if (chunksize > maxchunksize)  chunksize = maxchunksize;
    if (chunksize > remaining)     chunksize = remaining;

    if (istx)
    {
      CmdWrite();
    }
    else
    {
      CmdRead();
    }
    phase = 11;
    break;

  case 11:  // wait for completion
    if (!CmdFinished())
    {
      // TODO: timeout handling
      return;
    }

    // read next chunk
    address   += chunksize;
    dataptr   += chunksize;
    remaining -= chunksize;
    phase = 10; RunTransaction();  // phase jump
    return;
  }
}

void TSpiPsram::Run()
{
  if (!curtra)
  {
    return;
  }

  RunTransaction();

  if (curtra->completed)
  {
    // the callback function might add the same transaction object as new
    // therefore we have to remove the transaction from the chain before we call the callback
    TPsramTra * ptra = curtra; // save the transaction pointer for the callback

    curtra->completed = true;
    curtra = curtra->next; // advance to the next transaction
    phase = 0;

    // call the callback
    PCbClassCallback pcallback = PCbClassCallback(ptra->callback);
    if (pcallback)
    {
      TCbClass * obj = (TCbClass *)(ptra->callbackobj);
      (obj->*pcallback)(ptra->callbackarg);
    }
  }
  else
  {
    // operate the SPI as soon as possible:
    if (qspi)
    {
      qspi->Run();
    }
    else if (spi)
    {
      spi->Run();
    }
  }
}

bool TSpiPsram::ReadIdCode()
{
  // the read ID code command is different from the SPI FLASH: requires address, and the result is longer
  if (qspi)
  {
    qspi->StartReadData(0x9F | QSPICM_ADDR3, 0, &rxbuf[0], 8);
    qspi->WaitFinish();
  }
  else
  {
    spi->StartTransfer(0x9F, 0, SPITR_CMD1 | SPITR_ADDR3, 8, nullptr, &rxbuf[0]);
    spi->WaitFinish();
  }

  idcode = *(unsigned *)&(rxbuf[0]);
  idcode2 = *(unsigned *)&(rxbuf[4]);
  if ((idcode == 0) || (idcode == 0xFFFFFFFF))
  {
    // SPI communication error
    return false;
  }

  return true;
}

void TSpiPsram::ResetChip()
{
  if (qspi)
  {
    if (qpimode)
    {
      qspi->StartWriteData(0x66 | QSPICM_MMM, 0, nullptr, 0);
      qspi->WaitFinish();

      qspi->StartWriteData(0x99 | QSPICM_MMM, 0, nullptr, 0);
      qspi->WaitFinish();
    }
    else
    {
      qspi->StartWriteData(0x66, 0, nullptr, 0);
      qspi->WaitFinish();

      qspi->StartWriteData(0x99, 0, nullptr, 0);
      qspi->WaitFinish();
    }
  }
  else
  {
    spi->StartTransfer(0x66, 0, SPITR_CMD1, 0, nullptr, nullptr);
    spi->WaitFinish();

    spi->StartTransfer(0x99, 0, SPITR_CMD1, 0, nullptr, nullptr);
    spi->WaitFinish();
  }
}

void TSpiPsram::EnterQpiMode()
{
  if (!qspi || (qspi->multi_line_count != 4))
  {
    qpimode = false;
    return;
  }

  qspi->StartWriteData(0x35, 0, nullptr, 0);
  qspi->WaitFinish();
  qpimode = true;
}

void TSpiPsram::ExitQpiMode()
{
  if (!qspi || (qspi->multi_line_count != 4))
  {
    qpimode = false;
    return;
  }

  qspi->StartWriteData(0xF5 | QSPICM_MMM, 0, nullptr, 0);
  qspi->WaitFinish();
  qpimode = false;
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
    if (qpimode)
    {
      qspi->StartReadData(0xEB | QSPICM_MMM | QSPICM_ADDR | QSPICM_DUMMY2, address, dataptr, chunksize);
    }
    else if (qspi->multi_line_count == 4)
    {
      qspi->StartReadData(0xEB | QSPICM_SMM | QSPICM_ADDR | QSPICM_DUMMY3, address, dataptr, chunksize);
    }
    else
    {
      qspi->StartReadData(0x0B | QSPICM_SSS | QSPICM_ADDR | QSPICM_DUMMY1, address, dataptr, chunksize);
    }
  }
  else
  {
    spi->StartTransfer(0x0B, address, SPITR_CMD1 | SPITR_ADDR3 | SPITR_EXTRA1, datalen, nullptr, dataptr);
  }
}

void TSpiPsram::CmdWrite()
{
  if (qspi)
  {
    if (qpimode)
    {
      qspi->StartReadData(0x02 | QSPICM_MMM | QSPICM_ADDR, address, dataptr, chunksize);
    }
    else if (qspi->multi_line_count == 4)
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

