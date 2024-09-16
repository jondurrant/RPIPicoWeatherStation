/*
 * Sen0500Status.h
 *
 *  Created on: 16 Sept 2024
 *      Author: jondurrant
 */

#ifndef SRC_SEN0500STATUS_H_
#define SRC_SEN0500STATUS_H_

#include "pico/stdlib.h"
#include "PayloadPart.h"
#include "Sen0500.h"


class Sen0500Status : public Sen0500,  public PayloadPart{
public:
	Sen0500Status(i2c_inst_t * i2c = i2c0,  uint16_t addr = 0x0022);
	virtual ~Sen0500Status();

	void start();
	void stop();
	void reset();
	void sample();

	virtual char* writeJson( char* dest,const  char * name, size_t* remLen ) ;


private:
	bool xFirst = true;
	bool xError = true;

	float xCelcius;
	float xCelciusMax;
	float xCelciusMin;

	float xHumid;
	float xHumidMax;
	float xHumidMin;

	float xHPa;
	float xHPaMax;
	float xHPaMin;

	float xUV;
	float xUVMax;
	float xUVMin;

	float xLumi;
	float xLumiMax;
	float xLumiMin;


};

#endif /* SRC_SEN0500STATUS_H_ */
