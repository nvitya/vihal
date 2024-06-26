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
 *  file:     storman_sdcard.cpp
 *  brief:    Storage Manager for SDCARDs
 *  date:     2024-04-17
 *  authors:  nvitya
*/

#include "string.h"
#include <storman_sdcard.h>

// state machine codes
#define SMDS_IDLE                  0
#define SMDS_WAIT_NORMAL           1
#define SMDS_RD_WAIT_PARTIAL      10
#define SMDS_RD_PROCESS_PARTIAL   11
#define SMDS_WR_WAIT_PARTIAL_RD   20
#define SMDS_WR_WAIT_PARTIAL_WR   21
#define SMDS_FINISH              100

#define SMD_BLOCK_ADDR_MASK    0xFFFFFFFFFFFFFE00

bool TStorManSdcard::Init(TSdCard * asdcard)
{
	sdcard = asdcard;

	super::Init(&sdbuf[0], sizeof(sdbuf));

	erase_unit = 512;
	smallest_block = 512;

	return true;
}

void TStorManSdcard::StartPartialRead()
{
	uint64_t bladdr = (curaddr & SMD_BLOCK_ADDR_MASK);
	if (bladdr != sdbufaddr)
	{
		if (!sdcard->StartReadBlocks((bladdr >> 9), &sdbuf[0], 1))
		{
			curtra->errorcode = sdcard->errorcode;
			state = SMDS_FINISH;
			return;
		}

		state = SMDS_RD_WAIT_PARTIAL;
	}
	else
	{
		// already loaded
		ProcessPartialRead();
	}
}

void TStorManSdcard::ProcessPartialRead()
{
	uint32_t offset = (curaddr & 0x1FF);
	uint32_t rdsize = (512 - offset);

	if (rdsize > remaining)  rdsize = remaining;

	memcpy(dataptr, &sdbuf[offset], rdsize);

	remaining -= rdsize;
	dataptr   += rdsize;
	curaddr   += rdsize;

	if (remaining > 0)
	{
		StartPartialRead();
	}
	else
	{
		FinishCurTra();
	}
}

void TStorManSdcard::PreparePartialWrite()
{
	uint64_t bladdr = (curaddr & SMD_BLOCK_ADDR_MASK);
	if (bladdr != sdbufaddr)
	{
		if (!sdcard->StartReadBlocks((bladdr >> 9), &sdbuf[0], 1))
		{
			curtra->errorcode = sdcard->errorcode;
			state = SMDS_FINISH;
			return;
		}

		state = SMDS_WR_WAIT_PARTIAL_RD;
	}
	else
	{
		// already loaded
		StartPartialWrite();
	}
}

void TStorManSdcard::StartPartialWrite()
{
	uint32_t offset = (curaddr & 0x1FF);
	chunksize = (512 - offset);

	if (chunksize > remaining)  chunksize = remaining;

	memcpy(&sdbuf[offset], dataptr, chunksize);

	if (!sdcard->StartWriteBlocks((sdbufaddr >> 9), &sdbuf[0], 1))
	{
		curtra->errorcode = sdcard->errorcode;
		state = SMDS_FINISH;
		return;
	}

	state = SMDS_WR_WAIT_PARTIAL_WR;
}

void TStorManSdcard::FinishPartialWrite()
{
	remaining -= chunksize;
	dataptr   += chunksize;
	curaddr   += chunksize;

	if (remaining > 0)
	{
		PreparePartialWrite();
	}
	else
	{
		FinishCurTra();
	}
}

uint64_t TStorManSdcard::ByteSize()
{
  if (!sdcard || !sdcard->card_initialized)
  {
    return 0;
  }

  return (uint64_t(sdcard->card_megabytes) << 20);
}

void TStorManSdcard::Run()
{
  if (!firsttra || !sdcard)
  {
    return;
  }

	sdcard->Run();
	if (!sdcard->completed) // || !sdcard->card_initialized)
	{
		return;
	}

	uint8_t n;

	if (SMDS_IDLE == state)
	{
		// start (new request)
		curtra = firsttra;

		trastarttime = CLOCKCNT;
		state = 1;

		if (STRA_READ == curtra->trtype)
		{
			// check partial / full block

			remaining = curtra->datalen;
			dataptr = curtra->dataptr;
			curaddr = curtra->address;

			if ((curaddr & 0x1FF) || (remaining & 0x1FF))
			{
				// partial mode
				StartPartialRead();
				return;
			}
			else
			{
				// full block mode
				if (!sdcard->StartReadBlocks((curaddr >> 9), dataptr, (remaining >> 9)))
				{
					FinishCurTraError(sdcard->errorcode);
					return;
				}

				state = SMDS_WAIT_NORMAL;
			}
		}
		else if (STRA_WRITE == curtra->trtype)
		{
			// check partial / full block

			remaining = curtra->datalen;
			dataptr = curtra->dataptr;
			curaddr = curtra->address;

			if ((curaddr & 0x1FF) || (remaining & 0x1FF))
			{
				// partial mode
				PreparePartialWrite();
				return;
			}
			else
			{
				// full block mode
				if (!sdcard->StartWriteBlocks((curaddr >> 9), dataptr, (remaining >> 9)))
				{
					FinishCurTraError(sdcard->errorcode);
					return;
				}

				state = SMDS_WAIT_NORMAL;
			}
		}
		else
		{
			FinishCurTraError(ESTOR_NOTIMPL);
		}
	}
	else if (SMDS_WAIT_NORMAL == state) // finished, set the error code
	{
		curtra->errorcode = sdcard->errorcode;
		FinishCurTra();
	}
	else if (SMDS_RD_WAIT_PARTIAL == state)
	{
		if (sdcard->errorcode)
		{
			FinishCurTraError(sdcard->errorcode);
			return;
		}

		sdbufaddr = (curaddr & SMD_BLOCK_ADDR_MASK);
		ProcessPartialRead();
	}
	else if (SMDS_WR_WAIT_PARTIAL_RD == state)
	{
		if (sdcard->errorcode)
		{
			FinishCurTraError(sdcard->errorcode);
			return;
		}

		sdbufaddr = (curaddr & SMD_BLOCK_ADDR_MASK);
		StartPartialWrite();
	}
	else if (SMDS_WR_WAIT_PARTIAL_WR == state)
	{
		if (sdcard->errorcode)
		{
			FinishCurTraError(sdcard->errorcode);
			return;
		}

		FinishPartialWrite();
	}
	else if (SMDS_FINISH == state)
	{
		FinishCurTra();
	}

}
