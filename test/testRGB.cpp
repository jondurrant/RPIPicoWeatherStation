/*
 * testRGB.cpp
 *
 *  Created on: 5 Sep 2024
 *      Author: jondurrant
 */

#include "CppUTest/TestHarness.h"

#include "StatusAgent.h"
#include "testConfig.h"


TEST_GROUP(RGBGrp){

};

TEST(RGBGrp, TestSwitch){
	StatusAgent agent(
			SWT_PAD,
			RED_PAD,
			GRN_PAD,
			BLU_PAD
			);
	agent.start("RGB",  TASK_PRIORITY);

	for (int i=0; i < 3; i++ ){
		agent.setStatus(0x00FF0000);
		vTaskDelay(1500);
		agent.setStatus(0x0000FF00);
		vTaskDelay(1500);
		agent.setStatus(0x000000FF);
		vTaskDelay(1500);
	}
	agent.setStatus(0x00303030);
	vTaskDelay(1500);

	agent.stop();
   CHECK_EQUAL(0, 	0);
}

TEST(RGBGrp, TestNoSwitch){
	StatusAgent agent(
			0xFF,
			RED_PAD,
			GRN_PAD,
			BLU_PAD
			);
	agent.start("RGB",  TASK_PRIORITY);
	agent.setStatus(0x00FF0000);
	vTaskDelay(1500);
	agent.setStatus(0x0000FF00);
	vTaskDelay(1500);
	agent.setStatus(0x000000FF);
	vTaskDelay(1500);
	agent.setStatus(0x00303030);
	vTaskDelay(1500);
	agent.stop();
   CHECK_EQUAL(0, 	0);
}





