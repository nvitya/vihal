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

#ifndef SRC_SDCARD_SPI_H_
#define SRC_SDCARD_SPI_H_

#include "sdcard.h"
#include "hwpins.h"
#include "hwspi.h"

//#define SDCARD_SPI_BUF_SIZE  512  // just for testing

#ifndef SDCARD_SPI_BUF_SIZE
  #define SDCARD_SPI_BUF_SIZE  128  // allocated twice !
#endif

static_assert(SDCARD_SPI_BUF_SIZE >= 64, "The SDCARD SPI buffer size must be minimum 64 bytes");

class TSdCardSpi : public TSdCard
{
private:
  typedef TSdCard super;

public:
  THwSpi *      spi = nullptr;
  TGpioPin *    pin_cs = nullptr;

  bool          Init(THwSpi * aspi);
  void          Run();  // overrides base

protected:
  uint32_t      us_clocks = 0;
  uint32_t      cmd_start_time = 0;
  uint8_t       cmdlen = 0;
  uint16_t      rxbytes = 0;
  uint8_t       trycnt = 0;

  uint8_t       txbuf[SDCARD_SPI_BUF_SIZE];
  uint8_t       rxbuf[SDCARD_SPI_BUF_SIZE];

  uint16_t      rxidx = 0;
  uint16_t      max_rxidx = 0;

  uint8_t       cmd_pad_bytes = 0; // will be resetted after every CmdSend

  uint32_t      remainingbytes = 0;
  uint32_t      crcremaining = 0;

  uint16_t      blockcrc = 0;

  void          CmdSend(uint8_t acmd, uint32_t acmdarg, uint16_t arxbytes);
  void          SpiStartRead(uint16_t arxbytes);
  void          CopyReadData();  // updates remainingbytes
  void          CopyCrc();       // updates crcremaining

  uint8_t       GetResult8();
  uint32_t      GetResult32();
  bool          GetResult128(uint8_t * dataptr);

  void          RunInitialization();  // initialization state machine

  void          RunTransfer();

  bool          FindResponseCode(); // sets the rxidx (points to rxbuf[])
  bool          FindDataStart();    // sets the rxidx (points to rxbuf[])
  bool          FindToken(uint8_t atoken, uint8_t amask = 0xFF);   // sets the rxidx (points to rxbuf[])
  bool          FindNotBusy();      // check if not 0x00 received

  void          FinishTransfer(int aerrorcode);

};

#endif /* SRC_SDCARD_SPI_H_ */
