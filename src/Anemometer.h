/*
 * Anemometer.h
 *
 *  Created on: 14 Jul 2024
 *      Author: jondurrant
 */

#ifndef SRC_SRC_ANEMOMETER_H_
#define SRC_SRC_ANEMOMETER_H_

#include "pico/stdlib.h"
#include "GPIOObserver.h"

#ifndef ANENOMETER_SAMPLE_MS
#define ANENOMETER_SAMPLE_MS 1000
#endif

#define ANENOMETER_KMPH 2.7

class Anemometer : public GPIOObserver {
public:

	Anemometer(uint8_t gp);
	virtual ~Anemometer();

	void start();

	void stop();

	double pulsesPerSec();

	double kmph();

	double maxPulsesPerSec();
	double maxKmph();

	double minPulsesPerSec();
	double minKmph();

	double gustKmph();


	/***
	 * handle GPIO  events
	 * @param gpio - GPIO number
	 * @param events - Event
	 */
	virtual void handleGPIO(uint gpio, uint32_t events);

protected:

	virtual void updateStats(uint32_t now);

private:
	uint8_t xGP = 0xFF;
	bool xActive = false;

	uint32_t xLastTimestamp;
	uint32_t xNextTimestamp;

	uint xPulses = 0;
	uint xGust = 0;
	double xPulsesPerSec = 0.0;

	double xMaxPulses = 0.0;
	double xMinPulses = 0.0;
};

#endif /* SRC_SRC_ANEMOMETER_H_ */
