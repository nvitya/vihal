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
// file:     sdcard_spi.h
// brief:    SDCARD access over SPI bus
// created:  2022-11-12
// authors:  nvitya

#include "string.h"
#include "hwclk.h"
#include <sdcard_spi.h>
#include "traces.h"

bool TSdCardSpi::Init(THwSpi * aspi)
{
  initialized = false;

  us_clocks = SystemCoreClock / 1000000;

  spi = aspi;
  if (!pin_cs)
  {
    pin_cs = spi->manualcspin;
  }

  if (!pin_cs)
  {
    return false;
  }

  spi->manualcspin = nullptr;  // do not let the SPI to handle the CS pin automatically !
  spi->datazero = 0xFFFFFFFF;  // send 0xFF for the RX only transactions !

  card_initialized = false;
  initstate = 0;

  initialized = true;
  return true;
}

void TSdCardSpi::Run()
{
  if (!initialized)
  {
    return;
  }

  spi->Run();
  if (!spi->finished)
  {
    return;
  }

  if (!card_initialized)
  {
    RunInitialization();
    return;
  }

  RunTransfer(); // run the transfer state machine
}

void TSdCardSpi::RunTransfer()
{
  int i;
  uint8_t  cmd;
  uint32_t cmdarg;

  switch (trstate)
  {
  case 0: // idle
    break;

  //------------------------------------------
  // READ
  //------------------------------------------

  case 1: // start read blocks
    cmdarg = startblock;
    if (!high_capacity)  cmdarg <<= 9; // byte addressing for low capacity cards

    remainingbytes = 512;
    blockcrc = 0;
    crcremaining = 2;  // wait extra 2 bytes at the end

    pin_cs->Set0();

    if (remainingblocks > 1)
    {
      cmd = 18; // read multiple blocks
      trstate = 30;
    }
    else
    {
      cmd = 17; // read single block only
      ++trstate;
    }

    CmdSend(cmd, cmdarg, cmdlen + 8);
    cmd_start_time = CLOCKCNT;

    break;

  case 2:  // analyze the first result block, cmd result + data maybe

    if (!FindResponseCode() || (rxbuf[rxidx] != 0))
    {
      FinishTransfer(HWERR_READ);
      break;
    }

    ++rxidx; // skip the response code

    if (!FindDataStart())
    {
      SpiStartRead(8);
      trstate = 3;
      break;
    }

    // there is some data already here
    trstate = 4; // jump to state 4 to process it
    RunTransfer();
    return;

  case 3: // waiting for data start
    if (!FindDataStart())
    {
      if (CLOCKCNT - cmd_start_time > us_clocks * 50000)
      {
        FinishTransfer(HWERR_TIMEOUT);
        return;
      }

      // try again
      SpiStartRead(8);
      trstate = 3;
      break;
    }

    trstate = 4; // jump to state 4 to process the data chunk
    RunTransfer();
    return;

  case 4: // reading data chunks
    CopyReadData();  // copy already received data

    spi->StartTransfer(0, 0, 0, remainingbytes, nullptr, dataptr);
    dataptr += remainingbytes;
    ++trstate;
    break;

  case 5: // add the crc
    SpiStartRead(4);  // and two additional bytes
    ++trstate;
    break;

  case 6:
    CopyCrc();

    // block read finished
    --remainingblocks;
    ++startblock;
    if (0 == remainingblocks)
    {
      FinishTransfer(0);
      break;
    }

    // continue reading the next block
    trstate = 1;
    RunTransfer();
    return;

  //------------------------------------------
  // read multiple blocks
  //------------------------------------------

  case 30: // analyze the first result block, cmd result + data maybe
    if (!FindResponseCode() || (rxbuf[rxidx] != 0))
    {
      FinishTransfer(HWERR_READ);
      break;
    }

    ++rxidx; // skip the response code

    if (!FindDataStart())
    {
      SpiStartRead(8);
      trstate = 33;
      break;
    }

    // there is some data already here
    trstate = 34; // jump to state 4 to process it
    RunTransfer();
    return;

  case 33: // waiting for data start
    if (!FindDataStart())
    {
      if (CLOCKCNT - cmd_start_time > us_clocks * 50000)
      {
        FinishTransfer(HWERR_TIMEOUT);
        return;
      }

      // try again
      SpiStartRead(8);
      trstate = 33;
      break;
    }

    trstate = 34; // jump to state 4 to process the data chunk
    RunTransfer();
    return;

  case 34: // reading data chunks
    CopyReadData();  // copy already received data

    spi->StartTransfer(0, 0, 0, remainingbytes, nullptr, dataptr);
    dataptr += remainingbytes;
    ++trstate;
    break;

  case 35: // read the crc + give some chance to data start
    SpiStartRead(16);
    ++trstate;
    break;

  case 36:
    CopyCrc();

    // block read finished
    --remainingblocks;
    if (remainingblocks)
    {
      remainingbytes = 512;
      crcremaining = 2;

      // jump to the state 33
      trstate = 33;
      RunTransfer();
      return;
    }

    CmdSend(12, 0, 16);  // send stop command
    ++trstate;
    break;

  case 37:
    if (!FindResponseCode())
    {
      FinishTransfer(HWERR_READ);
      break;
    }

    if (rxbuf[rxidx] != 0)
    {
      ++rxidx;
      if (rxbuf[rxidx] != 0)
      {
        FinishTransfer(HWERR_READ);
        break;
      }
    }

    FinishTransfer(0);
    return;


  //------------------------------------------
  // WRITE
  //------------------------------------------

  case 11: // start write blocks
    cmdarg = startblock;
    if (!high_capacity)  cmdarg <<= 9; // byte addressing for low capacity cards

    remainingbytes = 512;
    blockcrc = 0;
    crcremaining = 2;  // wait extra 2 bytes at the end

    pin_cs->Set0();

    if (remainingblocks > 1)
    {
      cmd = 25; // start multiple block write
      trstate = 20;
    }
    else
    {
      cmd = 24; // uses only single block transfer
      ++trstate;
    }

    CmdSend(cmd, cmdarg, 8);
    cmd_start_time = CLOCKCNT;
    break;

  case 12: // single sector write command response

    if (!FindResponseCode() || (rxbuf[rxidx] != 0))
    {
      FinishTransfer(HWERR_WRITE);
      break;
    }

    // send the sector data with the 0xFE data start token
    spi->StartTransfer(0xFE, 0, SPITR_CMD1, 512, dataptr, nullptr);
    ++trstate;
    break;

  case 13: // start polling write command acceptance
    SpiStartRead(8); // also sends 2x0xFF for the CRC for the first time
    ++trstate;
    break;

  case 14: // wait write acknowledge (0xE5)
    if (!FindToken(0xE5))
    {
      FinishTransfer(HWERR_WRITE);
      break;
    }

    ++trstate;
    // no break here, go to the next state !

  case 15:
    if (!FindNotBusy()) // wait until not 0x00 received
    {
      SpiStartRead(8);
      break;
    }

    FinishTransfer(0);
    break;

  //------------------------------------
  // write multiple
  //------------------------------------

  case 20: // send first data

    if (!FindResponseCode() || (rxbuf[rxidx] != 0))
    {
      FinishTransfer(HWERR_WRITE);
      break;
    }

    trycnt = 0;
    // send the sector with leading 0xFC marker
    spi->StartTransfer(0xFC, 0, SPITR_CMD1, 512, dataptr, nullptr);
    ++trstate;
    break;

  case 21: // send 2 CRC (0xFFFF) + start polling write command acceptance
    SpiStartRead(8);
    ++trstate;
    break;

  case 22: // wait write acknowledge (0xE5)
    if (!FindToken(0x05, 0x1F))
    {
      ++trycnt;
      if (trycnt > 8)
      {
        FinishTransfer(HWERR_WRITE);
        return;
      }
    }

    ++trstate;
    // no break here !

  case 23:
    if (!FindNotBusy()) // wait until not 0x00 received
    {
      SpiStartRead(8);
      break;
    }

    // continue with the next sector
    --remainingblocks;
    dataptr += 512;

    if (remainingblocks)
    {
      // send the next block with leading 0xFC marker
      spi->StartTransfer(0xFC, 0, SPITR_CMD1, 512, dataptr, nullptr);
      trstate = 21;
      return;
    }

    // send stop
    i = 0;
    txbuf[i++] = 0xFD;
    while (i < 16)  txbuf[i++] = 0xFF;
    spi->StartTransfer(0, 0, 0, i, &txbuf[0], &rxbuf[0]);
    trycnt = 0;
    trstate = 25;
    rxidx = 2;
    break;

  case 25:  // wait for ready
    if (!FindNotBusy()) // wait until not 0x00 received
    {
      SpiStartRead(8);
      break;
    }

    FinishTransfer(0);
    break;

  } // case
}

void TSdCardSpi::RunInitialization()
{
  switch (initstate)
  {
  case 0: // initialize variables, prepare 2ms wait

    actual_clockspeed = initial_speed;
    spi->SetSpeed(actual_clockspeed);
    pin_cs->Set1();

    //SetBusWidth(1);

    card_present = false;
    card_megabytes = 0;
    high_capacity = false;
    card_v2 = false;

    cmd_start_time = CLOCKCNT;

    //SendCmd(1, 0, SDCMD_RES_NO | SDCMD_OPENDRAIN); // never fails
    ++initstate;
    break;

  case 1:  // wait 2ms
    if (CLOCKCNT - cmd_start_time >= 2000 * us_clocks)
    {
      // send 80 clocs with CS high and MOSI high
      memset(&txbuf[0], 0xFF, 10);
      spi->StartTransfer(0, 0, 0, 10, &txbuf[0], &rxbuf[0]);
      ++initstate;
    }
    break;

  case 2: // send initialization command
  {
    pin_cs->Set0();

    cmd_pad_bytes = 2; // add two 0xFF-s before the command
    CmdSend(0, 0, 8);

    ++initstate;
    break;
  }

  case 3:  // process CMD0 (reset) response
  {
    if (GetResult8() == 0xFF)
    {
      initstate = 100; // jump to delayed re-initialization
      break;
    }

    card_present = true;

    CmdSend(8, 0x000001AA, 8);  // test for card v2
    ++initstate;
    break;
  }

  case 4: // check CMD8 result
    if (GetResult32() != 0xFFFFFFFF)
    {
      card_v2 = true;
    }

    // make the card leave the idle state with CMD55+CMD41
    trycnt = 0; // the CMD55+CMD41 might be repeated multiple times

    CmdSend(55, 0, 3); // prepare application specific command (41)
    ++initstate;
    break;

  case 5:
  {
    if (0xFF == GetResult8())
    {
      // error in application specific command prepare
      initstate = 100;
      break;
    }

    uint32_t carg = 0;
    if (card_v2)  carg |= (1 << 30);

    CmdSend(41, carg, 8);
    ++initstate;
    break;
  }

  case 6:  // check the command result
  {
    uint8_t cmdres = GetResult8();
    if (cmdres != 0)
    {
      // try again
      ++trycnt;
      if (trycnt > 150)
      {
        initstate = 100;
        break;
      }

      // repeat the query
      CmdSend(55, 0, 3);
      initstate = 5;
      break;
    }

    // fine go on with the rest

    // read the OCR with CMD58
    CmdSend(58, 0, 8);
    ++initstate;
    break;
  }

  case 7: // check OCR read result
    reg_ocr = GetResult32();

    // get the CSD register
    CmdSend(9, 0, 24);
    ++initstate;
    break;

  case 8: // processing CSD register read result
    if (!GetResult128(&reg_csd[0]))
    {
      initstate = 100;
      break;
    }

    ProcessCsd();

    pin_cs->Set1();

    // send 8 clock with CS high
    txbuf[0] = 0xFF;
    spi->StartTransfer(0,  0,  0, 1,  &txbuf[0],  &rxbuf[0]);
    ++initstate;
    break;

  case 9:
  {
    if (forced_clockspeed)
    {
      actual_clockspeed = forced_clockspeed;
    }
    else
    {
      actual_clockspeed = max_clockspeed;
      if (actual_clockspeed > csd_max_speed)  actual_clockspeed = csd_max_speed;
    }
    spi->SetSpeed(actual_clockspeed); // switch to high speed

    card_initialized = true; // turn off this state machine
    initstate = 50;
    break;
  }

  case 50: // card initialized
    break;

  case 100: // re-init delay
    pin_cs->Set1();
    cmd_start_time = CLOCKCNT;
    ++initstate;
    break;

  case 101:
    if (CLOCKCNT - cmd_start_time > (SystemCoreClock >> 1))
    {
      initstate = 0; // repeat the initialization sequence
    }
    break;

  } // end switch

}

void TSdCardSpi::CmdSend(uint8_t acmd, uint32_t acmdarg, uint16_t arxbytes)
{
  int i = 0;

  while (cmd_pad_bytes)
  {
    txbuf[i++] = 0xFF;
    --cmd_pad_bytes;
  }

  txbuf[i++] = 0x40 | acmd;
  txbuf[i++] = (acmdarg >> 24) & 0xFF;
  txbuf[i++] = (acmdarg >> 16) & 0xFF;
  txbuf[i++] = (acmdarg >>  8) & 0xFF;
  txbuf[i++] = (acmdarg >>  0) & 0xFF;
  // CRC: used and checked only for a few commands
  uint8_t crc = 0xFF;
  if ( 0 == acmd)  crc = 0x95;  // when acmdarg = 0
  if ( 8 == acmd)  crc = 0x87;  // when acmdarg = 0x000001AA
  if (58 == acmd)  crc = 0xFD;  // when acmdarg = 0
  txbuf[i++] = crc;

  cmdlen = i;
  rxbytes = arxbytes;

  while (arxbytes)
  {
    txbuf[i++] = 0xFF;
    --arxbytes;
  }

  spi->StartTransfer(0,  0,  0, i,  &txbuf[0],  &rxbuf[0]);

  rxidx = cmdlen;
  max_rxidx = cmdlen + rxbytes;
}

void TSdCardSpi::SpiStartRead(uint16_t arxbytes)
{
  cmdlen = 0;
  rxbytes = arxbytes;
  memset(&txbuf[0], 0xFF, rxbytes);

  spi->StartTransfer(0,  0,  0, rxbytes,  &txbuf[0],  &rxbuf[0]);

  rxidx = 0;
  max_rxidx = rxbytes;
}



uint8_t TSdCardSpi::GetResult8()
{
  if (!FindResponseCode())
  {
    return 0xFF;
  }
  return rxbuf[rxidx];
}

uint32_t TSdCardSpi::GetResult32()
{
  if (!FindResponseCode())
  {
    return 0xFFFFFFFF;
  }

  return ( (rxbuf[rxidx + 1] << 24) | (rxbuf[rxidx + 2] << 16)
           | (rxbuf[rxidx + 3] << 8) | rxbuf[rxidx + 4] );
}

bool TSdCardSpi::GetResult128(uint8_t * dataptr)
{
  if (!FindResponseCode())
  {
    return false;
  }

  ++rxidx; // skip the response marker

  if (!FindDataStart())
  {
    return false;
  }

  uint32_t remaining = 16;
  while (remaining)
  {
    *dataptr++ = rxbuf[rxidx + remaining - 1];
    --remaining;
  }

  return true;
}

bool TSdCardSpi::FindResponseCode()
{
  while (rxidx < max_rxidx)
  {
    if ((rxbuf[rxidx] & 0x80) == 0)
    {
      return true;
    }
    ++rxidx;
  }
  return false;
}

bool TSdCardSpi::FindToken(uint8_t atoken, uint8_t amask)
{
  while (rxidx < max_rxidx)
  {
    if (atoken == (rxbuf[rxidx] & amask))
    {
      ++rxidx; // skip the token
      return true;
    }
    ++rxidx;
  }
  return false;
}

bool TSdCardSpi::FindNotBusy()
{
  while (rxidx < max_rxidx)
  {
    if (rxbuf[rxidx] != 0x00)
    {
      return true;
    }
    ++rxidx;
  }
  return false;
}

bool TSdCardSpi::FindDataStart()
{
  while (rxidx < max_rxidx)
  {
    if (0xFE == rxbuf[rxidx])
    {
      ++rxidx; // skip the marker
      return true;
    }
    ++rxidx;
  }
  return false;
}

void TSdCardSpi::CopyReadData()
{
  while (remainingbytes && (rxidx < max_rxidx))
  {
    *dataptr++ = rxbuf[rxidx];
    ++rxidx;
    --remainingbytes;
  }
}

void TSdCardSpi::CopyCrc()
{
  while (crcremaining && (rxidx < max_rxidx))
  {
    if (2 == crcremaining)
    {
      blockcrc |= (rxbuf[rxidx] << 8);
    }
    else
    {
      blockcrc |= rxbuf[rxidx];
    }

    ++rxidx;
    --crcremaining;
  }
}

void TSdCardSpi::FinishTransfer(int aerrorcode)
{
  pin_cs->Set1();

  errorcode = aerrorcode;
  completed = true;
  trstate = 0;
}
