/*
 * testRain.cpp
 *
 *  Created on: 5 Sep 2024
 *      Author: jondurrant
 */

#include "CppUTest/TestHarness.h"

#include "Rain.h"
#include "testConfig.h"


TEST_GROUP(RainGrp){

};

TEST(RainGrp, TestCount){

	Rain rain(RAIN);
	printf("Rain count start\n");
	vTaskDelay(3000);
	printf("Rain count %d\n", rain.pulseCount());

   CHECK(rain.pulseCount() > 0);
}







