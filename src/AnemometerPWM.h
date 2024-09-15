/*
 * AnemometerPWM.h
 *
 *  Created on: 12 Aug 2024
 *      Author: jondurrant
 */

#ifndef SRC_ANEMOMETERPWM_H_
#define SRC_ANEMOMETERPWM_H_

#include "pico/stdlib.h"

#include "PayloadPart.h"


#define ANENOMETER_KMPH 2.7

class AnemometerPWM : public PayloadPart{
public:
	AnemometerPWM(uint8_t gp);
	virtual ~AnemometerPWM();

	void start();
	void stop();
	void reset();
	void sample();

	double pulsesPerSec();

	double kmph();

	double minKmph();
	double maxKmph();
	double gustKmph();

	virtual char* writeJson( char* dest,const  char * name, size_t* remLen ) ;

private:
	uint8_t xGP = 0xFF;
	uint8_t xSlicer = 0xFF;

	uint32_t xStartTimestamp = 0;
	uint32_t xStopTimestamp = 0;

	uint32_t xSampleCount = 0;;
	uint32_t xSampleTimestamp = 0;


	double xMinKmph = 0.0;
	double xMaxKmph = 0.0;
	double xGustKmph = 0.0;
};

#endif /* SRC_ANEMOMETERPWM_H_ */
