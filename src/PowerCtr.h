/*
 * PowerCtr.h
 *
 *  Created on: 8 Aug 2024
 *      Author: jondurrant
 */

#ifndef SRC_POWERCTR_H_
#define SRC_POWERCTR_H_


#include "pico/stdlib.h"
class PowerCtr {
public:
	PowerCtr(uint8_t gp);
	virtual ~PowerCtr();

	void pullUpI2C(uint8_t sda, uint8_t scl);

	void on();
	void off();


private:
	uint8_t xGP= 0xFF;
	uint8_t xSDA = 0xFF;
	uint8_t xSCL = 0xFF;

};

#endif /* SRC_POWERCTR_H_ */
