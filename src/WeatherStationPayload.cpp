/*
 * WeatherStationPayload.cpp
 *
 *  Created on: 12 Sept 2024
 *      Author: jondurrant
 */

#include "WeatherStationPayload.h"
#include "json-maker/json-maker.h"
#include "hardware/rtc.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include "FreeRTOS.h"
#include "Task.h"

WeatherStationPayload::WeatherStationPayload(RTCStatus * rtc) {
	addPart("header", this);
	pRTC = rtc;
}

WeatherStationPayload::WeatherStationPayload() {
	addPart("header", this);
	pRTC = NULL;
}

WeatherStationPayload::~WeatherStationPayload() {
	// TODO Auto-generated destructor stub
}

char * WeatherStationPayload::json(){
	char *p = xBuffer;
	size_t rem = WS_PAYLOAD_LEN;

	p = json_objOpen( p,  NULL, &rem );

	std::map<std::string, PayloadPart *>::iterator it = xPayloadParts.begin();
	while (it != xPayloadParts.end()){
		p = it->second->writeJson(p,   it->first.c_str(), &rem);
		it++;
	}

	p = json_objClose( p,  &rem );
	p =  json_end( p,  &rem );

	return xBuffer;
}

char* WeatherStationPayload::writeJson( char* dest, const char * name, size_t* remLen ){
	char *p = dest;
	datetime_t t;

	t.year = 0;
	t.month=0;
	t.day=0;
	t.hour=0;
	t.min = 0;
	t.sec = 0;


	if (pRTC != NULL){
		if (!rtc_get_datetime(&t)){
			t.year = pRTC->get_year();
			t.month=pRTC->get_mon();
			t.day=pRTC->get_day();
			t.hour=pRTC->get_hou();
			t.min = pRTC->get_min();
			t.sec = pRTC->get_sec();
		}
	}

	p = json_objOpen( p, name, remLen );
	p = json_str(p, "type", "WeatherStationPayload", remLen );
	p = json_uint(p, "version",  1, remLen );
		p = json_objOpen( p, "timestamp", remLen );
		p = json_uint(p, "year",  t.year, remLen );
		p = json_uint(p, "month",  t.month, remLen );
		p = json_uint(p, "day",  t.day, remLen );
		p = json_uint(p, "hour",  t.hour, remLen );
		p = json_uint(p, "min",  t.min, remLen );
		p = json_uint(p, "sec",  t.sec, remLen );
		p = json_objClose( p, remLen );
	p = json_objClose( p, remLen );

	return p;
}

void WeatherStationPayload::addPart(const char * name, PayloadPart * p){
	xPayloadParts[name] = p;
}

