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
// created:  2022-22-12
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

  case 1: // start read blocks

    //TRACE("s.r. STA=%08X\r\n", regs->STA);

    cmdarg = startblock;
    if (!high_capacity)  cmdarg <<= 9; // byte addressing for low capacity cards
    cmd = 17; // uses only single block transfer

    remainingbytes = 512;
    blockcrc = 0;
    crcremaining = 2;  // wait extra 2 bytes at the end

    pin_cs->Set0();
    CmdSend(cmd, cmdarg, sizeof(rxbuf) - 8);

    ++trstate;
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
      SpiStartRead(sizeof(rxbuf));
      trstate = 3;
      break;
    }

    // there is some data already here
    trstate = 4; // jump to state 4 to process
    RunTransfer();
    return;

  case 3: // waiting for data start
    if (!FindDataStart())
    {
      SpiStartRead(sizeof(rxbuf));
      trstate = 3;
      break;
    }

    trstate = 4; // jump to state 4 to process the data chunk
    RunTransfer();
    return;

  case 4: // reading data chunks
    CopyReadData();

    if (remainingbytes)
    {
      SpiStartRead(sizeof(rxbuf));
      break; // stay here
    }

    CopyCrc();
    if (crcremaining)
    {
      SpiStartRead(4);
      break; // stay here
    }

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
    RunTransfer();  return;
    break;

  case 11: // start write blocks

    FinishTransfer(HWERR_NOTIMPL);
    break;
  }
}

void TSdCardSpi::RunInitialization()
{
  switch (initstate)
  {
  case 0: // initialize variables, prepare 2ms wait

    spi->SetSpeed(initial_speed);
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
    uint32_t highspeed = clockspeed;
    if (highspeed > csd_max_speed)  highspeed = csd_max_speed;

    spi->SetSpeed(highspeed); // switch to high speed

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

uint32_t TSdCardSpi::GetRegBits(void * adata, uint32_t startpos, uint8_t bitlen)
{
  uint32_t * d32p = (uint32_t *)adata;
  uint32_t widx = (startpos >> 5);
  d32p += widx;
  uint8_t bitidx = (startpos & 31);

  uint32_t result = (*d32p >> bitidx);

  if (bitidx + bitlen > 32)
  {
    ++d32p;
    result |= (*d32p << (32 - bitidx));
  }

  if (bitlen < 32)
  {
    result &= ((1 << bitlen) - 1);
  }

  return result;
}

//! SD/MMC transfer rate unit codes (10K) list
static const uint32_t sd_trans_units[8] = { 10, 100, 1000, 10000, 0, 0, 0, 0 };
//! SD transfer multiplier factor codes (1/10) list
static const uint32_t sd_trans_values[16] = { 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };

void TSdCardSpi::ProcessCsd()
{
  /* CSD bits:

       126(2): Version
       122(4): Spec. version
        96(8): Tran speed
  V1:
        47(3): V1 C size mult.
       62(12): V1 C size
        80(4): V1 Read BL Len
  v2:
       48(22): V2 C. Size
  */
    csd_ver = GetRegBits(&reg_csd[0], 126, 2);

    uint32_t transunit = sd_trans_units[GetRegBits(&reg_csd[0], 96, 3)];
    uint32_t transval  = sd_trans_values[GetRegBits(&reg_csd[0], 99, 4)];
    csd_max_speed = 1000 * transunit * transval;

    // decode card size

    /*
     * Get card capacity.
     * ----------------------------------------------------
     * For normal SD/MMC card:
     * memory capacity = BLOCKNR * BLOCK_LEN
     * Where
     * BLOCKNR = (C_SIZE+1) * MULT
     * MULT = 2 ^ (C_SIZE_MULT+2)       (C_SIZE_MULT < 8)
     * BLOCK_LEN = 2 ^ READ_BL_LEN      (READ_BL_LEN < 12)
     * ----------------------------------------------------
     * For high capacity SD card:
     * memory capacity = (C_SIZE+1) * 512K byte
     */

    if (csd_ver >= 1)
    {
      card_megabytes = ((GetRegBits(&reg_csd[0], 48, 22) + 1) >> 1);
      high_capacity = true;
    }
    else
    {
      uint32_t c_size = GetRegBits(&reg_csd[0], 62, 12);
      uint32_t c_size_mult = GetRegBits(&reg_csd[0], 47, 3);
      uint32_t block_len = (1 << GetRegBits(&reg_csd[0], 80, 4));

      uint32_t blocknr = (c_size + 1) * (1 << (c_size_mult + 2));

      card_megabytes = ((blocknr * block_len) >> 20);
      high_capacity = false;
    }

    //TRACE("SD card max speed = %u MHz, size = %u MBytes\r\n", csd_max_speed / 1000000, card_megabytes);
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

  ++rxidx; // skip the data marker

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
