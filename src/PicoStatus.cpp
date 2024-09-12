/*
 * PicoStatus.cpp
 *
 *  Created on: 5 Sept 2024
 *      Author: jondurrant
 */

#include "PicoStatus.h"
#include "hardware/adc.h"
#include "json-maker/json-maker.h"
#include "pico/unique_id.h"

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
	 return false;
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


char* PicoStatus::writeJson( char* dest, const char * name, size_t* remLen ){
	char * p = dest;
	char picoId[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];
	pico_get_unique_board_id_string (picoId,  2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1);

	p = json_objOpen( p, name, remLen );
	p = json_str(p, "id", picoId, remLen);
	if (isVBUS()){
		p = json_str(p, "source", "VBUS", remLen );
	} else {
		p = json_str(p, "source", "VSYS", remLen );
	}
	p = json_double(p,  "volts",  vsysVolts(), remLen );
	p = json_double(p,  "celcius",  tempCelcius(), remLen );

	p = json_objClose( p, remLen );
	return p;
}
