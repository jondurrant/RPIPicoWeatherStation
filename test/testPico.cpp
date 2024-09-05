/*
 * testRain.cpp
 *
 *  Created on: 5 Sep 2024
 *      Author: jondurrant
 */

#include "CppUTest/TestHarness.h"

#include "PicoStatus.h"
#include "testConfig.h"
#include <cstdio>



TEST_GROUP(PicoStatusGrp){

};

TEST(PicoStatusGrp, Test){
	PicoStatus p;

	if (p.isVSYS()){
		printf("Powered via VSYS \n");
	} else {
		printf("Powered via VBUS \n");
	}

	printf("Volts is %.2f\n", p.vsysVolts());

	printf("Temp %.2f\n", p.tempCelcius());
}




