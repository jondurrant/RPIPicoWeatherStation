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

//Format should look like
/*
 *  "pico": {
            "id": "Test1",
            "source": "VBUS",
            "charge_volts": {
                "current": 4.42068,
                "min": 4.63096,
                "max": 4.68896
                },
            "bat_volts": {
                "current": 4.42068,
                "min": 4.63096,
                "max": 4.68896
                },
            "celcius": {
                "current": 23.8615,
                "min": 24.7978,
                "max": 25.8095
                },
            "vbus_sec": 106.0,
            "vsys_sec": 30.0
            },
 */
char* PicoStatus::writeJson( char* dest, const char * name, size_t* remLen ){
	char * p = dest;
	char picoId[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];
	pico_get_unique_board_id_string (picoId,  2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1);

	p = json_objOpen( p, name, remLen );
	p = json_str(p, "id", picoId, remLen);
	if (isVBUS()){
		p = json_str(p, "source", "VBUS", remLen );

		//charge_volts
		p = json_objOpen( p, "charge_volts", remLen );
			p = json_double(p,  "current", xVolts, remLen );
			p = json_double(p,  "min", xMinVolts, remLen );
			p = json_double(p,  "max", xMaxVolts, remLen );
		p = json_objClose( p, remLen );
	} else {
		p = json_str(p, "source", "VSYS", remLen );

		//bat_volts
		p = json_objOpen( p, "bat_volts", remLen );
			p = json_double(p,  "current", xVolts, remLen );
			p = json_double(p,  "min", xMinVolts, remLen );
			p = json_double(p,  "max", xMaxVolts, remLen );
		p = json_objClose( p, remLen );
	}

	//Temp
	p = json_objOpen( p, "celcius", remLen );
		p = json_double(p,  "current",  xCelcius, remLen );
		p = json_double(p,  "min",  xMinCelcius, remLen );
		p = json_double(p,  "max",  xMaxCelcius, remLen );
	p = json_objClose( p, remLen );

	p = json_double(p,  "vbus_sec",  xVBUSSec, remLen );
	p = json_double(p,  "vsys_sec",  xVSYSSec, remLen );

	p = json_objClose( p, remLen );
	return p;
}
