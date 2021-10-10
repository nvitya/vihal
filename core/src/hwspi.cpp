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
// file:     hwspi.cpp
// brief:    SPI vendor-independent implementations
//           only SPI master mode supported so far
// created:  2018-02-10
// authors:  nvitya

#include "platform.h"
#include "hwspi.h"

static_assert(HWSPI_MAX_XFER_BLOCK >= 4, "Minimum 4 SPI transfer blocks are required!");

void THwSpi::BeginTransaction()
{
	if (manualcspin)
	{
		manualcspin->Set0();
	}
}

void THwSpi::EndTransaction()
{
	if (manualcspin)
	{
		manualcspin->Set1();
	}
}

void THwSpi::WaitSendFinish()
{
	while (!SendFinished())
	{
		// wait
	}
}

#if 0
int THwSpi::RunTransfer(TSpiTransfer * axfer)
{
	int result = 0;

	TSpiTransfer * xfer = axfer;

	while (xfer)
	{
		if (xfer->length > 0)
		{
			// sending and receiving
			unsigned char * psrc = (unsigned char *)xfer->src;
			unsigned char * pdst = (unsigned char *)xfer->dst;
			unsigned txremaining = xfer->length;
			unsigned rxremaining = xfer->length;
			unsigned short rxdata;

			while ((txremaining > 0) || (rxremaining > 0))
			{
				if (txremaining > 0)
				{
					if (psrc)
					{
						if (TrySendData(*psrc))
						{
							++psrc;
							--txremaining;
						}
					}
					else if (TrySendData(0))
					{
						--txremaining;
					}
				}

				if ((rxremaining > 0) && (TryRecvData(&rxdata)))
				{
					if (pdst)
					{
						*pdst = rxdata;
						++pdst;
					}
					--rxremaining;
				}
			}

			result += xfer->length;
		}
		xfer = xfer->next;
	}

	return result;
}
#endif

void THwSpi_pre::PrepareTransfer(uint32_t acmd, uint32_t aaddr, uint32_t aflags,
		                             uint32_t alen, uint8_t * asrc, uint8_t * adst)
{
	TSpiXferBlock * pblock = &xferblock[0];

	blockcnt = 0;
	finished = false;
	state = 0; // sets back to start state

	if (aflags & SPITR_CMD_MASK) // send command ?
	{
		data_cmd = acmd;
		pblock->src = (uint8_t *)&data_cmd;
		pblock->dst = nullptr; // may be redirected to data_void
		if (aflags & SPITR_CMD) // use default size?
		{
			pblock->len = default_cmd_len;
		}
		else
		{
			pblock->len = (aflags & 7);
			if (pblock->len > 4)  pblock->len = 4;
		}
		++blockcnt;
		++pblock;
	}

	if (aflags & SPITR_ADDR_MASK) // send address ?
	{
		data_addr = aaddr;
		pblock->src = (uint8_t *)&data_addr;
		pblock->dst = nullptr; // may be redirected to data_void
		if (aflags & SPITR_ADDR) // use default size?
		{
			pblock->len = default_addr_len;
		}
		else
		{
			pblock->len = ((aflags >> 4) & 7);
			if (pblock->len > 4)  pblock->len = 4;
		}
		++blockcnt;
		++pblock;
	}

	if (aflags & SPITR_EXTRA_MASK) // send extra ?
	{
		pblock->src = (uint8_t *)&data_extra;
		pblock->dst = nullptr; // may be redirected to data_void
		if (aflags & SPITR_EXTRA) // use default size?
		{
			pblock->len = default_extra_len;
		}
		else
		{
			pblock->len = ((aflags >> 8) & 7);
			if (pblock->len > 4)  pblock->len = 4;
		}
		++blockcnt;
		++pblock;
	}

	// finally the data block
	pblock->src = asrc;
	pblock->dst = adst;
	pblock->len = alen;
	++blockcnt;
}

void THwSpi::WaitFinish()
{
	while (!finished)
	{
		Run();
	}
}
