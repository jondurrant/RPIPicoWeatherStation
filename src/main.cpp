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
#include "PowerCtr.h"

#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "DeepSleepRTOS.h"
#include "wolfssl/ssl.h"

#include "WifiHelper.h"
#include "Request.h"

#include "StatusAgent.h"
#include "WeatherStation.h"

//Check these definitions where added from the makefile
#ifndef WIFI_SSID
#error "WIFI_SSID not defined"
#endif
#ifndef WIFI_PASSWORD
#error "WIFI_PASSWORD not defined"
#endif

#define TASK_PRIORITY     ( tskIDLE_PRIORITY + 1UL )

#define I2CCHAN i2c1
#define SDA1_PAD 18
#define SCL1_PAD 19

#define SDA0_PAD 20
#define SCL0_PAD 21
#define RTC_VCC 22


#define SWT_PAD 6
#define LED_PAD 7
#define RED_PAD 7
#define GRN_PAD 10
#define BLU_PAD 11
#define DELAY 500 // in microseconds

#define WAKE_PAD 26

#define ANEM_PAD 13
#define DIRP_PAD 	   14
#define DIRA_PAD    28

#define SNR_CTR 12

#define RAIN 15


void runTimeStats(){
  TaskStatus_t         * pxTaskStatusArray;
  volatile UBaseType_t uxArraySize, x;
  unsigned long        ulTotalRunTime;


  /* Take a snapshot of the number of tasks in case it changes while this
  function is executing. */
  uxArraySize = uxTaskGetNumberOfTasks();
  printf("Number of tasks %d\n", uxArraySize);

  /* Allocate a TaskStatus_t structure for each task.  An array could be
  allocated statically at compile time. */
  pxTaskStatusArray = (TaskStatus_t*) pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

  if (pxTaskStatusArray != NULL){
    /* Generate raw status information about each task. */
    uxArraySize = uxTaskGetSystemState(pxTaskStatusArray,
                                       uxArraySize,
                                       &ulTotalRunTime);



    /* For each populated position in the pxTaskStatusArray array,
    format the raw data as human readable ASCII data. */
    for (x = 0; x < uxArraySize; x++){
      printf("Task: %d \t cPri:%d \t bPri:%d \t hw:%d \t%s\n",
             pxTaskStatusArray[x].xTaskNumber,
             pxTaskStatusArray[x].uxCurrentPriority,
             pxTaskStatusArray[x].uxBasePriority,
             pxTaskStatusArray[x].usStackHighWaterMark,
             pxTaskStatusArray[x].pcTaskName
      );
    }


    /* The array is no longer needed, free the memory it consumes. */
    vPortFree(pxTaskStatusArray);
  } else{
    printf("Failed to allocate space for stats\n");
  }

  HeapStats_t heapStats;
  vPortGetHeapStats(&heapStats);
  printf("HEAP avl: %d, blocks %d, alloc: %d, free: %d\n",
         heapStats.xAvailableHeapSpaceInBytes,
         heapStats.xNumberOfFreeBlocks,
         heapStats.xNumberOfSuccessfulAllocations,
         heapStats.xNumberOfSuccessfulFrees
  );

}


void flash(uint count=1){
	const uint LED_PIN = LED_PAD;

	for (uint i=0; i < count; i++){
		gpio_put(LED_PIN, 1);
		sleep_ms(DELAY);
		gpio_put(LED_PIN, 0);
		sleep_ms(DELAY);
	}
}




//int main() {
void old_main_task(void* params){
	uint resurrect = 0;
//    stdio_init_all();
//    sleep_ms(2000);
    printf("GO\n");

    //flash(3);

    //Setup LED
    //const uint LED_PIN = LED_PAD;
    //gpio_init(LED_PIN);
   // gpio_set_dir(LED_PIN, GPIO_OUT);

    StatusAgent statusAgent(
    		SWT_PAD,
			RED_PAD,
			GRN_PAD,
			BLU_PAD);
    statusAgent.start("Status", TASK_PRIORITY);
    //statusAgent.setStatus(0x0012821A);





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
	 DeepSleepRTOS * deepSleep = DeepSleepRTOS::singleton();
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
		  // flash(3);

		   vTaskDelay(5000);

	   }

	   printf("\nWAIT\n");
	   vTaskDelay(5000);

	   printf("\nLOW POWER\n");
	   snrCtr.off();
	   vane.stop();
	   anem.stop();
	   vTaskDelay(5000);

	   printf("\nSLEEP\n");
	   uart_default_tx_wait_blocking();
	   //portSUPPRESS_TICKS_AND_SLEEP(1);
	   deepSleep->sleep(1, WAKE_PAD);
	   //deepSleep->sleepMin(1);

	   printf("WAKE\n");
	   snrCtr.on();
	   vane.start();
	   anem.start();


   }

}

void debugCB(const int logLevel, const char *const logMessage){
	printf("WOLFSSL DEBUG(%d): %s\n", logLevel, logMessage);
}

bool wifiFirst = true;
bool wifiOn(){
	if (WifiHelper::init()){
		printf("Wifi Controller Initialised\n");
	} else {
		printf("Failed to initialise controller\n");

		return false;
	}

	if (wifiFirst){
		WifiHelper::sntpAddServer("0.uk.pool.ntp.org");
		WifiHelper::sntpAddServer("1.uk.pool.ntp.org");
		WifiHelper::sntpAddServer("2.uk.pool.ntp.org");
		WifiHelper::sntpAddServer("3.uk.pool.ntp.org");
		wifiFirst = false;
	}
	WifiHelper::sntpStartSync();

	printf("Connecting to WiFi... %s \n", WIFI_SSID);

	//if (WifiHelper::join(WIFI_SSID, WIFI_PASSWORD)){
	if (WifiHelper::join(WIFI_SSID, WIFI_PASSWORD)){
		printf("Connect to Wifi\n");
	}
	else {
		printf("Failed to connect to Wifi \n");
		return false;
	}


	//Print MAC Address
	char macStr[20];
	WifiHelper::getMACAddressStr(macStr);
	printf("MAC ADDRESS: %s\n", macStr);

	//Print IP Address
	char ipStr[20];
	WifiHelper::getIPAddressStr(ipStr);
	printf("IP ADDRESS: %s\n", ipStr);

	return true;
}

bool wifiOff(){
	return WifiHelper::deInit();
}

void main_task(void* params){
	printf("Main Task Started\n");

	wolfSSL_Init();
	wolfSSL_SetLoggingCb( debugCB);
	//wolfSSL_Debugging_ON();

	WeatherStation station;
	station.init();

    StatusAgent statusAgent(
    		SWT_PAD,
			RED_PAD,
			GRN_PAD,
			BLU_PAD);
    statusAgent.start("Status", TASK_PRIORITY);
    statusAgent.setStatus(0xFF0000);

	DeepSleepRTOS::singleton()->setRTC(station.getRTC());

	for(;;){
		station.start();
		bool on = wifiOn();

		for (int i=0; i < 10; i ++){
			vTaskDelay(2000);
			station.sample();
		}
		station.stop();

		if (on){
			station.submit();
		}
		wifiOff();

		printf("Going to sleep\n");
		vTaskDelay(1000);
		DeepSleepRTOS::singleton()->sleep(1,  WAKE_PAD);
		printf("Awake\n");

		//vTaskDelay(10000);
		vTaskDelay(1000);
	}

}


void vLaunch(void) {
  TaskHandle_t task;

  xTaskCreate(main_task, "MainThread", 5000*2, NULL, TASK_PRIORITY, &task);

  /* Start the tasks and timer running. */
  vTaskStartScheduler();

  printf("SCHEDULED STOPPED\n");
  sleep_ms(5000);
}


int main(void) {
  stdio_init_all();
  sleep_ms(2000);
  printf("GO\n");

  /* Configure the hardware ready to run the demo. */
  const char* rtos_name;
  rtos_name = "FreeRTOS";
  printf("Starting %s on core 0:\n", rtos_name);
  vLaunch();

  return 0;
}
