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
 *  file:     filesystem.cpp
 *  brief:    File System Base Class
 *  date:     2024-04-17
 *  authors:  nvitya
*/

#include "string.h"
#include <filesystem.h>
#include "traces.h"

#if 0
  #define TRACE_PATH(...)  TRACE( __VA_ARGS__ )
#else
  #define TRACE_PATH(...)
#endif

TFile::TFile(TFileSystem * afilesys)
{
	filesys = afilesys;
}

void TFile::Open(const char * aname, uint32_t aflags)
{
	if (!finished)
	{
		TRACE("TFile::Open: File Busy!\r\n");
		return;
	}

	opened = false;
	open_flags = aflags;
	directory = (0 != (open_flags & FOPEN_DIRECTORY));
	strncpy(path, aname, sizeof(path));

	filesys->AddTransaction(this, FSTRA_FILE_OPEN);
}

void TFile::FinishTra(int aresult)
{
	result = aresult;
	finished = true;

	if (callback)
	{
		(* (callback))(this, callbackarg);
	}
}

void TFile::Read(void * dst, uint32_t len)
{
	if (!finished)
	{
		TRACE("TFile::Read: File Busy!\r\n");
		return;
	}

	dataptr = (uint8_t *)dst;
	datalen = len;
	transferlen = 0;

	remaining = len;

	if (!opened)
	{
		FinishTra(FSRESULT_FILE_NOT_OPEN);
		return;
	}

	if (!directory)
	{
		// check file end
		if (filepos + remaining > fdata.size)
		{
			if (filepos > fdata.size)  // this should not happen !
			{
				filepos = fdata.size;
			}

			remaining = (fdata.size - filepos);
		}
	}

	filesys->AddTransaction(this, FSTRA_FILE_READ);
}

void TFile::Seek(uint64_t afilepos)
{
	if (!finished)
	{
		TRACE("TFile::Seek: File Busy!\r\n");
		return;
	}

	if (!opened)
	{
		FinishTra(FSRESULT_FILE_NOT_OPEN);
		return;
	}

	if (!directory)
	{
		if (afilepos > fdata.size)
		{
			FinishTra(FSRESULT_SEEK_BEYOND_EOF);
			return;
		}
	}

	targetpos = afilepos;

	// check if withing the current cluster
	if ((targetpos & filesys->cluster_start_mask) == (filepos & filesys->cluster_start_mask))
	{
		filepos = targetpos;
		curlocation = (curlocation & filesys->cluster_start_mask) + (targetpos & filesys->cluster_reminder_mask);
		FinishTra(0);
		return;
	}

	// set back to the beginning
	filepos = 0;
	curlocation = fdata.location;
	cluster_end = fdata.location + filesys->clusterbytes;

	// some fast path:
	if (afilepos < filesys->clusterbytes)
	{
		filepos = afilepos;
		curlocation += afilepos;
		FinishTra(0);
		return;
	}

	filesys->AddTransaction(this, FSTRA_FILE_SEEK);
}

int TFile::WaitComplete()
{
	while (!finished)
	{
		filesys->Run();
	}

	return result;
}

//----------------------------------------------------------------------------------------------------

void TFileSystem::Init(TStorManager * astorman, uint64_t afirstaddr, uint64_t amaxsize)
{
	pstorman = astorman;
	firstaddr = afirstaddr;
	maxsize = amaxsize;

	initialized = false;
	initstate = 0;
	fsok = false;

	memset(&stra, 0, sizeof(stra));
	stra.completed = true;
}

void TFileSystem::Run()
{
	if (!pstorman)
	{
		return;
	}

	pstorman->Run();
	if (!stra.completed)
	{
		return;
	}

	if (curop != FSOP_IDLE)
	{
		if (stra.errorcode)
		{
			stra.errorcode = 0;
			opresult = FSRESULT_IOERROR;
			curop = FSOP_IDLE;
		}
		else if (FSOP_DIR_READ == curop)
		{
			RunOpDirRead();
		}

		if (curop != FSOP_IDLE)
		{
			return;
		}
	}

	if (fsok)
	{
		// handle transactions
		if (curtra)
		{
			if (stra.errorcode)
			{
				stra.errorcode = 0;
				FinishCurTra(FSRESULT_IOERROR);
				return;
			}

			if (FSTRA_FILE_OPEN == curtra->tratype)
			{
				HandleFileOpen();
			}
			else if (FSTRA_FILE_READ == curtra->tratype)
			{
				if (curtra->directory)
				{
					HandleDirRead();
				}
				else
				{
					HandleFileRead();
				}
			}
			else if (FSTRA_FILE_SEEK == curtra->tratype)
			{
				HandleFileSeek();
			}
			else
			{
				// unknown transaction
				FinishCurTra(FSRESULT_NOTIMPL);
			}
		}
	}
	else
	{
		if (!initialized)
		{
			HandleInitState();
		}
	}
}

void TFileSystem::HandleFileOpen()
{
	// called only when curop == FSOP_IDLE

	if (0 == trastate)  // start path resolution
	{
		strncpy(path, curtra->path, sizeof(path));

		pseg_start = path;
		dir_location = rootdirstart;
		dir_cluster_end = dir_location + clusterbytes;
		trastate = 1;
	}

	if (1 == trastate) // search next path segment
	{
		if ((*pseg_start == '/') || (*pseg_start == '\\'))  ++pseg_start;

		pseg_end = pseg_start;
		while ((*pseg_end != 0) && (*pseg_end != '/') && (*pseg_end != '\\'))
		{
			++pseg_end;
			if (pseg_end - pseg_start > FS_FNAME_MAX_LEN - 1)
			{
				FinishCurTra(FSRESULT_INVALID_PATH);
				return;
			}
		}

		pseg_len = pseg_end - pseg_start;
		if (pseg_len <= 0)
		{
			if (curtra->directory && (0 == *pseg_end))  // handling root directory and "/" terminated paths
			{
				// directory found
				curtra->fdata = fdata;
				curtra->opened = true;
				curtra->filepos = 0;
				curtra->curlocation = dir_location;
				curtra->cluster_end = dir_cluster_end;
				FinishCurTra(0);
				return;
			}

			FinishCurTra(FSRESULT_INVALID_PATH);
			return;
		}

		trastate = 2; // continue at process directory entry in fdata
		if (StartOpDirRead(dir_location, dir_cluster_end))
		{
			return; // not finished yet
		}
	}

	// process directory entry in fdata

	while (true)
	{
		if (opresult)
		{
			if (opresult == FSRESULT_EOF)  opresult = FSRESULT_FILE_NOT_FOUND;
			FinishCurTra(opresult);
			return;
		}

		if (strncasecmp(pseg_start, fdata.name, pseg_len) == 0)
		{
      #ifdef TRACE_PATH
			  char segname[FS_FNAME_MAX_LEN];
			  strncpy(segname, pseg_start, pseg_len);
			  segname[pseg_len] = 0; // ensure zero termination !
			  TRACE_PATH("Segment \"%s\" found.\r\n", &segname[0]);
      #endif

			// segment found

			if (0 == *pseg_end) // this is the last path segment, this should be a file
			{
				if (curtra->directory)
				{
					if (0 == (fdata.attributes & FSATTR_DIR))
					{
						FinishCurTra(FSRESULT_DIR_NOT_FOUND);
						return;
					}
				}
				else if (fdata.attributes & FSATTR_NONFILE)
				{
					FinishCurTra(FSRESULT_FILE_NOT_FOUND);
					return;
				}

				curtra->fdata = fdata;
				curtra->opened = true;
				curtra->filepos = 0;
				curtra->curlocation = fdata.location;
				curtra->cluster_end = fdata.location + clusterbytes;
				FinishCurTra(0);
				return;
			}

			if (0 == (fdata.attributes & FSATTR_DIR))  // this must be directory
			{
				FinishCurTra(FSRESULT_FILE_NOT_FOUND);
				return;
			}

			dir_location = fdata.location;
			dir_cluster_end = dir_location + clusterbytes;

			// resolve the next segment
			pseg_start = pseg_end;
			trastate = 1;
			HandleFileOpen();
			return;
		}

		// else: continue the search

		dir_location = op_location;
		dir_cluster_end = op_cluster_end;
		if (StartOpDirRead(dir_location, dir_cluster_end))
		{
			return; // operation is still running (HW block read is in progress)
		}
	}
}

void TFileSystem::HandleDirRead()  // must be overridden
{
	// called only when curop == FSOP_IDLE

	if (0 == trastate)
	{
		trastate = 1;
		if (StartOpDirRead(curtra->curlocation, curtra->cluster_end))
		{
			return; // not finished yet
		}
	}
	else // process the result
	{
		curtra->curlocation = op_location;
		curtra->cluster_end = op_cluster_end;
		if (0 == opresult)
		{
			if (curtra->datalen < sizeof(fdata))
			{
				FinishCurTra(FSRESULT_INVALID_FDATABUF);
				return;
			}

			memcpy(curtra->dataptr, &fdata, sizeof(fdata)); // copy the local fdata
			curtra->transferlen = sizeof(fdata);
		}
		FinishCurTra(opresult);
	}
}

TFile * TFileSystem::NewFileObj(void * astorage, unsigned astoragesize)  // must be overridden
{
	return nullptr;
}

void TFileSystem::HandleInitState()
{
	// should be overridden
}

void TFileSystem::RunOpDirRead()
{
	// should be overridden
}

void TFileSystem::HandleFileRead() // must be overridden
{
	FinishCurTra(FSRESULT_NOTIMPL);
}

void TFileSystem::HandleFileSeek() // must be overridden
{
	FinishCurTra(FSRESULT_NOTIMPL);
}

void TFileSystem::AddTransaction(TFile * afile, TFsTraType atype)
{
	afile->nexttra = nullptr;
	afile->finished = false;
	afile->result = 0;
	afile->tratype = atype;

	if (lasttra)
	{
		lasttra->nexttra = afile;
		lasttra = afile;
	}
	else
	{
		// set as first
		curtra = afile;
		lasttra = afile;
	}
}

void TFileSystem::FinishCurTra(int aresult)
{
	// request completed
	// the callback function might add the same transaction object as new
	// therefore we have to remove the transaction from the chain before we call the callback

	TFile * ptra = curtra;

	// unchain:
	curtra = (TFile *)curtra->nexttra; // advance to the next transaction
	if (!curtra)  lasttra = nullptr;

	trastate = 0;

	ptra->FinishTra(aresult);
}

void TFileSystem::FinishCurOp(int aresult)
{
	opresult = aresult;
	curop = FSOP_IDLE;
}

bool TFileSystem::StartOpDirRead(uint64_t alocation, uint64_t acluster_end)
{
	op_location = alocation;
	op_cluster_end = acluster_end;

	curop = FSOP_DIR_READ;
	opstate = 0;

	RunOpDirRead();

	return (curop != FSOP_IDLE);
}

//---------------------------------------------------------------------------------------------

