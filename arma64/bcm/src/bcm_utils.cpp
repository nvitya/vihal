
#include <bcm_utils.h>
#include "platform.h"
#include "stdint.h"

#if 0

uint8_t * g_am_cm_regs = nullptr;

volatile uint32_t * am_get_cm_reg(uint32_t aregoffs)
{
	if (!g_am_cm_regs)
	{
		g_am_cm_regs = (uint8_t *)hw_memmap(HW_CM_PER_BASE, 0x400);
	}
	return (volatile uint32_t *)&g_am_cm_regs[aregoffs];
}

void am_enable_per(uint32_t acmregoffs, bool aenable)
{
	volatile uint32_t * cmreg = am_get_cm_reg(acmregoffs);
	if (aenable)
	{
		*cmreg = 0x00002;
	}
	else
	{
		*cmreg = 0x30000;
	}
}

#endif

#ifdef LINUX

#include "hw_utils.h"

void * map_hw_addr(uintptr_t aaddr, unsigned asize, void * * aptrvar)
{
	if (0x70000000 == (aaddr & 0xF0000000))  // convert to legacy address on linux
	{
		aaddr |= 0x80000000;
	}

  if (!aptrvar)
  {
  	return hw_memmap(aaddr, asize);
  }

  if (*aptrvar) // is already set ?
  {
  	return *aptrvar;
  }

  *aptrvar = hw_memmap(aaddr, asize);
	return *aptrvar;
}

#else

void * map_hw_addr(uintptr_t aaddr, unsigned asize, void * * aptrvar)
{
  if (!aptrvar)
  {
  	return (void *)aaddr;
  }

  if (*aptrvar) // is already set ?
  {
  	return *aptrvar;
  }

  *aptrvar = (void *)aaddr;
	return *aptrvar;
}

#endif
