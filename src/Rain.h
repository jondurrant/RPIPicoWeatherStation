/*
 * Rain.h
 *
 *  Created on: 7 Aug 2024
 *      Author: jondurrant
 */

#ifndef SRC_RAIN_H_
#define SRC_RAIN_H_

#include "pico/stdlib.h"
#include "PayloadPart.h"

#define RAIN_MM_PER_PUSLSE 0.2794

class Rain : public PayloadPart{
public:
	Rain(uint8_t gp);
	virtual ~Rain();


	uint pulseCount();

	float mmRain();
	float maxMMPS();
	float minMMPS();
	float mmps();
	float periodMM();
	float secSinceRain();

	void reset();

	void sample();

	virtual char* writeJson( char* dest,const  char * name, size_t* remLen ) ;

	void start();
	void stop();

private:
	uint8_t xGP;
	uint8_t xSlicer ;

	float xMaxMMPS;
	float xMinMMPS;
	float xMMPS;
	float xPeriodMM;
	uint xStartCount;
	uint xSampleCount;
	uint32_t xSampleMS;
	uint32_t xMSSinceCount;


};

#endif /* SRC_RAIN_H_ */
