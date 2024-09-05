/*
 * VoltMeter.cpp
 *
 *  Created on: 5 Sept 2024
 *      Author: jondurrant
 */

#include "VoltMeter.h"
#include "hardware/adc.h"

VoltMeter::VoltMeter(uint8_t gp) {
	xGp = gp;

}

VoltMeter::~VoltMeter() {
	// TODO Auto-generated destructor stub
}

float VoltMeter::volts(){

	const float conversion_factor = 3.3f / (1 << 12);
	adc_select_input(xGp - 26);
	float v = (float)adc_read() * conversion_factor;
	return v;
}
