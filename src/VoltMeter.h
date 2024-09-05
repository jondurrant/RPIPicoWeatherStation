/*
 * VoltMeter.h
 *
 *  Created on: 5 Sept 2024
 *      Author: jondurrant
 */

#ifndef SRC_VOLTMETER_H_
#define SRC_VOLTMETER_H_

#include "pico/stdlib.h"

class VoltMeter {
public:
	VoltMeter(uint8_t gp);
	virtual ~VoltMeter();

	float volts();

private:
	uint8_t xGp = 0xff;
};

#endif /* SRC_VOLTMETER_H_ */
