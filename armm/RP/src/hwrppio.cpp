/*
 * hwrppio.cpp
 *
 *  Created on: Aug 13, 2023
 *      Author: vitya
 */

#include "rp_utils.h"
#include "hwpins.h"
#include "hwrppio_instructions.h"
#include <hwrppio.h>

uint8_t hwpio_reset_done[2];  // will be initialized with zeroes as it goes to the .bss

pio_hw_t * hwpio_init_and_get_regs(uint8_t adevnum)
{
  uint32_t reset_mask;
  pio_hw_t * dregs;

  if (0 == adevnum)
  {
    dregs = pio0_hw;
    reset_mask = RESETS_RESET_PIO0_BITS;
  }
  else if (1 == adevnum)
  {
    dregs = pio1_hw;
    reset_mask = RESETS_RESET_PIO1_BITS;
  }
  else
  {
    return nullptr;
  }

  if (0 == hwpio_reset_done[adevnum])
  {
    rp_reset_control(reset_mask, true); // issue the reset
    hwpio_reset_done[adevnum] = 1;
  }

  rp_reset_control(reset_mask, false); // remove reset
  return dregs;
}

void THwRpPioPrg::Init(uint8_t adevnum, uint8_t aoffset)
{
  dregs = hwpio_init_and_get_regs(adevnum);
  if (!dregs)
  {
    return;
  }

  offset = aoffset;
  entry = aoffset;
  Clear();
}

void THwRpPioPrg::Add(uint16_t ainstr)
{
  if (offset + length < 32)
  {
    dregs->instr_mem[offset + length++] = ainstr;
  }
}

void THwRpPioPrg::Clear()
{
  length = 0;
  wrap_start = 0;
}

//---------------------------------------------------------------------------------------

bool THwRpPioSm::Init(uint8_t adevnum, uint8_t asmnum)
{
  initialized = false;
  devnum = adevnum;
  smnum  = asmnum;

  dregs = hwpio_init_and_get_regs(adevnum);
  if (!dregs)
  {
    return false;
  }

  if (smnum > 4)
  {
    return false;
  }

  regs = &dregs->sm[smnum];

  Stop();  // ensure that the state machine is not running

  tx_lsb   = (uint32_t *)&dregs->txf[smnum];
  tx_msb8 = (uint8_t *)tx_lsb;
  tx_msb8 += 3;
  tx_msb16 = (uint16_t *)tx_lsb;
  tx_msb16 += 1;

  rx_lsb   = (uint32_t *)&dregs->rxf[smnum];
  rx_msb8 = (uint8_t *)rx_lsb;
  rx_msb8 += 3;
  rx_msb16 = (uint16_t *)rx_lsb;
  rx_msb16 += 1;

  tx_fifo_full_bit  = (1 << (16 + smnum));
  rx_fifo_emtpy_bit = (1 << ( 8 + smnum));

  SetClkDiv(clkdiv);

  initialized = true;
  return true;
}

void THwRpPioSm::SetPrg(THwRpPioPrg * aprg)
{
  prg = aprg;

  if (prg->wrap_end > prg->length)  prg->wrap_end = prg->length;

  execctrl &= ~(0x3FF << 7); // clear WRAP_TOP, WRAP_BOTTOM
  execctrl |=  (((prg->offset + prg->wrap_start) << 7) | ((prg->offset + prg->wrap_end - 1) << 12));
  regs->execctrl = execctrl;
}

void THwRpPioSm::SetupPinsSideSet(unsigned abase, unsigned acount)
{
  sideset_len = (acount & 7);
  pinctrl &= ~((0x1F  << 10) | (0x7         << 29));
  pinctrl |=  ((abase << 10) | (sideset_len << 29));
  regs->pinctrl = pinctrl;
  SetupPioPins(abase, acount);
}

void THwRpPioSm::SetupPinsSet(unsigned abase, unsigned acount)
{
  pinctrl &= ~((0x1F  << 5) | (0x7    << 26));
  pinctrl |=  ((abase << 5) | (acount << 26));
  regs->pinctrl = pinctrl;
  SetupPioPins(abase, acount);
}

void THwRpPioSm::SetupPinsOut(unsigned abase, unsigned acount)
{
  pinctrl &= ~((0x1F  << 0) | (0x3F   << 20));
  pinctrl |=  ((abase << 0) | (acount << 20));
  regs->pinctrl = pinctrl;
  SetupPioPins(abase, acount);
}

void THwRpPioSm::SetupPinsIn(unsigned abase, unsigned acount)
{
  pinctrl &= ~((0x1F  << 15));
  pinctrl |=  ((abase << 15));
  regs->pinctrl = pinctrl;
  SetupPioPins(abase, acount);
}

void THwRpPioSm::SetClkDiv(uint32_t aclkdiv)
{
  clkdiv = aclkdiv;
  regs->clkdiv = clkdiv;
}

void THwRpPioSm::SetClkDiv(uint32_t abasespeed, uint32_t targetfreq)
{
  unsigned divi = abasespeed / targetfreq;
  unsigned divf;
  if (divi < 1)
  {
    divi = 1;
    divf = 0;
  }
  else
  {
    divf = ((abasespeed - divi * targetfreq) << 8) / targetfreq;
  }
  SetClkDiv((divi << 16) + (divf << 8));
}

void THwRpPioSm::SetupPioPins(unsigned abase, unsigned acount)
{
  int af = (1 == devnum ? PINCFG_AF_7 : PINCFG_AF_6);
  for (unsigned n = 0; n < acount; ++n)
  {
    hwpinctrl.PinSetup(0, abase + n, af);
  }
}

void THwRpPioSm::SetPinDir(uint32_t apin, unsigned aoutput)
{
  uint32_t prev_pinctrl = regs->pinctrl;

  regs->pinctrl = (apin << 5) | (1 << 26);   // change the pinctrl for the SET instructions
  regs->instr = pio_encode_set(pio_pindirs, aoutput);
  regs->pinctrl = prev_pinctrl;
}

void THwRpPioSm::Start()
{
  if (!initialized)
  {
    return;
  }

  regs->shiftctrl = shiftctrl;
  regs->execctrl  = execctrl;

  regs->instr = pio_encode_jmp(prg->entry);  // force jump to the entry point

  uint32_t tmp = (dregs->ctrl & 0xF);
  tmp |= (1 << smnum); // start the state machine
  dregs->ctrl = tmp;
}

void THwRpPioSm::Stop()
{
  uint32_t tmp = (dregs->ctrl & 0xF);
  tmp &= ~(1 << smnum); // start the state machine
  dregs->ctrl = tmp;
}

void THwRpPioSm::SetOutShift(bool shift_right, bool autopull, unsigned threshold)
{
  shiftctrl &= ~(PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_BITS | PIO_SM0_SHIFTCTRL_AUTOPULL_BITS | PIO_SM0_SHIFTCTRL_PULL_THRESH_BITS);
  if (shift_right)  shiftctrl |= PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_BITS;
  if (autopull)     shiftctrl |= PIO_SM0_SHIFTCTRL_AUTOPULL_BITS;
  shiftctrl |= ((threshold & 0x1F) << PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB);

  regs->shiftctrl = shiftctrl;
}

void THwRpPioSm::SetInShift(bool shift_right, bool autopush, unsigned threshold)
{
  shiftctrl &= ~(PIO_SM0_SHIFTCTRL_IN_SHIFTDIR_BITS | PIO_SM0_SHIFTCTRL_AUTOPUSH_BITS | PIO_SM0_SHIFTCTRL_PUSH_THRESH_BITS);
  if (shift_right)  shiftctrl |= PIO_SM0_SHIFTCTRL_IN_SHIFTDIR_BITS;
  if (autopush)     shiftctrl |= PIO_SM0_SHIFTCTRL_AUTOPUSH_BITS;
  shiftctrl |= ((threshold & 0x1F) << PIO_SM0_SHIFTCTRL_PUSH_THRESH_LSB);

  regs->shiftctrl = shiftctrl;
}

bool THwRpPioSm::TrySend32(uint32_t adata)
{
  if (dregs->fstat & tx_fifo_full_bit) // TX FIFO FULL ?
  {
    return false;
  }

  *tx_lsb = adata;
  return true;
}

bool THwRpPioSm::TrySend16(uint16_t adata)
{
  if (dregs->fstat & tx_fifo_full_bit) // TX FIFO FULL ?
  {
    return false;
  }

  *tx_lsb = adata;
  return true;
}

bool THwRpPioSm::TrySend8(uint8_t adata)
{
  if (dregs->fstat & tx_fifo_full_bit) // TX FIFO FULL ?
  {
    return false;
  }

  *tx_lsb = adata;
  return true;
}


bool THwRpPioSm::TryRecv32(uint32_t * adata)
{
  if (dregs->fstat & rx_fifo_emtpy_bit) // RX FIFO EMPTY ?
  {
    return false;
  }

  *adata = *rx_lsb;
  return true;
}

bool THwRpPioSm::TryRecv16(uint16_t * adata)
{
  if (dregs->fstat & rx_fifo_emtpy_bit) // RX FIFO EMPTY ?
  {
    return false;
  }

  *adata = *(uint16_t *)rx_lsb;
  return true;
}

bool THwRpPioSm::TryRecv8(uint8_t * adata)
{
  if (dregs->fstat & rx_fifo_emtpy_bit) // RX FIFO EMPTY ?
  {
    return false;
  }

  *adata = *(uint8_t *)rx_lsb;
  return true;
}


bool THwRpPioSm::TrySendMsb8(uint8_t adata)
{
  if (dregs->fstat & tx_fifo_full_bit) // TX FIFO FULL ?
  {
    return false;
  }

  *tx_msb8 = adata;
  return true;
}

bool THwRpPioSm::TryRecvMsb8(uint8_t * adata)
{
  if (dregs->fstat & rx_fifo_emtpy_bit) // RX FIFO EMPTY ?
  {
    return false;
  }

  *adata = *rx_msb8;
  return true;
}


bool THwRpPioSm::TrySendMsb16(uint16_t adata)
{
  if (dregs->fstat & tx_fifo_full_bit) // TX FIFO FULL ?
  {
    return false;
  }

  *tx_msb16 = adata;
  return true;
}

bool THwRpPioSm::TryRecvMsb16(uint16_t * adata)
{
  if (dregs->fstat & rx_fifo_emtpy_bit) // RX FIFO EMPTY ?
  {
    return false;
  }

  *adata = *rx_msb16;
  return true;
}
