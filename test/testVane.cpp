/*
 * testRain.cpp
 *
 *  Created on: 5 Sep 2024
 *      Author: jondurrant
 */

#include "CppUTest/TestHarness.h"

#include "WindVane.h"
#include "testConfig.h"


TEST_GROUP(VaneGrp){

};

TEST(VaneGrp, TestVane){

	WindVane vane(DIRA_PAD, DIRP_PAD);
	vane.start();
	for (int i=0; i < 10; i++){
		for (int y=0; y < 10; y++){
			vane.sample();
			vTaskDelay(100);
		}
		printf("Vane %0.2f \n", vane.getDegrees());
	}
	vane.stop();

}







