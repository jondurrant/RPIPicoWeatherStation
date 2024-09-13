/*
 * WeatherStationPayload.h
 *
 *  Created on: 12 Sept 2024
 *      Author: jondurrant
 */

#ifndef SRC_WEATHERSTATIONPAYLOAD_H_
#define SRC_WEATHERSTATIONPAYLOAD_H_

#include "JSONSerialisable.h"
#if CPPUTEST_USE_NEW_MACROS
   #undef new
#endif
#include <map>
#if CPPUTEST_USE_NEW_MACROS
   #include "CppUTest/MemoryLeakDetectorNewMacros.h"
#endif
#include "PayloadPart.h"
#include <string>

#ifndef WS_PAYLOAD_LEN
#define WS_PAYLOAD_LEN 1024
#endif

class WeatherStationPayload  :
		public JSONSerialisable,
		public PayloadPart {
public:
	WeatherStationPayload();
	virtual ~WeatherStationPayload();

	virtual char * json();

	virtual char* writeJson( char* dest, const char * name, size_t* remLen );

	void addPart(const char * name, PayloadPart * p);

private:
	char xBuffer[WS_PAYLOAD_LEN];

	std::map<std::string,  PayloadPart *> xPayloadParts;

};

#endif /* SRC_WEATHERSTATIONPAYLOAD_H_ */
