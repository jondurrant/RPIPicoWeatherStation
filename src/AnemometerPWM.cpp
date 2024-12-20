/*
 * AnemometerPWM.cpp
 *
 *  Created on: 12 Aug 2024
 *      Author: jondurrant
 */

#include "AnemometerPWM.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include <cstdio>
#include "json-maker/json-maker.h"

AnemometerPWM::AnemometerPWM(uint8_t gp)  {
	xGP = gp;

	gpio_init(xGP);
	gpio_pull_up(xGP);
	gpio_set_dir(xGP, GPIO_IN);

	if (pwm_gpio_to_channel(xGP) != PWM_CHAN_B){
		printf("ERROR - GPIO Must be PWM Channel B\n");
	} else {
		xSlicer = pwm_gpio_to_slice_num(xGP);

	   // Count once for every 1 cycles the PWM B input is HIGH
	   pwm_config cfg = pwm_get_default_config();
	   pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_FALLING);
	   pwm_config_set_clkdiv(&cfg, 1);
	   pwm_init(xSlicer, &cfg, false);
	   gpio_set_function(xGP, GPIO_FUNC_PWM);
	   pwm_set_enabled(xSlicer, true);
	}

}

AnemometerPWM::~AnemometerPWM() {
	// TODO Auto-generated destructor stub
}



void AnemometerPWM::start(){
	pwm_set_counter	(xSlicer, 0);
	xStartTimestamp =  to_ms_since_boot (get_absolute_time () );
	xStopTimestamp = 0;
	xSampleCount = 0;
	xSampleTimestamp = to_ms_since_boot (get_absolute_time () );
}

void AnemometerPWM::stop(){
	   pwm_set_enabled(xSlicer, false);
	   gpio_disable_pulls (xGP);
	   xStopTimestamp =  to_ms_since_boot (get_absolute_time () );
}

double AnemometerPWM::pulsesPerSec(){
	uint32_t end;
	if (xStopTimestamp == 0){
		end = to_ms_since_boot (get_absolute_time () );
	} else {
		end = xStopTimestamp;
	}

	uint32_t ms = end - xStartTimestamp;
	uint32_t count = pwm_get_counter(xSlicer);
	//printf("Count %d\n", count);
	double pps =  (double)count / ((double) ms /1000.0);

	if (xStopTimestamp == 0){
		uint32_t newCount = count - xSampleCount;
		xSampleCount = count;
		ms = end - xSampleTimestamp;
		xSampleTimestamp = end;


		double sample =  ((double)newCount / ((double) ms /1000.0)) * ANENOMETER_KMPH ;
		/*
		printf("Updating Stats %f = (%d /  (%d / 1000.0) * %f\n",
				sample,
				newCount ,
				ms,
				ANENOMETER_KMPH
				);
				*/
		if (sample > xMaxKmph){
			xMaxKmph = sample;
		}
		if ((xMinKmph == 0.0) || (xMinKmph > sample)){
			xMinKmph = sample;
		}
	}

	return pps;
}

double AnemometerPWM::kmph(){
	return pulsesPerSec() * ANENOMETER_KMPH;
}

double AnemometerPWM::minKmph(){
	return xMinKmph;
}

double AnemometerPWM::maxKmph(){
	return xMaxKmph;
}

double AnemometerPWM::gustKmph(){
	return xGustKmph;
}


void AnemometerPWM::reset(){
	pwm_set_counter	(xSlicer, 0);
	xStartTimestamp = 0;
	xStopTimestamp = 0;

	xSampleCount = 0;;
	xSampleTimestamp = 0;

	xMinKmph = 0.0;
	xMaxKmph = 0.0;
	xGustKmph = 0.0;
}

void AnemometerPWM::sample(){
	double kmph = pulsesPerSec() * ANENOMETER_KMPH;
	if (kmph > xGustKmph){
		xGustKmph = kmph;
	}
}

/*
 *  "anem": {
            "kmph": {
                "current": 0.0,
                "max": 0.0,
                "min": 0.0
                },
            }
 */
char* AnemometerPWM::writeJson( char* dest,const  char * name, size_t* remLen ) {
	char * p = dest;

	p = json_objOpen( p, name, remLen );
	p = json_objOpen( p, "kmph", remLen );
		p = json_double(p,  "current",  kmph(), remLen );
		p = json_double(p,  "max",  maxKmph(), remLen );
		p = json_double(p,  "min",  minKmph(), remLen );
		p = json_double(p,  "gust_kmph",  gustKmph(), remLen );
	p = json_objClose( p, remLen );
	p = json_objClose( p, remLen );
	return p;
}
