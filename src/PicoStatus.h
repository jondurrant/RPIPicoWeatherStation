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

	void start();
	void stop();
	void reset();
	void sample();

	virtual char* writeJson( char* dest, const char * name, size_t* remLen );

private:
	uint xVBUSSec 					= 0;
	uint xVSYSSec 					= 0;
	uint32_t xLastSample 	= 0;

	float xVolts;
	float xMaxVolts;
	float xMinVolts;

	float xCelcius;
	float xMaxCelcius;
	float xMinCelcius;

};

#endif /* SRC_PICOSTATUS_H_ */
