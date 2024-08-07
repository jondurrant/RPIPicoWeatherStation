/*
 * Rain.h
 *
 *  Created on: 7 Aug 2024
 *      Author: jondurrant
 */

#ifndef SRC_RAIN_H_
#define SRC_RAIN_H_

#include "pico/stdlib.h"

#define RAIN_MM_PER_PUSLSE 0.2794

class Rain {
public:
	Rain(uint8_t gp);
	virtual ~Rain();


	uint pulseCount();

	float mmRain();

	void reset();

private:
	uint8_t xGP;
	uint8_t xSlicer ;
};

#endif /* SRC_RAIN_H_ */
