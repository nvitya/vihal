/*
 *  file:     hw_utils.h
 *  brief:    Special System Utilities for direct HW Access
 *  date:     2020-10-01
 *  authors:  nvitya
*/

#ifndef HW_UTILS_H_
#define HW_UTILS_H_

#include "stdint.h"

void * hw_memmap(uintptr_t aaddr, unsigned asize);

#endif /* HW_UTILS_H_ */
