/*
 * DeepSleepRTOS.cpp
 *
 *  Created on: 16 Jul 2024
 *      Author: jondurrant
 */

#include "DeepSleepRTOS.h"
#include "pico/sleep.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/rtc.h"
#include "hardware/rosc.h"
#include "hardware/structs/scb.h"
#include "hardware/sync.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"

#include "FreeRTOS.h"


DeepSleepRTOS::DeepSleepRTOS() {
	 storeClocks();

}

DeepSleepRTOS::~DeepSleepRTOS() {
	// TODO Auto-generated destructor stub
}

void DeepSleepRTOS::setRTC(DS3231 *rtc){
	pRTC = rtc;
}

void DeepSleepRTOS::gpio_callback(uint gpio, uint32_t events) {
	DeepSleepRTOS::singleton()->recover();

	//DEBUG
	//printf("GPIO Triggered Wake %d\n", gpio);
}

void DeepSleepRTOS::sleep(uint8_t wakePad){
	if (wakePad <= 28){
		gpio_init(wakePad);
		gpio_pull_up(wakePad);
		gpio_set_dir(wakePad, GPIO_IN);
		uint core = get_core_num();
		if (!xOwnGPIOCallbacks) {
			//Have callback function so just enable
			gpio_set_irq_enabled(
						wakePad,
					   GPIO_IRQ_EDGE_FALL |
					   GPIO_IRQ_EDGE_RISE,
					   true);
		} else {
			gpio_set_irq_enabled_with_callback(
					wakePad,
				   GPIO_IRQ_EDGE_FALL |
				   GPIO_IRQ_EDGE_RISE,
				   true,
				   DeepSleepRTOS::gpio_callback
				   );
		}
	}

	sleep_run_from_xosc();
	sleep_until_interupt();

	if (pRTC != NULL){
		recover_from_sleep(scb_orig, clock0_orig, clock1_orig);
	}

	if (wakePad <= 28){
		gpio_set_irq_enabled(
				wakePad,
			   GPIO_IRQ_EDGE_FALL |
			   GPIO_IRQ_EDGE_RISE,
			   false);
		gpio_disable_pulls(wakePad);
	}

}

void DeepSleepRTOS::rtcCB(void) {
	//gpio_put(5, true);

	DeepSleepRTOS::singleton()->recover();
	//DEBUG
	//printf("Int RTC Triggered Waked\n");
}

void DeepSleepRTOS::recover(){
	recover_from_sleep(scb_orig, clock0_orig, clock1_orig);
}


void DeepSleepRTOS::sleep(uint minutes, uint8_t wakePad){
	notifyObservers(minutes, false);
	if (pRTC != NULL){
		pRTC->clear_alarm();
		pRTC->set_delay(minutes);
		pRTC->off();
	} else {
		if (!rtc_running()){
			rtc_init();
			 datetime_t t = {
			            .year  = 2020,
			            .month = 06,
			            .day   = 05,
			            .dotw  = 5, // 0 is Sunday, so 5 is Friday
			            .hour  = 15,
			            .min   = 45,
			            .sec   = 00
			    };
			 if(!rtc_set_datetime(&t)){
				 printf("RTC Set Failed\n");
				 uart_default_tx_wait_blocking();
			 }
			 //Wait for RTC to update
			 sleep_ms(250);
		}
		datetime_t t;
		if (!rtc_get_datetime (&t)){
			printf("RTC Broken\n");
			uart_default_tx_wait_blocking();
		}
		//printf("Current Time %d:%d:%d \n", t.hour, t.min, t.sec);

		t.year = -1;
		t.month = -1;
		t.day = -1;
		t.hour = -1;
		t.dotw = -1;
		t.min = (t.min + minutes) %60;
		/*Debug
		t.min = -1;
		t.sec = (t.sec + 10) % 60;
		printf("Sleep %u min until %d-%d-%d (%d) %d:%d:%d \n", minutes,
				t.year,
				t.month,
				t.day,
				t.dotw,
				t.hour,
				t.min,
				t.sec);
	    uart_default_tx_wait_blocking();
	    */
		rtc_set_alarm ( &t,  DeepSleepRTOS::rtcCB);
	}
	sleep(wakePad);
	if (pRTC != NULL){
		pRTC->on();
		pRTC->clear_alarm();
	}
	notifyObservers(minutes, true);
}

void DeepSleepRTOS::sleepMin(uint minutes){
	sleep(minutes, 0xFF);
}

void DeepSleepRTOS::recover_from_sleep(uint scb_orig, uint clock0_orig, uint clock1_orig){

    //Re-enable ring Oscillator control
    rosc_write(&rosc_hw->ctrl, ROSC_CTRL_ENABLE_LSB);
    //gpio_put(8, true); //DEBUG

    //reset procs back to default
    scb_hw->scr = scb_orig;
    clocks_hw->sleep_en0 = clock0_orig;
    clocks_hw->sleep_en1 = clock1_orig;

    //reset clocks
    clocks_init();
   stdio_init_all();

   return;
}

void DeepSleepRTOS::sleep_until_interupt( ) {


    // Turn off all clocks when in sleep mode except for RTC
	if (pRTC == NULL){
		clocks_hw->sleep_en0 =
				xClocks |
				CLOCKS_SLEEP_EN0_CLK_RTC_RTC_BITS;
	} else {
		clocks_hw->sleep_en0 = xClocks;
	}
    clocks_hw->sleep_en1 = 0x0;

    uint save = scb_hw->scr;
    // Enable deep sleep at the proc
    scb_hw->scr = save | M0PLUS_SCR_SLEEPDEEP_BITS;

    // Go to sleep
    //__wfi();
    portSUPPRESS_TICKS_AND_SLEEP(60 * 1000);
}




DeepSleepRTOS * DeepSleepRTOS::pSingleton  = NULL;
DeepSleepRTOS * DeepSleepRTOS::singleton(){
	if (pSingleton == NULL){
		pSingleton = new DeepSleepRTOS;
	}
	return pSingleton;
}

void DeepSleepRTOS::addObserver(DormantNotification *obs){
	xObservers.push_back(obs);
}

void DeepSleepRTOS::delObserver(DormantNotification *obs){
	xObservers.remove(obs);
}

void DeepSleepRTOS::notifyObservers(uint minutes, bool wake){
	for (auto itr = xObservers.begin();
	        itr != xObservers.end(); itr++) {
			if (!wake) {
				(*itr)->notifyDormant(minutes);
			} else {
				(*itr)->notifyWake(minutes);
			}
	    }
}

void DeepSleepRTOS::storeClocks(){
	  scb_orig = scb_hw->scr;
	  clock0_orig = clocks_hw->sleep_en0;
	  clock1_orig = clocks_hw->sleep_en1;
}


void DeepSleepRTOS::enablePWM(){
	xClocks = xClocks | CLOCKS_SLEEP_EN0_CLK_SYS_PWM_BITS;
}

void DeepSleepRTOS::enableRTC(){
	xClocks = xClocks |  CLOCKS_SLEEP_EN0_CLK_RTC_RTC_BITS;
}

void DeepSleepRTOS::enableJTAG(){
	xClocks = xClocks |  CLOCKS_SLEEP_EN0_CLK_SYS_JTAG_BITS;
}

void DeepSleepRTOS::enableUart0(){
	xClocks = xClocks |  CLOCKS_SLEEP_EN1_CLK_SYS_UART0_BITS;
}


void DeepSleepRTOS::enableUart1(){
	xClocks = xClocks |  CLOCKS_SLEEP_EN1_CLK_PERI_UART0_BITS;
}

void DeepSleepRTOS::enableTimer(){
	xClocks = xClocks |  CLOCKS_SLEEP_EN1_CLK_SYS_TIMER_BITS;
}

void DeepSleepRTOS::enableUSB(){
	xClocks = xClocks |  CLOCKS_ENABLED0_CLK_SYS_PLL_USB_BITS;
}

void DeepSleepRTOS::enablePIO0(){
	xClocks = xClocks |  CLOCKS_ENABLED0_CLK_SYS_PADS_BITS;
}

void DeepSleepRTOS::enablePIO1(){
	xClocks = xClocks |  CLOCKS_ENABLED0_CLK_SYS_PIO1_BITS;
}


void DeepSleepRTOS::enableDMA(){
	xClocks = xClocks |  CLOCKS_ENABLED0_CLK_SYS_DMA_BITS;
}

void DeepSleepRTOS::setOwnGPIOCallbacks(bool on){
	xOwnGPIOCallbacks = on;
}

