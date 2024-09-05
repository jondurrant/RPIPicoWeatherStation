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
#include "DS3231.hpp"
#include "VoltMeter.h"


TEST_GROUP(DS3231Grp){

};

TEST(DS3231Grp, Test){
	 DS3231 rtc(i2c0,  SDA0_PAD,  SCL0_PAD);
	 rtc.set_power_gp(RTC_VCC);
	 rtc.on();
	 vTaskDelay(500);
	 printf("RTC: %s\n", rtc.get_time_str());
	 printf("RTC Temp %f\n", rtc.get_temp_f());

}


TEST(DS3231Grp, BattVolt){
	VoltMeter volts(RTC_BAT);
	printf("RTC Batt %f V\n", volts.volts());
}




