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
/*
 *  file:     hwsdmmc_imxrt.cpp
 *  brief:    IMXRT Internal SD/SDIO/MMC driver
 *  created:  2024-04-10
 *  authors:  nvitya
*/

#include "platform.h"
#include "string.h"
#include "hwsdmmc.h"
#include "imxrt_utils.h"
#include "clockcnt.h"

#include "traces.h"

bool THwSdmmc_imxrt::Init(int adevnum)
{
  unsigned code;
  unsigned perid;
  //unsigned periphclock = atsam_peripheral_clock();

  if (1 == adevnum)
  {
    regs = USDHC1;
    imxrt_set_clock_gate(6, 2, 3);
  }
#if defined(USDHC2)
  else if (2 == adevnum)
  {
    regs = USDHC2;
    imxrt_set_clock_gate(6, 4, 3);
  }
#endif
  else
  {
    regs = nullptr;
    return false;
  }

  // the PFD2 = 196 MHz clock is already prepared in the hwclk_init()

  /* Reset ALL USDHC. */
  regs->SYS_CTRL |= USDHC_SYS_CTRL_RSTA_MASK | USDHC_SYS_CTRL_RSTC_MASK | USDHC_SYS_CTRL_RSTD_MASK;
  while (regs->SYS_CTRL & USDHC_SYS_CTRL_RSTA_MASK)
  {
    __NOP(); // wait for self-clear
  }

  regs->SYS_CTRL = (0
    | (0  << 28)  // RSTT: reset tuning
    | (0  << 27)  // INITA: initialization active
    | (0  << 26)  // RSTD: data circuit reset
    | (0  << 25)  // RSTC: reset for command line
    | (0  << 24)  // RSTA: software reset for all
    | (0  << 23)  // IPP_RST_N
    | (15 << 16)  // DTOCV(4): data timeout clocks, 0 = 2^14 .. 15 = 2^29
    | (32 <<  8)  // SDCLKFS(8): clock frequency select: 0 = /1, 1 = /2, 2 = /4, 4 = /8, 8 = /16, 32 = /64
    | (7  <<  4)  // DVS(4): divisor: 0 = /1, 1 = /2, 2 = /3, 14 = /15, 15 = /16
  );

  regs->WTMK_LVL = (0
    | (8    << 24)  // WR_BRST_LEN(5)
    | (0x40 << 16)  // WR_WML(8): write watermark level (words)
    | (8    <<  8)  // RD_BRST_LEN(5)
    | (0x40 <<  0)  // RD_WML(8): read watermark level (words)
  );


  protctl_base = (0
    | (0    << 30)  // NON_EXACT_BLK_RD
    | (1    << 27)  // BURST_LEN_EN(3)
    | (0    << 26)  // WECRM: wakeup event on card removal
    | (0    << 25)  // WECINS: wakeup event on card insertion
    | (0    << 24)  // WECINT: wakeup event on card interrupt
    | (4    << 21)  // reserved(3): keep the reset value
    | (0    << 20)  // RD_DONE_NO_8CLK
    | (0    << 19)  // IABG
    | (0    << 18)  // RWCTL
    | (0    << 17)  // CREQ
    | (0    << 16)  // SABGREQ
    | (0    <<  8)  // DMASEL(2): 0 = no DMA, 1 = ADMA1, 2 = ADMA2
    | (0    <<  7)  // CDSS
    | (0    <<  6)  // CDTL
    | (2    <<  4)  // EMODE(2): endian mode: 0 = big endian, 2 = little endian
    | (0    <<  3)  // D3CD: 1 = use DAT3 as card detect pin
    | (0    <<  1)  // DTW(2): data transfer width: 0 = 1-bit, 1 = 4-bit, 2 = 8-bit
    | (0    <<  0)  // LCTL: led control, 0 = LED off, 1 = LED on
  );
  regs->PROT_CTRL = protctl_base;

  mixctl_base = (0
    | (4    << 29)  // reserved(3): write 4
    | (0    << 25)  // FBCLK_SEL
    | (0    << 24)  // AUTO_TUNE_EN
    | (0    << 23)  // SMP_CLK_SEL
    | (0    << 22)  // EXE_TUNE
    | (0    <<  7)  // AC23EN: Auto CMD23 Enable
    | (0    <<  6)  // NIBBLE_POS: (ddr mode)
    | (0    <<  5)  // MSBSEL: 0 = single block, 1 = multi block
    | (0    <<  4)  // DTDSEL: data direction: 0 = write (host to card), 1 = read (card to host)
    | (0    <<  3)  // DDR_EN
    | (0    <<  2)  // AC12EN: auto CMD12 enable
    | (0    <<  1)  // BCEN: block count enable
    | (0    <<  0)  // DMAEN: 1 = enable DMA
  );
  regs->MIX_CTRL = mixctl_base;
  /* disable interrupt, enable all the interrupt status, clear status. */
  regs->INT_STATUS_EN = 0xFFFFFFFF;
  regs->INT_SIGNAL_EN = 0;
  regs->INT_STATUS    = 0xFFFFFFFF;

  SetSpeed(400000);
  SetBusWidth(1);

  // wait 74 SD clycles

  delay_us(200);

  return true;
}

void THwSdmmc_imxrt::SetSpeed(uint32_t speed)
{
  uint32_t  baseclock = 198000000;
  uint32_t  prediv = 4;
  uint32_t  pdcode = 0x02;

  // switch to 64 division
  if (speed < 4000000)
  {
    prediv = 64;
    pdcode = 0x20;
  }
  uint32_t  predivclk = baseclock / prediv;

  uint32_t  div = predivclk / speed;
  if ((div < 16) && (predivclk > speed * div))  ++div;
  //if (div * speed
  if (div <  1)  div = 1;
  if (div > 16)  div = 16;

  uint32_t tmp = (regs->SYS_CTRL & ~(0x0000FFF0));
  tmp |= (0
    | (pdcode    <<  8)  // SDCLKFS(8): clock frequency select: 0 = /1, 1 = /2, 2 = /4, 4 = /8, 8 = /16, 32 = /64
    | ((div - 1) <<  4)  // DVS(4): divisor: 0 = /1, 1 = /2, 2 = /3, 14 = /15, 15 = /16
  );
  regs->SYS_CTRL = tmp;
}

void THwSdmmc_imxrt::SetBusWidth(uint8_t abuswidth)
{
  // bit2-1: DTW(2): data transfer width: 0 = 1-bit, 1 = 4-bit, 2 = 8-bit
  protctl_base &= ~(3 << 1);

  if (8 == abuswidth)
  {
    protctl_base |= (2 << 1);
  }
  else if (4 == abuswidth)
  {
    protctl_base |= (1 << 1);
  }
  else
  {
    protctl_base |= (0 << 1);
  }

  regs->PROT_CTRL = protctl_base;
}

void THwSdmmc_imxrt::SendCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags)
{
  uint32_t xfrt = (0
    | (acmd << 24)  // CMDINX(6)
    | (0    << 22)  // CMDTYP(2): 0 = normal, 1 = suspend CMD52, 2 = resume CMD52, 3 = abort CMD12
    | (0    << 21)  // DPSEL: 0 = no data, 1 = data present
    | (0    << 20)  // CICEN: 0 = no index check
    | (0    << 19)  // CCCEN: 0 = no CRC check, 1 = enable CRC check
    | (0    << 16)  // RSPTYP(2): 0 = no response, 1 = 136-bit response, 2 = 48-bit response, 3 = 48-bit with busy
  );

  regs->PROT_CTRL = protctl_base;
  regs->MIX_CTRL  = mixctl_base;

  uint8_t restype = (cmdflags & SDCMD_RES_MASK);
  if (restype)
  {
    // response present
    if      (restype == SDCMD_RES_48BIT)   xfrt |= (2 << 16);
    else if (restype == SDCMD_RES_136BIT)  xfrt |= (1 << 16);
    else if (restype == SDCMD_RES_R1B)     xfrt |= (3 << 16);
  }

  regs->INT_STATUS = 0xFFFFFFFF;
  regs->CMD_ARG = cmdarg;
  regs->CMD_XFR_TYP = xfrt;

  cmderror = false;

  lastcmdtime = CLOCKCNT;
  cmdrunning = true;
}

bool THwSdmmc_imxrt::CmdFinished()
{
  uint32_t sr = regs->INT_STATUS;

  if (sr & (USDHC_INT_STATUS_CCE_MASK | USDHC_INT_STATUS_CTOE_MASK
            | USDHC_INT_STATUS_CEBE_MASK | USDHC_INT_STATUS_CIE_MASK
            | USDHC_INT_STATUS_DTOE_MASK | USDHC_INT_STATUS_DCE_MASK
            | USDHC_INT_STATUS_DEBE_MASK
           )
     )
  {
    cmderror = true;
    return true;
  }

  if (sr & USDHC_INT_STATUS_CC_MASK)
  {
    return true;
  }
  return false;
}

bool THwSdmmc_imxrt::TransferFinished()
{
  uint32_t sr = regs->INT_STATUS;
  if (sr & USDHC_INT_STATUS_TC_MASK)
  {
    return true;
  }
  return false;
}

void THwSdmmc_imxrt::CloseTransfer()
{

}

void THwSdmmc_imxrt::GetCmdResult128(void * adataptr)
{
  uint32_t * dst = (uint32_t *)adataptr;

  *dst++ = (regs->CMD_RSP0 << 8);
  *dst++ = (regs->CMD_RSP1 << 8) | ((regs->CMD_RSP0 >> 24) & 0xFF);
  *dst++ = (regs->CMD_RSP2 << 8) | ((regs->CMD_RSP1 >> 24) & 0xFF);
  *dst++ = (regs->CMD_RSP3 << 8) | ((regs->CMD_RSP2 >> 24) & 0xFF);
}

uint32_t THwSdmmc_imxrt::GetCmdResult32()
{
  return regs->CMD_RSP0;
}

bool THwSdmmc_imxrt::CmdResult32Ok()
{
  uint32_t cmdres = GetCmdResult32();
  if ((cmdres & SDMMC_OCR_ERRORBITS) != 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}

#define ADMA2_MAX_CHUNK  (65536 - 16)

void THwSdmmc_imxrt::PrepareAdma2(void * aaddr, uint32_t alen)
{
  if (alen > MAX_ADMA2_DESC * ADMA2_MAX_CHUNK)
  {
    alen = MAX_ADMA2_DESC * ADMA2_MAX_CHUNK;
  }

  memset(&adma2desc[0], 0, sizeof(adma2desc)); // clear all descriptors

  uint32_t      remaining = alen;
  uint32_t      addr = uint32_t(aaddr);
  TAdma2Desc *  pdesc = &adma2desc[0];

  while (remaining > 0)
  {
    uint32_t chunklen = remaining;
    if (chunklen > ADMA2_MAX_CHUNK)  chunklen = ADMA2_MAX_CHUNK;

    //memset(pdesc, 0, sizeof(TAdma2Desc));
    pdesc->length  = chunklen;
    pdesc->address = addr;
    pdesc->attr    = (ADMA2_ACT_TRAN | ADMA2_ATTR_VALID);
    if (chunklen >= remaining)
    {
      pdesc->attr |= ADMA2_ATTR_END;
    }

    remaining -= chunklen;
    addr += chunklen;
    ++pdesc;
  }

  __DSB();

  regs->ADMA_SYS_ADDR = uint32_t(&adma2desc[0]);
}

void THwSdmmc_imxrt::StartDataReadCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags, void * dataptr, uint32_t datalen)
{
  uint32_t xfrt = (0
    | (acmd << 24)  // CMDINX(6)
    | (0    << 22)  // CMDTYP(2): 0 = normal, 1 = suspend CMD52, 2 = resume CMD52, 3 = abort CMD12
    | (1    << 21)  // DPSEL: 0 = no data, 1 = data present
    | (0    << 20)  // CICEN: 0 = no index check
    | (0    << 19)  // CCCEN: 0 = no CRC check, 1 = enable CRC check
    | (0    << 16)  // RSPTYP(2): 0 = no response, 1 = 136-bit response, 2 = 48-bit response, 3 = 48-bit with busy
  );
  uint8_t restype = (cmdflags & SDCMD_RES_MASK);
  if (restype)
  {
    // response present
    if      (restype == SDCMD_RES_48BIT)   xfrt |= (2 << 16);
    else if (restype == SDCMD_RES_136BIT)  xfrt |= (1 << 16);
    else if (restype == SDCMD_RES_R1B)     xfrt |= (3 << 16);
  }

  regs->PROT_CTRL = (protctl_base
    | (2    <<  8)  // DMASEL(2): 0 = no DMA, 1 = ADMA1, 2 = ADMA2
  );

  uint32_t mixctl = (mixctl_base
    | (0    <<  5)  // MSBSEL: 0 = single block, 1 = multi block
    | (1    <<  4)  // DTDSEL: data direction: 0 = write (host to card), 1 = read (card to host)
    | (0    <<  3)  // DDR_EN
    | (0    <<  2)  // AC12EN: auto CMD12 enable
    | (0    <<  1)  // BCEN: block count enable
    | (1    <<  0)  // DMAEN: 1 = enable DMA
  );
  if (datalen >= 512)
  {
    mixctl |= (1 << 1);  // BCEN: block count enable
    if (datalen > 512)
    {
      mixctl |= (1 <<  5);   // MSBSEL: 0 = single block, 1 = multi block
    }
    regs->BLK_ATT = (0
      | ((datalen >> 9)  << 16) // BLKCNT(16): (datalen >> 9  =  datalen / 512)
      | (512             <<  0) // BLKSIZE(13):
    );
  }
  else
  {
    regs->BLK_ATT = (0
      | (1       << 16) // BLKCNT(16):
      | (datalen <<  0) // BLKSIZE(13):
    );
    mixctl |= (1 << 1);  // BCEN: block count enable
    mixctl |= (0 << 5);  // MSBSEL: 0 = single block, 1 = multi block
  }
  regs->MIX_CTRL  = mixctl;

  PrepareAdma2(dataptr, datalen);

  regs->INT_STATUS = 0xFFFFFFFF;
  regs->CMD_ARG = cmdarg;
  regs->CMD_XFR_TYP = xfrt;

  cmderror = false;
  lastcmdtime = CLOCKCNT;
  cmdrunning = true;
}

void THwSdmmc_imxrt::StartDataWriteCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags, void * dataptr, uint32_t datalen)
{
  uint32_t xfrt = (0
    | (acmd << 24)  // CMDINX(6)
    | (0    << 22)  // CMDTYP(2): 0 = normal, 1 = suspend CMD52, 2 = resume CMD52, 3 = abort CMD12
    | (1    << 21)  // DPSEL: 0 = no data, 1 = data present
    | (0    << 20)  // CICEN: 0 = no index check
    | (0    << 19)  // CCCEN: 0 = no CRC check, 1 = enable CRC check
    | (0    << 16)  // RSPTYP(2): 0 = no response, 1 = 136-bit response, 2 = 48-bit response, 3 = 48-bit with busy
  );
  uint8_t restype = (cmdflags & SDCMD_RES_MASK);
  if (restype)
  {
    // response present
    if      (restype == SDCMD_RES_48BIT)   xfrt |= (2 << 16);
    else if (restype == SDCMD_RES_136BIT)  xfrt |= (1 << 16);
    else if (restype == SDCMD_RES_R1B)     xfrt |= (3 << 16);
  }

  regs->PROT_CTRL = (protctl_base
    | (2    <<  8)  // DMASEL(2): 0 = no DMA, 1 = ADMA1, 2 = ADMA2
  );

  uint32_t mixctl = (mixctl_base
    | (0    <<  5)  // MSBSEL: 0 = single block, 1 = multi block
    | (0    <<  4)  // DTDSEL: data direction: 0 = write (host to card), 1 = read (card to host)
    | (0    <<  3)  // DDR_EN
    | (0    <<  2)  // AC12EN: auto CMD12 enable
    | (0    <<  1)  // BCEN: block count enable
    | (1    <<  0)  // DMAEN: 1 = enable DMA
  );
  if (datalen >= 512)
  {
    mixctl |= (1 << 1);  // BCEN: block count enable
    if (datalen > 512)
    {
      mixctl |= (1 <<  5);   // MSBSEL: 0 = single block, 1 = multi block
    }
    regs->BLK_ATT = (0
      | ((datalen >> 9)  << 16) // BLKCNT(16): (datalen >> 9  =  datalen / 512)
      | (512             <<  0) // BLKSIZE(13):
    );
  }
  else
  {
    regs->BLK_ATT = (0
      | (1       << 16) // BLKCNT(16):
      | (datalen <<  0) // BLKSIZE(13):
    );
    mixctl |= (1 << 1);  // BCEN: block count enable
    mixctl |= (0 << 5);  // MSBSEL: 0 = single block, 1 = multi block
  }
  regs->MIX_CTRL  = mixctl;

  PrepareAdma2(dataptr, datalen);

  regs->INT_STATUS = 0xFFFFFFFF;
  regs->CMD_ARG = cmdarg;
  regs->CMD_XFR_TYP = xfrt;

  cmderror = false;
  lastcmdtime = CLOCKCNT;
  cmdrunning = true;
}

void THwSdmmc_imxrt::StartDataWriteTransmit(void * dataptr, uint32_t datalen)
{
  // not necessary ?
}
