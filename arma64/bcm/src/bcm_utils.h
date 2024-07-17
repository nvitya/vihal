
#ifndef BCM_UTILS_H_
#define BCM_UTILS_H_

#include "stdint.h"

void * map_hw_addr(uintptr_t aaddr, unsigned asize, void * * aptrvar);

//volatile uint32_t * am_get_cm_reg(uint32_t aregoffs);
//void am_enable_per(uint32_t acmregoffs, bool aenable);

#endif /* BCM_UTILS_H_ */
