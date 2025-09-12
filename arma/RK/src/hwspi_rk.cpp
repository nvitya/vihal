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
 *  file:     hwspi_rk.h
 *  brief:    Rockchip SPI driver for VIHAL
 *  created:  2025-09-12
 *  authors:  nvitya
*/

#include <hwspi_rk.h>
#include "platform.h"
#include "hwspi.h"
#include "rk_utils.h"

#define SPI_SR_SSI    (0x40)
#define SPI_SR_STB    (0x20)
#define SPI_SR_RFF    (0x10)
#define SPI_SR_RFE    (0x08)
#define SPI_SR_TFE    (0x04)
#define SPI_SR_TFF    (0x02)
#define SPI_SR_BUSY   (0x01)

bool THwSpi_rk::Init(int adevnum)
{
	devnum = adevnum;
  //unsigned resetmask;

	initialized = false;

	// GPLL = 1500 MHz, probably it is GPLL / 8
  basespeed = 187500000;    // gpll_div = 200 MHz

	if (0 == devnum)
	{
		map_hw_addr(SPI0_BASE, sizeof(struct SPI_REG), (void * *)&regs);
    cru_reg_set_field(0x830, 10, 2, 0); // clk_spi0 + pclk_spi0

    // select the 24 MHz base clock
    cru_reg_set_field(0x388, 14, 2, 1); // 0 = xin_osc0_func, 1 = gpll_div, 2 = v0pll_div, 3 = v1pll_div
    cru_reg_set_field(0x388, 10, 4, 0); // 0 = do not divide
	}
	else if (1 == devnum)
  {
		map_hw_addr(SPI1_BASE, sizeof(struct SPI_REG), (void * *)&regs);
    cru_reg_set_field(0x830, 12, 2, 0); // clk_spi1 + pclk_spi1

    // select the 24 MHz base clock
    cru_reg_set_field(0x388, 8, 2, 1); // 0 = xin_osc0_func, 1 = gpll_div, 2 = v0pll_div, 3 = v1pll_div
    cru_reg_set_field(0x388, 4, 4, 0); // 0 = do not divide
  }
	else
	{
		regs = nullptr;
    return false;
	}

  regs->ENR = 0;
  if (regs->ENR) {}  // some delay

  regs->SER = 1; // enable slave select for NCS0

  uint32_t flcode;
  if (databits >= 16)     flcode = 2;
  else if (databits >= 8) flcode = 1;
  else                    flcode = 0;

  uint32_t ctrl0 = (0
  	| (0  << 25)  // LBK: 1 = loopback mode
  	| (0  << 23)  // SOI(2): SS_N output inverted
  	| (0  << 22)  // SM: 0 = SCLK_IN is masked
  	| (0  << 21)  // MTM: NS Microvire: 0 = non-sequential
  	| (0  << 20)  // OPM: 0 = Master mode, 1 = Slave mode
  	| (0  << 18)  // XFM(2): 0 = Tx + Rx, 1 = Tx Only, 2 = Rx Only
  	| (0  << 16)  // FRF(2): frame format, 0 = Motorola SPI, 1 = Ti SSP, 2 = NS Microwire
  	| ((rx_sample_delay & 3) << 14) // RSD(2): rx sample delay
    | (0  << 13)  // BHT: when 8-bit data: 0 = APB 16-bit -> spi 8-bit, 1 = APB 8-bit -> SPI 8-bit
    | (0  << 12)  // FBM: 0 = MSB first, 1 = LSB first
    | (0  << 11)  // EM: 0 = Little endian, 1 = Big Endien
    | (0  << 10)  // SSD: delay from CS to clock, 0 = half clock, 1 = one clock
    | (0  <<  8)  // CSM(2): 0 = keep CS low between frames, 1 = half clock high, 2 = one clock high
    | (0  <<  7)  // SCPOL: 0 = clock is low when inactive, 1 = clock is high when inactive
    | (0  <<  6)  // SCPH: 0 = serial clock toggles in middle of first data bit, 1 = serial clock toggles at start of first bit
    | ((databits-1) <<  2)  // CFS(4): Microwire control word length 3 = 4-bit, 7 = 8-bit, 15 = 16-bit
    | (flcode <<  2)  // DFS: 0 = 4-bit, 1 = 8-bit, 2 = 16-bit
  );
  if (idleclk_high)      ctrl0 |= (1 << 7);
  if (datasample_late)   ctrl0 |= (1 << 6);
  regs->CTRLR[0] = ctrl0;

  regs->CTRLR[1] = (0
    | (0  <<  0)  // DFNUM(32): number of data frames
  );

  regs->DMATDLR = 0;
  regs->DMARDLR = 0;

  regs->TXFTLR = 0;
  regs->RXFTLR = 0;

  regs->DMACR = 3; // enable the RX+TX DMA

  regs->ICR = 0x7F;  // clear all interrupts

  SetSpeed(speed);

  regs->ENR = 1;

	initialized = true;
	return true;
}

void THwSpi_rk::SetSpeed(unsigned aspeed)
{
  regs->ENR = 0;
  if (regs->ENR) {}
  speed = aspeed;
  unsigned baudr = ((basespeed / speed) + 1) & 0xFFFE; // round up to two
  if (baudr < 2)  baudr = 2;
  regs->BAUDR = baudr;
  regs->ENR = 1;
}

bool THwSpi_rk::TrySendData(uint8_t adata)
{
  if (regs->SR & SPI_SR_TFF)
  {
	  return false;
  }
  else
  {
    regs->TXDR = adata;
    return true;
  }
}

bool THwSpi_rk::TryRecvData(uint8_t * dstptr)
{
  if (regs->SR & SPI_SR_RFE)
  {
    return false;
  }
  else
  {
    *dstptr = regs->RXDR;
    return true;
  }
}

bool THwSpi_rk::SendFinished()
{
  if (regs->SR & SPI_SR_BUSY)
  {
    return false;
  }
  else
  {
    return true;
  }
}

void THwSpi_rk::SetCs(unsigned avalue)
{
}
