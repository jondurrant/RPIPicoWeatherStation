/**
 * Simple Hibernate and Recovery on a Raspberry PI Pico
 * LED is flashed on GPIO 2 while a wake
 *
 * RTC DS3231 connected on I2C to GP12 & 13
 * RTC SQW used for interupt to wake on GP10
 */

#include "pico/stdlib.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include <math.h>



#define LED_PAD 2
#define DELAY 500 // in microseconds
#define SDA_PAD 12
#define SCL_PAD 13

#define WAKE_PAD 10

#define ANON_PAD 15
#define DIRP_PAD 	   14
#define DIRA_PAD    28


void flash(uint count=1){
	const uint LED_PIN = LED_PAD;

	for (uint i=0; i < count; i++){
		gpio_put(LED_PIN, 1);
		sleep_ms(DELAY);
		gpio_put(LED_PIN, 0);
		sleep_ms(DELAY);
	}
}




uint pulses = 0;
uint pulsesPS = 0;
uint32_t nextSample = 0;
void gpio_callback(uint gpio, uint32_t events) {
	pulses++;

	uint32_t now = to_ms_since_boot (get_absolute_time () );
	if (now > nextSample){
		pulsesPS = (pulses + pulsesPS) / 2;
		pulses = 0;
		nextSample = now + 1000;
	}

    //printf("GPIO %d\n", gpio);
}
uint getPulsesPS(){
	uint32_t now = to_ms_since_boot (get_absolute_time () );
	if (now > nextSample){
		pulsesPS = (pulses + pulsesPS) / 2;
		pulses = 0;
		nextSample = now + 1000;
	}
	return pulsesPS;
}

double getKMPH(){
	return (double)getPulsesPS() * 2.7;
}


double volts(){
	 adc_select_input(DIRA_PAD - 26);
	 const float conversion_factor = 3.3f / (1 << 12);
	 uint16_t result = adc_read();
	 return( result * conversion_factor);
}

double res(){
	//r2 = (Vout * R1) / (Vin - Vout)

	double Vin = 3.3;
	double Vout = volts();
	double R1 = 4.7 * 1000;

	double R2 = (Vout * R1) / (Vin - Vout);

	return R2;

}

float degRes[16][2] = {
		{0,33000},
		{22.5, 6570},
		{45, 8200},
		{67.5, 891},
		{90, 1000},
		{112.5 , 688},
		{135, 2200},
		{157.5, 1410},
		{180, 3900},
		{202.5, 3140},
		{225, 16000},
		{247.5, 14120},
		{270, 120000},
		{292.5, 42120},
		{315, 64900},
		{337.5, 21880}

};


float windDeg(float offset=0.0){
	double r = res();

	float deg = degRes[0][0];
	float res = degRes[0][1];
	double err = fabs(res - r);
	for (int i =1 ; i < 16;  i++){
		double e = fabs(degRes[i][1] - r);
		if (e < err){
			deg = degRes[i][0];
			res = degRes[0][1];
			err = e;
		}
	}

	printf("R %f picked %f err %f\n",  r,  res, err );

	deg = deg + offset;
	if (deg >= 360.0){
		deg = deg - 360.0;
	}

	return deg;
}


int main() {
	uint resurrect = 0;
    stdio_init_all();
    sleep_ms(2000);
    printf("GO\n");

    flash(3);

    //Setup LED
    const uint LED_PIN = LED_PAD;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(ANON_PAD);
    gpio_pull_up(ANON_PAD);
    gpio_set_dir(ANON_PAD, GPIO_IN);
    gpio_set_irq_enabled_with_callback	(
    		ANON_PAD,
			GPIO_IRQ_EDGE_FALL,
			true,
			gpio_callback
			);

    gpio_init(DIRP_PAD);
    //gpio_pull_up(DIRP_PAD);
    //gpio_pull_down(DIRP_PAD);
    gpio_set_dir(DIRP_PAD, GPIO_OUT);
    gpio_put(DIRP_PAD, true);

    adc_init();
    gpio_init(DIRA_PAD);
    adc_gpio_init(DIRA_PAD);


   for (;;){
	   printf("PPS %u   KMPS %f\n", getPulsesPS(), getKMPH());
	   printf("Voltes %f  Res=%f Dir=%f\n", volts(), res(), windDeg(180.0));
	   sleep_ms(500);
	   //flash(3);
   }

}
