/*
 * testPayload.cpp
 *
 *  Created on: 12 Sept 2024
 *      Author: jondurrant
 */

#include "CppUTest/TestHarness.h"

#include "WeatherStationPayload.h"
#include "PicoStatus.h"
#include "testConfig.h"
#include <cstdio>



TEST_GROUP(PayloadGrp){

};

TEST(PayloadGrp, Test){
	WeatherStationPayload payload;

	printf("JSON:\n %s\n", payload.json());
}


TEST(PayloadGrp, Pico){
	WeatherStationPayload payload;

	PicoStatus p;
	payload.addPart("pico",  &p);


	printf("JSON:\n %s\n", payload.json());
}



