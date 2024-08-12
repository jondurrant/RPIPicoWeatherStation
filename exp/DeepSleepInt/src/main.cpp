/**
 * Deep Sleep and Recovery on a Raspberry PI Pico
 * LED is flashed on GPIO 2 while a wake
 *
 * Use Internal RTC for recovery
 */

#include "pico/stdlib.h"
#include "DeepSleep.h"
#include <cstdio>


#define LED_PAD 8
#define DELAY 500 // in microseconds

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

    //Setup LED
    const uint LED_PIN = LED_PAD;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(7);
    gpio_set_dir(7, GPIO_OUT);
    gpio_put(7, false);
    gpio_init(9);
	gpio_set_dir(9, GPIO_OUT);
	gpio_put(9, false);

    flash(10);

    //Drop into initial sleep for 1 minute
    DeepSleep* deepSleep = DeepSleep::singleton();

    while (true) { // Loop forever

        printf("SLEEP\n");
        uart_default_tx_wait_blocking();

        deepSleep->sleepMin(1);

		resurrect++;
		printf("RESSURECT %u\n", resurrect);

		flash(5);

    }

}
