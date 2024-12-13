/*
 * StatusAgent.h
 *
 *  Created on: 15 Aug 2024
 *      Author: jondurrant
 */

#ifndef SRC_STATUSAGENT_H_
#define SRC_STATUSAGENT_H_

#include "pico/stdlib.h"
#include "Agent.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

class StatusAgent : public Agent {
public:
	StatusAgent(
			uint8_t swt,
			uint8_t red,
			uint8_t grn,
			uint8_t blu
			);
	virtual ~StatusAgent();


	void setStatus(uint32_t rgb);


protected:
	/***
	 * Task main run loop
	 */
	virtual void run();

	/***
	 * Get the static depth required in words
	 * @return - words
	 */
	virtual configSTACK_DEPTH_TYPE getMaxStackSize();

	virtual void displayStatus(uint32_t status);


private:

	static void gpio_callback(uint gpio, uint32_t events);

	uint8_t xSWT;
	uint8_t xRED;
	uint8_t xGRN;
	uint8_t xBLU;

	uint32_t xStatus =0x00FF00;
};

#endif /* SRC_STATUSAGENT_H_ */
