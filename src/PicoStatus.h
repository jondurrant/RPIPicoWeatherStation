/*
 * PicoStatus.h
 *
 *  Created on: 5 Sept 2024
 *      Author: jondurrant
 */

#ifndef SRC_PICOSTATUS_H_
#define SRC_PICOSTATUS_H_

#include "PayloadPart.h"

class PicoStatus : public PayloadPart{
public:
	PicoStatus();
	virtual ~PicoStatus();

	bool isVSYS();
	bool isVBUS();
	float vsysVolts();
	float tempCelcius();

	virtual char* writeJson( char* dest, const char * name, size_t* remLen );

};

#endif /* SRC_PICOSTATUS_H_ */
