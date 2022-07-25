/* This file is a part of the VIHAL project: https://github.com/nvitya/vihal
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
 *  file:     self_flashing_rp.cpp
 *  brief:    Kendryte K210 SPI Self Flashing
 *  date:     2022-07-25
 *  authors:  nvitya
*/

#include "platform.h"

#if defined(SPI_SELF_FLASHING) && SPI_SELF_FLASHING

#include "string.h"

#include "hwspi.h"
#include "spiflash.h"
#include "spiflash_updater.h"
#include "clockcnt.h"
#include "traces.h"

/*********************************************************************
* Filename:   sha256.c
* Author:     Brad Conte (brad AT bradconte.com)
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Implementation of the SHA-256 hashing algorithm.
              SHA-256 is one of the three algorithms in the SHA2
              specification. The others, SHA-384 and SHA-512, are not
              offered in this implementation.
              Algorithm specification can be found here:
               * http://csrc.nist.gov/publications/fips/fips180-2/fips180-2withchangenotice.pdf
              This implementation uses little endian byte order.
*********************************************************************/

#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

/**************************** DATA TYPES ****************************/
typedef unsigned char BYTE;             // 8-bit byte
typedef unsigned int  WORD;             // 32-bit word, change to "long" for 16-bit machines

typedef struct {
  BYTE data[64];
  WORD datalen;
  unsigned long long bitlen;
  WORD state[8];
} SHA256_CTX;

/*************************** HEADER FILES ***************************/
#include <stdlib.h>
#include <memory.h>

/****************************** MACROS ******************************/
#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

/**************************** VARIABLES *****************************/
static const WORD k[64] = {
  0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
  0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
  0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
  0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
  0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
  0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
  0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
  0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

/*********************** FUNCTION DEFINITIONS ***********************/
static void sha256_transform(SHA256_CTX *ctx, const BYTE data[])
{
  WORD a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

  for (i = 0, j = 0; i < 16; ++i, j += 4)
    m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
  for ( ; i < 64; ++i)
    m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

  a = ctx->state[0];
  b = ctx->state[1];
  c = ctx->state[2];
  d = ctx->state[3];
  e = ctx->state[4];
  f = ctx->state[5];
  g = ctx->state[6];
  h = ctx->state[7];

  for (i = 0; i < 64; ++i) {
    t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i];
    t2 = EP0(a) + MAJ(a,b,c);
    h = g;
    g = f;
    f = e;
    e = d + t1;
    d = c;
    c = b;
    b = a;
    a = t1 + t2;
  }

  ctx->state[0] += a;
  ctx->state[1] += b;
  ctx->state[2] += c;
  ctx->state[3] += d;
  ctx->state[4] += e;
  ctx->state[5] += f;
  ctx->state[6] += g;
  ctx->state[7] += h;
}

static void sha256_init(SHA256_CTX *ctx)
{
  ctx->datalen = 0;
  ctx->bitlen = 0;
  ctx->state[0] = 0x6a09e667;
  ctx->state[1] = 0xbb67ae85;
  ctx->state[2] = 0x3c6ef372;
  ctx->state[3] = 0xa54ff53a;
  ctx->state[4] = 0x510e527f;
  ctx->state[5] = 0x9b05688c;
  ctx->state[6] = 0x1f83d9ab;
  ctx->state[7] = 0x5be0cd19;
}

static void sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len)
{
  WORD i;

  for (i = 0; i < len; ++i) {
    ctx->data[ctx->datalen] = data[i];
    ctx->datalen++;
    if (ctx->datalen == 64) {
      sha256_transform(ctx, ctx->data);
      ctx->bitlen += 512;
      ctx->datalen = 0;
    }
  }
}

static void sha256_final(SHA256_CTX *ctx, BYTE hash[])
{
  WORD i;

  i = ctx->datalen;

  // Pad whatever data is left in the buffer.
  if (ctx->datalen < 56) {
    ctx->data[i++] = 0x80;
    while (i < 56)
      ctx->data[i++] = 0x00;
  }
  else {
    ctx->data[i++] = 0x80;
    while (i < 64)
      ctx->data[i++] = 0x00;
    sha256_transform(ctx, ctx->data);
    memset(ctx->data, 0, 56);
  }

  // Append to the padding the total message's length in bits and transform.
  ctx->bitlen += ctx->datalen * 8;
  ctx->data[63] = ctx->bitlen;
  ctx->data[62] = ctx->bitlen >> 8;
  ctx->data[61] = ctx->bitlen >> 16;
  ctx->data[60] = ctx->bitlen >> 24;
  ctx->data[59] = ctx->bitlen >> 32;
  ctx->data[58] = ctx->bitlen >> 40;
  ctx->data[57] = ctx->bitlen >> 48;
  ctx->data[56] = ctx->bitlen >> 56;
  sha256_transform(ctx, ctx->data);

  // Since this implementation uses little endian byte ordering and SHA uses big endian,
  // reverse all the bytes when copying the final state to the output hash.
  for (i = 0; i < 4; ++i) {
    hash[i]      = (ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 4]  = (ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 8]  = (ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 20] = (ctx->state[5] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 24] = (ctx->state[6] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 28] = (ctx->state[7] >> (24 - i * 8)) & 0x000000ff;
  }
}

#define SELFFLASH_BUFSIZE       4096  // allocated twice! (this MCU has plenty of RAM)

extern unsigned __app_image_end;

extern "C" void cold_entry();

// crc32_small is implemented in the "rp_st2_crc.S", taken from the pico-bootrom repostitory:
//extern "C" uint32_t crc32_small(const uint8_t * buf, unsigned int len, uint32_t seed);

/* K210 SPI Flash contents:
0x000000, 1 byte:  AES flag, 0 = unencrypted, 1 = encrypted firmware
0x000001, 4 bytes:  firmware length (without sha256 checksum)
0x000005, length bytes: firmware data
after the firmware data: sha256 checksum

the contents will be loaded to address 0x80000000, and started at 0x80000000
*/

// do self flashing using the flash writer
bool spi_self_flashing(TSpiFlash * spiflash)
{
  uint8_t *   dp = (uint8_t *)SELF_FLASHING_RAM_ADDR;
  uint32_t    chunksize;
  uint32_t    img_remaining;
  uint32_t    fl_addr = 0;
  uint32_t    imgsize = ((3 + intptr_t(&__app_image_end)) & 0x00FFFFFC);  // should begin at 0x80000000
  SHA256_CTX  ctx;
  uint8_t     localbuf[SELFFLASH_BUFSIZE] __attribute__((aligned(8)));
  uint8_t     flbuf[SELFFLASH_BUFSIZE] __attribute__((aligned(8)));
  uint8_t     hashvalue[32];
  uint8_t     hashidx = 0;

  // Using the flash writer to first compare the flash contents:
  TSpiFlashUpdater  flashupdater(spiflash, flbuf, sizeof(flbuf));

  sha256_init(&ctx);

  // prepare the first chunk

  // everything is pretty much unaligned because of the 5-byte header, so first we copy everything into the local buffer
  localbuf[0] = 0; // no encryption
  localbuf[1] = ((imgsize >>  0) & 0xFF);
  localbuf[2] = ((imgsize >>  8) & 0xFF);
  localbuf[3] = ((imgsize >> 16) & 0xFF);
  localbuf[4] = ((imgsize >> 24) & 0xFF);

  sha256_update(&ctx, &localbuf[0], 5);
  sha256_update(&ctx, dp, imgsize);
  sha256_final(&ctx, hashvalue);

  chunksize = 5 + imgsize;
  if (chunksize > sizeof(localbuf))
  {
    chunksize = sizeof(localbuf);
  }

  memcpy(&localbuf[5], dp, chunksize - 5);

  img_remaining = imgsize - (chunksize - 5);
  dp += (chunksize - 5);

  if (chunksize < sizeof(localbuf))
  {
    // add some part of the hash
    while ((hashidx < 32) && (chunksize < sizeof(localbuf)))
    {
      localbuf[chunksize++] = hashvalue[hashidx++];
    }
  }

  TRACE("Self-Flashing:\r\n");
  TRACE("  mem = %08X -> flash = %08X, len = %u ...\r\n", unsigned(SELF_FLASHING_RAM_ADDR), SELF_FLASHING_SPI_ADDR, imgsize);

  unsigned sumwrites = 0;
  unsigned summatch = 0;

  unsigned  t0, t1;
  t0 = CLOCKCNT;

  while (chunksize)
  {
    if (!flashupdater.UpdateFlash(fl_addr, &localbuf[0], chunksize))
    {
      TRACE("  ERROR!\r\n");
      return false;
    }
    sumwrites += flashupdater.writecnt;
    summatch  += flashupdater.samecnt;
    sha256_update(&ctx, &localbuf[0], chunksize);

    fl_addr += chunksize;
    chunksize = img_remaining;
    if (chunksize > sizeof(localbuf))
    {
      chunksize = sizeof(localbuf);
    }
    if (chunksize > 0)
    {
      memcpy(&localbuf[0], dp, chunksize);
      dp += chunksize;
      img_remaining -= chunksize;
    }

    // add some part of the hash
    while ((hashidx < 32) && (chunksize < sizeof(localbuf)))
    {
      localbuf[chunksize++] = hashvalue[hashidx++];
    }
  }

  t1 = CLOCKCNT;
  unsigned clocksperus = SystemCoreClock / 1000000;
  unsigned ssize_k = flashupdater.sectorsize >> 10;
  TRACE("  %u * %uk updated, %u * %uk matched, took %u us\r\n",
      sumwrites, ssize_k, summatch, ssize_k, (t1 - t0) / clocksperus);


  return true;
}

#endif  // ifdef SPI_SELF_FLASHING
