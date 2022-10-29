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
 *  file:     app_header.h
 *  brief:    Generic application header definition for own bootloaders
 *  date:     2022-06-01
 *  authors:  nvitya
*/

#ifndef APP_HEADER_H_
#define APP_HEADER_H_

#include "stdint.h"

#define APP_HEADER_SIGNATURE  0xAA559966

typedef struct TAppHeader
{
   uint32_t  signature;
   uint32_t  length;
   uint32_t  addr_load;
   uint32_t  addr_entry;

   uint32_t  customdata;
   uint32_t  compid;        // compatibility id (optional)
   uint32_t  csum_body;     // body checksum (optional)
   uint32_t  csum_head;     // header checksum (optional)
//
} TAppHeader;  // 32 bytes

#endif
