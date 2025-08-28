/*
 * hwrppio.h
 *
 *  Created on: Aug 13, 2023
 *      Author: vitya
 */

#ifndef SRC_HWRPPIO_H_
#define SRC_HWRPPIO_H_

#include "platform.h"
#include "hwrppio_instructions.h"

enum EPioIrqLine
{
  Irq0,
  Irq1
};

class THwRpPioPrg
{
public:
  uint8_t   offset = 0;
  uint8_t   length = 0;
  uint8_t   wrap_start = 0;  // relative to the
  uint8_t   wrap_end = 255;  // 255: last line of the program, one less will be written into WRAP_TOP
  uint8_t   entry = 0; // same as offset

  pio_hw_t *  dregs = nullptr;

  void      Init(uint8_t adevnum, uint8_t aoffset);
  void      Clear();
  void      Add(uint16_t ainstr);

/*
  void      AddWait();
  void      AddIn();
  void      AddOut();
  void      AddPush();
  void      AddPull();
  void      AddMov();
  void      AddIrq();
  void      AddSet();
*/
};

class THwRpPioDev
{
public:

};

class THwRpPioSm
{
public:  // optimization hint: the first 32 variables / addresses are accessed faster, they must be ordered by their size
  uint8_t        devnum = 0;
  uint8_t        smnum  = 0;
  uint8_t        sideset_len = 0;
  bool           initialized = false;

  pio_sm_hw_t *  regs = nullptr;
  pio_hw_t *     dregs = nullptr;

  uint32_t       rx_fifo_emtpy_bit = 0;
  uint32_t       tx_fifo_full_bit = 0;

  uint8_t *      tx_msb8  = nullptr;
  uint16_t *     tx_msb16 = nullptr;
  uint32_t *     tx_lsb   = nullptr;

  uint8_t *      rx_msb8  = nullptr;
  uint16_t *     rx_msb16 = nullptr;
  uint32_t *     rx_lsb   = nullptr;

  THwRpPioPrg *  prg = nullptr;
  uint32_t       pinctrl = 0;
  uint32_t       clkdiv = 0x10000;    // 1 = no clk division, runs with the SystemCoreClock
  uint32_t       execctrl = 0;        // some fields will be updated on SetPrg()
  uint32_t       shiftctrl = 0x000C0000;  // keep the defaults (IN_SHIFTDIR=1, OUT_SHIFTDIR=1)

  bool Init(uint8_t adevnum, uint8_t asmnum);
  void ClearFifos();
  void JoinFifosForTx();
  void JoinFifosForRx();
  void SetPrg(THwRpPioPrg * aprg);
  void Start();
  void Stop();

  void SetClkDiv(uint32_t aclkdiv);
  void SetClkDiv(uint32_t abasespeed, uint32_t targetfreq);

  inline void IrqClear(uint32_t amask) { dregs->irq = amask; }
  inline void IrqForce(uint32_t amask) { dregs->irq_force = amask; }
  inline void IrqEnable(uint32_t amask, EPioIrqLine airq_line) { dregs->irq_ctrl[airq_line].inte |= amask; }

  void SetPinDir(uint32_t apin, unsigned aoutput);
  void SetOutputs(uint32_t apin, uint32_t amask); // can be used to initialize the output states
  void SetOutShift(bool shift_right, bool autopull, unsigned threshold);
  void SetInShift(bool shift_right, bool autopush, unsigned threshold);

  void SetupPinsSideSet(unsigned abase, unsigned acount, bool optional, bool pindir);
  void SetupPinsSet(unsigned abase, unsigned acount);
  void SetupPinsOut(unsigned abase, unsigned acount, unsigned aextra_flags = 0x0);
  void SetupPinsIn(unsigned abase, unsigned acount, unsigned aextra_flags = 0x0);
  void SetupPinsJmp(unsigned abase, unsigned acount);

  inline void PreloadX(unsigned avalue, unsigned abits)
  {
    *tx_lsb = avalue;
    regs->instr = pio_encode_out(pio_x, abits);
  }

  void PreloadY(unsigned avalue, unsigned abits)
  {
    *tx_lsb = avalue;
    regs->instr = pio_encode_out(pio_y, abits);
  }

  uint32_t GetDmaRequest(bool istx);

  bool TrySend32(uint32_t adata);
  bool TrySend16(uint16_t adata);
  bool TrySend8(uint8_t adata);

  bool TryRecv32(uint32_t * adata);
  bool TryRecv16(uint16_t * adata);
  bool TryRecv8(uint8_t * adata);

  bool TrySendMsb8(uint8_t adata);
  bool TrySendMsb16(uint16_t adata);

  bool TryRecvMsb8(uint8_t * adata);
  bool TryRecvMsb16(uint16_t * adata);

public:
  void SetupPioPins(unsigned abase, unsigned acount, unsigned aextra_flags = 0x0);

};

class THwRpPioApp
{
public:
  bool      initialized = false;
  uint8_t   prgoffset = 0;

  virtual ~THwRpPioApp() { }
};

#endif /* SRC_HWRPPIO_H_ */
