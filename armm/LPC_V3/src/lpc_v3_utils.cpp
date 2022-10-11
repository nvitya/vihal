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
 *  file:     lpc_v3_utils.cpp
 *  brief:    LPC v3 (LPC54x, LPC55x) utilities
 *  date:     2022-10-11
 *  authors:  nvitya
*/

#include "lpc_v3_utils.h"

static const unsigned fcbaseaddr[] = FLEXCOMM_BASE_ADDRS;

FLEXCOMM_Type * flexcomm_init(int fcnum, int pselid)
{
  if ((fcnum < 0) || (fcnum >= FLEXCOMM_COUNT))
  {
    return nullptr;
  }

  // turn on the flexcomm hw:
  if (fcnum <= 7)
  {
    SYSCON->AHBCLKCTRLSET[1] = (1 << (11 + fcnum));
  }
  else
  {
    SYSCON->AHBCLKCTRLSET[2] = (1 << (14 + (fcnum - 8)));
  }

  // set the comm type
  FLEXCOMM_Type * fc = (FLEXCOMM_Type *)(fcbaseaddr[fcnum]);
  fc->PSELID = pselid;

  #ifdef SYSCON_FCCLKSELX_COUNT
    SYSCON->FCCLKSELX[fcnum] = 3; // select the 96 MHz free running oscillator as clock source
  #else
    SYSCON->FCLKSEL[fcnum] = 1; // select the 48 MHz free running oscillator as clock source
  #endif

  return fc;
}
