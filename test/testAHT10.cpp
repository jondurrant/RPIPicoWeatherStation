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
#include "ahtxx/ahtxx.hpp"
#include "AHT10Status.h"


TEST_GROUP(AHT10Grp){

};




TEST(AHT10Grp, TestStatus){
	PowerCtr snrCtr(SNR_CTR);

	 i2c_init(I2CCHAN, 100 * 1000);
	gpio_set_function(SDA1_PAD, GPIO_FUNC_I2C);
	gpio_set_function(SCL1_PAD, GPIO_FUNC_I2C);
	snrCtr.pullUpI2C(SDA1_PAD, SCL1_PAD);
	snrCtr.on();

	AHT10Status myAHT10(AHT10_ADDRESS_0X38, I2CCHAN, SDA1_PAD,  SCL1_PAD, 100 * 1);

	myAHT10.start();

	printf("AHT10 Status   \t%.2fC,  %.2f\n",
								myAHT10.AHT10_readTemperature(true),
								myAHT10.AHT10_readHumidity(true));
}


TEST(AHT10Grp, Test){
	PowerCtr snrCtr(SNR_CTR);


	 i2c_init(I2CCHAN, 100 * 1000);
	gpio_set_function(SDA1_PAD, GPIO_FUNC_I2C);
	gpio_set_function(SCL1_PAD, GPIO_FUNC_I2C);
	snrCtr.pullUpI2C(SDA1_PAD, SCL1_PAD);
	snrCtr.on();

	 LIB_AHTXX myAHT10(AHT10_ADDRESS_0X38, I2CCHAN, SDA1_PAD,  SCL1_PAD, 100 * 1);

	 myAHT10.AHT10_InitI2C(AHT10_SENSOR);
	while (myAHT10.AHT10_begin() != true) {
			printf("AHT10 not connect r fail load calib coeff \r\n");
			busy_wait_ms(5000);
	}

	printf("AHT10   \t%.2fC,  %.2f\n",
							myAHT10.AHT10_readTemperature(true),
							myAHT10.AHT10_readHumidity(true));

}







