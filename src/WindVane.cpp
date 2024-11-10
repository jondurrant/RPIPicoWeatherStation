/*
 * WindVane.cpp
 *
 *  Created on: 14 Jul 2024
 *      Author: jondurrant
 */

#include "WindVane.h"
#include "hardware/adc.h"
#include <math.h>
#include "json-maker/json-maker.h"

float WindVane::XdegRes[16][2] = {
		{0,33000},
		{22.5, 6570},
		{45, 8200},
		{67.5, 891},
		{90, 1000},
		{112.5 , 688},
		{135, 2200},
		{157.5, 1410},
		{180, 3900},
		{202.5, 3140},
		{225, 16000},
		{247.5, 14120},
		{270, 120000},
		{292.5, 42120},
		{315, 64900},
		{337.5, 21880}

};

WindVane::WindVane(uint8_t adcGP, uint8_t pwrGP) {
	xAdcGP = adcGP;
	xPwrGP = pwrGP;

	 gpio_init(xPwrGP);

	gpio_set_dir(xPwrGP, GPIO_OUT);
	gpio_put(xPwrGP, false);

	gpio_init(xAdcGP);
	adc_gpio_init(xAdcGP);
}

WindVane::~WindVane() {
	// TODO Auto-generated destructor stub
}


void WindVane::setOffset(double deg){
	xOffset = deg;
}

void WindVane::start(){
	gpio_put(xPwrGP, true);
	xDeg = 0.0;
	xMin = -1.0;
	xMax = 0.0;
	xFirst = true;

	xActive = true;
}

void WindVane::stop(){
	gpio_put(xPwrGP, false);
	xActive = false;
}

void WindVane::sample(){
	if (!xActive)
		return;

	//Measure Volts
	 adc_select_input(xAdcGP - 26);
	 const float conversion_factor = 3.3f / (1 << 12);
	 uint16_t result = adc_read();
	 double Vout = ( result * conversion_factor);


	//Measure Resistance of R2
	double Vin = 3.3;

	double R1 = 4.7 * 1000;
	double R2 = (Vout * R1) / (Vin - Vout);

	float deg = WindVane::XdegRes[0][0];
	float res = WindVane::XdegRes[0][1];
	double err = fabs(res - R2);
	for (int i =1 ; i < 16;  i++){
		double e = fabs(WindVane::XdegRes[i][1] - R2);
		if (e < err){
			deg = WindVane::XdegRes[i][0];
			res = WindVane::XdegRes[0][1];
			err = e;
		}
	}

	//printf("R %f picked %f err %f\n",  r,  res, err );

	deg = deg + xOffset;
	if (deg >= 360.0){
		deg = deg - 360.0;
	}


	if (xFirst){
		xFirst = false;
		xDeg = deg;
	} else {
		xDeg = (xDeg + deg) / 2.0;
	}

	if (xDeg > xMax ) {
		xMax = xDeg;
	}
	if ((xMin < 0.0) || (xDeg < xMin)){
		xMin = xDeg;
	}
}


double WindVane::getDegrees(){
	return xDeg;
}

double WindVane::getMinDeg(){
	if (xMin < 0.0)
		return 0.0;
	return xMin;
}

double WindVane::getMaxDeg(){
	return xMax;
}

void WindVane::reset(){
	xOffset = 0.0;
	xDeg = 0.0;
	xMin = -1.0;
	xMax = 0.0;
	xActive = false;
	xFirst = false;
}

char* WindVane::writeJson( char* dest,const  char * name, size_t* remLen ) {
	char * p = dest;

	p = json_objOpen( p, name, remLen );
	p = json_double(p,  "current",  getDegrees(), remLen );
	p = json_double(p,  "max",  getMaxDeg(), remLen );
	p = json_double(p,  "min",  getMinDeg(), remLen );
	p = json_objClose( p, remLen );
	return p;
}




