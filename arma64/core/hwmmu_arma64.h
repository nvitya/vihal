/*
 * hwmmu_arma64.h
 *
 *  Created on: Jul 23, 2024
 *      Author: vitya
 */

// some parameters were taken from: https://krinkinmu.github.io/2024/01/14/aarch64-virtual-memory.html

#ifndef HWMMU_ARMA64_H_
#define HWMMU_ARMA64_H_

#include "platform.h"

// Using 16k Mode the virtual address is sliced the following way:
//   13 -  0:   direct offset     1 Byte granularity
//   24 - 13:   L3               16 kByte granularity
//   35 - 25:   L2               32 MByte granularity
//   46 - 36:   L1 (not used)    64 GByte granularity

#define HWMMU_PHYADDR_BITS   35
#define HWMMU_BLKSIZE_BITS   25   // 32 MByte granularity
#define HWMMU_MAX_BLKIDX   2048


// VMSAv8-64 Descriptor Format

#define MMUDESC_VALID       (1 << 0)
#define MMUDESC_BLOCK       (1 << 1)

#define MMU_MT_NORMAL              0
#define MMU_MT_NORMAL_NOCACHE      2
#define MMU_MT_DEVICE_NGNRNE       3
#define MMU_MT_DEVICE_NGNRE        4

#define MMU_MATTR_NORMAL        0xFF  // idx 0
#define MMU_MATTR_NOCACHE       0x44  // idx 2
#define MMU_MATTR_DEVREGS       0x00  // idx 3: nGnRnE
#define MMU_MATTR_DEVMEM        0x04  // idx 4: nGnRE

#define MMUCFG_NORMAL         0x0000
#define MMUCFG_NOCACHE        0x0002
#define MMUCFG_DEVREGS        0x0003
#define MMUCFG_DEVMEM         0x0004
#define MMUCFG_MA_MASK        0x0007
#define MMUCFG_MA_SHIFT       0x0000


class THwMmu
{
public:
  virtual ~THwMmu() { }

  uint64_t *  ptable = nullptr;
  uint64_t    table_items = 0;

  uint64_t    blksize  = 0;
  uint64_t    highmask = 0;
  uint64_t    lowmask  = 0;

  void Init();  // sets all ranges to invalid !

  void MapRange(uintptr_t avaddr, uintptr_t apaddr, uintptr_t asize, uint32_t aflags);
  void Activate();
private:
  void PrepareMair();
};

extern THwMmu  hwmmu;

#endif /* HWMMU_ARMA64_H_ */
