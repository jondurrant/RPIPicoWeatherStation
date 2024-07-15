/*
 * Anemometer.cpp
 *
 *  Created on: 14 Jul 2024
 *      Author: jondurrant
 */

#include "Anemometer.h"
#include "GPIOInputMgr.h"
#include <cstdio>

Anemometer::Anemometer(uint8_t gp) {
	xGP = gp;

	GPIOInputMgr::getMgr()->addObserver(xGP, this);
}

Anemometer::~Anemometer() {
	// TODO Auto-generated destructor stub
}


void Anemometer::start(){
	gpio_pull_up(xGP);

	xLastTimestamp = 0;
	xNextTimestamp = 0;
	xPulses =0;
	xPulsesPerSec = 0.0;
	 xMaxPulses = 0.0;
	 xGust = 0;
	xActive = true;
}

void Anemometer::stop(){
	xActive = false;
	gpio_disable_pulls (xGP);
}

void Anemometer::handleGPIO(uint gpio, uint32_t events){
	if (! xActive){
		return;
	}

	xPulses++;

	uint32_t now = to_ms_since_boot (get_absolute_time () );
	updateStats(now);
}

void Anemometer::updateStats(uint32_t now){

	if (now > xNextTimestamp){
		if (xPulses > xGust){
			xGust = xPulses;
		}
		xPulsesPerSec = ((double)xPulses + xPulsesPerSec) / 2.0;
		xPulses = 0;
		xNextTimestamp = now + ANENOMETER_SAMPLE_MS;
		xLastTimestamp = now;

		if (xPulsesPerSec > xMaxPulses){
			xMaxPulses = xPulsesPerSec;
		}
		if (
				(xMinPulses == 0.0) ||
				(xPulsesPerSec < xMinPulses)
				){
			xMinPulses = xPulsesPerSec;
		}
	}


}

double Anemometer::pulsesPerSec(){
	uint32_t now = to_ms_since_boot (get_absolute_time () );
	updateStats(now);

	return xPulsesPerSec;
}

double Anemometer::kmph(){
	return pulsesPerSec() * ANENOMETER_KMPH;
}

double Anemometer::maxPulsesPerSec(){
	return xMaxPulses;
}

double Anemometer::maxKmph(){
	return xMaxPulses * ANENOMETER_KMPH;
}

double Anemometer::minPulsesPerSec(){
	return xMinPulses;
}


double Anemometer::minKmph(){
	return xMinPulses * ANENOMETER_KMPH;
}


double Anemometer::gustKmph(){
	return (double)xGust * ANENOMETER_KMPH;
}
