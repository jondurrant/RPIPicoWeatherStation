/*
 * WeatherStation.h
 *
 *  Created on: 13 Sept 2024
 *      Author: jondurrant
 */

#ifndef SRC_WEATHERSTATION_H_
#define SRC_WEATHERSTATION_H_

#include "pico/stdlib.h"

#include "config.h"


#include "AnemometerPWM.h"
#include "WindVane.h"
#include "Rain.h"
#include "hardware/i2c.h"
//#include "Sen0500.h"
#include "Sen0500Status.h"
//#include "ahtxx/ahtxx.hpp"
#include "AHT10Status.h"
#include "PicoStatus.h"
#include "PowerCtr.h"
//#include "DS3231.hpp"
#include "RTCStatus.h"

#ifndef WEATHER_STATION_HTTP_BUF
#define WEATHER_STATION_HTTP_BUF 4096
#endif

#ifndef WEATHER_STATION_SUBMIT_URL
#define WEATHER_STATION_SUBMIT_URL "http://vmu22a.local.jondurrant.com:5080/sampleSubmit"
#endif

class WeatherStation {
public:
	WeatherStation();
	virtual ~WeatherStation();

	void init();

	void start();
	void stop();
	void reset();
	void sample();
	void submit();

	RTCStatus * getRTC();

private:
	AnemometerPWM 	*pAnem;
	WindVane 					*pVane;
	Rain 								*pRain;

	PowerCtr 						*pSnrCtr;
	//LIB_AHTXX 				*pAHT10;
	AHT10Status				*pAHT10;
	//Sen0500 						*pSen0500;
	Sen0500Status 			*pSen0500;
	RTCStatus					*pRTC;

	PicoStatus					*pPico;

	char 								xHttpBuffer[WEATHER_STATION_HTTP_BUF];

};

#endif /* SRC_WEATHERSTATION_H_ */
