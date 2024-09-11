/*
 * testRain.cpp
 *
 *  Created on: 5 Sep 2024
 *      Author: jondurrant
 */

#include "CppUTest/TestHarness.h"

#include "SDLog.h"
#include "PowerCtr.h"
#include "testConfig.h"
#include <cstdio>



TEST_GROUP(SDLogGrp){

};

TEST(SDLogGrp, Test){
	SDLog log;
	//PowerCtr ctr(5);
	//ctr.on();

	CHECK(log.init());
	CHECK(log.mount());

	CHECK(log.list());

}




