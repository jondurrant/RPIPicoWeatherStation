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
#include "math.h"

extern "C" {
#include <power_status.h>
}


PicoStatus::PicoStatus() {
	adc_set_temp_sensor_enabled(true);
//reset();

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

void PicoStatus::start(){
	//NOP
}

void PicoStatus::stop(){
	//NOP
}

void PicoStatus::reset(){
	 xVBUSSec 					= 0;
	 xVSYSSec 					= 0;

	xCelcius 				= tempCelcius();
	xMaxCelcius 		= xCelcius;
	xMinCelcius 		= xCelcius;

	xVolts						= vsysVolts();
	xMaxVolts			= xVolts;
	xMinVolts				= xVolts;
}

void PicoStatus::sample(){
	uint32_t now = to_ms_since_boot (get_absolute_time () );

	if (xLastSample != 0){
		uint32_t sec = (now - xLastSample) / 1000;
		//Reasonable value check
		if (sec  <  (24*60*60)){
			if (isVSYS()){
				xVSYSSec += sec;
			} else {
				xVBUSSec += sec;
			}
		}


		xCelcius 				= (xCelcius + tempCelcius())/2.0;
		xMaxCelcius 		= fmax(xMaxCelcius, xCelcius);
		xMinCelcius		= fmin(xMinCelcius, xCelcius);

		xVolts						= vsysVolts();
		xMaxVolts			= fmax(xVolts, xMaxVolts);
		xMinVolts				= fmin(xVolts, xMinVolts);
	} else {
		reset();
	}

	xLastSample = now;
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
	p = json_double(p,  "volts", xVolts, remLen );
	p = json_double(p,  "min_volts", xMinVolts, remLen );
	p = json_double(p,  "max_volts", xMaxVolts, remLen );

	p = json_double(p,  "celcius",  xCelcius, remLen );
	p = json_double(p,  "min_celcius",  xMinCelcius, remLen );
	p = json_double(p,  "max_celcius",  xMaxCelcius, remLen );

	p = json_double(p,  "vbus_sec",  xVBUSSec, remLen );
	p = json_double(p,  "vsys_sec",  xVSYSSec, remLen );

	p = json_objClose( p, remLen );
	return p;
}
