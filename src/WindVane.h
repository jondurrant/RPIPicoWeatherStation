/*
 * WindVane.h
 *
 *  Created on: 14 Jul 2024
 *      Author: jondurrant
 */

#ifndef SRC_SRC_WINDVANE_H_
#define SRC_SRC_WINDVANE_H_

#include "pico/stdlib.h"

class WindVane {
public:
	WindVane(uint8_t adcGP, uint8_t pwrGP);
	virtual ~WindVane();

	void setOffset(double deg);

	void start();
	void stop();

	void sample();


	double getDegrees();
	double getMinDeg();
	double getMaxDeg();

private:
	uint8_t xAdcGP = 0;
	uint8_t xPwrGP = 0;

	double xOffset = 0.0;
	double xDeg = 0.0;
	double xMin = -1.0;
	double xMax = 0.0;

	bool xActive = false;
	bool xFirst = false;

	static float XdegRes[16][2] ;

};

#endif /* SRC_SRC_WINDVANE_H_ */
