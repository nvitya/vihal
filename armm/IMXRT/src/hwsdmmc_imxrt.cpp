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
    | (0x80 << 16)  // WR_WML(8): write watermark level (words)
    | (8    <<  8)  // RD_BRST_LEN(5)
    | (0x80 <<  0)  // RD_WML(8): read watermark level (words)
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


#if 0

  // DMA channel is an user setting
  dma.Init(dma_channel, 0); // 0 = HSMCI DMA Peripheral Id (Transmit and Receive)

  // Set the Data Timeout Register to 2 Mega Cycles
  regs->HSMCI_DTOR = HSMCI_DTOR_DTOMUL_1048576 | HSMCI_DTOR_DTOCYC(2);
  // Set Completion Signal Timeout to 2 Mega Cycles
  regs->HSMCI_CSTOR = HSMCI_CSTOR_CSTOMUL_1048576 | HSMCI_CSTOR_CSTOCYC(2);
  // Set Configuration Register
  regs->HSMCI_CFG = HSMCI_CFG_FIFOMODE | HSMCI_CFG_FERRCTRL; // | HSMCI_CFG_LSYNC;

  // Set power saving to maximum value
  regs->HSMCI_MR = HSMCI_MR_PWSDIV_Msk;

  SetSpeed(400000);
  SetBusWidth(1);

  // Enable the HSMCI and the Power Saving
  regs->HSMCI_CR = HSMCI_CR_MCIEN | HSMCI_CR_PWSEN;

  regs->HSMCI_DMA = 0;

#endif

  // wait 74 SD clycles

  delay_us(200);

  return true;
}

void THwSdmmc_imxrt::SetSpeed(uint32_t speed)
{
#if 0
  uint32_t mck = atsam_peripheral_clock();

  if (speed > 25000000)
  {
    HSMCI->HSMCI_CFG |= HSMCI_CFG_HSMODE;
  }
  else
  {
    HSMCI->HSMCI_CFG &= ~HSMCI_CFG_HSMODE;
  }

  uint32_t clkdiv = 0;
  uint32_t rest = 0;

  // Speed = MCK clock / (2 * (CLKDIV + 1))
  if ((speed * 2) < mck)
  {
    clkdiv = mck / (2 * speed);
    rest = mck % (2 * speed);
    if (rest > 0)
    {
      // Ensure that the card speed not be higher than expected.
      clkdiv++;
    }
    if (clkdiv > 0)
    {
      clkdiv -= 1;
    }
  }
  else
  {
    clkdiv = 0;
  }
  regs->HSMCI_MR &= ~HSMCI_MR_CLKDIV_Msk;
  regs->HSMCI_MR |= HSMCI_MR_CLKDIV(clkdiv);
#endif
}

void THwSdmmc_imxrt::SetBusWidth(uint8_t abuswidth)
{
#if 0
  uint32_t hsmci_slot = HSMCI_SDCR_SDCSEL_SLOTA;
  uint32_t hsmci_bus_width;

  switch (abuswidth)
  {
  case 4:
    hsmci_bus_width = HSMCI_SDCR_SDCBUS_4;
    break;

  case 8:
    hsmci_bus_width = HSMCI_SDCR_SDCBUS_8;
    break;

  default:
  case 1:
    hsmci_bus_width = HSMCI_SDCR_SDCBUS_1;
    break;
  }
  HSMCI->HSMCI_SDCR = hsmci_slot | hsmci_bus_width;
#endif
}

void THwSdmmc_imxrt::SendCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags)
{
#if 0
  regs->HSMCI_MR &= ~(HSMCI_MR_WRPROOF | HSMCI_MR_RDPROOF | HSMCI_MR_FBYTE);
  regs->HSMCI_DMA = 0;
  regs->HSMCI_BLKR = 0;

  uint32_t cmdr = (acmd & 0x3F);
  uint8_t restype = (cmdflags & SDCMD_RES_MASK);
  if (restype)
  {
    // response present
    cmdr |= HSMCI_CMDR_MAXLAT; // increase latency for commands with response
    if (restype == SDCMD_RES_48BIT)  cmdr |= HSMCI_CMDR_RSPTYP_48_BIT;
    else if (restype == SDCMD_RES_136BIT)  cmdr |= HSMCI_CMDR_RSPTYP_136_BIT;
    else if (restype == SDCMD_RES_R1B)  cmdr |= HSMCI_CMDR_RSPTYP_R1B;
  }
  if (cmdflags & SDCMD_OPENDRAIN)
  {
    cmdr |= HSMCI_CMDR_OPDCMD_OPENDRAIN;
  }

  cmderror = false;

  regs->HSMCI_ARGR = cmdarg;
  regs->HSMCI_CMDR = cmdr; // start the execution

  lastcmdtime = CLOCKCNT;
  cmdrunning = true;
#endif
}

bool THwSdmmc_imxrt::CmdFinished()
{
#if 0
  uint32_t sr = regs->HSMCI_SR;

  if (sr & (HSMCI_SR_CSTOE | HSMCI_SR_RTOE | HSMCI_SR_RENDE | HSMCI_SR_RDIRE | HSMCI_SR_RINDE))
  {
    cmderror = true;
    return true;
  }

  if (sr & HSMCI_SR_CMDRDY)
  {
    return true;
  }
#endif
  return false;
}

bool THwSdmmc_imxrt::TransferFinished()
{
  if (dma.initialized && dma.Active())
  {
    return false;
  }

#if 0
  if (0 == (regs->HSMCI_SR & HSMCI_SR_XFRDONE))
  {
    return false;
  }
#endif

  return true;
}

void THwSdmmc_imxrt::CloseTransfer()
{
#if 0
  regs->ICR = 0xFFFFFFFF;
#endif
}

void THwSdmmc_imxrt::GetCmdResult128(void * adataptr)
{
#if 0
  uint32_t * dst = (uint32_t *)adataptr;
  dst += 3;
  for (int n = 0; n < 4; ++n)
  {
    *dst = regs->HSMCI_RSPR[0];
    --dst;
  }
#endif
}

uint32_t THwSdmmc_imxrt::GetCmdResult32()
{
  return 0; //regs->HSMCI_RSPR[0];
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

void THwSdmmc_imxrt::StartDataReadCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags, void * dataptr, uint32_t datalen)
{
#if 0
  // Enabling Read/Write Proof allows to stop the HSMCI Clock during
  // read/write  access if the internal FIFO is full.
  // This will guarantee data integrity, not bandwidth.
  regs->HSMCI_MR |= HSMCI_MR_WRPROOF | HSMCI_MR_RDPROOF;

  if (datalen & 0x3)
  {
    regs->HSMCI_MR |= HSMCI_MR_FBYTE;
  }
  else
  {
    regs->HSMCI_MR &= ~HSMCI_MR_FBYTE;
  }

  regs->HSMCI_DMA = HSMCI_DMA_DMAEN;

  uint32_t cmdr = (acmd & 0x3F);
  uint8_t restype = (cmdflags & SDCMD_RES_MASK);
  if (restype)
  {
    // response present
    cmdr |= HSMCI_CMDR_MAXLAT; // increase latency for commands with response
    if (restype == SDCMD_RES_48BIT)  cmdr |= HSMCI_CMDR_RSPTYP_48_BIT;
    else if (restype == SDCMD_RES_136BIT)  cmdr |= HSMCI_CMDR_RSPTYP_136_BIT;
    else if (restype == SDCMD_RES_R1B)  cmdr |= HSMCI_CMDR_RSPTYP_R1B;
  }
  if (cmdflags & SDCMD_OPENDRAIN)
  {
    cmdr |= HSMCI_CMDR_OPDCMD_OPENDRAIN;
  }

  // add data transfer flags
  cmdr |= HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRDIR_READ;
  if (datalen <= 512)
  {
    cmdr |= HSMCI_CMDR_TRTYP_SINGLE;
    regs->HSMCI_BLKR = (datalen << 16) | (1 << 0);
  }
  else
  {
    cmdr |= HSMCI_CMDR_TRTYP_MULTIPLE;
    regs->HSMCI_BLKR = (512 << 16) | (datalen >> 9);  // fix 512 byte blocks
  }

  cmderror = false;

  // start the DMA channel
  dma.Prepare(false, (void *)&regs->HSMCI_FIFO[0], 0);
  dmaxfer.flags = 0; // use defaults
  dmaxfer.bytewidth = 4;  // destination must be aligned !!!
  dmaxfer.count = (datalen >> 2);
  dmaxfer.dstaddr = dataptr;
  dma.StartTransfer(&dmaxfer);

  regs->HSMCI_ARGR = cmdarg;
  regs->HSMCI_CMDR = cmdr; // start the execution

  lastcmdtime = CLOCKCNT;
  cmdrunning = true;
#endif
}

void THwSdmmc_imxrt::StartDataWriteCmd(uint8_t acmd, uint32_t cmdarg, uint32_t cmdflags, void * dataptr, uint32_t datalen)
{
#if 0
  // Enabling Read/Write Proof allows to stop the HSMCI Clock during
  // read/write  access if the internal FIFO is full.
  // This will guarantee data integrity, not bandwidth.
  regs->HSMCI_MR |= HSMCI_MR_WRPROOF | HSMCI_MR_RDPROOF;

  if (datalen & 0x3)
  {
    regs->HSMCI_MR |= HSMCI_MR_FBYTE;
  }
  else
  {
    regs->HSMCI_MR &= ~HSMCI_MR_FBYTE;
  }

  regs->HSMCI_DMA = HSMCI_DMA_DMAEN;

  uint32_t cmdr = (acmd & 0x3F);
  uint8_t restype = (cmdflags & SDCMD_RES_MASK);
  if (restype)
  {
    // response present
    cmdr |= HSMCI_CMDR_MAXLAT; // increase latency for commands with response
    if (restype == SDCMD_RES_48BIT)  cmdr |= HSMCI_CMDR_RSPTYP_48_BIT;
    else if (restype == SDCMD_RES_136BIT)  cmdr |= HSMCI_CMDR_RSPTYP_136_BIT;
    else if (restype == SDCMD_RES_R1B)  cmdr |= HSMCI_CMDR_RSPTYP_R1B;
  }
  if (cmdflags & SDCMD_OPENDRAIN)
  {
    cmdr |= HSMCI_CMDR_OPDCMD_OPENDRAIN;
  }

  // add data transfer flags
  cmdr |= HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRDIR_WRITE;
  if (datalen <= 512)
  {
    cmdr |= HSMCI_CMDR_TRTYP_SINGLE;
    regs->HSMCI_BLKR = (datalen << 16) | (1 << 0);
  }
  else
  {
    cmdr |= HSMCI_CMDR_TRTYP_MULTIPLE;
    regs->HSMCI_BLKR = (512 << 16) | (datalen >> 9);  // fix 512 byte blocks
  }

  cmderror = false;

  regs->HSMCI_ARGR = cmdarg;
  regs->HSMCI_CMDR = cmdr; // start the execution

  lastcmdtime = CLOCKCNT;
  cmdrunning = true;
#endif
}

void THwSdmmc_imxrt::StartDataWriteTransmit(void * dataptr, uint32_t datalen)
{
#if 0
  // start the DMA channel
  dma.Prepare(true, (void *)&regs->HSMCI_FIFO[0], 0);
  dmaxfer.flags = 0; // use defaults
  dmaxfer.bytewidth = 4;  // destination must be aligned !!!
  dmaxfer.count = (datalen >> 2);
  dmaxfer.srcaddr = dataptr;
  dma.StartTransfer(&dmaxfer);
#endif
}
