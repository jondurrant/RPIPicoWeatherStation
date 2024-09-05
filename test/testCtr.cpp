/*
 * testRain.cpp
 *
 *  Created on: 5 Sep 2024
 *      Author: jondurrant
 */

#include "CppUTest/TestHarness.h"

#include "PowerCtr.h"
#include "testConfig.h"
#include <cstdio>


TEST_GROUP(PwrGrp){

};

TEST(PwrGrp, Test){
	PowerCtr ctr(SNR_CTR);


	for (;;){
		ctr.on();
		printf("On\n");
		vTaskDelay(1000);
		ctr.off();
		printf("Off\n");
		vTaskDelay(1000);
	}


}







