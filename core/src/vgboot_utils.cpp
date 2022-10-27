/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     vgboot_utils.cpp
 *  brief:    VGBOOT Checksum Calculations
 *  date:     2022-10-27
 *  authors:  nvitya
*/

#include "platform.h"
#include "app_header.h"

uint32_t vgboot_checksum(void * pdata, unsigned datalen)
{
  uint32_t *  pu32 = (uint32_t *)pdata;
  uint32_t *  pu32_end = pu32 + (datalen >> 2);
  uint32_t    csum = 0;
  while (pu32 < pu32_end)
  {
    csum += *pu32++;
  }

  uint32_t rbytes = (datalen & 3); // remaining bytes
  if (rbytes)
  {
    uint32_t d = *pu32;
    if       (rbytes == 3)  csum += (d & 0x00FFFFFF);
    else if  (rbytes == 2)  csum += (d & 0x0000FFFF);
    else if  (rbytes == 1)  csum += (d & 0x000000FF);
  }
  return -csum;
}


