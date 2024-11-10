/*
 * AHT10Status.cpp
 *
 *  Created on: 14 Sept 2024
 *      Author: jondurrant
 */

#include "AHT10Status.h"

#include "json-maker/json-maker.h"
#include <cstdio>
#include "FreeRTOS.h"
#include "task.h"

AHT10Status::AHT10Status(
		uint8_t address,
		i2c_inst_t* i2c_type,
		uint8_t sdata ,
		uint8_t sclk ,
		uint16_t clockspeed):
		LIB_AHTXX( address, i2c_type,  sdata ,  sclk , clockspeed){
	// NOP
}

AHT10Status::~AHT10Status() {
	// NOP
}

void AHT10Status::start(){
	AHT10_InitI2C(AHT10_SENSOR);

	for (int i=0; i < AHT10RETRY; i++){
		  if (AHT10_begin()) {
			  xError = false;

			  //printf("OK: Cel=%.2f\n", AHT10_readTemperature(true));
			  return;
		  }
		 printf("AHT10 not connect r fail load calib coeff \r\n");
		vTaskDelay(500);
	}

	xError = true;
}

void AHT10Status::stop(){
	AHT10_DeInit();
}

void AHT10Status::reset(){
	 xFirst = true;
	 xCel = 0.0;
	 xHum = 0.0;
	 xMinCel = 0.0;
	 xMaxCel = 0.0;
	 xMinHum = 0.0;
	 xMaxHum = 0.0;
}

void AHT10Status::sample(){
	xCel = AHT10_readTemperature(true);
	xHum =  AHT10_readHumidity(true);
	if (xFirst){
		xFirst = false;
		xMinCel = xMaxCel = xCel;
		xMinHum = xMaxHum = xHum;
	} else {
		if (xCel > xMaxCel){
			xMaxCel = xCel;
		}
		if (xCel < xMinCel){
			xMinCel = xCel;
		}
		if (xHum > xMaxHum){
			xMaxHum = xHum;
		}
		if (xHum < xMinHum){
			xMinHum = xHum;
		}
	}


}


/*
 * Target format
 * "aht10": {
            "error": False,
            "celcius": {
                "current": 19.7811,
                "max": 19.8552,
                "min": 19.7775
                },
            "humidity": {
                "current": 67.9592,
                "max": 68.5895,
                "min": 67.5763
                }
            },
 */
char* AHT10Status::writeJson( char* dest,const  char * name, size_t* remLen ) {
	char * p = dest;

	if (xError){
		xCel = 0.0;
		xHum = 0.0;
	}

	p = json_objOpen( p, name, remLen );
	p = json_bool(p,  "error",  xError, remLen );

	p = json_objOpen( p, "celcius", remLen );
		p = json_double(p,  "current",  xCel, remLen );
		p = json_double(p,  "max",  xMaxCel, remLen );
		p = json_double(p,  "min",  xMinCel, remLen );
	p = json_objClose( p, remLen );

	p = json_objOpen( p, "humidity", remLen );
		p = json_double(p,  "current",  xHum, remLen );
		p = json_double(p,  "max",  xMaxHum, remLen );
		p = json_double(p,  "min",  xMinHum, remLen );
	p = json_objClose( p, remLen );

	p = json_objClose( p, remLen );
	return p;
}
