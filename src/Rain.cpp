/*
 * Rain.cpp
 *
 *  Created on: 7 Aug 2024
 *      Author: jondurrant
 */

#include "Rain.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include <cstdio>

Rain::Rain(uint8_t gp) {
	xGP = gp;

	gpio_init(xGP);
	gpio_pull_up(xGP);
	gpio_set_dir(xGP, GPIO_IN);

	if (pwm_gpio_to_channel(xGP) != PWM_CHAN_B){
		printf("ERROR - GPIO Must be PWM Channel B\n");
	} else {
		xSlicer = pwm_gpio_to_slice_num(xGP);

	   // Count once for every 1 cycles the PWM B input is HIGH
	   pwm_config cfg = pwm_get_default_config();
	   pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_FALLING);
	   pwm_config_set_clkdiv(&cfg, 1);
	   pwm_init(xSlicer, &cfg, false);
	   gpio_set_function(xGP, GPIO_FUNC_PWM);
	   pwm_set_enabled(xSlicer, true);
	}
}

Rain::~Rain() {
	// TODO Auto-generated destructor stub
}

uint Rain::pulseCount(){
	return pwm_get_counter(xSlicer);
}

float Rain::mmRain(){
	return RAIN_MM_PER_PUSLSE * (float)pulseCount();
}

void Rain::reset(){
	pwm_set_counter	(xSlicer, 0);
}

