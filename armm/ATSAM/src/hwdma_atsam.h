/* -----------------------------------------------------------------------------
 * This file is a part of the NVCM project: https://github.com/nvitya/nvcm
 * Copyright (c) 2018 Viktor Nagy, nvitya
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
 *  file:     hwdma_atsam.h
 *  brief:    ATSAM DMA
 *  version:  1.00
 *  date:     2018-02-10
 *  authors:  nvitya
*/

#ifndef HWDMA_ATSAM_H_
#define HWDMA_ATSAM_H_

#define HWDMA_PRE_ONLY
#include "hwdma.h"

#if defined(XDMAC)
  #define HW_DMA_REGS  XdmacChid    // e.g. ATSAME70
#elif defined(DMAC)
  #define HW_DMA_REGS  DmacCh_num   // e.g. ATSAM3X
#else
  #define HW_DMA_REGS  void  // the older versions don't have central DMA
#endif

#ifdef HW_HAS_PDMA

  // DMA channel coupled to the peripheral, in documentation referred as PDC: Peripheral DMA Controller
  // (older Atmel chips)
  typedef struct TPdmaRegs
  {
    __IO uint32_t RPR;      /**< \brief (Offset: 0x100) Receive Pointer Register */
    __IO uint32_t RCR;      /**< \brief (Offset: 0x104) Receive Counter Register */
    __IO uint32_t TPR;      /**< \brief (Offset: 0x108) Transmit Pointer Register */
    __IO uint32_t TCR;      /**< \brief (Offset: 0x10C) Transmit Counter Register */
    __IO uint32_t RNPR;     /**< \brief (Offset: 0x110) Receive Next Pointer Register */
    __IO uint32_t RNCR;     /**< \brief (Offset: 0x114) Receive Next Counter Register */
    __IO uint32_t TNPR;     /**< \brief (Offset: 0x118) Transmit Next Pointer Register */
    __IO uint32_t TNCR;     /**< \brief (Offset: 0x11C) Transmit Next Counter Register */
    __O  uint32_t PTCR;     /**< \brief (Offset: 0x120) Transfer Control Register */
    __I  uint32_t PTSR;     /**< \brief (Offset: 0x124) Transfer Status Register */
  //
  } TPdmaRegs;

  #define HW_PDMA_REGS  TPdmaRegs

#endif

class THwDmaChannel_atsam : public THwDmaChannel_pre
{
public:
	unsigned           chbit = 0;
	int                perid = -1;
  bool               rxvoid = false;     // special flags to signalize that RX DMA was not activated via DMATR_NO_DST_INC flag
  unsigned           orig_count = 0;     // saved original count to handle Remaining() from completed count
	HW_DMA_REGS *      regs = nullptr;

	bool Init(int achnum, int aperid);

	void Prepare(bool aistx, void * aperiphaddr, unsigned aflags);

#if defined(HW_PDMA_REGS)

	HW_PDMA_REGS *     pdmaregs = nullptr;  // Some Atmel systems have two different DMA system
  bool InitPeriphDma(bool aistx, void * aregs, void * aaltregs);  // special function for Atmel PDMA

#endif

	inline void  StartPreparedTransfer() { Enable(); }
  inline bool  Active()  { return Enabled(); }

#if defined(XDMAC)
  inline void      Enable()     { XDMAC->XDMAC_GE = chbit; }
  inline bool      Enabled()    { return ((XDMAC->XDMAC_GS & chbit) != 0); }
  void             Disable();
  inline unsigned  Remaining()  { return regs->XDMAC_CUBC; }

  void             PrepareTransfer(THwDmaTransfer * axfer);
#else
  void             Enable();
  bool             Enabled();
  void             Disable();
  unsigned         Remaining();

  void             PrepareTransfer(THwDmaTransfer * axfer);
#endif

#if defined(HW_PDMA_REGS)
private:
  ALWAYS_INLINE void      PerDmaEnable();
  ALWAYS_INLINE bool      PerDmaEnabled();
  ALWAYS_INLINE void      PerDmaDisable();
  ALWAYS_INLINE unsigned  PerDmaRemaining();
  ALWAYS_INLINE void      PerDmaPrepareTransfer(THwDmaTransfer * axfer);
#endif

#if defined(DMAC)
private:
  ALWAYS_INLINE void      DmacEnable();
  ALWAYS_INLINE bool      DmacEnabled();
  ALWAYS_INLINE void      DmacDisable();
  ALWAYS_INLINE unsigned  DmacRemaining();
  ALWAYS_INLINE void      DmacPrepareTransfer(THwDmaTransfer * axfer);
#endif

};

#define HWDMACHANNEL_IMPL  THwDmaChannel_atsam

#endif // def HWDMA_ATSAM_H_
