/*
 * PicoStatus.h
 *
 *  Created on: 5 Sept 2024
 *      Author: jondurrant
 */

#ifndef SRC_PICOSTATUS_H_
#define SRC_PICOSTATUS_H_

class PicoStatus {
public:
	PicoStatus();
	virtual ~PicoStatus();

	bool isVSYS();
	bool isVBUS();
	float vsysVolts();
	float tempCelcius();

};

#endif /* SRC_PICOSTATUS_H_ */
