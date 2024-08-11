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
 *  file:     hwdma_stm32_v3.cpp
 *  brief:    DMA for the STM32H7R/S series: GPDMA + HPDMA
 *  date:     2024-08-10
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>

#include "hwdma.h"

#if defined(HWDMA_V3)

#include "hwdma_stm32_v3.h"

__attribute__((section("bss_RAM2"), aligned(1024)))  // target AXI RAM
THwDmaLli3 hwdma_lli_block[32];  // 512 Byte

const unsigned char stm32_dma_irq_status_shifts[8] = {0, 6, 16, 22, 0, 6, 16, 22};

bool THwDmaChannel_stm32::Init(int admanum, int achannel, int arequest)
{
	initialized = false;

	regs = nullptr;
	gregs = nullptr;
	plli3 = nullptr;

	int dma = (admanum & 0x01);
	int ch  = (achannel & 0x0F);

	uint32_t channel_base_addr;

	if (HWDMA_GPDMA == dma)
	{
		RCC->AHB1ENR |= RCC_AHB1ENR_GPDMA1EN;
		gregs = GPDMA1;
		channel_base_addr = (GPDMA1_BASE + 0x50);
	}
	else if (HWDMA_HPDMA == dma)
	{
		RCC->AHB5ENR |= RCC_AHB5ENR_HPDMA1EN;
    gregs = HPDMA1;
    channel_base_addr = (HPDMA1_BASE + 0x50);
	}
	else
	{
		return false;
	}

  dmanum = dma;
  chnum = ch;
  rqnum = (arequest & 0x7F);

  regs = (DMA_Channel_TypeDef *)(channel_base_addr + 0x80 * ch);
  plli3 = &hwdma_lli_block[dmanum * 16 + chnum];

  Disable();  // suspend + reset when enabled

  // initialize static registers

  ccr_base = (0
    |  ((priority & 3) << 22)  // PRIO(2)
    |  (0              << 17)  // LAP: linked list read port: 0 = AXI/AHB, 1 = AHB
  );

  regs->CCR = ccr_base;

  regs->CLBAR = (uint32_t(plli3) & 0xFFFF0000); // set the linked list base address

  // no special 2D mode support here
  regs->CTR3 = 0;
  regs->CBR2 = 0;

  ClearIrqFlag();

	Prepare(true, nullptr, 0); // set some defaults

	initialized = true;

	return true;
}

void THwDmaChannel_stm32::Prepare(bool aistx, void * aperiphaddr, unsigned aflags)
{
	istx = aistx;
	periphaddr = aperiphaddr;
}

void THwDmaChannel_stm32::Disable()
{
  uint32_t ccr = regs->CCR;
  if (ccr & 1)
  {
    while (0 == (ccr & (1 << 2))) // not suspended ?
    {
      regs->CCR = (ccr | (1 << 2)); // request suspend
      ccr = regs->CCR;
    }
    regs->CCR = (ccr | (1 << 1));  // Reset
    while (regs->CCR & 1)
    {
      // wait until the reset finishes
    }
  }
}

void THwDmaChannel_stm32::Enable()
{
	regs->CCR |= 1;  // start the channel
}

void THwDmaChannel_stm32::PrepareTransfer(THwDmaTransfer * axfer)
{
  uint32_t ccr = regs->CCR;
  if (ccr & 1) // channel enabled ?
  {
    Disable();  // this will suspend, then reset the channel
  }

	ClearIrqFlag();

  // size decoding using a micro-table:
  //                      87654321
  uint32_t sizecode = ((0x33332210 >> ((axfer->bytewidth-1) << 2)) & 0xF);

  uint32_t tr1 = (0
    | (0        << 30)  // DAP: destination bus port: 0 = AXI/AHB, 1 = AHB
    | (0        << 28)  // DWX: destination word exchange in 64-bit double-words
    | (0        << 27)  // DHX: destination half-word exchange in 32-bit words
    | (0        << 26)  // DBX: destination byte-exchange in 16-bit half-words
    | (0        << 20)  // DBL_1(6): destination burst length - 1
    | (0        << 19)  // DINC: Destination incrementing, 0 = fixed (burst), 1 = continuosly incremented
    | (sizecode << 16)  // DDW_LOG2(2): Destination width: 0 = byte, 1 = 16-bit, 2 = 32-bit, 3 = 64-bit (AXI only)
    | (0        << 14)  // SAP: source bus port: 0 = AXI/AHB, 1 = AHB
    | (0        << 13)  // SBX: source byte-exchange in 16-bit half-words
    | (0        << 11)  // PAM(2): padding + aligment if the src+dst sizes do not match. 0 = right aligned + zero padded
    | (0        <<  4)  // SBL_1(6): source burst length - 1
    | (0        <<  3)  // SINC: Source incrementing, 0 = fixed (burst), 1 = continuosly incremented
    | (sizecode <<  0)  // SDW_LOG2(2): Source width: 0 = byte, 1 = 16-bit, 2 = 32-bit, 3 = 64-bit (AXI only)
  );

  uint32_t tr2 = (0
    | (0        << 30)  // TCEM(2): Transfer complete event, 0 = block level, 2 = LLI level, 3 = channel level
    | (0        << 24)  // TRIGPOL(2): 0 = no trigger, 1 = rising edge, 2 = falling edge
    | (0        << 16)  // TRIGSEL(6): trigger event input selection
    | (0        << 14)  // TRIGM(2): trigger mode
    | (per_flow_controller << 12)  // PFREQ: Peripheral flow control
    | (0        << 11)  // BREQ: Block hardware request, 0 = one HW rq. per burst, 1 = one HW requests per block
    | (0        << 10)  // DREQ: Destination hw rq., 0 = source hw rq., 1 = destination hw rq.
    | (0        <<  9)  // SWREQ: software request
    | (rqnum    <<  0)  // REQSEL(7): hw request selection
  );

	if (axfer->flags & DMATR_MEM_TO_MEM)
	{
	  tr1 |= (0
      | (1 << 19)  // DINC: Destination incrementing, 0 = fixed (burst), 1 = continuosly incremented
      | (1 <<  3)  // SINC: Source incrementing, 0 = fixed (burst), 1 = continuosly incremented
	  );
    tr2 |= (0
      | (1 <<  9)  // SWREQ: software request
    );
    regs->CSAR = (uint32_t)axfer->srcaddr;
    regs->CDAR = (uint32_t)axfer->dstaddr;
	}
	else if (istx)  // MEM -> PER
	{
    tr1 |= (0
      | (per_bus_port << 30)  // DAP: destination bus port: 0 = AXI/AHB, 1 = AHB
      | (1            <<  3)  // SINC: Source incrementing, 0 = fixed (burst), 1 = continuosly incremented
    );
    tr2 |= (0
      | (1 << 10)  // DREQ: Destination hw rq., 0 = source hw rq., 1 = destination hw rq.
    );
    regs->CSAR = (uint32_t)axfer->srcaddr;
    regs->CDAR = (uint32_t)periphaddr;
	}
	else            // PER -> MEM
	{
    tr1 |= (0
      | (1            << 19)  // DINC: Destination incrementing, 0 = fixed (burst), 1 = continuosly incremented
      | (per_bus_port << 14)  // SAP: source bus port: 0 = AXI/AHB, 1 = AHB
    );
    //tr2 |= (0
    //  | (0 << 10)  // DREQ: Destination hw rq., 0 = source hw rq., 1 = destination hw rq.
    //);
    regs->CSAR = (uint32_t)periphaddr;
    regs->CDAR = (uint32_t)axfer->dstaddr;
	}

	regs->CTR1 = tr1;
	regs->CTR2 = tr2;

	uint32_t bytecount = axfer->count * axfer->bytewidth;
	regs->CBR1 = bytecount;

	ccr = ccr_base;
	if (axfer->flags & DMATR_IRQ)
	{
	  ccr |= (1 << 8); // TCIE: transfer coplete interrupt enable
	}
  if (axfer->flags & DMATR_IRQ_HALF)
  {
    ccr |= (1 << 9); // HTIE: half transfer coplete interrupt enable
  }

  if (axfer->flags & DMATR_CIRCULAR)
  {
    uint32_t lli = (0
      | (1 << 29)   // UB1: update/load the CBR1 (byte count) register
      | (1 << 16)   // ULL: update/load the CLLR register
      | ((uint32_t(plli3) & 0xFFFF) << 0) // LA(16)
    );

    if (istx)  // MEM -> PER
    {
      lli |= (1 << 28);  // USA: update/load the CSAR (source address) register)
      plli3->SA_DA = (uint32_t)axfer->srcaddr;
    }
    else       // PER -> MEM
    {
      lli |= (1 << 27);  // UDA: update/load the CDAR (destination address) register)
      plli3->SA_DA = (uint32_t)axfer->dstaddr;
    }

    plli3->CBR = bytecount;
    plli3->NEXTLLI = lli;

    regs->CLLR = lli;
  }
  else
  {
    regs->CLLR = 0; // no linked list usage
  }
}

#endif // if HWDMA_V3
