/*
 * clockcnt_am.h
 *
 *  Created on: Jul 3, 2024
 *      Author: vitya
 */

#ifndef CLOCKCNT_AM_H_
#define CLOCKCNT_AM_H_

extern uint32_t clockcnt_get();

#ifndef CLOCKCNT_ECAP_NUM
  #define CLOCKCNT_ECAP_NUM  0
#endif
#define CLOCKCNT           clockcnt_get()
#define CLOCKCNT_BITS      32

#endif /* CLOCKCNT_AM_H_ */
