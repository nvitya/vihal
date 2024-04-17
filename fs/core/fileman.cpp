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
 *  file:     fileman.cpp
 *  brief:    Global File Manager Class + Object
 *  date:     2024-04-17
 *  authors:  nvitya
*/

#include <fileman.h>

TFileManager fileman;

void TFileManager::Init()
{
	fscount = 0;
}

bool TFileManager::AddFileSystem(TFileSystem * afilesystem)
{
	if (fscount >= FILESYS_MAX_FSYS)
	{
		return false;
	}

	fslist[fscount] = afilesystem;
	afilesystem->fsidx = fscount;

	++fscount;

	return true;
}

void TFileManager::Run()
{
	for (unsigned fsidx = 0; fsidx < fscount; ++fsidx)
	{
		fslist[fsidx]->Run();
	}
}

TFile * TFileManager::NewFileObj(const char * adrivepath, void * astorage, unsigned astoragesize)
{
	TFileSystem * fsys = FileSystemFromPath(adrivepath);
	if (!fsys)
	{
		return nullptr;
	}

	return fsys->NewFileObj(astorage, astoragesize);
}

void TFileManager::ReleaseFileObj(TFile * afile)
{
	if (afile)
	{
		if (afile->allocated_on_heap)
		{
			afile->allocated_on_heap = false; // prevents also double free
			delete afile;
		}
	}
}

TFileSystem * TFileManager::FileSystemFromPath(const char * adrivepath)
{
	if (adrivepath)
	{
		char c = *adrivepath;
		if ((0 != c) && ('/' != c))  // drive letter present ?
		{
			if ((c >= '0') && (c <= '9'))
			{
				unsigned didx = c - '0';
				if (didx < FILESYS_MAX_FSYS)
				{
					return fslist[didx];
				}
			}

			return nullptr;  // invalid file system requested
		}
	}

	return fslist[0];  // return the first filesystem
}
