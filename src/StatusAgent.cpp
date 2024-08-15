/*
 * StatusAgent.cpp
 *
 *  Created on: 15 Aug 2024
 *      Author: jondurrant
 */

#include "StatusAgent.h"
#include <cstdio>

StatusAgent::StatusAgent(
		uint8_t swt,
		uint8_t red,
		uint8_t grn,
		uint8_t blu
		){
	xSWT = swt;
	xRED = red;
	xGRN = grn;
	xBLU = blu;

	gpio_init(xSWT);
	gpio_set_dir(xSWT, GPIO_IN);
	gpio_pull_up(xSWT);

	gpio_init(xRED);
	uint slicer = pwm_gpio_to_slice_num(xRED);
	gpio_set_function(xRED, GPIO_FUNC_PWM);
	pwm_set_enabled(slicer, true);
	//gpio_set_dir(xRED, GPIO_OUT);
	pwm_set_gpio_level (xRED, 0 );


	gpio_init(xGRN);
	slicer = pwm_gpio_to_slice_num(xGRN);
	gpio_set_function(xGRN, GPIO_FUNC_PWM);
	pwm_set_enabled(slicer, true);
	//gpio_set_dir(xGRN, GPIO_OUT);
	pwm_set_gpio_level (xGRN, 0 );



	gpio_init(xBLU);
	slicer = pwm_gpio_to_slice_num(xBLU);
	gpio_set_function(xBLU, GPIO_FUNC_PWM);
	pwm_set_enabled(slicer, true);
	//gpio_set_dir(xBLU, GPIO_OUT);
	pwm_set_gpio_level (xBLU, 0 );
	printf("BLU %u slicer %u\n", xBLU, slicer);
}

StatusAgent::~StatusAgent() {
	// NOP
}

configSTACK_DEPTH_TYPE StatusAgent::getMaxStackSize(){
	return 256;
}

void StatusAgent::run(){
	bool lastSwtState = false;
	for (;;){
		bool swtState = gpio_get(xSWT);
		if (swtState != lastSwtState){
			lastSwtState = swtState;

			if (!swtState){
				displayStatus( xStatus);
			} else {
				displayStatus( 0x00);
			}
		}
		vTaskDelay(1000);
	}
}

void StatusAgent::displayStatus(uint32_t status){
	uint16_t blu = status & 0xFF;
	blu = blu * blu;
	pwm_set_gpio_level (xBLU,  blu);

	uint16_t grn = (status & 0xFF00) >> 8;
	grn = grn * grn;
	pwm_set_gpio_level (xGRN,  grn);

	uint16_t red = (status & 0xFF0000) >> 16;
	red = red * red;
	pwm_set_gpio_level (xRED,  red);

	printf("Displayed(%d,%d,%d) %X %X %X\n", xRED, xGRN, xBLU,  red, grn, blu);
}


void StatusAgent::setStatus(uint32_t rgb){
	xStatus = rgb;
}


