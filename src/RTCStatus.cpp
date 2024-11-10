/*
 * RTCStatus.cpp
 *
 *  Created on: 15 Sept 2024
 *      Author: jondurrant
 */

#include "RTCStatus.h"
#include "json-maker/json-maker.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"
#include "math.h"

RTCStatus::RTCStatus(
		i2c_inst_t *i2c,
		uint8_t sdaPin,
		uint8_t sclPin,
		uint8_t batGP):
	DS3231(i2c, sdaPin, sclPin){
	pBat = new VoltMeter(batGP);

	reset();

}

RTCStatus::~RTCStatus() {
	delete pBat;
}


void RTCStatus::start(){

}

void RTCStatus::stop(){

}

void RTCStatus::reset(){
	float xCelcius = get_temp_f();
	float xMaxCelcius = xCelcius;
	float xMinCelcius = xCelcius;

	xVolts = batVolts();
	xMaxVolts = xVolts;
	xMinVolts = xVolts;
}

void RTCStatus::sample(){
	datetime_t t;

	if (rtc_get_datetime(&t)){

		if (t.hour != get_hou()){
			 set_date(t.day,  t.month,  t.year);
			 bool pm = false;
			 if (t.hour >= 12){
				 pm = true;
			 }
			 set_time(t.hour, t.min, t.sec, false,  pm);
		}
	}

	xCelcius = get_temp_f();
	xMaxCelcius = fmax(xCelcius, xMaxCelcius);
	xMinCelcius = fmin(xCelcius, xMinCelcius);

	xVolts = batVolts();
	xMaxVolts = fmax(xMaxVolts, xVolts);
	xMinVolts = fmin(xMinVolts, xVolts);

}


double RTCStatus::batVolts(){
	double d =  pBat->volts();
	//Adjust as using two 33GOhm resistors as divider
	d = d * ( 3.2/0.55);
	return d;
}

char* RTCStatus::writeJson( char* dest,const  char * name, size_t* remLen ) {
	char * p = dest;

	p = json_objOpen( p, name, remLen );
	p = json_objOpen( p, "timestamp", remLen );
			p = json_int(p, "year",  get_year(), remLen );
			p = json_uint(p, "month",  get_mon(), remLen );
			p = json_uint(p, "day",  get_day(), remLen );
			p = json_uint(p, "hour",  get_hou(), remLen );
			p = json_uint(p, "min",  get_min(), remLen );
			p = json_uint(p, "sec",  get_sec(), remLen );
	p = json_objClose( p, remLen );

	p = json_objOpen( p, "celcius", remLen );
		p = json_double(p,  "current", xCelcius, remLen );
		p = json_double(p,  "max", xMaxCelcius, remLen );
		p = json_double(p,  "min", xMinCelcius, remLen );
	p = json_objClose( p, remLen );

	p = json_objOpen( p, "bat_volts", remLen );
		p = json_double(p,  "current", xVolts, remLen );
		p = json_double(p,  "max", xMaxVolts, remLen );
		p = json_double(p,  "min", xMinVolts, remLen );
	p = json_objClose( p, remLen );

	p = json_objClose( p, remLen );
	return p;
}


