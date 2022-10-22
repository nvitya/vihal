/*
 * network.cpp
 *
 *  Created on: Mar 19, 2022
 *      Author: vitya
 */

#include "platform.h"
#include "network.h"

uint16_t calc_icmp_checksum(void * pdata, uint32_t datalen)
{
  //((uint8_t *)&pdata)[datalen] = 0; // for odd size handling

  uint32_t n;
  uint32_t clen = ((datalen + 1) >> 1);
  uint32_t sum = 0;
  uint16_t * pd16 = (uint16_t *)pdata;

  for (n = 0; n < clen; ++n)
  {
    sum += __REV16(*pd16++);
  }

  sum = (sum & 0xffff) + (sum >> 16);


  return (uint16_t) (~sum);
}
