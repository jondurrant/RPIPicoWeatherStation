/*
 * PowerCtr.cpp
 *
 *  Created on: 8 Aug 2024
 *      Author: jondurrant
 */

#include "PowerCtr.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"


PowerCtr::PowerCtr(uint8_t gp){
	xGP = gp;

	 gpio_init(xGP);
	 gpio_set_dir(xGP, GPIO_OUT);
	 gpio_put(xGP, false);

}

PowerCtr::~PowerCtr() {
}



void PowerCtr::pullUpI2C(uint8_t sda, uint8_t scl){
	xSDA = sda;
	xSCL = scl;
}

void PowerCtr::on(){
	if (xGP <= 28){
		gpio_put(xGP, true);
		//printf("Pull up %u\n", xGP);
	}
	if (xSDA <= 28){
		gpio_pull_up(xSDA);
	}
	if (xSCL <= 28){
		gpio_pull_up(xSCL);
	}
}


void PowerCtr::off(){
	if (xGP <= 28){
		gpio_put(xGP, false);
	}
	if (xSDA <= 28){
		gpio_disable_pulls(xSDA);
	}
	if (xSCL <= 28){
		gpio_disable_pulls(xSCL);
	}
}
