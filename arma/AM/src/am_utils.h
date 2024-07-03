/*
 * am_utils.h
 *
 *  Created on: Jul 3, 2024
 *      Author: vitya
 */

#ifndef AM_UTILS_H_
#define AM_UTILS_H_

volatile uint32_t * am_get_cm_reg(uint32_t aregoffs);
void am_enable_per(uint32_t acmregoffs, bool aenable);

#endif /* AM_UTILS_H_ */
