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
#include "Anemometer.h"
#include "WindVane.h"
#include "hardware/adc.h"


#define LED_PAD 2
#define DELAY 500 // in microseconds
#define SDA_PAD 12
#define SCL_PAD 13

#define WAKE_PAD 10

#define ANEM_PAD 15
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


    Anemometer anem(ANEM_PAD);
    anem.start();

    adc_init();
    WindVane vane(DIRA_PAD, DIRP_PAD);
    vane.start();



   for (;;){
	   for (int i=0; i < 10; i++){
		   vane.sample();
		   sleep_ms(100);
	   }
	   printf("PPS %.2f   KMPS %.2f      %.2f < %.2f Gust: %.2f \n",
			   anem.pulsesPerSec(),
			   anem.kmph(),
			   anem.minKmph(),
			   anem.maxKmph(),
			   anem.gustKmph()
			   );

	   printf("Vane %.2f  %.2f < %.2f \n",
			   vane.getDegrees(),
			   vane.getMinDeg(),
			   vane.getMaxDeg()
			   );
	   //flash(3);
   }

}
