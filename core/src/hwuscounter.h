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
// file:     hwuscounter.h
// brief:    us (microseconds) counter using some internal timer hardware
// created:  2024-04-06
// authors:  nvitya

#ifndef HWUSCOUNTER_H_PRE_
#define HWUSCOUNTER_H_PRE_

#include "platform.h"

class THwUsCounter_pre
{
public:
  bool      initialized = false;
};

#endif // ndef HWUSCOUNTER_H_PRE_

#ifndef HWUSCOUNTER_PRE_ONLY

//-----------------------------------------------------------------------------

#ifndef HWUSCOUNTER_H_
#define HWUSCOUNTER_H_

#include "mcu_impl.h"

// fallback implementation using the CLOCKCNT

class THwUsCounter_noimpl : public THwUsCounter_pre
{
public: // mandatory
  bool      Init();
  uint32_t  Get32();
  uint64_t  Get64(); // must be called at least every half hour
protected:
  uint32_t  cnt32;
  uint32_t  last_clkcnt_ud; // last microseconds clock counter (without remainder)
  uint32_t  clocks_per_us;

  uint32_t  cnt32_prev = 0;
  uint64_t  cnt64_high = 0;
};

#ifndef HWUSCOUNTER_IMPL

//#warning "HWUSCOUNTER is not implemented!"

#define HWUSCOUNTER_IMPL   THwUsCounter_noimpl

#endif // ndef HWUSCOUNTER_IMPL

//-----------------------------------------------------------------------------

class THwUsCounter : public HWUSCOUNTER_IMPL
{
};

extern THwUsCounter  uscounter;

#endif /* HWUSCOUNTER_H_ */

#else
  #undef HWUSCOUNTER_PRE_ONLY
#endif
