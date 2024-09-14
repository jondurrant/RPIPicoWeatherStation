/*
 * WeatherStation.cpp
 *
 *  Created on: 13 Sept 2024
 *      Author: jondurrant
 */

#include "WeatherStation.h"
#include "WeatherStationPayload.h"

WeatherStation::WeatherStation() {
}

void WeatherStation::init(){
	 	pAnem 		=	new AnemometerPWM(ANEM_PAD);
		pVane 		=	new WindVane(DIRA_PAD, DIRP_PAD);
		pRain			=	new Rain(RAIN);

		pSnrCtr 	= 	new PowerCtr(SNR_CTR);
		pAHT10	= new LIB_AHTXX(
					AHT10_ADDRESS_0X38,
					I2CCHAN,
					SDA1_PAD,
					SCL1_PAD,
					100 * 1);
		pSen0500 	= new Sen0500(I2CCHAN);
		pRtc 				= new DS3231(i2c0,  SDA0_PAD,  SCL0_PAD);


		//Perm running sensors
		pAnem->start();
}

WeatherStation::~WeatherStation() {
	delete pAnem;
	delete pVane;
	delete pRain;

	delete pSnrCtr;
	delete pAHT10;
	delete pSen0500;
	delete pRtc ;
}

void WeatherStation::start(){
	pVane->start();
	pRain->start();

}

void WeatherStation::stop(){
	pVane->stop();
	pRain->stop();
}

void WeatherStation::sample(){
	pRain->sample();
}

void WeatherStation::submit(){
	WeatherStationPayload payload;
	payload.addPart("rain", pRain);

	printf("%s\n", payload.json());
}

void WeatherStation::reset(){
	//pAnem->reset();
	//pVane->reset();
	pRain->reset();
}
