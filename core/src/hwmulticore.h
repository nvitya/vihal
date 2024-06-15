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
// file:     hwmulticore.h
// brief:    Some Multi-Core Handling Utilities
// created:  2024-06-16


#ifndef HWMULTICORE_H_PRE_
#define HWMULTICORE_H_PRE_

#include "platform.h"

class THwMultiCore_pre
{
public:
  bool      initialized = false;
  uint8_t   coreid = 0;
};

#endif // ndef HWMULTICORE_H_PRE_

#ifndef HWMULTICORE_PRE_ONLY

//-----------------------------------------------------------------------------


#ifndef HWMULTICORE_H_
#define HWMULTICORE_H_

#include "mcu_impl.h"

// fallback implementation

class THwMultiCore_noimpl : public THwMultiCore_pre
{
public: // mandatory
  bool      Init(uint8_t acoreid) { coreid = acoreid; return false; }

  bool      TryIpcSend(uint32_t * data) { return false; }
  bool      TryIpcRecv(uint32_t * data) { return false; }
  bool      IpcSend(uint32_t * data, uint32_t timeout_us) { return false; }
  bool      IpcRecv(uint32_t * data, uint32_t timeout_us) { return false; }

  void      ResetCore(uint8_t acoreid) { }
  bool      StartCore(uint8_t acoreid, uint32_t aentry, uint32_t astack, uint32_t avectors) { return false; }
};

#ifndef HWMULTICORE_IMPL

#define HWMULTICORE_IMPL   THwMultiCore_noimpl

#endif // ndef HWMULTICORE_IMPL

//-----------------------------------------------------------------------------

class THwMultiCore : public HWMULTICORE_IMPL
{
public:
};

extern THwMultiCore  multicore;

#endif /* HWMULTICORE_H_ */

#else
  #undef HWMULTICORE_PRE_ONLY
#endif

