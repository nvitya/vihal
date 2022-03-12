/*
 * hwclk.h
 *
 *  Created on: Oct 21, 2021
 *      Author: vitya
 */

#ifndef HWCLK_H_
#define HWCLK_H_

#include "platform.h"

#define HWCLK_EXTCLK_BYPASS  0x80000000
#define HWCLK_EXTCLK_MASK    0x0FFFFFFF

// external_clock_hz: 0 = use the internal RC oscillator
bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz);  // implemented MCU specific sources

// external_clock: false = use the internal RC oscillator
bool hwlsclk_init(bool external_clock);  // implemented MCU specific sources

#endif /* HWCLK_H_ */
