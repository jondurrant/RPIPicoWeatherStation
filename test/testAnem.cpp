/*
 * testRain.cpp
 *
 *  Created on: 5 Sep 2024
 *      Author: jondurrant
 */

#include "CppUTest/TestHarness.h"

#include "AnemometerPWM.h"
#include "testConfig.h"


TEST_GROUP(AnemGrp){

};

TEST(AnemGrp, TestCount){

	AnemometerPWM anem(ANEM_PAD);
	printf("Wind count start\n");
	anem.start();
	vTaskDelay(3000);

	printf("Wind count %f\n", anem.pulsesPerSec());
	anem.stop();

   CHECK(anem.pulsesPerSec() > 0.0);
}







