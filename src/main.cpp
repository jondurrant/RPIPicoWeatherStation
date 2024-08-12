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
#include "AnemometerPWM.h"
#include "WindVane.h"
#include "hardware/adc.h"

#include "Rain.h"
#include "hardware/i2c.h"
#include "Sen0500.h"
#include "ahtxx/ahtxx.hpp"

#include "DS3231.hpp"
#include "DeepSleep.h"
#include "PowerCtr.h"

#define I2CCHAN i2c1
#define SDA1_PAD 18
#define SCL1_PAD 19

#define SDA0_PAD 20
#define SCL0_PAD 21
#define RTC_VCC 22


#define LED_PAD 8
#define DELAY 500 // in microseconds

#define WAKE_PAD 26

#define ANEM_PAD 13
#define DIRP_PAD 	   14
#define DIRA_PAD    28

#define SNR_CTR 12



#define RAIN 15


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






    //Anemometer anem(ANEM_PAD);
    AnemometerPWM anem(ANEM_PAD);
    anem.start();

    adc_init();
    WindVane vane(DIRA_PAD, DIRP_PAD);
    vane.start();



    Rain rain(RAIN);
    uint32_t last = 0;


    PowerCtr snrCtr(SNR_CTR);

    i2c_init(I2CCHAN, 100 * 1000);
	gpio_set_function(SDA1_PAD, GPIO_FUNC_I2C);
	gpio_set_function(SCL1_PAD, GPIO_FUNC_I2C);
	snrCtr.pullUpI2C(SDA1_PAD, SCL1_PAD);
	snrCtr.on();

	 LIB_AHTXX myAHT10(AHT10_ADDRESS_0X38, I2CCHAN, SDA1_PAD,  SCL1_PAD, 100 * 1);

	 DS3231 rtc(i2c0,  SDA0_PAD,  SCL0_PAD);
	 rtc.set_power_gp(RTC_VCC);
	 printf("RTC: %s\n", rtc.get_time_str());
	 DeepSleep * deepSleep = DeepSleep::singleton();
	 deepSleep->setRTC(&rtc);

	 float temp;
	 float humid;
	 uint16_t atmos;
	 uint16_t ver;
	 float uv;
	 float lumi;

	 Sen0500 sen(I2CCHAN);


	myAHT10.AHT10_InitI2C(AHT10_SENSOR);
	while (myAHT10.AHT10_begin() != true) {
			printf("AHT10 not connect r fail load calib coeff \r\n");
			busy_wait_ms(5000);
	}

   for (;;){

	   for (int i=0; i < 5; i++){

		 printf("\n");
		 printf("RTC: %s\n", rtc.get_time_str());

		 sen.readTemp(&temp);
		 sen.readHumid(&humid);
		 sen.readPressure(&atmos);
		 sen.readUV(&uv);
		 sen.readLumi(&lumi);

		  printf("SEN0500 \t%.2f C, %.2f%%, %dhPa, %.5fmW/M2, %0.5flx\n",
				 temp,
				 humid,
				 atmos,
				 uv,
				lumi
				 );

		  printf("AHT10   \t%.2fC,  %.2f\n",
						myAHT10.AHT10_readTemperature(true),
						myAHT10.AHT10_readHumidity(true));

		   printf("Rain %u = %0.2f\n",
				   rain.pulseCount(),
				   rain.mmRain()
				   );

		   uint32_t now =  to_ms_since_boot (get_absolute_time());
		   if ((now - last) > 5000){
			   last = now;
			   rain.reset();
			   printf("RESET\n");
		   }





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
		   flash(3);

	   }

	   printf("\nWAIT\n");
	   sleep_ms(5000);

	   printf("\nLOW POWER\n");
	   snrCtr.off();
	   vane.stop();
	   anem.stop();
	   sleep_ms(5000);

	   printf("\nSLEEP\n");
	   uart_default_tx_wait_blocking();
	   deepSleep->sleep(1, WAKE_PAD);
	   //deepSleep->sleepMin(1);

	   printf("WAKE\n");
	   snrCtr.on();
	   vane.start();


   }



}
