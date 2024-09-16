/*
 * WeatherStation.cpp
 *
 *  Created on: 13 Sept 2024
 *      Author: jondurrant
 */

#include "WeatherStation.h"
#include "WeatherStationPayload.h"
#include "hardware/adc.h"

WeatherStation::WeatherStation() {
}

void WeatherStation::init(){
		adc_init();
	 	pAnem 		=	new AnemometerPWM(ANEM_PAD);
		pVane 		=	new WindVane(DIRA_PAD, DIRP_PAD);
		pRain			=	new Rain(RAIN);

		i2c_init(I2CCHAN, 100 * 1000);
		gpio_set_function(SDA1_PAD, GPIO_FUNC_I2C);
		gpio_set_function(SCL1_PAD, GPIO_FUNC_I2C);

		pSnrCtr 	= 	new PowerCtr(SNR_CTR);
		pSnrCtr->pullUpI2C(SDA1_PAD, SCL1_PAD);
		//pAHT10	= new LIB_AHTXX(
		pAHT10	= new AHT10Status(
					AHT10_ADDRESS_0X38,
					I2CCHAN,
					SDA1_PAD,
					SCL1_PAD,
					100 * 1);
		pSen0500 	= new Sen0500Status(I2CCHAN);
		pRTC 				= new RTCStatus(i2c0,  SDA0_PAD,  SCL0_PAD, RTC_BAT);

		pPico 				= new PicoStatus();


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
	delete pRTC ;
	delete pPico;
}

void WeatherStation::start(){
	pSnrCtr->on();
	pVane->start();
	pRain->start();
	pAnem->start();
	pAHT10->start();
	pRTC->start();
	pSen0500->start();

}

void WeatherStation::stop(){
	pVane->stop();
	pRain->stop();
	pAnem->stop();
	pAHT10->stop();
	pSen0500->stop();
	pSnrCtr->off();

	pRTC->stop();
}

void WeatherStation::sample(){
	pRain->sample();
	pVane->sample();
	pAnem->sample();
	pAHT10->sample();
	pRTC->sample();
	pSen0500->sample();
}

void WeatherStation::submit(){
	WeatherStationPayload payload;
	payload.addPart("pico", pPico);
	payload.addPart("rain", pRain);
	payload.addPart("vain", pVane);
	payload.addPart("anem", pAnem);
	payload.addPart("aht10", pAHT10);
	payload.addPart("rtc", pRTC);
	payload.addPart("sen0500", pSen0500);
	printf("%s\n", payload.json());
}

void WeatherStation::reset(){
	pAnem->reset();
	pVane->reset();
	pRain->reset();
	pAHT10->reset();
}
