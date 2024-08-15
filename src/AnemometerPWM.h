/*
 * AnemometerPWM.h
 *
 *  Created on: 12 Aug 2024
 *      Author: jondurrant
 */

#ifndef SRC_ANEMOMETERPWM_H_
#define SRC_ANEMOMETERPWM_H_

#include "pico/stdlib.h"


#define ANENOMETER_KMPH 2.7

class AnemometerPWM {
public:
	AnemometerPWM(uint8_t gp);
	virtual ~AnemometerPWM();

	void start();

	void stop();

	double pulsesPerSec();

	double kmph();

	double minKmph();
	double maxKmph();
	double gustKmph();

private:
	uint8_t xGP = 0xFF;
	uint8_t xSlicer = 0xFF;

	uint32_t xStartTimestamp = 0;
	uint32_t xStopTimestamp = 0;

	uint32_t xSampleCount = 0;;
	uint32_t xSampleTimestamp = 0;


	double xMinKmph = 0.0;
	double xMaxKmph = 0.0;
};

#endif /* SRC_ANEMOMETERPWM_H_ */