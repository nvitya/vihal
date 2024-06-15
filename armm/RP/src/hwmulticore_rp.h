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
// file:     hwmulticore_rp.h
// brief:    VIHAL RP2040 Multi-Core Utilities
// created:  2024-06-15
// authors:  nvitya

#ifndef HWMULTICORE_RP_H_
#define HWMULTICORE_RP_H_

#define HWMULTICORE_PRE_ONLY
#include "hwmulticore.h"

#define RP_FIFO_REGS_OFFSET  0x050

typedef struct
{
  volatile uint32_t  ST;
  volatile uint32_t  WR;
  volatile uint32_t  RD;
//
} TRpMailBoxRegs;

#define RP_MBX_FIFO_ST_VLD  (1 << 0)
#define RP_MBX_FIFO_ST_RDY  (1 << 1)
#define RP_MBX_FIFO_ST_WOF  (1 << 2)
#define RP_MBX_FIFO_ST_ROE  (1 << 3)

class THwMultiCore_rp : public THwMultiCore_pre
{
public:
  TRpMailBoxRegs *  mbregs = nullptr;

  void      Init(uint8_t acoreid);

  bool      TryIpcSend(uint32_t * data);
  bool      TryIpcRecv(uint32_t * data);
  bool      IpcSend(uint32_t * data, uint32_t timeout_us);
  bool      IpcRecv(uint32_t * data, uint32_t timeout_us);

  void      ResetCore(uint8_t acoreid);
  bool      StartCore(uint8_t acoreid, uint32_t aentry, uint32_t astack, uint32_t avectors);
};

#define HWMULTICORE_IMPL THwMultiCore_rp

#endif /* HWMULTICORE_RP_H_ */
