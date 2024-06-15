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
// file:     hwmulticore_rp.cpp
// brief:    VIHAL RP2040 Multi-Core Utilities
// created:  2024-06-15
// authors:  nvitya

#include "hwmulticore.h"
#include "hwmulticore_rp.h"
#include "hwuscounter.h"

void THwMultiCore_rp::Init(uint8_t acoreid)
{
  coreid = acoreid;

  mbregs     = (TRpMailBoxRegs *)(SIO_BASE + RP_FIFO_REGS_OFFSET);
  //regs_set = (TRpMailBoxRegs *)(SIO_BASE + RP_FIFO_REGS_OFFSET + 0x2000);
  //regs_clr = (TRpMailBoxRegs *)(SIO_BASE + RP_FIFO_REGS_OFFSET + 0x3000);

  if (!uscounter.initialized)  uscounter.Init();
}

bool THwMultiCore_rp::TryIpcSend(uint32_t * data)
{
  if (mbregs->ST & RP_MBX_FIFO_ST_RDY)
  {
    mbregs->WR = *data;
    __SEV();  // send the event to the other core
    return true;
  }
  else
  {
    return false;
  }
}

bool THwMultiCore_rp::TryIpcRecv(uint32_t * data)
{
  if (mbregs->ST & RP_MBX_FIFO_ST_VLD)
  {
    *data = mbregs->RD;
    return true;
  }
  else
  {
    return false;
  }
}

bool THwMultiCore_rp::IpcSend(uint32_t * data, uint32_t timeout_us)
{
  uint32_t st = 0;
  while (!TryIpcSend(data))
  {
    if (timeout_us)
    {
      if (0 == st)
      {
        st = uscounter.Get32();
      }
      else if (uscounter.Get32() - st > timeout_us)
      {
        return false;
      }
    }
  }
  return true;
}

bool THwMultiCore_rp::IpcRecv(uint32_t * data, uint32_t timeout_us)
{
  uint32_t st = 0;
  while (!TryIpcRecv(data))
  {
    if (timeout_us)
    {
      if (0 == st)
      {
        st = uscounter.Get32();
      }
      else if (uscounter.Get32() - st > timeout_us)
      {
        return false;
      }
    }
  }
  return true;
}


#define PSM_FRCE_OFF_OFFSET       0x00000004
#define PSM_FRCE_OFF_PROC1_BITS   (1 << 16)
#define PSM_FRCE_OFF_PROC0_BITS   (1 << 15)

void THwMultiCore_rp::ResetCore(uint8_t acoreid)
{
  uint32_t procbit = PSM_FRCE_OFF_PROC1_BITS;
  if (0 == acoreid)
  {
    procbit = PSM_FRCE_OFF_PROC0_BITS;
    return;
  }

  // Use atomic aliases just in case core 1 is also manipulating some PSM state
  io_rw_32 * power_off     = (io_rw_32 *) (PSM_BASE + PSM_FRCE_OFF_OFFSET);
  io_rw_32 * power_off_set = hw_set_alias(power_off);
  io_rw_32 * power_off_clr = hw_clear_alias(power_off);

  // Hard-reset core 1.
  // Reading back confirms the core 1 reset is in the correct state, but also
  // forces APB IO bridges to fence on any internal store buffering
  *power_off_set = procbit;
  while (!(*power_off & procbit))
  {
    __NOP();
  };

  // Bring core 1 back out of reset. It will drain its own mailbox FIFO, then push
  // a 0 to our mailbox to tell us it has done this.
  *power_off_clr = procbit;
}

bool THwMultiCore_rp::StartCore(uint8_t acoreid, uint32_t aentry, uint32_t astack, uint32_t avectors)
{
  // this works only for the secondary core !
  if (acoreid != 1)
  {
    return false;
  }

  // vector_table is value for VTOR register
  // sp is initial stack pointer (SP)
  // entry is the initial program counter (PC) (don't forget to set the thumb bit!)
  const uint32_t cmd_sequence[] = { 0, 0, 1, avectors, astack, aentry };

  bool     bok = true;
  uint32_t response = 0;
  uint32_t seq = 0;
  do
  {
    uint32_t cmd = cmd_sequence[seq];
    // Always drain the READ FIFO (from core 1) before sending a 0
    if (!cmd)
    {
      while (TryIpcRecv(&response)) { }
      //multicore_fifo_drain();

      // Execute a SEV as core 1 may be waiting for FIFO space via WFE
      __SEV();
    }

    if (!IpcSend(&cmd, 10000))
    {
      return false;
    }
    if (!IpcRecv(&response, 10000))
    {
      return false;
    }

    // Move to next state on correct response (echo-d value) otherwise start over
    if (cmd != response)
    {
      seq = 0; // start over
    }
    else
    {
      ++seq;
    }
  }
  while (seq < (sizeof(cmd_sequence) >> 2));

  return true;
}

