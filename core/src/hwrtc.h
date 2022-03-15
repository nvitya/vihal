/*
 * hwrtc.h
 *
 *  Created on: 12.03.2022
 *      Author: Bergi
 */

#include "platform.h"

#ifndef HWRTC_H_PRE_
#define HWRTC_H_PRE_

class THwRtc_pre
{
public:
  typedef struct
  {
    uint16_t msec;
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
  } time_t;
};


#endif

#ifndef HWRTC_PRE_ONLY




#ifndef HWRTC_H_
#define HWRTC_H_

#include "mcu_impl.h"

class THwRtc : public HWRTC_IMPL
{


};

#endif

#else
  #undef HWRTC_PRE_ONLY
#endif
