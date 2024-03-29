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
 *  file:     hwuart_esp.cpp
 *  brief:    ESP32-C3/C6 UART
 *  date:     2022-01-29
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>

#include "hwuart_esp.h"
#include "esp_utils.h"

#if defined(MCUSF_32C3)  // different register layouts !

bool THwUart_esp::Init(int adevnum)
{
  uint32_t tmp;
  unsigned code;

  devnum = adevnum;
  initialized = false;

  regs = nullptr;

  uhci = UHCI0;
  uhci->CONF0 |= (1 << 11); // keep the UHCI CLOCK running

  uint32_t sys_rst_mask;
  SYSTEM->PERIP_CLK_EN0 |= (SYSTEM_UART_MEM_CLK_EN | SYSTEM_UHCI0_CLK_EN);

  if (0 == devnum)
  {
    regs = UART0;
    SYSTEM->PERIP_CLK_EN0 |= SYSTEM_UART_CLK_EN;
    sys_rst_mask = SYSTEM_UART_RST;
  }
  else if (1 == devnum)
  {
    regs = UART1;
    SYSTEM->PERIP_CLK_EN0 |= SYSTEM_UART1_CLK_EN;
    sys_rst_mask = SYSTEM_UART1_RST;
  }

  if (!regs)
  {
    return false;
  }

  // following the Espressif recommended reset sequence:

  SYSTEM->PERIP_RST_EN0 &= ~sys_rst_mask;  // clear system UART reset
  regs->CLK_CONF |= UART_RST_CORE;         // set UART unit reset
  SYSTEM->PERIP_RST_EN0 |= sys_rst_mask;   // set system UART reset
  if (SYSTEM->PERIP_RST_EN0) {} // some sync
  SYSTEM->PERIP_RST_EN0 &= ~sys_rst_mask;  // remove system UART reset
  regs->CLK_CONF &= ~UART_RST_CORE;        // remove UART unit reset

  regs->ID &= ~UART_UPDATE;  // clear UART_UPDATE_CTRL
  while (regs->ID & UART_UPDATE)  { /* wait */ }

  unsigned periphclock = esp_apb_speed();

  regs->CLK_CONF = (0
    | (0  <<  0)  // SCLK_DIV_A(6)
    | (0  <<  6)  // SCLK_DIV_B(6)
    | (0  << 12)  // SCLK_DIV_NUM(8)
    | (1  << 20)  // SCLK_SEL(2): 1 = APB(80 MHz), 3 = XTAL(40 MHz)
    | (1  << 22)  // SCLK_EN
    | (0  << 23)  // RST_CODE
    | (1  << 24)  // TX_SCLK_EN
    | (1  << 25)  // RX_SCLK_EN
  );

  // * 16 for the fractional divider
  uint32_t brdiv_m16 = (periphclock << 4) / baudrate;  // *16 = 1

  regs->CLKDIV = (0
      | ((brdiv_m16 >> 4)  <<  0)  // CLKDIV(12)
      | ((brdiv_m16 & 15)  << 20)  // CLKDIV_FRAG(4)
  );


  uint32_t conf0 = (0
    | (0  <<  0)  // PARITY
    | (0  <<  1)  // PARITY_EN
    | (0  <<  2)  // BIT_NUM(2): 0 = 5 bit, 3 = 8 bit
    | (0  <<  4)  // STOP_BIT_NUM(2): 0 = 1 stop bit, 1 = 1.5, 2 = 2, 3 = 3
    | (0  <<  6)  // SW_RTS
    | (0  <<  7)  // SW_DTR
    | (0  <<  8)  // TXD_BRK
    | (0  <<  9)  // IRDA_DPLX
    | (0  << 10)  // IRDA_TX_EN
    | (0  << 11)  // IRDA_WCTL
    | (0  << 12)  // IRDA_TX_INV
    | (0  << 13)  // IRDA_RX_INV
    | (0  << 14)  // LOOPBACK
    | (0  << 15)  // TX_FLOW_EN:
    | (0  << 16)  // IRDA_EN
    | (0  << 17)  // RXFIFO_RST
    | (0  << 18)  // TXFIFO_RST
    | (0  << 19)  // RXD_INV
    | (0  << 20)  // CTS_INV
    | (0  << 21)  // DSR_INV
    | (0  << 22)  // TXD_INV
    | (0  << 23)  // RTS_INV
    | (0  << 24)  // DTR_INV
    | (1  << 25)  // CLK_EN: 0 = clock active only at writes, 1 = force clock on
    | (1  << 26)  // ERR_WR_MASK: 1 = do not store wrong data in RX FIFO
    | (0  << 27)  // AUTOBAUD_EN:
    | (1  << 28)  // MEM_CLK_EN: 1 = enable UART RAM clock
  );
  // data bits
  conf0 |= ((databits - 5) << 2);
  // stop bits
  if (4 == halfstopbits)  conf0 |= (2 << 4);  // set two stop bits
  // parity
  if (parity)
  {
    conf0 |= (1 << 1); // enable parity
    if (!oddparity)  conf0 |= (1 << 0);
  }
  regs->CONF0 = conf0;

  uint32_t conf1 = (0
    | (0x60 <<  0)  // RXFIFO_FULL_THRHD(9)
    | (0x60 <<  8)  // TXFIFO_EMPTY_THRHD(9)
    | (0  << 18)  // RX_DAT_OVF: 1 = disable RX data overflow detection
    | (0  << 19)  // RX_TOUT_FLOW_DIW
    | (0  << 20)  // RX_FLOW_EN
    | (0  << 21)  // RX_TOUT_EN
  );
  regs->CONF1 = conf1;

  // update regs:
  regs->ID |= UART_UPDATE;
  while (regs->ID & UART_UPDATE)  { /* wait */ }

  initialized = true;

  return true;
}

#elif defined(MCUSF_32C6) // different register layouts !

bool THwUart_esp::Init(int adevnum)
{
  uint32_t tmp;
  unsigned code;

  devnum = adevnum;
  initialized = false;

  regs = nullptr;

  uhci = UHCI0;
  uhci->CONF0 |= (1 << 11); // keep the UHCI CLOCK running

  unsigned pcr_sclk_conf = (0
    | (0  <<  0)  // SCLK_DIV_A(6)
    | (0  <<  6)  // SCLK_DIV_B(6)
    | (0  << 12)  // SCLK_DIV_NUM(8)
    | (1  << 20)  // SCLK_SEL(2): 0 = none, 1 = PLL80M, 2 = RC_FAST, 3 = XTAL
    | (1  << 22)  // SCLK_EN:
  );

  if (0 == devnum)
  {
    regs = UART0;
    PCR->UART0_PD_CTRL = (PCR_UART0_MEM_FORCE_PU);
    PCR->UART0_CONF |= PCR_UART0_CLK_EN;
    PCR->UART0_CONF |= PCR_UART0_RST_EN;
    PCR->UART0_CONF &= ~PCR_UART0_RST_EN;
    PCR->UART0_SCLK_CONF = pcr_sclk_conf;
  }
  else if (1 == devnum)
  {
    regs = UART1;
    PCR->UART1_PD_CTRL = (PCR_UART0_MEM_FORCE_PU);
    PCR->UART1_CONF |= PCR_UART0_CLK_EN;
    PCR->UART1_CONF |= PCR_UART0_RST_EN;
    PCR->UART1_CONF &= ~PCR_UART0_RST_EN;
    PCR->UART1_SCLK_CONF = pcr_sclk_conf;
  }

  if (!regs)
  {
    return false;
  }

  while (regs->REG_UPDATE)  { /* wait */ }

  regs->CLK_CONF = (0
    | (1  << 24)  // TX_SCLK_EN
    | (1  << 25)  // RX_SCLK_EN
    | (0  << 26)  // TX_RST_CORE
    | (0  << 27)  // RX_RST_CORE
  );

  unsigned periphclock = 80000000;

  // * 16 for the fractional divider
  uint32_t brdiv_m16 = (periphclock << 4) / baudrate;  // *16 = 1

  regs->CLKDIV_SYNC = (0
      | ((brdiv_m16 >> 4)  <<  0)  // CLKDIV(12)
      | ((brdiv_m16 & 15)  << 20)  // CLKDIV_FRAG(4)
  );

  uint32_t conf0 = (0
    | (0  <<  0)  // PARITY
    | (0  <<  1)  // PARITY_EN
    | (0  <<  2)  // BIT_NUM(2): 0 = 5 bit, 3 = 8 bit
    | (0  <<  4)  // STOP_BIT_NUM(2): 0 = 1 stop bit, 1 = 1.5, 2 = 2, 3 = 3
    | (0  <<  6)  // TXD_BRK
    | (0  <<  7)  // IRDA_DPLX
    | (0  <<  8)  // IRDA_TX_EN
    | (0  <<  9)  // IRDA_WCTL
    | (0  << 10)  // IRDA_TX_INV
    | (0  << 11)  // IRDA_RX_INV
    | (0  << 12)  // LOOPBACK
    | (0  << 13)  // TX_FLOW_EN:
    | (0  << 14)  // IRDA_EN
    | (0  << 15)  // RXD_INV
    | (0  << 16)  // TXD_INV
    | (0  << 17)  // DIS_RX_DAT_OVFa
    | (1  << 18)  // ERR_WR_MASK: 1 = do not store wrong data in RX FIFO
    | (0  << 19)  // AUTOBAUD_EN:
    | (1  << 20)  // MEM_CLK_EN: 1 = enable UART RAM clock
    | (0  << 21)  // SW_RTS
    | (0  << 22)  // RXFIFO_RST
    | (0  << 23)  // TXFIFO_RST
  );


  // data bits
  conf0 |= ((databits - 5) << 2);
  // stop bits
  if (4 == halfstopbits)  conf0 |= (2 << 4);  // set two stop bits
  // parity
  if (parity)
  {
    conf0 |= (1 << 1); // enable parity
    if (!oddparity)  conf0 |= (1 << 0);
  }
  regs->CONF0_SYNC = conf0;

  uint32_t conf1 = (0
    | (0x60 <<  0)  // RXFIFO_FULL_THRHD(8)
    | (0x60 <<  8)  // TXFIFO_EMPTY_THRHD(8)
    | (0    << 16)  // CTS_INV
    | (0    << 17)  // DSR_INV
    | (0    << 18)  // RTS_INV
    | (0    << 19)  // DTR_INV
    | (0    << 20)  // SW_DTR
    | (1    << 21)  // CLK_EN
  );
  regs->CONF1 = conf1;

  // update regs:
  regs->REG_UPDATE = 1;
  while (regs->REG_UPDATE) { /* wait */ }

  initialized = true;

  return true;
}

#else
  #error "unhandled ESP32 subfamily"
#endif


bool THwUart_esp::TrySendChar(char ach)
{
  uint32_t sr = regs->STATUS;
	if (((sr >> 16) & UART_TXFIFO_CNT) >= tx_fifo_size) // TX FIFO Full ?
	{
		return false;
	}

	regs->FIFO = ach;
	return true;
}

bool THwUart_esp::TryRecvChar(char * ach)
{
  uint32_t sr = regs->STATUS;
	if (0 == (sr & UART_RXFIFO_CNT)) // RX FIFO Empty?
	{
	  return false;
	}

	*ach = regs->FIFO;
	return true;
}

bool THwUart_esp::SendFinished()
{
  uint32_t sr = regs->STATUS;
  if (0 == (sr & UART_TXFIFO_CNT)) // TX FIFO Empty?
  {
    return true;
  }
	else
	{
		return false;
	}
}

void THwUart_esp::DmaAssign(bool istx, THwDmaChannel * admach)
{
	if (istx)
	{
		txdma = admach;
	}
	else
	{
		rxdma = admach;
	}
  admach->Prepare(istx, (void *)&regs->FIFO, 0);

  // select this uart for UHCI (DMA)
  uint32_t uhciconf = uhci->CONF0;
  uhciconf &= ~(3 << 2);
  uhciconf |= (1 << (2 + devnum));
  uhci->CONF0 = uhciconf;
}

bool THwUart_esp::DmaStartSend(THwDmaTransfer * axfer)
{
	if (!txdma)
	{
		return false;
	}

	txdma->StartTransfer(axfer);

	return true;
}

bool THwUart_esp::DmaStartRecv(THwDmaTransfer * axfer)
{
	if (!rxdma)
	{
		return false;
	}

	rxdma->StartTransfer(axfer);

	return true;
}
