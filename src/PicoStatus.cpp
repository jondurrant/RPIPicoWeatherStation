/*
 * PicoStatus.cpp
 *
 *  Created on: 5 Sept 2024
 *      Author: jondurrant
 */

#include "PicoStatus.h"
#include "hardware/adc.h"

extern "C" {
#include <power_status.h>
}


PicoStatus::PicoStatus() {
	adc_set_temp_sensor_enabled(true);

}

PicoStatus::~PicoStatus() {
	// TODO Auto-generated destructor stub
}


bool PicoStatus::isVSYS(){
	bool vsys;
	 if (power_source(&vsys) == PICO_OK) {
		 return vsys;
	 }
}

bool PicoStatus::isVBUS(){
	return ! isVSYS();
}

float PicoStatus::vsysVolts(){
	float volts;
	power_voltage(&volts);
	return volts;
}

float PicoStatus::tempCelcius(){
	const float conversion_factor = 3.3f / (1 << 12);
	adc_select_input(4);

	float v = (float)adc_read() * conversion_factor;
	float temp = 27.0 - ((v - 0.706)/0.001721);
	return temp;
}
