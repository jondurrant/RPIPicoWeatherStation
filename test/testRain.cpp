/*
 * testRain.cpp
 *
 *  Created on: 5 Sep 2024
 *      Author: jondurrant
 */

#include "CppUTest/TestHarness.h"

#include "testConfig.h"
#include "Rain.h"
#include "testConfig.h"
#include "WeatherStationPayload.h"
#include <cstdio>


TEST_GROUP(RainGrp){

};

TEST(RainGrp, TestCount){

	Rain rain(RAIN);
	printf("Rain count start\n");
	vTaskDelay(3000);
	printf("Rain count %d\n", rain.pulseCount());

   CHECK(rain.pulseCount() > 0);
}

TEST(RainGrp, TestStats){

	Rain rain(RAIN);
	printf("Rain count start\n");
	rain.start();

	for (int i=0; i < 10; i++){
		printf("%d\n", i);
		rain.sample();
		vTaskDelay(2000);
	}
	rain.stop();

	printf("Rain count %d\n", rain.pulseCount());
	printf("mmRain %.2f\n", rain.mmRain());
	printf("maxMMPS %.2f\n", rain.maxMMPS());
	printf("minMMPS %.2f\n", rain.minMMPS());
	printf("periodMM %.2f\n", rain.periodMM());
	printf("secSinceRain %.2f\n", rain.secSinceRain());

	WeatherStationPayload payload;
	payload.addPart("rain", &rain);
	printf("JSON:\n %s\n", payload.json());

   CHECK(rain.pulseCount() > 0);
}









