/*
 * testRain.cpp
 *
 *  Created on: 5 Sep 2024
 *      Author: jondurrant
 */

#include "CppUTest/TestHarness.h"

#include "Sen0500.h"
#include "PowerCtr.h"
#include "testConfig.h"
#include <cstdio>
#define I2CCHAN i2c1



TEST_GROUP(Sen0500Grp){

};

TEST(Sen0500Grp, Test){
    PowerCtr snrCtr(SNR_CTR);

    i2c_init(I2CCHAN, 100 * 1000);
	gpio_set_function(SDA1_PAD, GPIO_FUNC_I2C);
	gpio_set_function(SCL1_PAD, GPIO_FUNC_I2C);
	snrCtr.pullUpI2C(SDA1_PAD, SCL1_PAD);
	snrCtr.on();

	 Sen0500 sen(I2CCHAN);

	 vTaskDelay(10000);

	 float temp;
	 float humid;
	 uint16_t atmos;
	 float uv;
	 float lumi;


	 for (int i=0; i < 10; i++){

		 sen.readTemp(&temp);
		 sen.readHumid(&humid);
		 sen.readPressure(&atmos);
		 sen.readUV(&uv);
		 sen.readLumi(&lumi);



		  printf("SEN0500 \t%.2f C, %.2f%%, %dhPa, %.5fmW/M2, %0.5flx\n",
				 temp,
				 humid,
				 atmos,
				 uv,
				lumi
				 );

		  vTaskDelay(2000);
	 }
}




