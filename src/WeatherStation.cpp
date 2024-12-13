/*
 * WeatherStation.cpp
 *
 *  Created on: 13 Sept 2024
 *      Author: jondurrant
 */

#include "WeatherStation.h"
#include "WeatherStationPayload.h"
#include "hardware/adc.h"
#include "Request.h"
#include "ConfigRequest.h"

#include "tiny-json.h"
#include "NVSOnboard.h"

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


		//Check Configuration
		int32_t nOffset;
		if (NVS_OK == NVSOnboard::getInstance()->get_i32("offset", &nOffset)){
			pVane->setOffset(double(nOffset));
		}
		NVSOnboard::getInstance()->get_u32("sleep", &xSleepMin);


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
	pPico->start();

}

void WeatherStation::stop(){
	pVane->stop();
	pRain->stop();
	pAnem->stop();
	pAHT10->stop();
	pSen0500->stop();
	pSnrCtr->off();

	pRTC->stop();
	pPico->stop();
}

void WeatherStation::sample(){
	pRain->sample();
	pVane->sample();
	pAnem->sample();
	pAHT10->sample();
	pRTC->sample();
	pSen0500->sample();
	pPico->sample();
}

void WeatherStation::submit(){
	WeatherStationPayload payload(getRTC());
	payload.addPart("pico", pPico);
	payload.addPart("rain", pRain);
	payload.addPart("vain", pVane);
	payload.addPart("anem", pAnem);
	payload.addPart("aht10", pAHT10);
	payload.addPart("rtc", pRTC);
	payload.addPart("sen0500", pSen0500);
	printf("%s\n", payload.json());

	Request req(xHttpBuffer, WEATHER_STATION_HTTP_BUF);
	req.postJSON(WEATHER_STATION_SUBMIT_URL, &payload);
}

void WeatherStation::reset(){
	pAnem->reset();
	pVane->reset();
	pRain->reset();
	pAHT10->reset();
	pPico->reset();
}

RTCStatus * WeatherStation::getRTC(){
	return pRTC;
}


void WeatherStation::checkConfig(){
	ConfigRequest conReq;
	Request req(xHttpBuffer, WEATHER_STATION_HTTP_BUF);
	printf("Req JSON: %s\n", conReq.json());
	if (req.postJSON(WEATHER_STATION_CONFIG_URL, &conReq)){
		printf("Payload %.*s\n",  req.getPayloadLen(), req.getPayload());

		int l = req.getPayloadLen();
		memcpy(xHttpBuffer, req.getPayload(), l);
		xHttpBuffer[l] = 0;

		json_t mem[8];
		json_t const* json = json_create(xHttpBuffer, mem, sizeof mem / sizeof *mem );
		if ( !json ) {
			puts("Error json create.");
			return ;
		}

		json_t const* jVersion = json_getProperty( json, "version" );
		if ( !jVersion || JSON_INTEGER != json_getType( jVersion ) ) {
			printf("No VERSION\n");
			return ;
		}
		int32_t nVersion = json_getInteger( jVersion );

		bool newConfig = false;
		int32_t version;
		if (NVS_OK  != NVSOnboard::getInstance()->get_i32("version", &version)) {
			newConfig = true;
		} else {
			if (nVersion > version){
				newConfig = true;
			}
		}

		if (newConfig){
			printf("Updating Config from %d to %d\n", version, nVersion);
			NVSOnboard::getInstance()->set_i32("version", nVersion);

			json_t const* jOffset = json_getProperty( json, "offset" );
			if ( !jOffset || JSON_INTEGER != json_getType( jOffset ) ) {
				printf("No OFFSET\n");
			} else {
				int32_t nOffset = json_getInteger( jOffset );
				NVSOnboard::getInstance()->set_i32("offset", nOffset);
				pVane->setOffset(double(nOffset));
			}

			json_t const* jSleep = json_getProperty( json, "sleep" );
			if ( !jSleep || JSON_INTEGER != json_getType( jSleep ) ) {
				printf("No SLEEP\n");
			} else {
				xSleepMin = json_getInteger( jSleep );
				NVSOnboard::getInstance()->set_u32("sleep", xSleepMin);
			}

			NVSOnboard::getInstance()->commit();
		} else {
			printf("Current Config is latest\n");
		}
	} else {
		printf("Failed to get config\n");
	}


}

uint32_t WeatherStation::getSleepMin(){
	return xSleepMin;
}
