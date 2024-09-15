/*
 * AHT10Status.h
 *
 *  Created on: 14 Sept 2024
 *      Author: jondurrant
 */

#ifndef SRC_AHT10STATUS_H_
#define SRC_AHT10STATUS_H_

#include "pico/stdlib.h"

#include "PayloadPart.h"
#include "ahtxx/ahtxx.hpp"

#ifndef AHT10RETRY
#define AHT10RETRY 5
#endif

class AHT10Status: public  LIB_AHTXX, public PayloadPart{
public:
	AHT10Status(uint8_t address, i2c_inst_t* i2c_type, uint8_t sdata , uint8_t sclk ,uint16_t clockspeed);
	virtual ~AHT10Status();

	void start();
	void stop();
	void reset();
	void sample();

	virtual char* writeJson( char* dest,const  char * name, size_t* remLen ) ;

private:
	bool xError = true;
	bool xFirst = true;
	double xCel = 0.0;
	double xHum = 0.0;
	double xMinCel = 0.0;
	double xMaxCel = 0.0;
	double xMinHum = 0.0;
	double xMaxHum = 0.0;
};

#endif /* SRC_AHT10STATUS_H_ */
