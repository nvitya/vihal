/*
 *  file:     hw_utils.cpp
 *  brief:    Special System Utilities for direct HW Access
 *  date:     2020-10-01
 *  authors:  nvitya
*/

#include "platform.h"

#include <stdio.h>
#include <stdlib.h>
#include "stdint.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define MEM_PAGE_SIZE   (4 * 1024)

void * hw_memmap(uintptr_t aaddr, unsigned asize)
{
	int  mem_fd;

	mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (mem_fd < 0)
	{
		return nullptr;
	}

	uintptr_t startaddr = aaddr & ~(MEM_PAGE_SIZE - 1);

	size_t memblksize = ((asize + (MEM_PAGE_SIZE - 1)) & ~MEM_PAGE_SIZE);

	uint8_t * mmapresult = (uint8_t *)mmap(
    nullptr,                 // target address within our address space, nullptr = selected by the system
    memblksize,              // map length
    PROT_READ | PROT_WRITE,  // Enable reading & writting to mapped memory
    MAP_SHARED,              // shared with other processes
    mem_fd,                  // file to map
    startaddr // offset
   );

	close(mem_fd); // not required anymore

	if (mmapresult == MAP_FAILED)
	{
		return nullptr;
	}

	return mmapresult + (aaddr & (MEM_PAGE_SIZE - 1));
}
