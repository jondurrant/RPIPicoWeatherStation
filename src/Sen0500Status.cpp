/*
 * Sen0500Status.cpp
 *
 *  Created on: 16 Sept 2024
 *      Author: jondurrant
 */

#include "Sen0500Status.h"
#include <math.h>
#include "json-maker/json-maker.h"

Sen0500Status::Sen0500Status(i2c_inst_t * i2c,  uint16_t addr):
	Sen0500(i2c, addr) {
	reset();
}

Sen0500Status::~Sen0500Status() {
	// NOP
}


void Sen0500Status::start(){

}

void Sen0500Status::stop(){

}

void Sen0500Status::reset(){
	xFirst = true;
	xError = true;

	xCelcius = 0.0;
	xCelciusMax = 0.0;
	xCelciusMin = 0.0;

	xHumid = 0.0;
	xHumidMax = 0.0;
	xHumidMin = 0.0;

	xHPa = 0.0;
	xHPaMax = 0.0;
	xHPaMin = 0.0;

	xUV = 0.0;
	xUVMax = 0.0;
	xUVMin = 0.0;

	xLumi = 0.0;
	xLumiMax = 0.0;
	xLumiMin = 0.0;
}

void Sen0500Status::sample(){
	xError = false;

	//Ceclius
	float cel;
	if (!readTemp(&cel)){
		xError = true;
		return;
	} else {
		if (cel < -40.0){
			xError = true;
			return;
		}
		xCelcius = cel;
		if (xFirst){
			xCelciusMax = cel;
			xCelciusMin = cel;
		} else {
			xCelciusMax 	= fmax(xCelciusMax, cel);
			xCelciusMin 	= fmin(xCelciusMin, cel);
		}
	}

	//Humid
	float hum;
	if (!readHumid(&hum)){
		xError = true;
		return;
	} else {
		xHumid = hum;
		if (xFirst){
			xHumidMax = hum;
			xHumidMin = hum;
		} else {
			xHumidMax 	= fmax(xHumidMax, hum);
			xHumidMin 		= fmin(xHumidMin, hum);
		}
	}


	//HPA
	uint16_t hpa;
	if (!readPressure(&hpa)){
		xError = true;
		return;
	} else {
		xHPa = hpa;
		if (xFirst){
			xHPaMax = hpa;
			xHPaMin = hpa;
		} else {
			xHPaMax 		= fmax(xHPaMax, hpa);
			xHPaMin 		= fmin(xHPaMin, hpa);
		}
	}

	//UV
	float uv;
	if (!readUV(&uv)){
		xError = true;
		return;
	} else {
		xUV = uv;
		if (xFirst){
			xUVMax = uv;
			xUVMin = uv;
		} else {
			xUVMax 		= fmax(xUVMax, uv);
			xUVMin 		= fmin(xUVMin, uv);
		}
	}

	//Lumi
	float luni;
	if (!readLumi(&luni)){
		xError = true;
		return;
	} else {
		xLumi = luni;
		if (xFirst){
			xLumiMax = luni;
			xLumiMin = luni;
		} else {
			xLumiMax 		= fmax(xLumiMax, luni);
			xLumiMin 		= fmin(xLumiMin, luni);
		}
	}

	//Handle first reset
	if (xFirst){
		xFirst = false;
	}
}

char* Sen0500Status::writeJson( char* dest,const  char * name, size_t* remLen ) {
	char * p = dest;

	p = json_objOpen( p, name, remLen );
	p = json_bool(p,  "error",  xError, remLen );


	//Celcius
	p = json_objOpen( p, "celcius", remLen );
		p = json_double(p,  "current",  xCelcius, remLen );
		p = json_double(p,  "min",  xCelciusMin, remLen );
		p = json_double(p,  "max",  xCelciusMax, remLen );
	p = json_objClose( p, remLen );

	//Humid
	p = json_objOpen( p, "humid", remLen );
		p = json_double(p,  "current",  xHumid, remLen );
		p = json_double(p,  "min",  xHumidMin, remLen );
		p = json_double(p,  "max",  xHumidMax, remLen );
	p = json_objClose( p, remLen );

	//hPa
	p = json_objOpen( p, "hPa", remLen );
		p = json_double(p,  "current",  xHPa, remLen );
		p = json_double(p,  "min",  xHPaMin, remLen );
		p = json_double(p,  "max",  xHPaMax, remLen );
	p = json_objClose( p, remLen );

	//hPa
	p = json_objOpen( p, "uv", remLen );
		p = json_double(p,  "current",  xUV, remLen );
		p = json_double(p,  "min",  xUVMin, remLen );
		p = json_double(p,  "max",  xUVMax, remLen );
	p = json_objClose( p, remLen );

	//Lumi
	p = json_objOpen( p, "lumi", remLen );
		p = json_double(p,  "current",  xLumi, remLen );
		p = json_double(p,  "min",  xLumiMin, remLen );
		p = json_double(p,  "max",  xLumiMax, remLen );
	p = json_objClose( p, remLen );

	p = json_objClose( p, remLen );
	return p;
}

