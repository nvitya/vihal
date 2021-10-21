/*
 * hwclk.h
 *
 *  Created on: Oct 21, 2021
 *      Author: vitya
 */

#ifndef HWCLK_H_
#define HWCLK_H_

#include "platform.h"

// external_clock_hz: 0 = use the internal RC oscillator
bool hwclk_init(unsigned external_clock_hz, unsigned target_speed_hz);  // implemented MCU specific sources

#endif /* HWCLK_H_ */
