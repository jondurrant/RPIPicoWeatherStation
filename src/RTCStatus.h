/*
 * RTCStatus.h
 *
 *  Created on: 15 Sept 2024
 *      Author: jondurrant
 */

#ifndef SRC_RTCSTATUS_H_
#define SRC_RTCSTATUS_H_

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "VoltMeter.h"
#include "PayloadPart.h"
#include "DS3231.hpp"

class RTCStatus : public DS3231,  public PayloadPart{
public:
	RTCStatus(i2c_inst_t *i2c, uint8_t sdaPin, uint8_t sclPin, uint8_t batGP);
	virtual ~RTCStatus();

	void start();
	void stop();
	void reset();
	void sample();

	double batVolts();

	virtual char* writeJson( char* dest,const  char * name, size_t* remLen ) ;

private:
	VoltMeter *pBat = NULL;

	float xCelcius = 0.0;
	float xMaxCelcius = 0.0;
	float xMinCelcius = 0.0;

	float xVolts = 0.0;
	float xMaxVolts = 0.0;
	float xMinVolts = 0.0;
};

#endif /* SRC_RTCSTATUS_H_ */
