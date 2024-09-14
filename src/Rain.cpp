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
#include "json-maker/json-maker.h"

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
	 reset();
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

float Rain::maxMMPS(){
	return xMaxMMPS;
}

float Rain::minMMPS(){
	return xMinMMPS;
}

float Rain::periodMM(){
	return xPeriodMM;
}

void Rain::reset(){
	pwm_set_counter	(xSlicer, 0);
	xPeriodMM = 0.0;
	xMaxMMPS = 0.0;
	xMinMMPS = 0.0;
	xSampleMS = to_ms_since_boot (get_absolute_time () );
	xSampleCount = 0;
	xMSSinceCount = 0;
}
void Rain::start(){
	xPeriodMM = 0.0;
	xStartCount = pulseCount();
}
void Rain::stop(){
	xPeriodMM = (float)(pulseCount() - xStartCount ) * RAIN_MM_PER_PUSLSE;
}

void Rain::sample(){
	uint count =  pulseCount();
	uint32_t now =  to_ms_since_boot (get_absolute_time () );
	uint inc = count - xSampleCount;
	float sec = (float)(now - xSampleMS) / 1000.0;
	float mmps = ((float)inc * RAIN_MM_PER_PUSLSE) / sec;

	if (sec == 0.0){
		mmps = 0.0;
	}

	if (mmps > xMaxMMPS){
		xMaxMMPS = mmps;
	}

	if ((xMinMMPS != 0.0) && (mmps != 0.0)) {
		if (mmps < xMinMMPS){
			xMinMMPS = mmps;
		}
	}

	if (inc != 0){
		xMSSinceCount = 0;
	} else {
		xMSSinceCount += (now - xSampleMS);
	}
	xSampleCount = count;
	xSampleMS = now;
}


char* Rain::writeJson( char* dest,const  char * name, size_t* remLen ) {
	char * p = dest;

	p = json_objOpen( p, name, remLen );
	p = json_double(p,  "cumlative_mm",  mmRain(), remLen );
	p = json_double(p,  "max_mmps",  maxMMPS(), remLen );
	p = json_double(p,  "min_mmps",  minMMPS(), remLen );
	p = json_double(p,  "period_mm",  periodMM(), remLen );
	p = json_double(p,  "since_sec",  secSinceRain(), remLen );
	p = json_objClose( p, remLen );
	return p;
}

float Rain::secSinceRain(){
	return (float)xMSSinceCount / 1000.0;
}

