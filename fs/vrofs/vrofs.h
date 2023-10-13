/* -----------------------------------------------------------------------------
 * VROFS - Read Only File System for Embedded Systems
 * Copyright (c) 2023 Viktor Nagy, nvitya
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
 *  file:     vrofs.h
 *  brief:    Main VROFS Header and Specification
 *  created:  2023-10-11
 *  authors:  nvitya
*/

#ifndef SRC_VROFS_H_
#define SRC_VROFS_H_

#include "stdint.h"

/* VROFS Specification - v1.0

The VROFS consists of the following three consecutive parts:
  1. Main header:   TVrofsMainHead (32 Bytes)
  2. Index:         file count * Index Record size (24 - 256 Bytes)
  3. Data Block:    variable length, concatenated data of all the files
                    the individual file data start is always 8-Bytes aligned

The index records have always a fix size, so that searching a specific entry in the index is simpler.
The Maximum file path length is 112 bytes.
*/

#define VROFS_ID_10  "VROFSV10"

#define VROFS_FLAG_ORDERED    1  // signals sorted directory index, allowing faster searches for big directories

#define VROFS_MAX_PATH_LEN  112

typedef struct
{
	char       vrofsid[8];         // "VROFSV10"
	char       label[8];           // user label for the file system, the unused space must be padded with zeroes
	uint32_t   main_head_bytes;    // the total length of this Main Head structure,
	                               // including the 8-byte vrofsid
	uint32_t   index_block_bytes;  // the total length of the index block
	uint32_t   data_block_bytes;   // size of the total data block in Bytes
	uint8_t    index_rec_bytes;    // size of the index records = sizeof(TVrofsIndexRec), allows longer / shorter file names
	uint8_t    flags;              // file system flags
	uint8_t    _reserved[2];       // all zeroes
//
} TVrofsMainHead;  // 32 Bytes

typedef struct
{
	uint32_t   data_bytes;    // size of the file data in Bytes
	uint32_t   offset;        // beginning of the file data, offset within the data block (in bytes)
	                          // The file data start is 8-byte aligned
	uint8_t    _reserved[7];  // all 0x00-s
	uint8_t    path_len;      // length of the actual path data, might be less than the total available space
	char       path[VROFS_MAX_PATH_LEN]; // 8 - 112 bytes (must be dividible by 8)
//
} TVrofsIndexRec; // 24 - 128 Bytes


#endif /* SRC_VROFS_H_ */
