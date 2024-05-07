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
// file:     sdcard_sdmmc.h
// brief:    SDCARD access over SD/MMC controller
// created:  2022-11-16
// authors:  nvitya
// warning:
//   TODO: very low speed with -O2 !

#include "sdcard_sdmmc.h"
#include "clockcnt.h"
#include "traces.h"

bool TSdCardSdmmc::Init(THwSdmmc * asdmmc)
{
  initialized = false;

  us_clocks = SystemCoreClock / 1000000;

  sdmmc = asdmmc;
  if (!sdmmc)
  {
    return false;
  }

  card_initialized = false;
  initstate = 0;

  initialized = true;
  return true;
}

void TSdCardSdmmc::Run()
{
  if (!initialized)
  {
    return;
  }

  if (sdmmc->cmdrunning)
  {
    if (!sdmmc->CmdFinished())
    {
      return;
    }
    sdmmc->cmdrunning = false;
  }

  if (!card_initialized)
  {
    RunInitialization();
    return;
  }

  RunTransfer(); // run the transfer state machine
}


void TSdCardSdmmc::RunInitialization()
{
  int i;
  uint32_t carg;

  //TRACE("SD state = %i\r\n", state);

  switch (initstate)
  {
  case 0: // send init clocks

    // required low settings for the initialization
    actual_clockspeed = initial_speed;
    sdmmc->SetSpeed(actual_clockspeed); // initial speed = 400 kHz
    sdmmc->SetBusWidth(1);

    card_present = false;
    high_capacity = false;
    card_v2 = false;

    //SendCmd(1, 0, SDCMD_RES_NO | SDCMD_OPENDRAIN); // never fails
    ++initstate;
    break;

  case 1: // set SDCARD to init
    sdmmc->SendCmd(0, 0, SDCMD_RES_NO); // never fails
    ++initstate;
    break;

  case 2:
    sdmmc->SendCmd(8, 0x1AA, SDCMD_RES_48BIT | SDCMD_OPENDRAIN);  // test for V2
    ++initstate;
    break;

  case 3:
    if (!sdmmc->cmderror && (0xFFFFFFFF != sdmmc->GetCmdResult32()))
    {
      card_v2 = true;
    }
    initstate = 5;
    repeatcnt = 0;
    break;

  // Wait until the card is ready

  case 5:
    // prepare application specific command
    sdmmc->SendCmd(55, 0, SDCMD_RES_48BIT | SDCMD_OPENDRAIN);
    ++initstate;
    break;

  case 6:
    if (sdmmc->cmderror)
    {
      // no card available
      card_present = false;
      initstate = 100;
    }
    else
    {
      // get operating status
      carg = 0x801f8000;

      if (card_v2)  carg |= (1u << 30);
      sdmmc->SendCmd(41, carg, SDCMD_RES_48BIT | SDCMD_OPENDRAIN);
      ++initstate;
    }
    break;

  case 7:
    if (sdmmc->cmderror)  initstate = 100;
    else
    {
      reg_ocr = sdmmc->GetCmdResult32();
      if (reg_ocr & 0x80000000)
      {
        //TRACE("SDCARD OCR = %08X\r\n", reg_ocr);
        high_capacity = (reg_ocr & (1u << 30) ? true : false);
        initstate = 10;
      }
      else
      {
        // repeat OCR register read
        ++repeatcnt;
        if (repeatcnt < 10)
        {
          initstate = 5;
        }
        else
        {
          initstate = 0;
        }
      }
    }
    break;

  // read Card Identification Data (CID)
  case 10:
    sdmmc->SendCmd(2, 0, SDCMD_RES_136BIT | SDCMD_OPENDRAIN);
    ++initstate;
    break;

  case 11:
    if (sdmmc->cmderror)  initstate = 100;
    else
    {
      sdmmc->GetCmdResult128(&reg_cid[0]);
#if 0
      TRACE("SDCARD CID = ");
      for (i = 0; i < 16; ++i)
      {
        TRACE(" %02X", reg_cid[i]);
      }
      TRACE("\r\n");
#endif

      card_present = true;
      initstate = 20;
    }
    break;

  // Ask for a Relative Card Address (RCA)
  case 20:
    sdmmc->SendCmd(3, 0, SDCMD_RES_48BIT | SDCMD_OPENDRAIN);
    ++initstate;
    break;

  case 21:
    if (sdmmc->cmderror)  initstate = 100;
    else
    {
      rca = (sdmmc->GetCmdResult32() & 0xFFFF0000);  // keep it high-shifted to spare later shiftings
      //TRACE("SDCARD RCA = %08X\r\n", rca);
      ++initstate;
    }
    break;

  // get the Card Specific Data (CSD)
  case 22:
    sdmmc->SendCmd(9, rca, SDCMD_RES_136BIT | SDCMD_OPENDRAIN);
    ++initstate;
    break;

  case 23:
    if (sdmmc->cmderror)  initstate = 100;
    else
    {
      sdmmc->GetCmdResult128(&reg_csd[0]);
#if 0
      TRACE("SDCARD CSD = ");
      for (i = 0; i < 16; ++i)
      {
        TRACE(" %02X", reg_csd[i]);
      }
      TRACE("\r\n");
#endif

      ++initstate;

      ProcessCsd();
    }
    break;

  // Select the card and put into transfer mode
  case 24:
    //SendCmd(7, rca, SDCMD_RES_R1B);
    sdmmc->SendCmd(7, rca, SDCMD_RES_48BIT);
    ++initstate;
    break;

  case 25:
    if (sdmmc->cmderror)
    {
      TRACE("Error selecting card!\r\n");
      initstate = 100;
    }
    else
    {
      ++initstate;
    }
    break;

  // get the SCR register
  case 26:
    // prepare application specific command
    sdmmc->SendCmd(55, rca, SDCMD_RES_48BIT);
    ++initstate;
    break;

  case 27:
    sdmmc->StartDataReadCmd(51, 0, SDCMD_RES_48BIT, &reg_scr[0], sizeof(reg_scr));
    ++initstate;
    break;
  case 28:
    if (sdmmc->cmderror)
    {
      TRACE("SCR register read error!\r\n");
      initstate = 100;
    }
    else if (sdmmc->TransferFinished()) // !dma.initialized || !dma.Active())
    {
#if 0
      TRACE("SDCARD SCR = ");
      for (i = 0; i < 8; ++i)
      {
        TRACE(" %02X", reg_scr[i]);
      }
      TRACE("\r\n");
#endif

      ++initstate;
    }
    break;

  // set bus width to 4 bit
  case 29:
    // prepare application specific command
    sdmmc->SendCmd(55, rca, SDCMD_RES_48BIT);
    ++initstate;
    break;

  case 30:
    sdmmc->SendCmd(6, 2, SDCMD_RES_48BIT);  // Switch command, 2 = 4 bit bus
    ++initstate;
    break;

  case 31:
    if (sdmmc->cmderror)   initstate = 100;
    else
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

      sdmmc->SetSpeed(actual_clockspeed);
      sdmmc->SetBusWidth(bus_width);
      ++initstate;
    }
    break;

  case 32:
    sdmmc->SendCmd(16, 512, SDCMD_RES_48BIT); // set block size
    ++initstate;
    break;

  case 33:
    if (sdmmc->cmderror)   initstate = 100;
    else
    {
      //TRACE("SDCARD initialized, ready to accept transfer commands.\r\n");
      initstate = 50;
    }
    break;

  case 50:
    // ready to accept transfer commands, go to normal state
    card_initialized = true;
    completed = true;
    initstate = 0;
    break;

  case 100: // delay after error
    if (CLOCKCNT - sdmmc->lastcmdtime > after_error_delay_clocks)
    {
      initstate = 0;
    }
    break;
  }
}

void TSdCardSdmmc::RunTransfer()
{
  switch (trstate)
  {
  case 0: // idle
    break;

  case 1: // start read blocks

    //TRACE("s.r. STA=%08X\r\n", regs->STA);

    chunk_blocks = remainingblocks;
    if (chunk_blocks > 128)  chunk_blocks = 128;  // 64k maximal chunks
    StartCmdReadBlocks();

    cmd_start_time = CLOCKCNT;
    trstate = 2;
    break;

  case 2: // check command result
    if (sdmmc->cmderror)
    {
      // the previous Write command might still be running, repeat the command
      if (CLOCKCNT - cmd_start_time > 20000 * us_clocks)
      {
        FinishTransfer(HWERR_TIMEOUT);
      }
      else
      {
        StartCmdReadBlocks();  // try again
      }
      return;
    }

    if (!sdmmc->CmdResult32Ok())
    {
      //TRACE("SDCARD_SDMMC: RD cmd %u res=%08X]\r\n", sdmmc->curcmd, sdmmc->GetCmdResult32());
      //FinishTransfer(1);
      //return;
    }

    cmd_start_time = CLOCKCNT;
    trstate = 101;
    break;


  case 11: // start write blocks
    //TRACE("s.w. STA=%08X\r\n", regs->STA);

    chunk_blocks = remainingblocks;
    if (chunk_blocks > 128)  chunk_blocks = 128;  // 64k maximal chunks
    StartCmdWriteBlocks();

    wr_errors = 0;
    cmd_start_time = CLOCKCNT;
    trstate = 12;
    break;

  case 12:  // wait until the write command is accepted
    if (sdmmc->cmderror)
    {
      // the previous Write command might still be running, repeat the command
      if (CLOCKCNT - cmd_start_time > 20000 * us_clocks)
      {
        FinishTransfer(HWERR_TIMEOUT);
      }
      else
      {
        StartCmdWriteBlocks();
      }
      return;
    }

    if (!sdmmc->CmdResult32Ok())
    {
      //TRACE("SDCARD_SDMMC: WR cmd %u res=%08X]\r\n", sdmmc->curcmd, sdmmc->GetCmdResult32());

      //FinishTransfer(1);
      //return;
    }

    sdmmc->StartDataWriteTransmit(dataptr, chunk_blocks * 512);

    cmd_start_time = CLOCKCNT;
    trstate = 101;
    break;

  // common states for read and write

  case 101: // wait until the block transfer finishes
    if (!sdmmc->TransferFinished())
    {
      if (CLOCKCNT - cmd_start_time > 300000 * us_clocks)  // 300 ms timeout
      {
        FinishTransfer(HWERR_TIMEOUT);
      }
      return;
    }

    remainingblocks -= chunk_blocks;
    curblock        += chunk_blocks;
    dataptr         += chunk_blocks * 512;

    if (chunk_blocks > 1)
    {
      // send the stop transmission command
      sdmmc->SendCmd(12, 0, SDCMD_RES_R1B);
      cmd_start_time = CLOCKCNT;
      trstate = 105;  // wait until the stop command finishes
    }
    else
    {
      if (iswrite)
      {
        // go to status polling for writes
        sdmmc->SendCmd(13, rca, SDCMD_RES_R1B);  // send query status cmd
        trstate = 111;
      }
      else
      {
        FinishTransfer(0);
      }
    }
    break;

  case 105: // Handle tranmission stop
    if (sdmmc->cmderror)
    {
      if (CLOCKCNT - cmd_start_time > 20000 * us_clocks)
      {
        FinishTransfer(HWERR_TIMEOUT);
      }
      else
      {
        sdmmc->SendCmd(12, 0, SDCMD_RES_R1B);  // send again
      }
      return;
    }

    if (!sdmmc->CmdResult32Ok())
    {
      //TRACE("SDCARD_SDMMC: STP cmd %u res=%08X]\r\n", sdmmc->curcmd, sdmmc->GetCmdResult32());
      //FinishTransfer(HWERR_UNKNOWN);
      //return;
    }

    if (!iswrite)
    {
      if (remainingblocks > 0)
      {
        trstate = 1;
        RunTransfer();
        return;
      }
      FinishTransfer(0);
      return;
    }

    // go to status polling for writes
    sdmmc->SendCmd(13, rca, SDCMD_RES_R1B);  // send query status cmd
    trstate = 111;
    break;

  case 111:  // wait until the write operation finishes
    if (sdmmc->cmderror)
    {
      if (CLOCKCNT - cmd_start_time > 20000 * us_clocks)
      {
        FinishTransfer(HWERR_TIMEOUT);
      }
      else
      {
        sdmmc->SendCmd(13, rca, SDCMD_RES_R1B);  // send query status cmd
      }
      return;
    }

    last_wr_status = sdmmc->GetCmdResult32();
    wr_errors |= (last_wr_status & (0
      | (1 << 19) // ERROR
      | (1 << 20) // CC_ERROR
      | (1 << 21) // CARD_ECC_FAILED
      | (1 << 22) // ILLEGAL_COMMAND
      | (1 << 25) // CARD_IS_LOCKED
      | (1 << 26) // WP_VIOLATION
    ));

    uint32_t stcode = ((last_wr_status >> 9) & 0xF);
    // SDCARD states:
    //   0 = idle
    //   1 = ready
    //   2 = ident
    //   3 = stby
    //   4 = tran
    //   5 = data
    //   6 = rcv
    //   7 = prg
    //   8 = dis
    if (stcode != 7) // 7 = prg
    {
      if (wr_errors)
      {
        FinishTransfer(HWERR_WRITE);
      }
      else
      {
        if (remainingblocks > 0)
        {
          trstate = 11;
          RunTransfer();
          return;
        }

        FinishTransfer(0);
      }
      return;
    }

    if (CLOCKCNT - cmd_start_time > 300000 * us_clocks)  // 300 ms timeout
    {
      FinishTransfer(HWERR_TIMEOUT);
      return;
    }

    sdmmc->SendCmd(13, rca, SDCMD_RES_R1B);  // send query status cmd again
    break;

  } // case
}

void TSdCardSdmmc::FinishTransfer(int aerrorcode)
{
  sdmmc->CloseTransfer();
  errorcode = aerrorcode;
  trstate = 0;
  completed = true;
}

void TSdCardSdmmc::StartCmdReadBlocks()
{
  uint8_t  cmd;
  uint32_t cmdarg;

  cmdarg = curblock;
  if (!high_capacity)  cmdarg <<= 9; // byte addressing for low capacity cards
  cmd = (chunk_blocks > 1 ? 18 : 17);

  sdmmc->StartDataReadCmd(cmd, cmdarg, SDCMD_RES_48BIT, dataptr, chunk_blocks * 512);
}

void TSdCardSdmmc::StartCmdWriteBlocks()
{
  uint8_t  cmd;
  uint32_t cmdarg;

  cmdarg = curblock;
  if (!high_capacity)  cmdarg <<= 9; // byte addressing for low capacity cards
  cmd = (chunk_blocks > 1 ? 25 : 24);

  sdmmc->StartDataWriteCmd(cmd, cmdarg, SDCMD_RES_48BIT, dataptr, chunk_blocks * 512);
}
