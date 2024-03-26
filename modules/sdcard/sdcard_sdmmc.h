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


#ifndef SDCARD_SDMMC_H_
#define SDCARD_SDMMC_H_

#include "sdcard.h"
#include "hwpins.h"
#include "hwsdmmc.h"

class TSdCardSdmmc : public TSdCard
{
private:
  typedef TSdCard super;

public:
  THwSdmmc *    sdmmc = nullptr;
  uint8_t       bus_width = 4;

  bool          Init(THwSdmmc * asdmmc);
  void          Run();  // overrides base

protected:

  uint32_t      us_clocks = 0;

  uint32_t      cmd_start_time = 0;
  uint8_t       cmdlen = 0;

  uint32_t      remainingbytes = 0;

  //uint8_t       curcmd = 0;
  //uint32_t      curcmdflags = 0;
  //uint32_t      curcmdarg = 0;

  uint32_t      after_error_delay_clocks = 1;

  void          RunInitialization();  // initialization state machine
  void          RunTransfer();

  void          FinishTransfer(int aerrorcode);

  void          StartCmdReadBlocks();
  void          StartCmdWriteBlocks();

};

#endif /* SDCARD_SDMMC_H_ */
