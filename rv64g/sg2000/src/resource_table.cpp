/*
 *  file:     resource_table.cpp
 *  brief:    Resource Table for CV1800B/SG2002 remote processor code, required for the proper loading
 *  created:  2024-04-21
 *  authors:  nvitya
*/

#include <stdint.h>
#include "platform.h"

typedef uint32_t u32;
typedef uint64_t u64;
typedef uint8_t u8;

struct resource_table
{
    u32  ver;
    u32  num;
    u32  reserved[2];
    u32  offset[0];
};

struct fw_rsc_hdr
{
    u32  type;
    u8   data[0];
} __attribute__((packed));

struct fw_rsc_carveout
{
    u32  da;
    u32  pa;
    u32  len;
    u32  flags;
    u32  reserved;
    u8   name[32];
} __attribute__((packed));

enum fw_resource_type
{
    RSC_CARVEOUT        = 0,
    RSC_DEVMEM          = 1,
    RSC_TRACE           = 2,
    RSC_VDEV            = 3,
    RSC_LAST            = 4,
    RSC_VENDOR_START    = 128,
    RSC_VENDOR_END      = 512,
};

#define NO_RESOURCE_ENTRIES     1
#define NUM_TABLE_ENTRIES       1

struct remote_resource_table
{
    struct resource_table    resource_table;
    u32                      offset[NO_RESOURCE_ENTRIES];
    struct fw_rsc_hdr        carve_out;
    struct fw_rsc_carveout   carve_out_data;
} __attribute__((packed));

extern struct remote_resource_table resources;

#if defined(BOARD_MILKV_DUO)
  #define START_ADDR  0x83F40000
  #define IMAGE_LEN   0x000C0000
#else
  #define START_ADDR  0x8FE00000
  #define IMAGE_LEN   0x00200000
#endif

__attribute__((section(".resource_table"),used))
struct remote_resource_table resources =
{
    .resource_table = {1, NUM_TABLE_ENTRIES, {0}},
    .offset         = { offsetof(struct remote_resource_table, carve_out) },
    .carve_out      = {RSC_CARVEOUT},
    .carve_out_data = {START_ADDR, START_ADDR, IMAGE_LEN, 0x0, 0, "text"}
};
